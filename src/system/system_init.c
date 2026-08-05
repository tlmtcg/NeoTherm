#include "system_init.h"

#include <stdio.h>

#include "app_config.h"
#include "climate.h"
#include "clock.h"
#include "debug.h"
#include "event.h"
#include "event_dispatcher.h"
#include "history.h"
#include "logger.h"
#include "program.h"
#include "relay.h"
#include "runtime.h"
#include "scheduler.h"
#include "storage.h"
#include "thermal_model.h"
#include "thermostat.h"
#include "alarm.h"
#include "alarm_runtime.h"
#include "console.h"
#include "../core/weather/weather.h"
#include "../services/weather_service/weather_service.h"
#include "../services/storage_service/storage_service.h"
#include "../services/time_service/time_service.h"

bool system_init(void)
{
    logger_init();

    LOG_INFO("APP",
             "Application starting...");

    debug_init();

    if (!app_config_init("../app.ini"))
    {
        LOG_ERROR("CONFIG",
                  "Unable to load configuration.");

        return false;
    }

#ifdef DEBUG
    debug_dump_app_config(app_config_get());
#endif

    runtime_init();

    clock_init();

    clock_sync_from_system(); // mise à l'heure par le PC

    clock_sync_to_runtime();

    event_init();
    event_dispatcher_init();

    alarm_init();
    alarm_runtime_init();

    console_init();

    thermal_model_init();
    climate_init();
    relay_init();
    history_init();
    if (!storage_init())
    {
        alarm_set(ALARM_STORAGE, 0);

        LOG_ERROR("SYSTEM", "Alarm storage error");

        return false;
    }
    storage_service_init();
    time_service_init();
    program_init();

    weather_init();
    weather_service_init();

    thermostat_init();

    scheduler_init();

    const app_config_t *cfg = app_config_get();

    if (!scheduler_register(
            "Climate",
            climate_tick,
            cfg->climate_period))
    {
        LOG_ERROR("SCHED",
                  "Unable to register Climate task.");

        return false;
    }

    if (!scheduler_register(
            "Thermostat",
            thermostat_update,
            cfg->thermostat_period))
    {
        LOG_ERROR("SCHED",
                  "Unable to register Thermostat task.");

        return false;
    }

    if (!scheduler_register(
            "HistorySave",
            history_task_callback,
            cfg->history_save_period))
    {
        LOG_ERROR("SCHED",
                  "Unable to register History Save task.");

        return false;
    }

    if (!scheduler_register(
            "HistoryToCav",
            history_csv_task_callback,
            3600))
    {
        LOG_ERROR("SCHED",
                  "Unable to register History Csv Save task.");

        return false;
    }

    if (!scheduler_register(
            "WeatherService",
            weather_service_tick,
            1))
    {
        LOG_ERROR("SCHED",
                  "Unable to register Thermostat task.");

        return false;
    }

    LOG_INFO("APP",
             "%u task(s) registered",
             scheduler_task_count());

#ifdef UNIT_TEST
    debug_dump_app_config(app_config_get());
#endif

    LOG_INFO("APP",
             "Application initialized.");

    return true;
}
