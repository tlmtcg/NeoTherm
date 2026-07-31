#include "app.h"

#include <stdlib.h>
#include <stdio.h>

#include "system_init.h"
#include "test_runner.h"
#include "event.h"
#include "scheduler.h"

#ifdef _WIN32
#include <windows.h>
#endif

bool app_init(void)
{
    if (!system_init())
    {
        return false;
    }

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
