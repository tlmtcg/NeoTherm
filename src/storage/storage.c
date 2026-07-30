#include "storage.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "logger.h"


#define STORAGE_FILE "../runtime.ini"


static float s_setpoint = 20.5f;

static thermostat_mode_t s_mode =
    THERMOSTAT_AUTO;



static const char *mode_to_string(
        thermostat_mode_t mode)
{
    switch(mode)
    {
        case THERMOSTAT_AUTO:
            return "AUTO";

        case THERMOSTAT_MANUAL:
            return "MANUAL";

        case THERMOSTAT_HORS_GEL:
            return "HORS_GEL";

        case THERMOSTAT_OFF:
            return "OFF";

        default:
            return "UNKNOWN";
    }
}



static bool string_to_mode(
        const char *text,
        thermostat_mode_t *mode)
{
    char buffer[32];

    sscanf(text, "%31s", buffer);


    if(strcmp(buffer,"AUTO")==0)
    {
        *mode = THERMOSTAT_AUTO;
        return true;
    }


    if(strcmp(buffer,"MANUAL")==0)
    {
        *mode = THERMOSTAT_MANUAL;
        return true;
    }


    if(strcmp(buffer,"HORS_GEL")==0)
    {
        *mode = THERMOSTAT_HORS_GEL;
        return true;
    }


    if(strcmp(buffer,"OFF")==0)
    {
        *mode = THERMOSTAT_OFF;
        return true;
    }


    return false;
}



/*
 * Ecriture complète du runtime
 */

static bool storage_write(void)
{
    FILE *fp = fopen(
        STORAGE_FILE,
        "w");


    if(fp == NULL)
    {
        return false;
    }


    fprintf(fp,
            "[runtime]\n");


    fprintf(fp,
            "setpoint=%.1f\n",
            s_setpoint);


    fprintf(fp,
            "mode=%s\n",
            mode_to_string(s_mode));


    fclose(fp);


    return true;
}



bool storage_init(void)
{
    FILE *fp = fopen(
        STORAGE_FILE,
        "r");


    if(fp == NULL)
    {
        LOG_INFO("STORAGE",
                 "No runtime file, using defaults");

        return true;
    }


    char line[128];


    while(fgets(line,
                sizeof(line),
                fp))
    {

        if(strncmp(line,
                   "setpoint=",
                   9)==0)
        {
            s_setpoint =
                strtof(line + 9,
                       NULL);
        }


        else if(strncmp(line,
                        "mode=",
                        5)==0)
        {
            string_to_mode(
                line + 5,
                &s_mode);
        }
    }


    fclose(fp);


    LOG_INFO("STORAGE",
             "Runtime storage initialized");


    return true;
}




bool storage_save_mode(
        thermostat_mode_t mode)
{
    s_mode = mode;


    if(!storage_write())
    {
        return false;
    }


    LOG_INFO("STORAGE",
             "Mode saved : %s",
             mode_to_string(mode));


    return true;
}



bool storage_load_mode(
        thermostat_mode_t *mode)
{
    if(mode == NULL)
    {
        return false;
    }


    *mode = s_mode;


    return true;
}




bool storage_save_setpoint(
        float value)
{
    s_setpoint = value;


    if(!storage_write())
    {
        return false;
    }


    LOG_INFO("STORAGE",
             "Setpoint saved : %.1f",
             value);


    return true;
}




bool storage_load_setpoint(
        float *value)
{
    if(value == NULL)
    {
        return false;
    }


    *value = s_setpoint;


    return true;
}



void storage_dump(void)
{
    printf("\n========== STORAGE ==========\n");


    printf("Setpoint : %.2f\n",
           s_setpoint);


    printf("Mode     : %s\n",
           mode_to_string(s_mode));


    printf("=============================\n");
}

bool storage_save_runtime(
        const runtime_config_t *cfg)
{
    if (cfg == NULL)
    {
        return false;
    }

    FILE *fp = fopen(
        STORAGE_FILE,
        "w");

    if (fp == NULL)
    {
        return false;
    }

    fprintf(fp,
            "[runtime]\n");

    fprintf(fp,
            "mode=%s\n",
            mode_to_string(cfg->mode));

    fprintf(fp,
            "setpoint=%.1f\n",
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

    fclose(fp);

    LOG_INFO("STORAGE",
             "Runtime configuration saved");

    return true;
}

bool storage_load_runtime(
        runtime_config_t *cfg)
{
    if (cfg == NULL)
    {
        return false;
    }

    FILE *fp = fopen(
        STORAGE_FILE,
        "r");

    if (fp == NULL)
    {
        return false;
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
            string_to_mode(
                value,
                &cfg->mode);
        }
        else if (strcmp(line, "setpoint") == 0)
        {
            cfg->setpoint = strtof(
                value,
                NULL);
        }
        else if (strcmp(line, "hysteresis") == 0)
        {
            cfg->hysteresis = strtof(
                value,
                NULL);
        }
        else if (strcmp(line, "relay_delay") == 0)
        {
            cfg->relay_delay = (uint32_t)strtoul(
                value,
                NULL,
                10);
        }
        else if (strcmp(line, "latitude") == 0)
        {
            cfg->latitude = strtof(
                value,
                NULL);
        }
        else if (strcmp(line, "longitude") == 0)
        {
            cfg->longitude = strtof(
                value,
                NULL);
        }
    }

    fclose(fp);

    LOG_INFO("STORAGE",
             "Runtime configuration loaded");

    return true;
}
