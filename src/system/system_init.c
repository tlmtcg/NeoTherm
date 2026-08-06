#include "system_init.h"

#include "alarm.h"
#include "alarm_runtime.h"
#include "app_config.h"
#include "climate.h"
#include "clock.h"
#include "console.h"
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
#include "weather.h"

#include "storage_service.h"
#include "time_service.h"
#include "weather_service.h"
#include "../services/alarm_service/alarm_service.h"
#include "alarm_history.h"
#include "alarm_storage.h"
#include "alarm_history_task.h"

bool system_init(void)
{
    logger_init();

    LOG_INFO("SYSTEM",
             "System initialization");

    /*
     * Configuration
     */

    debug_init();

    if (!app_config_init("../app.ini"))
    {
        LOG_ERROR("CONFIG",
                  "Unable to load configuration");

        return false;
    }

    const app_config_t *cfg =
        app_config_get();

    if (cfg->debug_enabled)
    {
        debug_dump_app_config(cfg);
    }

    /*
     * Runtime
     */

    if (!runtime_init())
    {
        LOG_ERROR("SYSTEM",
                  "Runtime initialization failed");

        return false;
    }

    clock_init();
    clock_sync_from_system();
    clock_sync_to_runtime();

    /*
     * Infrastructure
     */

    event_init();
    event_dispatcher_init();

    alarm_service_init();
    alarm_storage_init();
    alarm_history_init();
    alarm_history_load();
    alarm_runtime_init();

    console_init();

    /*
     * Core
     */

    thermal_model_init();
    climate_init();
    relay_init();
    history_init();
    program_init();

    weather_init();

    thermostat_init();

    /*
     * Storage
     */

    if (!storage_init())
    {
        alarm_set(ALARM_STORAGE, 0);

        LOG_ERROR("SYSTEM",
                  "Storage initialization failed");

        return false;
    }

    storage_service_init();

    /*
     * Services
     */

    time_service_init();

    if (!weather_service_init())
    {
        LOG_ERROR("SYSTEM",
                  "Weather service initialization failed");

        return false;
    }

    /*
     * Scheduler
     */

    scheduler_init();

    if (!scheduler_register(
            "Climate",
            climate_tick,
            cfg->climate_period))
    {
        LOG_ERROR("SCHED",
                  "Unable to register Climate");

        return false;
    }

    if (!scheduler_register(
            "Thermostat",
            thermostat_update,
            cfg->thermostat_period))
    {
        LOG_ERROR("SCHED",
                  "Unable to register Thermostat");

        return false;
    }

    /* Vérifie chaque seconde si une mise à jour météo est nécessaire */
    if (!scheduler_register(
            "WeatherService",
            weather_service_tick,
            1))
    {
        LOG_ERROR("SCHED",
                  "Unable to register WeatherService");

        return false;
    }

    if (!scheduler_register(
            "HistorySave",
            history_task_callback,
            cfg->history_save_period))
    {
        LOG_ERROR("SCHED",
                  "Unable to register HistorySave");

        return false;
    }

    if (!scheduler_register(
            "HistoryCsv",
            history_csv_task_callback,
            3600))
    {
        LOG_ERROR("SCHED",
                  "Unable to register HistoryCsv");

        return false;
    }

     if (!scheduler_register(
            "HistorySave",
            history_task_callback,
            cfg->history_save_period))
    {
        LOG_ERROR("SCHED",
                  "Unable to register HistorySave");

        return false;
    }


    if (!scheduler_register(
            "HistoryCsv",
            history_csv_task_callback,
            3600))
    {
        LOG_ERROR("SCHED",
                  "Unable to register HistoryCsv");

        return false;
    }


    if (!scheduler_register(
            "AlarmHistorySave",
            alarm_history_task,
            3600))
    {
        LOG_ERROR("SCHED",
                  "Unable to register AlarmHistorySave");

        return false;
    }

    LOG_INFO("SYSTEM",
             "%u task(s) registered",
             scheduler_task_count());

    LOG_INFO("SYSTEM",
             "System initialized");

    return true;
}
