#include "app.h"

#include <stdlib.h>
#include <stdio.h>

#include "system_init.h"
#ifdef UNIT_TEST
#include "../tests/test_runner.h"
#endif
#include "event.h"
#include "scheduler.h"
#include "clock.h"
#include "event_dispatcher.h"
#include "console.h"
#include "webserver.h"

#ifdef _WIN32
#include <windows.h>
#endif

static bool s_exit_requested = false;

bool app_request_exit(void)
{
    s_exit_requested = true;

    return true;
}

bool app_exit_requested(void)
{
    return s_exit_requested;
}

bool app_init(void)
{
    LOG_INFO(
        "APP",
        "Starting NeoTherm");

    return system_init();
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
        event_dispatcher_dispatch(
            &event);
    }
}

/*
 *==========================================================
 * Boucle principale
 *==========================================================
 */

bool app_run(void)
{
#ifdef UNIT_TEST
    test_runner_run();
#endif

    /*
     * Démarrage du serveur Web.
     */
    if (!webserver_start())
    {
        LOG_ERROR(
            "APP",
            "Unable to start web server");

        return false;
    }

    while (!app_exit_requested())
    {
        scheduler_update();

        app_process_events();

        console_update();

        webserver_update();

        clock_add_second();

#ifdef _WIN32
        Sleep(100);
#endif
    }

    webserver_stop();

    LOG_INFO(
        "APP",
        "Application stopped");

    return true;
}
