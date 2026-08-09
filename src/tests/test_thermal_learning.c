#include "test_thermal_learning.h"

#include <stdio.h>

#include "clock.h"
#include "history.h"
#include "../core/thermal_learning/thermal_learning.h"
#include "test_utils.h"


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
        .second = 0
    };


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


    printf("PASS : Thermal learning\n");


    return true;
}
