#include "cmd_setpoint.h"
#include "thermostat.h"

#include <stdio.h>
#include <stdlib.h>

bool cmd_setpoint(const char *args){

    if (args == NULL || args[0] == '\0')
    {
        printf("Usage : setpoint <value>\n");
        return false;
    }


    float setpoint = atof(args);


    thermostat_set_setpoint(setpoint);


    printf("Setpoint set to %.2f C\n",
           setpoint);


    return true;

}