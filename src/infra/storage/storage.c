#include "storage.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "storage_ini.h"
#include "logger.h"
#include "runtime.h"
#include "thermostat.h"
#include "weather_provider.h"

bool storage_init(void)
{
    LOG_INFO("STORAGE",
             "Storage initialized");

    return true;
}

void storage_dump(void)
{
    runtime_config_t cfg;

    printf("\n========== STORAGE ==========\n");

    if (!storage_load_runtime(&cfg))
    {
        printf("No runtime configuration\n");
        printf("=============================\n");
        return;
    }

    printf("Mode        : %s\n",
           thermostat_mode_to_string(cfg.mode));

    printf("Setpoint    : %.2f C\n",
           cfg.setpoint);

    printf("Hysteresis  : %.2f C\n",
           cfg.hysteresis);

    printf("Relay delay : %u s\n",
           cfg.relay_delay);

    printf("Latitude    : %.6f\n",
           cfg.latitude);

    printf("Longitude   : %.6f\n",
           cfg.longitude);

    printf("=============================\n");
}

bool storage_save_runtime(
    const runtime_config_t *cfg)
{
    if (cfg == NULL)
    {
        LOG_ERROR("STORAGE",
                  "Invalid runtime configuration");

        return false;
    }

    FILE *fp = fopen(
        STORAGE_FILE,
        "w");

    if (fp == NULL)
    {
        LOG_ERROR("STORAGE",
                  "Unable to open %s",
                  STORAGE_FILE);

        return false;
    }

    fprintf(fp,
            "[runtime]\n");

    fprintf(fp,
            "mode=%s\n",
            thermostat_mode_to_string(cfg->mode));

    fprintf(fp,
            "setpoint=%.2f\n",
            cfg->setpoint);

    fprintf(fp,
            "hysteresis=%.2f\n",
            cfg->hysteresis);

    fprintf(fp,
            "relay_delay=%u\n",
            cfg->relay_delay);

    fprintf(fp,
            "latitude=%.6f\n",
            cfg->latitude);

    fprintf(fp,
            "longitude=%.6f\n",
            cfg->longitude);

    fprintf(fp,
            "date_time=%04u-%02u-%02u %02u:%02u:%02u\n",
            cfg->date_time.year,
            cfg->date_time.month,
            cfg->date_time.day,
            cfg->date_time.hour,
            cfg->date_time.minute,
            cfg->date_time.second);

    fprintf(fp,
        "weather_update_period=%u\n",
        cfg->weather_update_period_sec);

    fprintf(fp,
        "weather_provider=%s\n",
        weather_provider_to_string(
            cfg->weather_provider));
    

    if (fclose(fp) != 0)
    {
        LOG_ERROR("STORAGE",
                  "File close failed");

        return false;
    }

    LOG_INFO("STORAGE",
             "Runtime configuration saved");

    return true;
}

storage_load_result_t storage_load_runtime(
    runtime_config_t *cfg)
{
    if (cfg == NULL)
    {
        LOG_ERROR("STORAGE",
                  "Invalid runtime pointer");

        return STORAGE_LOAD_ERROR;
    }

    /*
     * Valeurs par défaut
     */
    *cfg = runtime_default_config;

    FILE *fp = fopen(
        STORAGE_FILE,
        "r");

    if (fp == NULL)
    {
        LOG_WARN("STORAGE",
                 "No runtime file, using defaults");

        return STORAGE_LOAD_DEFAULT;
    }

    char line[128];

    while (fgets(line,
                 sizeof(line),
                 fp))
    {
        char *value = strchr(line, '=');

        if (value == NULL)
        {
            continue;
        }

        *value++ = '\0';

        line[strcspn(line, "\r\n")] = '\0';
        value[strcspn(value, "\r\n")] = '\0';

        if (strcmp(line, "mode") == 0)
        {
            if (!thermostat_string_to_mode(value,
                                           &cfg->mode))
            {
                LOG_WARN("STORAGE",
                         "Invalid mode : %s",
                         value);
            }
        }

        else if (strcmp(line, "setpoint") == 0)
        {
            cfg->setpoint =
                strtof(value, NULL);
        }

        else if (strcmp(line, "hysteresis") == 0)
        {
            cfg->hysteresis =
                strtof(value, NULL);
        }

        else if (strcmp(line, "relay_delay") == 0)
        {
            cfg->relay_delay =
                (uint32_t)strtoul(value,
                                  NULL,
                                  10);
        }

        else if (strcmp(line, "latitude") == 0)
        {
            cfg->latitude =
                strtof(value, NULL);
        }

        else if (strcmp(line, "longitude") == 0)
        {
            cfg->longitude =
                strtof(value, NULL);
        }

        else if (strcmp(line, "weather_provider") == 0)
        {
            if (strcmp(value, "SIMULATOR") == 0)
            {
                cfg->weather_provider =
                    WEATHER_PROVIDER_SIMULATOR;
            }
            else if (strcmp(value, "OPENMETEO") == 0)
            {
                cfg->weather_provider =
                    WEATHER_PROVIDER_OPENMETEO;
            }
            else
            {
                LOG_WARN("STORAGE",
                         "Invalid weather_provider : %s",
                         value);
            }
        }

        else if (strcmp(line, "weather_update_period") == 0)
        {
            cfg->weather_update_period_sec =
                (uint32_t)strtoul(value,
                                  NULL,
                                  10);
        }

        else if (strcmp(line, "date_time") == 0)
        {
            unsigned year;
            unsigned month;
            unsigned day;
            unsigned hour;
            unsigned minute;
            unsigned second;

            if (sscanf(value,
                       "%u-%u-%u %u:%u:%u",
                       &year,
                       &month,
                       &day,
                       &hour,
                       &minute,
                       &second) == 6)
            {
                cfg->date_time.year = year;
                cfg->date_time.month = month;
                cfg->date_time.day = day;
                cfg->date_time.hour = hour;
                cfg->date_time.minute = minute;
                cfg->date_time.second = second;
            }
            else
            {
                LOG_WARN("STORAGE",
                         "Invalid date_time : %s",
                         value);
            }
        }

        else if (line[0] != '[')
        {
            LOG_DEBUG("STORAGE",
                      "Unknown key : %s",
                      line);
        }
    }

    if (fclose(fp) != 0)
    {
        LOG_ERROR("STORAGE",
                  "File close failed");

        return STORAGE_LOAD_ERROR;
    }

    LOG_INFO("STORAGE",
             "Runtime initialized");

    return STORAGE_LOAD_OK;
}

bool storage_test_clear(void)
{
    if (remove(STORAGE_FILE) != 0)
    {
        /*
         * Le fichier n'existe déjà pas :
         * le stockage est donc bien vide.
         */
        return true;
    }

    LOG_INFO(
        "STORAGE",
        "Runtime storage cleared");

    return true;
}
