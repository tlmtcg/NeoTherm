#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "test_utils.h"

#include "test_clock.h"
#include "test_scheduler.h"
#include "test_program.h"
#include "test_history.h"
#include "test_storage.h"
#include "test_thermostat.h"
#include "test_climate.h"
#include "test_relay.h"
#include "test_runtime.h"

typedef bool (*test_func_t)(void);

typedef struct
{
    const char *name;
    test_func_t function;

} test_entry_t;

static test_entry_t tests[] =
    {
        {"Clock", test_clock_run},
        {"Schedule", test_scheduler_run},
        {"Program", test_program_run},
        {"History", test_history_run},
        {"Storage", test_storage_run},
        {"Thermostat", test_thermostat_run},
        {"Climate", test_climate_run},
        {"Relay", test_relay_run},
        {"Runtime", test_runtime_run},

        /*
         * Tests à venir
         */

        // {"Thermal", test_thermal_run},
        // {"Event",   test_event_run},
        // {"Config",  test_config_run},
        // {"Logger",  test_logger_run},
};

void test_runner_run(void)
{
    printf("\n==============================\n");
    printf("NeoTherm Test Suite\n");
    printf("==============================\n\n");

    g_test_count = 0;
    g_test_passed = 0;

    uint32_t count =
        sizeof(tests) / sizeof(tests[0]);

    for (uint32_t i = 0; i < count; i++)
    {
        printf("\n");

        bool result =
            tests[i].function();

        if (result)
        {
            g_test_passed++;
        }

        g_test_count++;

        printf("%-20s %s\n",
               tests[i].name,
               result ? "PASS" : "FAIL");
    }

    printf("\n==============================\n");

    printf("%u / %u tests passed\n",
           g_test_passed,
           g_test_count);

    printf("==============================\n\n");
}
