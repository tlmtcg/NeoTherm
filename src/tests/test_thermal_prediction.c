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

    /*======================================================
     * INITIALISATION
     *=====================================================*/

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

    /*======================================================
     * TEST 1 : CHAUFFAGE + REFROIDISSEMENT
     *=====================================================*/

    console_print_header(
        "HEATING + COOLING");

    history_clear();

    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_TRUE(
        thermal_prediction_init());

    float temp = 18.0f;

    /*
     * Première mesure.
     */
    history_add(
        temp,
        5.0f,
        21.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Chauffage :
     *
     * +0.20 C/min
     */
    for (int i = 1; i < 20; i++)
    {
        clock_tick(60);

        temp += 0.20f;

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            true,
            true);
    }

    /*
     * Refroidissement :
     *
     * -0.10 C/min
     */
    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        temp -= 0.10f;

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            false,
            false);
    }

    /*======================================================
     * LEARNING
     *=====================================================*/

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

    printf(
        "Warming rate   : %.4f C/min\n",
        thermal_learning_get_warming_rate());

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    ASSERT_TRUE(
        thermal_learning_get_cooling_rate() > 0.0f);

    /*======================================================
     * PREDICTION
     *=====================================================*/

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    float current =
        thermal_prediction_get_temperature_minutes(
            0.0f);

    float natural_1 =
        thermal_prediction_get_temperature_minutes_state(
            1.0f,
            false);

    float natural_5 =
        thermal_prediction_get_temperature_minutes_state(
            5.0f,
            false);

    float heated_1 =
        thermal_prediction_get_temperature_minutes_state(
            1.0f,
            true);

    float heated_5 =
        thermal_prediction_get_temperature_minutes_state(
            5.0f,
            true);

    printf(
        "Current         : %.4f C\n",
        current);

    printf(
        "Natural +1 min  : %.4f C\n",
        natural_1);

    printf(
        "Natural +5 min  : %.4f C\n",
        natural_5);

    printf(
        "Heated +1 min   : %.4f C\n",
        heated_1);

    printf(
        "Heated +5 min   : %.4f C\n",
        heated_5);

    /*
     * La température actuelle doit être dans
     * la plage attendue.
     */
    ASSERT_TRUE(
        current > 19.0f);

    ASSERT_TRUE(
        current < 21.0f);

    /*
     * Le refroidissement doit faire baisser
     * la température.
     */
    ASSERT_TRUE(
        natural_1 < current);

    ASSERT_TRUE(
        natural_5 < natural_1);

    /*
     * Le chauffage doit faire monter
     * la température.
     */
    ASSERT_TRUE(
        heated_1 > current);

    ASSERT_TRUE(
        heated_5 > heated_1);

    /*======================================================
     * TEST 2 : CHAUFFAGE SEUL
     *=====================================================*/

    console_print_header(
        "HEATING ONLY");

    history_clear();

    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_TRUE(
        thermal_prediction_init());

    t.hour = 13;
    t.minute = 0;
    t.second = 0;

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    temp = 18.0f;

    /*
     * Première mesure.
     */
    history_add(
        temp,
        5.0f,
        21.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * +0.15 C/min
     */
    for (int i = 1; i < 30; i++)
    {
        clock_tick(60);

        temp += 0.15f;

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            true,
            true);
    }

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate      : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate   : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    printf(
        "Warming rate   : %.4f C/min\n",
        thermal_learning_get_warming_rate());

    ASSERT_TRUE(
        thermal_learning_is_valid());

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_warming_rate());

    /*
     * La prédiction globale reste valide grâce
     * au heat_rate.
     */
    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    /*
     * Prédiction avec chauffage.
     */
    float heating_prediction =
        thermal_prediction_get_heated_temperature_minutes(
            10.0f);

    printf(
        "Heat only      : +10 min = %.4f C\n",
        heating_prediction);

    ASSERT_TRUE(
        heating_prediction > temp);

    /*
     * Aucune information naturelle :
     *
     * cooling_rate = 0
     * warming_rate = 0
     *
     * La température courante doit donc
     * être conservée.
     */
    float natural_prediction =
        thermal_prediction_get_temperature_minutes_state(
            10.0f,
            false);

    ASSERT_EQ_FLOAT(
        temp,
        natural_prediction);

    /*======================================================
     * TEST 3 : WARMING SEUL
     *=====================================================*/

    console_print_header(
        "WARMING ONLY");

    history_clear();

    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_TRUE(
        thermal_prediction_init());

    t.hour = 14;
    t.minute = 0;
    t.second = 0;

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    temp = 18.0f;

    /*
     * Première mesure.
     */
    history_add(
        temp,
        26.0f,
        21.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    /*
     * Réchauffement naturel :
     *
     * +0.05 C/min
     */
    for (int i = 1; i < 30; i++)
    {
        clock_tick(60);

        temp += 0.05f;

        history_add(
            temp,
            26.0f,
            21.0f,
            THERMOSTAT_AUTO,
            false,
            false);
    }

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate      : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate   : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    printf(
        "Warming rate   : %.4f C/min\n",
        thermal_learning_get_warming_rate());

    /*
     * Aucun chauffage.
     */
    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_heat_rate());

    /*
     * Aucun refroidissement.
     */
    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    /*
     * Le warming doit être appris.
     */
    ASSERT_TRUE(
        thermal_learning_get_warming_rate() > 0.0f);

    /*
     * Le learning est valide grâce au warming.
     */
    ASSERT_TRUE(
        thermal_learning_is_valid());

    /*
     * La prédiction doit être valide.
     */
    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    float warming_prediction =
        thermal_prediction_get_temperature_minutes_state(
            10.0f,
            false);

    printf(
        "Warming only   : +10 min = %.4f C\n",
        warming_prediction);

    /*
     * La température doit augmenter.
     */
    ASSERT_TRUE(
        warming_prediction > temp);

    /*======================================================
     * TEST 4 : LEARNING INVALIDE
     *=====================================================*/

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

    /*======================================================
     * RESULTAT
     *=====================================================*/

    printf(
        "PASS : Thermal prediction\n");

    return true;
}
