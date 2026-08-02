#include "cmd_temp.h"

#include "climate.h"

#include <stdio.h>
#include <stdlib.h>


bool cmd_temp(const char *args)
{
    if (args == NULL || args[0] == '\0')
    {
        printf("Usage : temp <value>\n");
        return false;
    }


    float temperature = atof(args);


    climate_update(temperature);


    printf("Temperature set to %.2f C\n",
           temperature);


    return true;
}
