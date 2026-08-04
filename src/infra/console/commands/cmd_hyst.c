#include "cmd_hyst.h"
#include "thermostat.h"

#include <stdio.h>
#include <stdlib.h>

bool cmd_hyst(const char *args){

    if (args == NULL || args[0] == '\0')
    {
        printf("Usage : hyst <value>\n");
        return false;
    }

    float hyst = atof(args);

    thermostat_set_hysteresis(hyst);

    printf("Hysteresis set to %.2f C\n",
           hyst);

    return true;

}