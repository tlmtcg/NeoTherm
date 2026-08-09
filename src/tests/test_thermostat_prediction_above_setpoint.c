#include "test_thermostat_prediction_above_setpoint.h"

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

bool test_thermostat_prediction_above_setpoint_run(void)
{
    printf(
        "\n=============== THERMOSTAT PREDICTION ABOVE SETPOINT TEST ===============\n");

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
            .hour = 16,
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
     * TEMPERATURE ABOVE SETPOINT
     * --------------------------------------------------
     *
     * Consigne :
     *
     *     18.00 °C
     *
     * Température :
     *
     *     18.50 °C
     *
     * La température est au-dessus de la consigne.
     *
     * => aucune demande de chauffage.
     *
     * La prédiction reste cependant valide.
     */

    console_print_header(
        "TEMPERATURE ABOVE SETPOINT");

    climate_test_set_temperature(
        18.50f);

    /*
     * Synchronisation avec l'historique.
     */
    history_add(
        18.50f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    /*
     * La prédiction doit être valide.
     */
    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    /*
     * Aucune demande globale de chauffage.
     */
    ASSERT_FALSE(
        status->heating_request);

    /*
     * Le relais doit rester OFF.
     */
    ASSERT_FALSE(
        relay_get());

    printf(
        "Above setpoint : heating OFF\n");

    /*
     * --------------------------------------------------
     * RESUME
     * --------------------------------------------------
     */

    printf(
        "\nPASS : Thermostat prediction above setpoint\n");

    return true;
}

