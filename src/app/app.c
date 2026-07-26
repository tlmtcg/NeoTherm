#include "app.h"

#include <stdlib.h>

#include "logger.h"
#include "config.h"
#include "debug.h"
#include "event.h"
#include "climate.h"
#include "scheduler.h"
#include "scheduler_debug.h"
#include "relay.h"
#include "thermostat.h"
#include "storage.h"
// #include "thermostat_test.h"

#include "event.h"
#include "scheduler.h"
#include "logger.h"

#ifdef _WIN32
#include <windows.h>
#endif

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
     * Relay
     */
    relay_init();

    /*
     * Storage
     */
    storage_init();

    /*
     * Thermostat
     */
    thermostat_init();

    /*
     * Scheduler
     */
    scheduler_init();

    thermostat_set_mode(THERMOSTAT_AUTO);

    if (!scheduler_register(
            "Climate",
            climate_update,
            10))
    {
        LOG_ERROR("SCHED",
                  "Unable to register Climate task.");

        return false;
    }

    scheduler_register(
        "Thermostat",
        thermostat_update,
        10);

    LOG_INFO("APP",
             "%u task(s) registered",
             scheduler_task_count());

    scheduler_dump();

    // LOG_INFO("TEST",
    //          "Relay state = %s",
    //          relay_get() ? "ON" : "OFF");

    LOG_INFO("APP",
             "Application initialized.");

    // thermostat_test_run();

    thermostat_set_mode(
        THERMOSTAT_MANUAL);

    thermostat_set_setpoint(22.0f);

    return true;
}

void app_run(void)
{
    LOG_INFO("APP",
             "Application running.");

    while (1)
    {
        /*
         * Exécution scheduler
         */

        scheduler_update();

        /*
         * Traitement événements
         */

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

            case EVENT_RELAY_ON:

                LOG_INFO("APP",
                         "Relay event ON");

                break;

            case EVENT_RELAY_OFF:

                LOG_INFO("APP",
                         "Relay event OFF");

                break;

            default:

                break;
            }
        }

        /*
         * Simulation 100 ms
         */

#ifdef _WIN32
        Sleep(100);
#endif
    }
}

// void app_run(void)
// {
//     /*
//      * Pour l'instant :
//      * boucle de test simple
//      */

//     for (int i = 0; i < 100; i++)
//     {
//         scheduler_update();
//     }

//     event_t event;

//     while (event_get(&event))
//     {

//         switch (event.type)
//         {

//         case EVENT_CLIMATE_UPDATE:

//             LOG_INFO("APP",
//                      "Temperature event %.1f C",
//                      event.data.temperature);

//             break;

//         case EVENT_RELAY_ON:

//             LOG_INFO("APP",
//                      "Relay event : ON");

//             break;

//         case EVENT_RELAY_OFF:

//             LOG_INFO("APP",
//                      "Relay event : OFF");

//             break;

//         default:

//             LOG_WARN("APP",
//                      "Unhandled event %d",
//                      event.type);

//             break;
//         }
//     }

//     LOG_INFO("APP",
//              "Application ready.");
// }
