#include "clock.h"

#include <stdio.h>
#include <time.h>

#include "logger.h"
#include "runtime.h"
#include "console_utils.h"

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
             "Current time : %04u-%02u-%02u %02u:%02u:%02u",
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

static bool clock_is_valid(const clock_time_t *t)
{
    return t &&
           t->month >= 1 && t->month <= 12 &&
           t->day >= 1 && t->day <= 31 &&
           t->hour <= 23 &&
           t->minute <= 59 &&
           t->second <= 59;
}

bool clock_set_time(const clock_time_t *time)
{
    if (time == NULL)
    {
        return false;
    }

    if (!clock_is_valid(time))
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
    console_print_header("Clock");
    console_print_datetime("Current time", &s_time);
    console_print_separator();
}

void clock_add_seconds(uint32_t seconds)
{
    struct tm tm_time =
        {
            .tm_year = (int)s_time.year - 1900,
            .tm_mon = (int)s_time.month - 1,
            .tm_mday = (int)s_time.day,
            .tm_hour = (int)s_time.hour,
            .tm_min = (int)s_time.minute,
            .tm_sec = (int)s_time.second,
            .tm_isdst = -1};

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

    s_time.year = (uint32_t)(new_time->tm_year + 1900);
    s_time.month = (uint32_t)(new_time->tm_mon + 1);
    s_time.day = (uint32_t)new_time->tm_mday;
    s_time.hour = (uint32_t)new_time->tm_hour;
    s_time.minute = (uint32_t)new_time->tm_min;
    s_time.second = (uint32_t)new_time->tm_sec;
}

void clock_add_second(void)
{
    clock_add_seconds(1);
}

uint32_t clock_seconds_today(void)
{
    return s_time.hour * 3600U +
           s_time.minute * 60U +
           s_time.second;
}

bool clock_sync_to_runtime(void)
{
    return runtime_set_datetime(&s_time);
}

bool clock_sync_from_system(void)
{
    time_t now = time(NULL);

    if (now == (time_t)-1)
    {
        LOG_ERROR("CLOCK",
                  "Unable to read system time");

        return false;
    }

    struct tm *system_time = localtime(&now);

    if (system_time == NULL)
    {
        LOG_ERROR("CLOCK",
                  "Unable to convert system time");

        return false;
    }

    clock_time_t clock =
        {
            .year = (uint32_t)(system_time->tm_year + 1900),
            .month = (uint32_t)(system_time->tm_mon + 1),
            .day = (uint32_t)system_time->tm_mday,
            .hour = (uint32_t)system_time->tm_hour,
            .minute = (uint32_t)system_time->tm_min,
            .second = (uint32_t)system_time->tm_sec};

    if (!clock_set_time(&clock))
    {
        return false;
    }

    LOG_INFO("CLOCK",
             "Clock synchronized from system");

    return true;
}

uint32_t clock_get_timestamp(void)
{
    struct tm tm_time =
        {
            .tm_year = (int)s_time.year - 1900,
            .tm_mon = (int)s_time.month - 1,
            .tm_mday = (int)s_time.day,
            .tm_hour = (int)s_time.hour,
            .tm_min = (int)s_time.minute,
            .tm_sec = (int)s_time.second,
            .tm_isdst = -1};

    time_t t = mktime(&tm_time);

    if (t == (time_t)-1)
    {
        return 0;
    }

    return (uint32_t)t;
}