#include "schedule.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ini.h"
#include "logger.h"
#include <app_config.h>

#define DEFAULT_SETPOINT 17.0f

static schedule_day_t s_schedule[7];

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

    LOG_INFO("SCHEDULE",
             "Loading schedule : %s",
             cfg->schedule_file);

    if (!ini_init(cfg->schedule_file))
    {
        LOG_ERROR("SCHEDULE",
                  "Unable to load schedule file : %s",
                  cfg->schedule_file);

        return false;
    }

    const char *sections[7] = {
        "lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi", "dimanche"};

    const ini_runtime_t *rt = ini_get_runtime();
    // printf("[DEBUG] schedule_init: rt->count = %zu\n", rt ? rt->count : 0);

    for (uint8_t d = 0; d < 7; d++)
    {
        s_schedule[d].day = d;
        s_schedule[d].count = 0;

        for (size_t i = 0; i < rt->count; i++)
        {
            if (strcmp(rt->entries[i].section, sections[d]) == 0)
            {
                if (s_schedule[d].count >= 10)
                {
                    printf("[DEBUG] schedule_init: trop de points pour la section %s\n", sections[d]);
                    break;
                }

                unsigned int hour = 0;
                unsigned int minute = 0;
                if (sscanf(rt->entries[i].key, "%u:%u", &hour, &minute) == 2)
                {
                    float val = DEFAULT_SETPOINT;
                    if (ini_get_float(sections[d], rt->entries[i].key, &val))
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
                        printf("[DEBUG] Erreur ini_get_float pour %s / %s\n", sections[d], rt->entries[i].key);
                    }
                }
                else
                {
                    printf("[DEBUG] Format clé invalide (attendu HH:MM) : [%s] %s\n", rt->entries[i].section, rt->entries[i].key);
                }
            }
        }

        /*
         * Tri des points par ordre chronologique croissant
         * pour garantir un parcours correct dans schedule_get_setpoint().
         */
        for (uint32_t i = 0; i < s_schedule[d].count; i++)
        {
            for (uint32_t j = i + 1; j < s_schedule[d].count; j++)
            {
                uint32_t time_i = (uint32_t)s_schedule[d].points[i].hour * 60U + s_schedule[d].points[i].minute;
                uint32_t time_j = (uint32_t)s_schedule[d].points[j].hour * 60U + s_schedule[d].points[j].minute;

                if (time_i > time_j)
                {
                    schedule_point_t temp = s_schedule[d].points[i];
                    s_schedule[d].points[i] = s_schedule[d].points[j];
                    s_schedule[d].points[j] = temp;
                }
            }
        }
    }

    LOG_INFO("SCHEDULE",
             "Weekly schedule initialized from program.ini");

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
        if (s_day->count >= 10)
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
        for (uint32_t i = 0; i < s_day->count; i++)
        {
            for (uint32_t j = i + 1; j < s_day->count; j++)
            {
                uint32_t time_i =
                    (uint32_t)s_day->points[i].hour * 60U +
                    s_day->points[i].minute;

                uint32_t time_j =
                    (uint32_t)s_day->points[j].hour * 60U +
                    s_day->points[j].minute;

                if (time_i > time_j)
                {
                    schedule_point_t temp = s_day->points[i];
                    s_day->points[i] = s_day->points[j];
                    s_day->points[j] = temp;
                }
            }
        }
    }

    /*
     * Sauvegarde dans le fichier
     */
    const app_config_t *cfg = app_config_get();

    FILE *fp = fopen(cfg->schedule_file, "w");

    if (fp == NULL)
    {
        LOG_ERROR("SCHEDULE",
                  "Unable to open %s",
                  cfg->schedule_file);

        return false;
    }

    static const char *sections[7] =
        {
            "lundi",
            "mardi",
            "mercredi",
            "jeudi",
            "vendredi",
            "samedi",
            "dimanche"};

    for (uint8_t d = 0; d < 7; d++)
    {
        fprintf(fp,
                "[%s]\n",
                sections[d]);

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
                  cfg->schedule_file);

        return false;
    }

    LOG_INFO("SCHEDULE",
             "Program saved to %s",
             cfg->schedule_file);

    return true;
}
