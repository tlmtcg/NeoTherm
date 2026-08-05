#include "debug.h"
#include "logger.h"
#include "console_utils.h"
#include <stdio.h>

/*==========================================================
 * Initialisation
 *=========================================================*/

void debug_init(void)
{
    LOG_INFO("DEBUG",
         "Debug initialized");
}

/*==========================================================
 * Dump Application Configuration
 *=========================================================*/

void debug_dump_app_config(
    const app_config_t *config)
{
    console_print_header(
        "Application configuration");

    if (config == NULL)
    {
        console_print_string(
            "Configuration",
            "NULL");

        console_print_separator();

        return;
    }

    console_print_string(
        "Logger level",
        config->logger_level);

    console_print_uint(
        "Relay GPIO",
        config->relay_gpio);

    console_print_uint(
        "Climate period",
        config->climate_period);

    console_print_uint(
        "Thermostat period",
        config->thermostat_period);

    console_print_uint(
        "History save",
        config->history_save_period);

    console_print_separator();
}
