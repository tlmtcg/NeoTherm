#include "debug.h"

#include <stdio.h>

/*==========================================================
 * Initialisation
 *=========================================================*/

void debug_init(void)
{
    printf("Debug initialized\n");
}

/*==========================================================
 * Dump Application Configuration
 *=========================================================*/

void debug_dump_app_config(
    const app_config_t *config)
{
    if (config == NULL)
    {
        printf("\n===== APPLICATION CONFIG =====\n");
        printf("Configuration is NULL\n");
        printf("================================\n");

        return;
    }

    printf("\n===== APPLICATION CONFIG =====\n");

    printf("Logger level       : %s\n",
           config->logger_level);

    printf("Relay GPIO         : %u\n",
           config->relay_gpio);

    printf("Climate period     : %u ms\n",
           config->climate_period);

    printf("Thermostat period  : %u ms\n",
           config->thermostat_period);

    printf("================================\n");
}
