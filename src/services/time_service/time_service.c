#include "time_service.h"

#include <stdio.h>

#include "clock.h"
#include "console_utils.h"
#include "logger.h"

static bool s_available = false;

/*
 *==========================================================
 * Initialisation
 *==========================================================
 */

bool time_service_init(void)
{
    s_available = false;

    LOG_INFO("TIME_SERVICE",
             "Time service initialized");

    return true;
}

/*
 *==========================================================
 * Synchronisation
 *==========================================================
 */

bool time_service_sync(void)
{
    /*
     * Aujourd'hui :
     * heure du PC.
     *
     * Demain :
     * NTP + RTC.
     */

    if (!clock_sync_from_system())
    {
        LOG_ERROR("TIME_SERVICE",
                  "Unable to synchronize clock");

        s_available = false;

        return false;
    }

    s_available = true;

    LOG_INFO("TIME_SERVICE",
             "Clock synchronized");

    return true;
}

/*
 *==========================================================
 * Etat
 *==========================================================
 */

bool time_service_is_available(void)
{
    return s_available;
}

/*
 *==========================================================
 * Debug
 *==========================================================
 */

void time_service_dump(void)
{
    console_print_header("Time service");

    printf("Available    : %s\n",
           s_available ? "YES" : "NO");

    console_print_separator();
}
