
#include "cmd_outside.h"
#include "thermal_model.h"

#include <stdio.h>
#include <stdlib.h>

bool cmd_outside(const char *args){

    

    if (args == NULL || args[0] == '\0')
    {
        printf("Usage : outside <value>\n");
        return false;
    }

    float outside = atof(args);

    thermal_model_set_outside_temperature(outside);

    
    printf("Outside set to %.2f C\n",
           outside);


    return true;

}