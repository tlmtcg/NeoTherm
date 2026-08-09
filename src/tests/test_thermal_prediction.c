#include "test_thermal_prediction.h"

#include <stdio.h>

#include "clock.h"
#include "history.h"
#include "../thermal_learning/thermal_learning.h"
#include "../thermal_prediction/thermal_prediction.h"
#include "thermostat.h"
#include "test_utils.h"
#include "../console/console_utils.h"


bool test_thermal_prediction_run(void)
{
    printf(
        "\n=============== THERMAL PREDICTION TEST ===============\n");

    /*
     * ======================================================
     * INITIALISATION
     * ======================================================
     */

    ASSERT_TRUE(
        history_init());

    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_TRUE(
        thermal_prediction_init());

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
     * ======================================================
     * PHASE CHAUFFAGE
     * ======================================================
     *
     * +0.20 °C / minute
     */

    float temp = 18.0f;

    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            true,
            true);

        temp += 0.20f;
    }

    /*
     * ======================================================
     * PHASE REFROIDISSEMENT
     * ======================================================
     *
     * -0.10 °C / minute
     */

    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            false,
            false);

        temp -= 0.10f;
    }

    /*
     * ======================================================
     * ANALYSE LEARNING
     * ======================================================
     */

    ASSERT_TRUE(
        thermal_learning_update());

    ASSERT_TRUE(
        thermal_learning_is_valid());

    printf(
        "Heat rate      : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate   : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    /*
     * ======================================================
     * PREDICTION COMPLETE
     * ======================================================
     *
     * Les deux taux sont disponibles :
     *
     *     heat_rate    > 0
     *     cooling_rate > 0
     *
     * La prédiction complète doit donc être valide.
     */

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    /*
     * La dernière température enregistrée est
     * proche de 19.90 °C.
     */

    float current =
        thermal_prediction_get_temperature_minutes(
            0.0f);

    float prediction_1 =
        thermal_prediction_get_temperature_minutes(
            1.0f);

    float prediction_5 =
        thermal_prediction_get_temperature_minutes(
            5.0f);

    printf(
        "Current         : %.4f C\n",
        current);

    printf(
        "Prediction +1   : %.4f C\n",
        prediction_1);

    printf(
        "Prediction +5   : %.4f C\n",
        prediction_5);

    /*
     * ======================================================
     * VERIFICATION REFROIDISSEMENT
     * ======================================================
     */

    ASSERT_TRUE(
        current > 19.0f);

    ASSERT_TRUE(
        current < 21.0f);

    ASSERT_TRUE(
        prediction_1 < current);

    ASSERT_TRUE(
        prediction_5 < prediction_1);

    /*
     * ======================================================
     * TEST CHAUFFAGE UNIQUEMENT
     * ======================================================
     *
     * Le chauffage est appris mais aucun refroidissement
     * n'est disponible.
     *
     * La prédiction complète doit donc rester invalide.
     */

    console_print_header(
        "INCOMPLETE LEARNING");

    history_clear();

    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_TRUE(
        thermal_prediction_init());

    t.hour = 13;

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    temp = 18.0f;

    for (int i = 0; i < 30; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            true,
            true);

        temp += 0.15f;
    }

    ASSERT_TRUE(
        thermal_learning_update());

    /*
     * Le chauffage est appris.
     */

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    /*
     * Aucun refroidissement n'a été appris.
     */

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    /*
     * La prédiction complète doit être refusée,
     * car natural_10min nécessite cooling_rate.
     */

    ASSERT_FALSE(
        thermal_prediction_update());

    ASSERT_FALSE(
        thermal_prediction_is_valid());

    printf(
        "Heat only      : prediction invalid\n");

    /*
     * ======================================================
     * TEST LEARNING INVALIDE
     * ======================================================
     */

    console_print_header(
        "INVALID LEARNING");

    history_clear();

    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_TRUE(
        thermal_prediction_init());

    ASSERT_FALSE(
        thermal_learning_is_valid());

    ASSERT_FALSE(
        thermal_prediction_update());

    ASSERT_FALSE(
        thermal_prediction_is_valid());

    printf(
        "Invalid learning : prediction disabled\n");

    /*
     * ======================================================
     * RESULTAT
     * ======================================================
     */

    printf(
        "PASS : Thermal prediction\n");

    return true;
}

