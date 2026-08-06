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
#include "test_app_config.h"
#include "test_ini.h"
#include "test_event.h"
#include "test_logger.h"
#include "test_thermal_model.h"
#include "test_alarm.h"
#include "test_alarm_runtime.h"
#include "test_trim.h"
#include "test_weather.h"
#include "test_weather_service.h"
#include "test_time_service.h"
#include "test_storage_service.h"
#include "test_weather_provider_openmeteo.h"
#include "test_http_client.h"
#include "test_runtime_weather.h"
#include "test_weather_service_period.h"
#include "test_scheduler_weather.h"
#include "test_alarm_service.h"
#include "test_alarm_event.h"
#include "test_alarm_history.h"
#include "test_alarm_storage.h"
#include "test_cmd_alarm_history.h"

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
        {"Config", test_app_config_run},
        {"Ini", test_ini_run},
        {"Event", test_event_run},
        {"Logger", test_logger_run},
        {"Thermal", test_thermal_model_run},
        {"Alarmes", test_alarm_run},
        {"AlarmRuntime", test_alarm_runtime_run},
        {"Trim", test_trim_run},
        {"Weather", test_weather_run},
        {"Weather_service", test_weather_service_run},
        {"Time_service", test_time_service_run},
        {"Storage_service", test_storage_service_run},
        {"Weather_provider_openmeteo", test_weather_provider_openmeteo_run},
        {"HTTP client", test_http_client_run},
        {"Runtime_weather", test_runtime_weather_run},
        {"Weather_period", test_weather_service_period_run},
        {"Scheduler_weather", test_scheduler_weather_run},
        {"Alarm_service", test_alarm_service_run},
        {"Alarm_event", test_alarm_event_run},
        {"Alarm_history", test_alarm_history_run},
        {"Alarm_storage", test_alarm_storage_run},
        {"Alarm_history_storage", test_cmd_alarm_history_run},
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
