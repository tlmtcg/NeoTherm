#include "storage.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"


#define STORAGE_FILE "../runtime.ini"


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
    if(strcmp(text,"AUTO")==0)
    {
        *mode = THERMOSTAT_AUTO;
        return true;
    }

    if(strcmp(text,"MANUAL")==0)
    {
        *mode = THERMOSTAT_MANUAL;
        return true;
    }

    if(strcmp(text,"HORS_GEL")==0)
    {
        *mode = THERMOSTAT_HORS_GEL;
        return true;
    }

    if(strcmp(text,"OFF")==0)
    {
        *mode = THERMOSTAT_OFF;
        return true;
    }

    return false;
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


    fclose(fp);


    LOG_INFO("STORAGE",
             "Runtime storage initialized");


    return true;
}



bool storage_save_mode(
        thermostat_mode_t mode)
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
            "mode=%s\n",
            mode_to_string(mode));


    fclose(fp);


    LOG_INFO("STORAGE",
             "Mode saved : %s",
             mode_to_string(mode));


    return true;
}



bool storage_load_mode(
        thermostat_mode_t *mode)
{
    FILE *fp = fopen(
        STORAGE_FILE,
        "r");


    if(fp == NULL)
    {
        return false;
    }


    char line[128];


    while(fgets(line,
                sizeof(line),
                fp))
    {
        if(strncmp(line,
                   "mode=",
                   5)==0)
        {
            fclose(fp);

            return string_to_mode(
                    line + 5,
                    mode);
        }
    }


    fclose(fp);

    return false;
}



bool storage_save_setpoint(
        float value)
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
            value);


    fclose(fp);


    LOG_INFO("STORAGE",
             "Setpoint saved : %.1f",
             value);


    return true;
}



bool storage_load_setpoint(
        float *value)
{
    FILE *fp = fopen(
        STORAGE_FILE,
        "r");


    if(fp == NULL)
    {
        return false;
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
            fclose(fp);

            *value = strtof(
                    line + 9,
                    NULL);

            return true;
        }
    }


    fclose(fp);

    return false;
}
