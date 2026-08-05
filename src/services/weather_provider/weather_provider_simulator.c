#include "weather_provider_simulator.h"

#include <string.h>

#include "logger.h"

static bool s_initialized = false;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool weather_provider_simulator_init(void)
{
    s_initialized = true;

    LOG_INFO("WEATHER_PROVIDER",
             "Simulator initialized");

    return true;
}

/*==========================================================
 * Acquisition météo
 *=========================================================*/

bool weather_provider_simulator_fetch(
    weather_t *weather)
{
    if (!s_initialized)
    {
        return false;
    }

    if (weather == NULL)
    {
        return false;
    }

    memset(weather,
           0,
           sizeof(*weather));

    weather->valid = true;

    weather->temperature = 12.5f;
    weather->humidity = 75.0f;
    weather->pressure = 1015.0f;

    weather->wind_speed = 18.0f;
    weather->wind_gust = 30.0f;
    weather->wind_direction = 270.0f;

    weather->rain = 0.0f;

    LOG_INFO("WEATHER_PROVIDER",
             "Simulator data generated");

    return true;
}
