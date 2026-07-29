#include "clock.h"

#include <stdio.h>

#include "logger.h"

static clock_time_t s_time;

/*
 * Initialisation
 */
void clock_init(void)
{
    s_time.year = 2026;
    s_time.month = 1;
    s_time.day = 1;

    s_time.hour = 0;
    s_time.minute = 0;
    s_time.second = 0;

    LOG_INFO("CLOCK",
             "Clock initialized");
}

/*
 * Avance temps
 */
void clock_tick(
    uint32_t seconds)
{
    s_time.second += seconds;

    while (s_time.second >= 60)
    {
        s_time.second -= 60;
        s_time.minute++;
    }

    while (s_time.minute >= 60)
    {
        s_time.minute -= 60;
        s_time.hour++;
    }

    while (s_time.hour >= 24)
    {
        s_time.hour -= 24;
        s_time.day++;
    }

    /*
     * Simplification temporaire :
     * mois de 30 jours
     */
    if (s_time.day > 30)
    {
        s_time.day = 1;
        s_time.month++;
    }

    if (s_time.month > 12)
    {
        s_time.month = 1;
        s_time.year++;
    }
}

/*
 * Lecture
 */
bool clock_get_time(
    clock_time_t *time)
{
    if (time == NULL)
    {
        return false;
    }

    *time = s_time;

    return true;
}

bool clock_set_time(
    const clock_time_t *time)
{
    if (time == NULL)
    {
        return false;
    }

    /* Vérification minimale */
    if ((time->month < 1) || (time->month > 12) ||
        (time->day < 1)   || (time->day > 31)   ||
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