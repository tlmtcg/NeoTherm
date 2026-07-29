#include "schedule.h"

#include "logger.h"

#define DEFAULT_SETPOINT 17.0f

/*
 * Planning hebdomadaire
 *
 * day :
 * 0 = Lundi
 * 1 = Mardi
 * 2 = Mercredi
 * 3 = Jeudi
 * 4 = Vendredi
 * 5 = Samedi
 * 6 = Dimanche
 */

static uint8_t schedule_get_weekday(
    const clock_time_t *time)
{
    static const uint8_t month_table[] =
        {
            0, 3, 2, 5, 0, 3,
            5, 1, 4, 6, 2, 4};

    uint32_t year = time->year;

    if (time->month < 3)
    {
        year--;
    }

    uint32_t weekday =
        (year +
         year / 4 -
         year / 100 +
         year / 400 +
         month_table[time->month - 1] +
         time->day) %
        7;

    /*
     * Algorithme :
     * 0 = Dimanche
     * 1 = Lundi
     * ...
     * 6 = Samedi
     *
     * NeoTherm :
     * 0 = Lundi
     * ...
     * 6 = Dimanche
     */

    if (weekday == 0)
    {
        return 6;
    }

    return (uint8_t)(weekday - 1);
}

static schedule_day_t s_schedule[7] =
    {
        /*==================== Lundi ====================*/
        {
            .day = 0,
            .count = 5,
            .points =
                {
                    {0, 0, 17.0f},
                    {6, 0, 21.0f},
                    {8, 30, 18.0f},
                    {17, 30, 21.0f},
                    {22, 0, 17.0f}}},

        /*==================== Mardi ====================*/
        {
            .day = 1,
            .count = 5,
            .points =
                {
                    {0, 0, 17.0f},
                    {6, 0, 21.0f},
                    {8, 30, 18.0f},
                    {17, 30, 21.0f},
                    {22, 0, 17.0f}}},

        /*==================== Mercredi ====================*/
        {
            .day = 2,
            .count = 5,
            .points =
                {
                    {0, 0, 17.0f},
                    {6, 0, 21.0f},
                    {8, 30, 18.0f},
                    {17, 30, 21.0f},
                    {22, 0, 17.0f}}},

        /*==================== Jeudi ====================*/
        {
            .day = 3,
            .count = 5,
            .points =
                {
                    {0, 0, 17.0f},
                    {6, 0, 21.0f},
                    {8, 30, 18.0f},
                    {17, 30, 21.0f},
                    {22, 0, 17.0f}}},

        /*==================== Vendredi ====================*/
        {
            .day = 4,
            .count = 5,
            .points =
                {
                    {0, 0, 17.0f},
                    {6, 0, 21.0f},
                    {8, 30, 18.0f},
                    {17, 30, 21.0f},
                    {22, 0, 17.0f}}},

        /*==================== Samedi ====================*/
        {
            .day = 5,
            .count = 3,
            .points =
                {
                    {0, 0, 17.0f},
                    {7, 30, 21.0f},
                    {23, 0, 17.0f}}},

        /*==================== Dimanche ====================*/
        {
            .day = 6,
            .count = 3,
            .points =
                {
                    {0, 0, 18.0f},
                    {7, 30, 22.0f},
                    {23, 0, 18.0f}}}};

/*==========================================================
 * Initialisation
 *=========================================================*/

bool schedule_init(void)
{
    LOG_INFO("SCHEDULE",
             "Weekly schedule initialized");

    return true;
}

/*==========================================================
 * Retourne la consigne programmée
 *=========================================================*/

float schedule_get_setpoint(void)
{
    clock_time_t now;

    if (!clock_get_time(&now))
    {
        LOG_ERROR("SCHEDULE",
                  "Unable to read clock");

        return DEFAULT_SETPOINT;
    }

    /*
     * Conversion heure -> minutes
     */

    uint32_t current_minutes =
        (uint32_t)now.hour * 60U +
        (uint32_t)now.minute;

    /*

     * On calcule automatiquement
     * le jour de la semaine.
     */

    uint8_t weekday =
        schedule_get_weekday(&now);

    schedule_day_t *day =
        &s_schedule[weekday];

    float setpoint =
        DEFAULT_SETPOINT;

    for (uint32_t i = 0;
         i < day->count;
         i++)
    {
        uint32_t event_minutes =
            (uint32_t)day->points[i].hour * 60U +
            (uint32_t)day->points[i].minute;

        if (current_minutes >= event_minutes)
        {
            setpoint =
                day->points[i].setpoint;
        }
        else
        {
            break;
        }
    }

    LOG_INFO("SCHEDULE",
             "Current schedule %.1f C Jour = %d",
             setpoint,
             weekday
            );

    return setpoint;
}
