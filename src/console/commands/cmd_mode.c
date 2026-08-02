#include "cmd_mode.h"

#include "thermostat.h"

#include <stdio.h>
#include <string.h>


bool cmd_mode(const char *args)
{
    if (args == NULL)
    {
        printf("Usage : mode <auto|manual|off|horsgel>\n");
        return false;
    }


    if (strcmp(args, "auto") == 0)
    {
        thermostat_set_mode(THERMOSTAT_AUTO);
    }
    else if (strcmp(args, "manual") == 0)
    {
        thermostat_set_mode(THERMOSTAT_MANUAL);
    }
    else if (strcmp(args, "off") == 0)
    {
        thermostat_set_mode(THERMOSTAT_OFF);
    }
    else if (strcmp(args, "horsgel") == 0)
    {
        thermostat_set_mode(THERMOSTAT_HORS_GEL);
    }
    else
    {
        printf("Unknown mode : %s\n", args);
        return false;
    }


    return true;
}
