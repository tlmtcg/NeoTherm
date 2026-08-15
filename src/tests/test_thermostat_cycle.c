#include "test_thermostat_cycle.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "history.h"
#include "program.h"
#include "relay.h"
#include "thermostat.h"

#include "test_utils.h"
#include "../infra/console/console_utils.h"

/*
 * ==========================================================
 * TEST D'UN CYCLE COMPLET DU THERMOSTAT
 * ==========================================================
 *
 * Scénario :
 *
 *   1. Initialisation
 *   2. Température sous seuil ON
 *      -> demande ON
 *      -> relais ON
 *   3. Température dans la bande d'hystérésis
 *      -> demande ON
 *      -> relais ON
 *   4. Température au-dessus du seuil OFF
 *      -> demande OFF
 *      -> relais OFF
 *   5. Vérification finale
 *
 * Ce test ne teste pas les détails internes de
 * l'apprentissage ou de la prédiction.
 *
 * Il vérifie le comportement global du thermostat
 * sur un cycle de chauffage.
 */

/*
 * ==========================================================
 * AFFICHAGE DE L'ETAT
 * ==========================================================
 */

static void print_state(
    const char *label)
{
    const thermostat_status_t *status =
        thermostat_get_status();

    printf(
        "\n[%s]\n",
        label);

    printf(
        "Temperature : %.2f C\n",
        status->temperature);

    printf(
        "Setpoint    : %.2f C\n",
        status->setpoint);

    printf(
        "Request     : %s\n",
        status->heating_request
            ? "ON"
            : "OFF");

    printf(
        "Relay       : %s\n",
        relay_get()
            ? "ON"
            : "OFF");
}

/*
 * ==========================================================
 * VERIFICATION D'UN ETAT
 * ==========================================================
 */

static bool check_state(
    float expected_temperature,
    bool expected_request,
    bool expected_relay)
{
    const thermostat_status_t *status =
        thermostat_get_status();

    ASSERT_EQ_FLOAT(
        expected_temperature,
        status->temperature);

    ASSERT_TRUE(
        status->heating_request ==
        expected_request);

    ASSERT_TRUE(
        relay_get() ==
        expected_relay);

    return true;
}

/*
 * ==========================================================
 * INITIALISATION
 * ==========================================================
 */

static bool initialize_cycle(void)
{
    console_print_header(
        "INITIALISATION");

    clock_init();

    climate_init();

    relay_init();

    history_init();

    program_init();

    thermostat_init();

    /*
     * AUTO obligatoire pour ce test.
     */

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    /*
     * Désactivation de l'anti-cycle :
     *
     * le test doit vérifier immédiatement
     * les transitions du relais.
     */

    relay_test_reset();

    relay_set_min_switch_delay(0);

    /*
     * Heure déterministe.
     */

    clock_time_t time =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 17,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&time));

    clock_sync_to_runtime();

    return true;
}

/*
 * ==========================================================
 * ETAPE 1 : CHAUFFAGE
 * ==========================================================
 */

static bool test_heating_start(void)
{
    console_print_header(
        "HEATING START");

    /*
     * Synchronise la consigne du programme
     * avec le thermostat.
     */
    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    float setpoint =
        status->setpoint;

    float hysteresis =
        status->hysteresis;

    printf(
        "Thermostat setpoint : %.2f C\n",
        setpoint);

    printf(
        "Hysteresis          : %.2f C\n",
        hysteresis);

    /*
     * Température juste sous le seuil ON.
     *
     * Exemple :
     *
     * setpoint   = 18.00
     * hysteresis = 0.30
     *
     * seuil ON   = 17.70
     *
     * température = 17.60
     */

    float temperature =
        setpoint -
        hysteresis -
        0.10f;

    printf(
        "Test temperature    : %.2f C\n",
        temperature);

    climate_test_set_temperature(
        temperature);

    thermostat_update();

    print_state(
        "Sous seuil ON");

    /*
     * Vérification du scénario.
     */

    ASSERT_TRUE(
        temperature <
        (setpoint - hysteresis));

    ASSERT_TRUE(
        thermostat_get_status()
            ->heating_request);

    ASSERT_TRUE(
        relay_get());

    return true;
}

/*
 * ==========================================================
 * ETAPE 2 : MAINTIEN
 * ==========================================================
 */

static bool test_heating_hold(void)
{
    console_print_header(
        "HYSTERESIS HOLD");

    const thermostat_status_t *status =
        thermostat_get_status();

    float setpoint =
        status->setpoint;

    /*
     * Retour exactement sur la consigne.
     *
     * La température est :
     *
     *     > seuil ON
     *     < seuil OFF
     *
     * Le chauffage doit continuer.
     */

    climate_test_set_temperature(
        setpoint);

    thermostat_update();

    print_state(
        "Dans bande d'hysteresis");

    ASSERT_TRUE(
        setpoint >
        (status->setpoint -
         status->hysteresis));

    ASSERT_TRUE(
        setpoint <
        (status->setpoint +
         status->hysteresis));

    /*
     * Le relais reste ON.
     */

    ASSERT_TRUE(
        relay_get());

    /*
     * La demande normale reste ON
     * tant que le relais chauffe.
     */

    ASSERT_TRUE(
        thermostat_get_status()
            ->heating_request);

    return true;
}

/*
 * ==========================================================
 * ETAPE 3 : ARRET
 * ==========================================================
 */

static bool test_heating_stop(void)
{
    console_print_header(
        "HEATING STOP");

    const thermostat_status_t *status =
        thermostat_get_status();

    float setpoint =
        status->setpoint;

    float hysteresis =
        status->hysteresis;

    /*
     * On dépasse le seuil OFF.
     */

    float temperature =
        setpoint +
        hysteresis +
        0.10f;

    climate_test_set_temperature(
        temperature);

    thermostat_update();

    print_state(
        "Au-dessus seuil OFF");

    /*
     * La température doit être
     * supérieure au seuil OFF.
     */

    ASSERT_TRUE(
        temperature >=
        (setpoint + hysteresis));

    /*
     * La demande doit être OFF.
     */

    ASSERT_FALSE(
        thermostat_get_status()
            ->heating_request);

    /*
     * Le relais doit être OFF.
     */

    ASSERT_FALSE(
        relay_get());

    return true;
}

/*
 * ==========================================================
 * ETAPE 4 : VERIFICATION FINALE
 * ==========================================================
 */

static bool test_cycle_final_state(void)
{
    console_print_header(
        "FINAL STATE");

    const thermostat_status_t *status =
        thermostat_get_status();

    print_state(
        "Etat final");

    /*
     * Le thermostat doit être en AUTO.
     */

    ASSERT_TRUE(
        status->mode ==
        THERMOSTAT_AUTO);

    /*
     * Le chauffage doit être arrêté.
     */

    ASSERT_FALSE(
        status->heating_request);

    ASSERT_FALSE(
        relay_get());

    /*
     * Vérification de la température finale.
     */

    ASSERT_TRUE(
        status->temperature >=
        (status->setpoint +
         status->hysteresis));

    printf(
        "Cycle thermostat valide\n");

    return true;
}

/*
 * ==========================================================
 * TEST PRINCIPAL
 * ==========================================================
 */

bool test_thermostat_cycle_run(void)
{
    printf(
        "\n=============== "
        "THERMOSTAT INTEGRATION CYCLE TEST "
        "===============\n");

    /*
     * ------------------------------------------------------
     * INITIALISATION
     * ------------------------------------------------------
     */

    if (!initialize_cycle())
    {
        return false;
    }

    const thermostat_status_t *status =
        thermostat_get_status();

    printf(
        "Setpoint   : %.2f C\n",
        status->setpoint);

    printf(
        "Hysteresis : %.2f C\n",
        status->hysteresis);

    /*
     * ------------------------------------------------------
     * CYCLE DE CHAUFFAGE
     * ------------------------------------------------------
     */

    if (!test_heating_start())
    {
        return false;
    }

    if (!test_heating_hold())
    {
        return false;
    }

    if (!test_heating_stop())
    {
        return false;
    }

    /*
     * ------------------------------------------------------
     * ETAT FINAL
     * ------------------------------------------------------
     */

    if (!test_cycle_final_state())
    {
        return false;
    }

    printf(
        "\nPASS : Thermostat complete cycle\n");

    return true;
}
