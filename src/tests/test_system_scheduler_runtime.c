#include "test_system_scheduler_runtime.h"

#include <stdio.h>

#include "system_init.h"
#include "scheduler.h"
#include "test_utils.h"

bool test_system_scheduler_runtime_run(void)
{
    printf("\n=============== SYSTEM SCHEDULER RUNTIME TEST ===============\n");

    /*
     * Initialisation complète du système
     */
    ASSERT_TRUE(
        system_init());

    /*
     * Vérifie les tâches enregistrées
     */
    ASSERT_TRUE(
        scheduler_exists("Climate"));

    ASSERT_TRUE(
        scheduler_exists("Thermostat"));

    ASSERT_TRUE(
        scheduler_exists("WeatherService"));

    ASSERT_TRUE(
        scheduler_exists("HistorySave"));

    ASSERT_TRUE(
        scheduler_exists("HistoryCsv"));

    ASSERT_TRUE(
        scheduler_exists("AlarmHistorySave"));

    ASSERT_EQ_UINT32(
        6,
        scheduler_task_count());

    /*
     * Simulation longue
     */
    for (uint32_t i = 0; i < 10000; i++)
    {
        scheduler_update();
    }

    /*
     * Les tâches existent toujours
     */
    ASSERT_TRUE(
        scheduler_exists("Climate"));

    ASSERT_TRUE(
        scheduler_exists("Thermostat"));

    ASSERT_TRUE(
        scheduler_exists("WeatherService"));

    ASSERT_TRUE(
        scheduler_exists("HistorySave"));

    ASSERT_TRUE(
        scheduler_exists("HistoryCsv"));

    ASSERT_TRUE(
        scheduler_exists("AlarmHistorySave"));

    ASSERT_EQ_UINT32(
        6,
        scheduler_task_count());

    printf("SYSTEM SCHEDULER RUNTIME TEST PASS\n");

    return true;
}