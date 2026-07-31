#include "app_config.h"

#include <string.h>

#include "ini.h"
#include "logger.h"

static app_config_t s_config =
{
    .logger_level      = "INFO",

    .relay_gpio        = 4,

    .climate_period    = 10,
    .thermostat_period = 10
};

bool app_config_init(const char *filename)
{
    if (filename == NULL)
    {
        LOG_ERROR("APP_CONFIG",
                  "Filename is NULL");

        return false;
    }

    if (!ini_init(filename))
    {
        LOG_ERROR("APP_CONFIG",
                  "Unable to load %s",
                  filename);

        return false;
    }

    /*
     * Logger
     */

    (void)ini_get_string(
        "logger",
        "level",
        s_config.logger_level,
        sizeof(s_config.logger_level));

    /*
     * Relay
     */

    int value;

    if (ini_get_int("relay",
                    "gpio",
                    &value))
    {
        s_config.relay_gpio = (uint32_t)value;
    }

    /*
     * Scheduler
     */

    if (ini_get_int("scheduler",
                    "climate_period",
                    &value))
    {
        s_config.climate_period = (uint32_t)value;
    }

    if (ini_get_int("scheduler",
                    "thermostat_period",
                    &value))
    {
        s_config.thermostat_period = (uint32_t)value;
    }

    LOG_INFO("APP_CONFIG",
             "Application configuration loaded");

#ifdef DEBUG
    LOG_INFO("APP_CONFIG",
             "Relay GPIO=%u Climate=%u Thermostat=%u Logger=%s",
             s_config.relay_gpio,
             s_config.climate_period,
             s_config.thermostat_period,
             s_config.logger_level);
#endif

    return true;
}

const app_config_t *app_config_get(void)
{
    return &s_config;
}
