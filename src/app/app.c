#include "app.h"

#include <stdlib.h>
#include <stdio.h>

#include "logger.h"
#include "config.h"
#include "debug.h"
#include "event.h"
#include "thermal_model.h"
#include "climate.h"
#include "scheduler.h"
#include "scheduler_debug.h"
#include "relay.h"
#include "thermostat.h"
#include "storage.h"
#include "history_debug.h"
#include "history.h"
#include "program.h"
#include "test_runner.h"

#ifdef _WIN32
#include <windows.h>
#endif

bool app_init(void)
{
    logger_init();

    LOG_INFO("APP",
             "Application starting...");

    debug_init();

    if (!config_init("../config.ini"))
    {
        LOG_ERROR("CONFIG",
                  "Unable to load configuration.");

        return false;
    }

#ifdef DEBUG
    debug_dump_config(config_get_runtime());
#endif

    event_init();
    thermal_model_init();
    climate_init();
    relay_init();
    history_init();
    storage_init();
    program_init();
    thermostat_init();

    scheduler_init();

    if (!scheduler_register(
            "Climate",
            climate_tick,
            10))
    {
        LOG_ERROR("SCHED",
                  "Unable to register Climate task.");
        return false;
    }

    if (!scheduler_register(
            "Thermostat",
            thermostat_update,
            10))
    {
        LOG_ERROR("SCHED",
                  "Unable to register Thermostat task.");
        return false;
    }

    LOG_INFO("APP",
             "%u task(s) registered",
             scheduler_task_count());

#ifdef DEBUG
    scheduler_dump();
#endif

    LOG_INFO("APP",
             "Application initialized.");

    return true;
}

/*
 *==========================================================
 * Traitement des événements
 *==========================================================
 */

static void app_process_events(void)
{
    event_t event;

    while (event_get(&event))
    {
        switch (event.type)
        {
        case EVENT_CLIMATE_UPDATE:

            LOG_INFO("APP",
                     "Temperature event %.2f C",
                     event.data.temperature);
            break;

        default:
            break;
        }
    }
}

/*
 *==========================================================
 * Boucle principale
 *==========================================================
 */

bool app_run(void)
{
#ifdef TEST_MODE

    test_runner_run();

#else

    while (1)
    {
        scheduler_update();

        app_process_events();

#ifdef _WIN32
        Sleep(100);
#endif
    }

#endif

    return true;
}
