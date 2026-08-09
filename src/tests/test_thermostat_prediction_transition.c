#include "test_thermostat_prediction_transition.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "history.h"
#include "relay.h"
#include "thermostat.h"

#include "../thermal_learning/thermal_learning.h"
#include "../thermal_prediction/thermal_prediction.h"

#include "test_utils.h"
#include "../infra/console/console_utils.h"

bool test_thermostat_prediction_transition_run(void)
{
    printf(
        "\n=============== THERMOSTAT PREDICTION TRANSITION TEST ===============\n");

    /*
     * --------------------------------------------------
     * INITIALISATION
     * --------------------------------------------------
     */

    console_print_header("INITIALISATION");

    clock_init();
    climate_init();
    relay_init();
    history_init();

    thermal_learning_init();
    thermal_prediction_init();

    relay_test_reset();
    relay_set_min_switch_delay(0);

    thermostat_init();
    thermostat_set_mode(THERMOSTAT_AUTO);

    /*
     * --------------------------------------------------
     * HISTORIQUE THERMIQUE
     * --------------------------------------------------
     *
     * On fournit les taux directement via l'historique :
     *
     * chauffage      : +0.20 °C/min
     * refroidissement: -0.10 °C/min
     */

    console_print_header("THERMAL LEARNING");

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

    float temp = 18.0f;

    /*
     * Phase chauffage
     */
    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            18.0f,
            THERMOSTAT_AUTO,
            true,
            true);

        temp += 0.20f;
    }

    /*
     * Phase refroidissement
     */
    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            18.0f,
            THERMOSTAT_AUTO,
            false,
            false);

        temp -= 0.10f;
    }

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    /*
     * --------------------------------------------------
     * PREDICTIVE REQUEST
     * --------------------------------------------------
     *
     * Température actuelle :
     *
     *     17.90 °C
     *
     * Consigne :
     *
     *     18.00 °C
     *
     * Seuil normal ON :
     *
     *     17.70 °C
     *
     * Donc :
     *
     *     17.90 > 17.70
     *
     * => pas de demande normale.
     *
     * Avec un refroidissement de -0.10 °C/min :
     *
     *     naturel = 17.90 - 1.00 = 16.90 °C
     *
     * Avec chauffage :
     *
     *     chauffé = 17.90 + 2.00 = 19.90 °C
     *
     * => la prédiction doit demander le chauffage.
     */

    console_print_header(
        "PREDICTIVE REQUEST");

    climate_test_set_temperature(
        17.90f);

    /*
     * La prédiction travaille sur la dernière
     * mesure disponible dans l'historique.
     *
     * On ajoute donc la température injectée
     * dans l'historique avant de recalculer
     * la prédiction.
     */
    history_add(
        17.90f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    ASSERT_TRUE(
        status->heating_request);

    ASSERT_TRUE(
        relay_get());

    printf(
        "Predictive request : ON\n");

    /*
     * --------------------------------------------------
     * PASSAGE PREDICTIVE -> NORMALE
     * --------------------------------------------------
     *
     * Nouvelle température :
     *
     *     17.60 °C
     *
     * Comme :
     *
     *     17.60 <= 17.70
     *
     * la demande doit maintenant être une
     * demande normale.
     *
     * La demande de chauffage doit rester active.
     */

    console_print_header(
        "PREDICTIVE TO NORMAL REQUEST");

    climate_test_set_temperature(
        17.60f);

    /*
     * Synchronisation de l'historique avec
     * la nouvelle température de test.
     */
    history_add(
        17.60f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    thermostat_update();

    status =
        thermostat_get_status();

    ASSERT_TRUE(
        status->heating_request);

    ASSERT_TRUE(
        relay_get());

    printf(
        "Normal request     : ON\n");

    printf(
        "PASS\n");

    /*
     * --------------------------------------------------
     * RESUME
     * --------------------------------------------------
     */

    printf(
        "\nPASS : Thermostat prediction transition\n");

    return true;
}
