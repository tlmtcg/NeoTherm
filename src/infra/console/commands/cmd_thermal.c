#include "cmd_thermal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thermal_model.h"

bool cmd_thermal(const char *args)
{
    if ((args == NULL) || (*args == '\0'))
    {
        printf("Usage:\n");
        printf("  thermal status\n");
        printf("  thermal outside <temperature>\n");
        printf("  thermal heat <power>\n");
        printf("  thermal loss <factor>\n");
        printf("  thermal mass <value>\n");

        return false;
    }

    if (strcmp(args, "status") == 0)
    {
        thermal_dump();
        return true;
    }

    if (strncmp(args, "outside ", 8) == 0)
    {
        float value = (float)atof(args + 8);

        thermal_model_set_outside_temperature(value);

        thermal_dump();

        return true;
    }

    if (strncmp(args, "heat ", 5) == 0)
    {
        float value = (float)atof(args + 5);

        thermal_model_set_heat_power(value);

        thermal_dump();

        return true;
    }

    if (strncmp(args, "loss ", 5) == 0)
    {
        float value = (float)atof(args + 5);

        thermal_model_set_loss_factor(value);

        thermal_dump();

        return true;
    }

    if (strncmp(args, "mass ", 5) == 0)
    {
        float value = (float)atof(args + 5);

        thermal_model_set_thermal_mass(value);

        thermal_dump();

        return true;
    }

    printf("Unknown thermal command\n");

    return false;
}
