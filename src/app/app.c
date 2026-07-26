#include "app.h"

#include <stdlib.h>

#include "logger.h"
#include "config.h"
#include "debug.h"
#include "event.h"
#include "climate.h"
#include "scheduler.h"
#include "scheduler_debug.h"

bool app_init(void)
{
    /*
     * Logger
     */
    logger_init();

    LOG_INFO("APP",
             "Application starting...");

    /*
     * Debug
     */
    debug_init();

    /*
     * Configuration
     */
    if (!config_init("../config.ini"))
    {
        LOG_ERROR("CONFIG",
                  "Unable to load configuration.");

        return false;
    }

#ifdef DEBUG
    debug_dump_config(config_get_runtime());
#endif

    /*
     * Event system
     */
    event_init();

    /*
     * Climate
     */
    climate_init();

    /*
     * Scheduler
     */
    scheduler_init();

    if (!scheduler_register(
            "Climate",
            climate_update,
            10))
    {
        LOG_ERROR("SCHED",
                  "Unable to register Climate task.");

        return false;
    }

    LOG_INFO("APP",
         "%u task(s) registered",
         scheduler_task_count());

scheduler_dump();

scheduler_disable("Climate");

scheduler_dump();

scheduler_enable("Climate");

scheduler_dump();

    LOG_INFO("APP",
             "Application initialized.");

    return true;
}

void app_run(void)
{
    /*
     * Pour l'instant :
     * boucle de test simple
     */

    for (int i = 0; i < 50; i++)
    {
        scheduler_update();
    }

    event_t event;

    while (event_get(&event))
    {

        switch (event.type)
        {

        case EVENT_CLIMATE_UPDATE:

            LOG_INFO("APP",
                     "Temperature event %.1f C",
                     event.data.temperature);

            break;

        default:

            LOG_WARN("APP",
                     "Unhandled event %d",
                     event.type);

            break;
        }
    }

    LOG_INFO("APP",
             "Application ready.");
}
