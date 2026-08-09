
#include "test_thermostat_prediction.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "history.h"
#include "program.h"
#include "relay.h"
#include "storage.h"
#include "../thermal_learning/thermal_learning.h"
#include "../thermal_prediction/thermal_prediction.h"
#include "thermostat.h"
#include "test_utils.h"
#include "../infra/console/console_utils.h"

/*
 * ======================================================
 * TEST THERMOSTAT AUTO + PREDICTION
 * ======================================================
 *
 * Ce fichier teste uniquement l'interaction entre :
 *
 *   - thermostat AUTO
 *   - apprentissage thermique
 *   - prédiction thermique
 *   - demande de chauffage
 *   - relais
 *
 * Les tests de base du thermostat sont dans :
 *
 *   test_thermostat.c
 *
 */

/*
 * ======================================================
 * PREPARATION DU LEARNING
 * ======================================================
 *
 * Construit un historique artificiel permettant
 * d'obtenir :
 *
 *   Heat  = +0.20 °C/min
 *   Cool  = -0.10 °C/min
 *
 */

static bool prepare_thermal_learning(void)
{
    float temperature = 18.0f;

    /*
     * --------------------------------------------------
     * PHASE CHAUFFAGE
     * --------------------------------------------------
     */

    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temperature,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            true,
            true);

        temperature += 0.20f;
    }

    /*
     * --------------------------------------------------
     * PHASE REFROIDISSEMENT
     * --------------------------------------------------
     */

    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temperature,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            false,
            false);

        temperature -= 0.10f;
    }

    /*
     * --------------------------------------------------
     * APPRENTISSAGE
     * --------------------------------------------------
     */

    ASSERT_TRUE(
        thermal_learning_update());

    ASSERT_TRUE(
        thermal_learning_is_valid());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    return true;
}

/*
 * ======================================================
 * TEST : PREDICTION DISPONIBLE
 * ======================================================
 */

static bool test_prediction_available(void)
{
    console_print_header(
        "PREDICTION AVAILABLE");

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    printf("PASS\n");

    return true;
}

/*
 * ======================================================
 * TEST : AUTO SANS DEMANDE
 * ======================================================
 *
 * Température actuelle au-dessus du seuil ON.
 *
 * Le relais doit rester OFF.
 *
 */

static bool test_auto_no_request(void)
{
    console_print_header(
        "AUTO NO PREDICTIVE REQUEST");

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    /*
     * Consigne programme.
     */

    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    float setpoint =
        status->setpoint;

    float hysteresis =
        status->hysteresis;

    /*
     * Température dans la zone normale.
     */

    climate_test_set_temperature(
        setpoint);

    relay_set(false);

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    ASSERT_FALSE(
        status->heating_request);

    printf("PASS\n");

    return true;
}

/*
 * ======================================================
 * TEST : PREDICTION DEMANDE LE CHAUFFAGE
 * ======================================================
 *
 * Situation :
 *
 *   température actuelle :
 *       sous la consigne
 *
 *   mais au-dessus du seuil normal ON
 *
 *   température naturelle prévue :
 *       sous la marge prédictive
 *
 * Le chauffage doit donc être demandé
 * uniquement grâce à la prédiction.
 *
 */

/*
 * ======================================================
 * TEST : DEMANDE PREDICTIVE
 * ======================================================
 *
 * Situation recherchée :
 *
 *   Consigne       = 18.0 °C
 *   Hystérésis     = 0.3 °C
 *
 *   Seuil ON       = 17.7 °C
 *
 *   Température    = 17.9 °C
 *
 * La régulation normale ne demande donc PAS
 * le chauffage.
 *
 * Mais la température naturelle prévue à +10 min
 * doit descendre suffisamment bas pour déclencher
 * la demande prédictive.
 *
 */

static bool test_auto_predictive_request(void)
{
    console_print_header(
        "AUTO PREDICTIVE REQUEST");

    /*
     * --------------------------------------------------
     * PREPARATION
     * --------------------------------------------------
     */

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 14,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * --------------------------------------------------
     * HISTORIQUE
     * --------------------------------------------------
     *
     * Dernière température :
     *
     *     17.90 °C
     *
     * Chauffage OFF.
     */

    history_add(
        17.90f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    /*
     * --------------------------------------------------
     * PREDICTION
     * --------------------------------------------------
     */

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    /*
     * --------------------------------------------------
     * MODE AUTO
     * --------------------------------------------------
     *
     * IMPORTANT :
     *
     * thermostat_set_mode() peut provoquer une mise
     * à jour immédiate du thermostat.
     *
     * On positionne donc le mode avant de préparer
     * l'état final du scénario.
     */

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    /*
     * Le changement de mode peut avoir déclenché
     * une demande de chauffage.
     *
     * On force donc le relais OFF pour démarrer
     * réellement le scénario prédictif avec un
     * relais arrêté.
     */

    relay_test_reset();

    ASSERT_FALSE(
        relay_get());

    /*
     * --------------------------------------------------
     * TEMPERATURE ACTUELLE
     * --------------------------------------------------
     */

    climate_test_set_temperature(
        17.90f);

    /*
     * --------------------------------------------------
     * VERIFICATION PREDICTION
     * --------------------------------------------------
     */

    float natural_10min =
        thermal_prediction_get_temperature_minutes_state(
            10.0f,
            false);

    const thermostat_status_t *status =
        thermostat_get_status();

    float setpoint =
        status->setpoint;

    float hysteresis =
        status->hysteresis;

    printf(
        "Current       : %.2f C\n",
        17.90f);

    printf(
        "Setpoint      : %.2f C\n",
        setpoint);

    printf(
        "Natural +10   : %.2f C\n",
        natural_10min);

    /*
     * --------------------------------------------------
     * REGULATION NORMALE
     * --------------------------------------------------
     *
     * 17.90 > 17.70
     *
     * La température est donc au-dessus du seuil
     * normal de mise en marche.
     */

    ASSERT_TRUE(
        17.90f >
        (setpoint - hysteresis));

    /*
     * La température actuelle est néanmoins
     * inférieure à la consigne.
     */

    ASSERT_TRUE(
        17.90f <
        setpoint);

    /*
     * --------------------------------------------------
     * REGULATION PREDICTIVE
     * --------------------------------------------------
     *
     * La température naturelle prévue doit
     * descendre suffisamment sous la consigne.
     */

    ASSERT_TRUE(
        natural_10min <
        (setpoint - 0.5f));

    /*
     * --------------------------------------------------
     * CALCUL AUTO
     * --------------------------------------------------
     */

    thermostat_update();

    status =
        thermostat_get_status();

    /*
     * La demande doit maintenant être prédictive.
     */

    ASSERT_TRUE(
        status->heating_request);

    /*
     * Le relais était OFF au début du scénario.
     * La demande prédictive doit donc provoquer
     * son passage à ON.
     */

    ASSERT_TRUE(
        relay_get());

    printf("PASS\n");

    return true;
}



/*
 * ======================================================
 * TEST : PREDICTION NE DOIT PAS MAINTENIR LE RELAIS
 * ======================================================
 *
 * Une fois le chauffage ON, la demande prédictive
 * ne doit pas servir à maintenir artificiellement
 * le chauffage.
 *
 * Le relais reste ON uniquement grâce à
 * l'hystérésis jusqu'au seuil OFF.
 *
 */

static bool test_prediction_does_not_hold_relay(void)
{
    console_print_header(
        "PREDICTION DOES NOT HOLD RELAY");

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    const thermostat_status_t *status =
        thermostat_get_status();

    /*
     * Température dans la bande d'hystérésis.
     *
     * Elle est supérieure au seuil ON,
     * mais inférieure au seuil OFF.
     */

    float temperature =
        status->setpoint;

    climate_test_set_temperature(
        temperature);

    thermostat_update();

    /*
     * Le relais doit rester ON.
     *
     * Mais heating_request doit être FALSE :
     * la prédiction ne maintient pas la demande.
     */

    ASSERT_TRUE(
        relay_get());

    ASSERT_FALSE(
        status->heating_request);

    printf("PASS\n");

    return true;
}

/*
 * ======================================================
 * TEST : ARRET SUR SEUIL OFF
 * ======================================================
 */

static bool test_auto_predictive_stop(void)
{
    console_print_header(
        "AUTO PREDICTIVE STOP");

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    const thermostat_status_t *status =
        thermostat_get_status();

    float temperature =
        status->setpoint +
        status->hysteresis +
        0.1f;

    climate_test_set_temperature(
        temperature);

    /*
     * L'anti-cycle doit avoir le temps
     * de permettre la commutation.
     */

    clock_add_seconds(180);

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    ASSERT_FALSE(
        status->heating_request);

    printf("PASS\n");

    return true;
}

/*
 * ======================================================
 * TEST PRINCIPAL
 * ======================================================
 */

bool test_thermostat_prediction_run(void)
{
    printf(
        "\n=============== "
        "THERMOSTAT AUTO + PREDICTION TEST "
        "===============\n");

    /*
     * --------------------------------------------------
     * INITIALISATION
     * --------------------------------------------------
     */

    console_print_header(
        "INITIALISATION");

    clock_init();

    climate_init();

    relay_init();

    history_init();

    storage_init();

    program_init();

    thermal_learning_init();

    thermal_prediction_init();

    thermostat_init();

    relay_test_reset();

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    relay_set_min_switch_delay(0);

    /*
     * --------------------------------------------------
     * HEURE DE DEPART
     * --------------------------------------------------
     */

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 12,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * --------------------------------------------------
     * APPRENTISSAGE
     * --------------------------------------------------
     */

    console_print_header(
        "THERMAL LEARNING");

    ASSERT_TRUE(
        prepare_thermal_learning());

    /*
     * --------------------------------------------------
     * PREDICTION
     * --------------------------------------------------
     */

    ASSERT_TRUE(
        test_prediction_available());

    /*
     * --------------------------------------------------
     * TEST AUTO NORMAL
     * --------------------------------------------------
     */

    ASSERT_TRUE(
        test_auto_no_request());

    /*
     * --------------------------------------------------
     * TEST AUTO PREDICTIF
     * --------------------------------------------------
     */

    ASSERT_TRUE(
        test_auto_predictive_request());

    /*
     * --------------------------------------------------
     * TEST MAINTIEN
     * --------------------------------------------------
     */

    ASSERT_TRUE(
        test_prediction_does_not_hold_relay());

    /*
     * --------------------------------------------------
     * TEST ARRET
     * --------------------------------------------------
     */

    ASSERT_TRUE(
        test_auto_predictive_stop());

    /*
     * --------------------------------------------------
     * RESULTAT
     * --------------------------------------------------
     */

    printf(
        "\nPASS : Thermostat AUTO + Prediction\n");

    return true;
}
