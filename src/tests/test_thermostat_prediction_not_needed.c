#include "test_thermostat_prediction_not_needed.h"

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

bool test_thermostat_prediction_not_needed_run(void)
{
    printf(
        "\n=============== THERMOSTAT PREDICTION NOT NEEDED TEST ===============\n");

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
     * Chauffage      : +0.20 °C/min
     * Refroidissement: -0.10 °C/min
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
     * PREDICTION NOT NEEDED
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
     * Prévision naturelle :
     *
     *     17.90 - 1.00 = 16.90 °C
     *
     * ATTENTION :
     *
     * Ce scénario ne permettrait PAS de tester
     * "prediction not needed", car 16.90 °C est
     * inférieur à la consigne.
     *
     * On utilise donc une température de départ
     * suffisamment élevée pour que la prédiction
     * naturelle reste au-dessus de 18.00 °C.
     *
     * Exemple :
     *
     *     19.20 - 1.00 = 18.20 °C
     *
     * => le chauffage prédictif est inutile.
     */

    console_print_header(
        "PREDICTION NOT NEEDED");

    climate_test_set_temperature(
        19.20f);

    /*
     * La prédiction travaille sur la dernière
     * mesure de l'historique.
     *
     * On synchronise donc l'historique.
     */
    history_add(
        19.20f,
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

    /*
     * Aucune demande globale.
     */
    ASSERT_FALSE(
        status->heating_request);

    /*
     * Le relais doit rester OFF.
     */
    ASSERT_FALSE(
        relay_get());

    printf(
        "Prediction not needed : OFF\n");

    /*
     * --------------------------------------------------
     * RESUME
     * --------------------------------------------------
     */

    printf(
        "\nPASS : Thermostat prediction not needed\n");

    return true;
}

