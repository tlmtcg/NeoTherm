#include "clock.h"

#include <stdio.h>
#include <time.h>

#include "logger.h"
#include "runtime.h"

static clock_time_t s_time;

/*
 * Initialisation
 */

void clock_init(void)
{
    const runtime_config_t *cfg = runtime_get();

    if (cfg == NULL)
    {
        LOG_ERROR("CLOCK",
                  "Runtime unavailable");

        return;
    }


    s_time = cfg->date_time;


    LOG_INFO("CLOCK",
             "Clock initialized : %04u-%02u-%02u %02u:%02u:%02u",
             s_time.year,
             s_time.month,
             s_time.day,
             s_time.hour,
             s_time.minute,
             s_time.second);
}

/*
 * Avance temps
 */
void clock_tick(uint32_t seconds)
{
    clock_add_seconds(seconds);
}

/*
 * Lecture
 */
bool clock_get_time(clock_time_t *time)
{
    if (time == NULL)
    {
        return false;
    }

    *time = s_time;

    return true;
}

bool clock_set_time(const clock_time_t *time)
{
    if (time == NULL)
    {
        return false;
    }

    /* Vérification minimale */
    if ((time->month < 1) || (time->month > 12) ||
        (time->day < 1) || (time->day > 31) ||
        (time->hour > 23) ||
        (time->minute > 59) ||
        (time->second > 59))
    {
        return false;
    }

    s_time = *time;

    LOG_INFO("CLOCK",
             "Time set to %04u-%02u-%02u %02u:%02u:%02u",
             s_time.year,
             s_time.month,
             s_time.day,
             s_time.hour,
             s_time.minute,
             s_time.second);

    return true;
}

/*
 * Debug
 */
void clock_dump(void)
{
    LOG_INFO("CLOCK",
             "%04u-%02u-%02u %02u:%02u:%02u",
             s_time.year,
             s_time.month,
             s_time.day,
             s_time.hour,
             s_time.minute,
             s_time.second);
}

void clock_add_seconds(uint32_t seconds)
{
    struct tm tm_time =
    {
        .tm_year = (int)s_time.year - 1900,
        .tm_mon  = (int)s_time.month - 1,
        .tm_mday = (int)s_time.day,
        .tm_hour = (int)s_time.hour,
        .tm_min  = (int)s_time.minute,
        .tm_sec  = (int)s_time.second,
        .tm_isdst = -1
    };

    time_t t = mktime(&tm_time);

    if (t == (time_t)-1)
    {
        return;
    }

    t += (time_t)seconds;

    struct tm *new_time = localtime(&t);

    if (new_time == NULL)
    {
        return;
    }

    s_time.year   = (uint32_t)(new_time->tm_year + 1900);
    s_time.month  = (uint32_t)(new_time->tm_mon + 1);
    s_time.day    = (uint32_t)new_time->tm_mday;
    s_time.hour   = (uint32_t)new_time->tm_hour;
    s_time.minute = (uint32_t)new_time->tm_min;
    s_time.second = (uint32_t)new_time->tm_sec;
}

void clock_add_second(void)
{
    clock_add_seconds(1);
}

uint32_t clock_seconds_today(void)
{
    clock_time_t now;

    if (!clock_get_time(&now))
    {
        return 0;
    }

    return (uint32_t)now.hour * 3600U +
           (uint32_t)now.minute * 60U +
           (uint32_t)now.second;
}

bool clock_sync_to_runtime(void)
{
    return runtime_set_datetime(&s_time);
}
