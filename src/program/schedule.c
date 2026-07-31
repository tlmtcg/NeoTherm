#include "schedule.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ini.h"
#include "logger.h"
#include <app_config.h>

static void schedule_sort_day(schedule_day_t *day);

static schedule_day_t s_schedule[7];

static const char *s_schedule_file = NULL;

static const char *s_section_names[7] =
    {
        "lundi",
        "mardi",
        "mercredi",
        "jeudi",
        "vendredi",
        "samedi",
        "dimanche"};

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

    if (weekday == 0)
    {
        return 6;
    }

    return (uint8_t)(weekday - 1);
}

bool schedule_init(void)
{
    const app_config_t *cfg = app_config_get();

    s_schedule_file = cfg->schedule_file;

    LOG_INFO("SCHEDULE",
             "Loading schedule : %s",
             s_schedule_file);

    if (!ini_init(s_schedule_file))
    {
        LOG_ERROR("SCHEDULE",
                  "Unable to load schedule file : %s",
                  s_schedule_file);

        return false;
    }

    const ini_runtime_t *rt = ini_get_runtime();

    if (rt == NULL)
{
    LOG_ERROR("SCHEDULE",
              "INI runtime unavailable");
    return false;
}
    LOG_DEBUG("SCHEDULE", "schedule_init: rt->count = %zu", rt ? rt->count : 0);

    for (uint8_t d = 0; d < 7; d++)
    {
        s_schedule[d].day = d;
        s_schedule[d].count = 0;

        for (size_t i = 0; i < rt->count; i++)
        {
            if (strcmp(rt->entries[i].section, s_section_names[d]) == 0)
            {
                if (s_schedule[d].count >= SCHEDULE_MAX_POINTS)
                {
                    LOG_DEBUG("SCHEDULE", "schedule_init: trop de points pour la section %s", s_section_names[d]);
                    break;
                }

                unsigned int hour = 0;
                unsigned int minute = 0;
                if (sscanf(rt->entries[i].key, "%u:%u", &hour, &minute) == 2)
                {
                    float val = DEFAULT_SETPOINT;
                    if (ini_get_float(s_section_names[d], rt->entries[i].key, &val))
                    {
                        uint32_t idx = s_schedule[d].count;
                        s_schedule[d].points[idx].hour = (uint8_t)hour;
                        s_schedule[d].points[idx].minute = (uint8_t)minute;
                        s_schedule[d].points[idx].setpoint = val;
                        s_schedule[d].count++;
                        // printf("[DEBUG] Ajout point %s -> %02u:%02u = %.1f\n", sections[d], hour, minute, val);
                    }
                    else
                    {
                        LOG_DEBUG("SCHEDULE", "Erreur ini_get_float pour %s / %s", s_section_names[d], rt->entries[i].key);
                    }
                }
                else
                {
                    LOG_DEBUG("SCHEDULE", "Format clé invalide (attendu HH:MM) : [%s] %s", rt->entries[i].section, rt->entries[i].key);
                }
            }
        }

        /*
         * Tri des points par ordre chronologique croissant
         * pour garantir un parcours correct dans schedule_get_setpoint().
         */

        schedule_sort_day(&s_schedule[d]);
    }

    LOG_INFO("SCHEDULE",
             "Weekly schedule initialized from %s",
             s_schedule_file);

    return true;
}

float schedule_get_setpoint(void)
{
    clock_time_t now;

    if (!clock_get_time(&now))
    {
        LOG_ERROR("SCHEDULE",
                  "Unable to read clock");

        return DEFAULT_SETPOINT;
    }

    uint32_t current_minutes =
        (uint32_t)now.hour * 60U +
        (uint32_t)now.minute;

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
             weekday);

    return setpoint;
}

bool schedule_set_point(uint8_t day,
                        uint8_t hour,
                        uint8_t minute,
                        float setpoint)
{
    if (day > 6 || hour > 23 || minute > 59)
    {
        return false;
    }

    schedule_day_t *s_day = &s_schedule[day];

    bool updated = false;

    /*
     * Recherche d'un point existant
     */
    for (uint32_t i = 0; i < s_day->count; i++)
    {
        if ((s_day->points[i].hour == hour) &&
            (s_day->points[i].minute == minute))
        {
            s_day->points[i].setpoint = setpoint;
            updated = true;
            break;
        }
    }

    /*
     * Nouveau point
     */
    if (!updated)
    {
        if (s_day->count >= SCHEDULE_MAX_POINTS)
        {
            LOG_ERROR("SCHEDULE",
                      "Too many schedule points for day %u",
                      day);

            return false;
        }

        uint32_t idx = s_day->count;

        s_day->points[idx].hour = hour;
        s_day->points[idx].minute = minute;
        s_day->points[idx].setpoint = setpoint;

        s_day->count++;

        /*
         * Tri chronologique
         */
        schedule_sort_day(s_day);
    }

    return schedule_save();
}

static void schedule_sort_day(schedule_day_t *day)
{
    /*
     * Tri chronologique
     */
    for (uint32_t i = 0; i < day->count; i++)
    {
        for (uint32_t j = i + 1; j < day->count; j++)
        {
            uint32_t time_i =
                (uint32_t)day->points[i].hour * 60U +
                day->points[i].minute;

            uint32_t time_j =
                (uint32_t)day->points[j].hour * 60U +
                day->points[j].minute;

            if (time_i > time_j)
            {
                schedule_point_t temp = day->points[i];
                day->points[i] = day->points[j];
                day->points[j] = temp;
            }
        }
    }
}

bool schedule_save()
{
    /*
     * Sauvegarde dans le fichier
     */

    FILE *fp = fopen(s_schedule_file, "w");

    if (fp == NULL)
    {
        LOG_ERROR("SCHEDULE",
                  "Unable to open %s",
                  s_schedule_file);

        return false;
    }

    for (uint8_t d = 0; d < 7; d++)
    {
        fprintf(fp,
                "[%s]\n",
                s_section_names[d]);

        for (uint32_t i = 0; i < s_schedule[d].count; i++)
        {
            fprintf(fp,
                    "%02u:%02u=%.1f\n",
                    s_schedule[d].points[i].hour,
                    s_schedule[d].points[i].minute,
                    s_schedule[d].points[i].setpoint);
        }

        fprintf(fp, "\n");
    }

    if (fclose(fp) != 0)
    {
        LOG_ERROR("SCHEDULE",
                  "Unable to close %s",
                  s_schedule_file);

        return false;
    }

    LOG_INFO("SCHEDULE",
             "Program saved to %s",
             s_schedule_file);

    return true;
}