#include "test_thermal_learning.h"

#include <stdio.h>

#include "clock.h"
#include "history.h"
#include "../core/thermal_learning/thermal_learning.h"
#include "test_utils.h"
#include "../console/console_utils.h"

/*==========================================================
 * TEST : TRANSITION ON -> OFF
 *=========================================================*/

static bool test_transition_heating_to_off(void)
{
    console_print_header(
        "LEARNING TRANSITION ON -> OFF");

    history_clear();

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
     * Phase chauffage.
     *
     * 18.0 -> 18.2 -> 18.4
     *
     * Le chauffage est actif.
     */

    history_add(
        18.0f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.2f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.4f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Chauffage arrêté.
     *
     * 18.4 -> 18.6
     *
     * Cet intervalle correspond encore
     * à la fin de la phase de chauffage
     * car previous.heating == true.
     */

    clock_tick(60);

    history_add(
        18.6f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    /*
     * Refroidissement.
     *
     * 18.6 -> 18.5
     *
     * Cet intervalle doit être compté
     * comme refroidissement.
     */

    clock_tick(60);

    history_add(
        18.5f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    /*
     * Trois intervalles de chauffage :
     *
     * 18.0 -> 18.2
     * 18.2 -> 18.4
     * 18.4 -> 18.6
     *
     * Tous doivent être considérés comme
     * des intervalles de chauffage.
     */

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() >
        0.0f);

    /*
     * Un intervalle de refroidissement :
     *
     * 18.6 -> 18.5
     */

    ASSERT_TRUE(
        thermal_learning_get_cooling_rate() >
        0.0f);

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : TRANSITION ON -> OFF SANS REFROIDISSEMENT
 *=========================================================*/

static bool test_transition_does_not_create_cooling(void)
{
    console_print_header(
        "TRANSITION ON -> OFF WITHOUT COOLING");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 13,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * Chauffage actif.
     */

    history_add(
        18.0f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.2f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Arrêt du chauffage.
     *
     * La température continue simplement
     * à monter.
     */

    clock_tick(60);

    history_add(
        18.4f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    /*
     * Pas encore de refroidissement.
     *
     * La température monte encore.
     */

    clock_tick(60);

    history_add(
        18.6f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    /*
     * Le refroidissement ne doit pas être
     * artificiellement créé par la transition.
     */

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() >
        0.0f);

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : OVERSHOOT APRES TRANSITION ON -> OFF
 *=========================================================*/

static bool test_overshoot_after_heating_stop(void)
{
    console_print_header(
        "OVERSHOOT AFTER HEATING STOP");

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
     * Consigne :
     *
     * 18.0 C
     *
     * Chauffage actif.
     */

    history_add(
        17.8f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.0f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.2f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Chauffage arrêté.
     *
     * La température continue à monter.
     */

    clock_tick(60);

    history_add(
        18.4f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    clock_tick(60);

    history_add(
        18.6f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    clock_tick(60);

    history_add(
        18.5f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    printf(
        "Overshoot    : %.2f C\n",
        thermal_learning_get_overshoot());

    /*
     * Le maximum après arrêt est :
     *
     *     18.6 - 18.0 = 0.6 C
     */

    ASSERT_EQ_FLOAT(
        0.6f,
        thermal_learning_get_overshoot());

    printf("PASS\n");

    return true;
}


/*==========================================================
 * TEST : TRANSITION ON -> OFF SANS REFROIDISSEMENT
 *=========================================================*/

// static bool test_transition_does_not_create_cooling(void)
// {
//     console_print_header(
//         "TRANSITION ON -> OFF WITHOUT COOLING");

//     history_clear();

//     clock_time_t t =
//         {
//             .year = 2026,
//             .month = 8,
//             .day = 3,
//             .hour = 13,
//             .minute = 0,
//             .second = 0};

//     ASSERT_TRUE(
//         clock_set_time(&t));

//     clock_sync_to_runtime();

//     /*
//      * Chauffage actif.
//      */

//     history_add(
//         18.0f,
//         5.0f,
//         18.0f,
//         THERMOSTAT_AUTO,
//         true,
//         true);

//     clock_tick(60);

//     history_add(
//         18.2f,
//         5.0f,
//         18.0f,
//         THERMOSTAT_AUTO,
//         true,
//         true);

//     /*
//      * Arrêt du chauffage.
//      *
//      * La température continue simplement
//      * à monter.
//      */

//     clock_tick(60);

//     history_add(
//         18.4f,
//         5.0f,
//         18.0f,
//         THERMOSTAT_AUTO,
//         false,
//         false);

//     /*
//      * Pas encore de refroidissement.
//      *
//      * La température monte encore.
//      */

//     clock_tick(60);

//     history_add(
//         18.6f,
//         5.0f,
//         18.0f,
//         THERMOSTAT_AUTO,
//         false,
//         false);

//     ASSERT_TRUE(
//         thermal_learning_update());

//     printf(
//         "Heat rate    : %.4f C/min\n",
//         thermal_learning_get_heat_rate());

//     printf(
//         "Cooling rate : %.4f C/min\n",
//         thermal_learning_get_cooling_rate());

//     /*
//      * Le refroidissement ne doit pas être
//      * artificiellement créé par la transition.
//      */

//     ASSERT_EQ_FLOAT(
//         0.0f,
//         thermal_learning_get_cooling_rate());

//     ASSERT_TRUE(
//         thermal_learning_get_heat_rate() >
//         0.0f);

//     printf("PASS\n");

//     return true;
// }

bool test_thermal_learning_run(void)
{
    printf("\n=============== THERMAL LEARNING TEST ===============\n");

    ASSERT_TRUE(history_init());

    ASSERT_TRUE(thermal_learning_init());

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 12,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * Chauffage + refroidissement
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

        temp += 0.2f;
    }

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

        temp -= 0.1f;
    }

    ASSERT_TRUE(
        thermal_learning_update());

    printf("Heat rate      : %.4f C/tick\n",
           thermal_learning_get_heat_rate());

    printf("Cooling rate   : %.4f C/tick\n",
           thermal_learning_get_cooling_rate());

    printf("Overshoot      : %.2f C\n",
           thermal_learning_get_overshoot());

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    ASSERT_TRUE(
        thermal_learning_get_cooling_rate() > 0.0f);

    /*
     * Chauffage uniquement
     */

    history_clear();

    t.hour = 13;

    ASSERT_TRUE(clock_set_time(&t));

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

    printf(
        "Heating only : Heat=%.4f Cool=%.4f Overshoot=%.2f\n",
        thermal_learning_get_heat_rate(),
        thermal_learning_get_cooling_rate(),
        thermal_learning_get_overshoot());

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_overshoot());

    /*
     * Refroidissement uniquement
     */

    history_clear();

    t.hour = 14;

    ASSERT_TRUE(clock_set_time(&t));

    clock_sync_to_runtime();

    temp = 22.0f;

    for (int i = 0; i < 30; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            false,
            false);

        temp -= 0.12f;
    }

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Cooling only : Heat=%.4f Cool=%.4f Overshoot=%.2f\n",
        thermal_learning_get_heat_rate(),
        thermal_learning_get_cooling_rate(),
        thermal_learning_get_overshoot());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_heat_rate());

    ASSERT_TRUE(
        thermal_learning_get_cooling_rate() > 0.0f);

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_overshoot());

    ASSERT_TRUE(
        test_transition_heating_to_off());

    ASSERT_TRUE(
        test_transition_does_not_create_cooling());

    ASSERT_TRUE(
        test_overshoot_after_heating_stop());

    printf("PASS : Thermal learning\n");

    return true;
}
