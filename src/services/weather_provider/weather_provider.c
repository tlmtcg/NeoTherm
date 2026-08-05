#include "weather_provider.h"

#include "weather_provider_simulator.h"
#include "logger.h"
#include <stdlib.h>
#include "../weather_service/weather_service.h"
#include "weather_provider_simulator.h"
#include "weather_provider_openmeteo.h"

static bool s_initialized = false;

static uint32_t s_fetch_count = 0;

static weather_provider_t s_active_provider =
    WEATHER_PROVIDER_SIMULATOR;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool weather_provider_init(void)
{
    if (!weather_provider_simulator_init())
    {
        LOG_ERROR("WEATHER_PROVIDER",
                  "Simulator initialization failed");

        return false;
    }

    s_initialized = true;

    LOG_INFO("WEATHER_PROVIDER",
             "Weather provider initialized");

    return true;
}

/*==========================================================
 * Téléchargement météo
 *=========================================================*/

bool weather_provider_fetch(
    weather_t *weather)
{
    if (weather == NULL)
    {
        return false;
    }

    if (!s_initialized)
    {
        LOG_ERROR("WEATHER_PROVIDER",
                  "Provider not initialized");

        return false;
    }

     s_fetch_count++;

    switch(s_active_provider)
    {

    case WEATHER_PROVIDER_SIMULATOR:

        return weather_provider_simulator_fetch(
            weather);


    case WEATHER_PROVIDER_OPENMETEO:

        return weather_provider_openmeteo_fetch(
            weather);


    default:

        return false;
    }
}

/*==========================================================
 * Etat
 *=========================================================*/

bool weather_provider_is_available(void)
{
    return s_initialized;
}

/*==========================================================
 * Debug
 *=========================================================*/

void weather_provider_dump(void)
{
    LOG_INFO("WEATHER_PROVIDER",
             "Initialized : %s",
             s_initialized ? "YES" : "NO");
}

const char *weather_provider_to_string(
    weather_provider_t provider)
{
    switch (provider)
    {
        case WEATHER_PROVIDER_SIMULATOR:
            return "SIMULATOR";

        case WEATHER_PROVIDER_OPENMETEO:
            return "OPENMETEO";

        default:
            return "UNKNOWN";
    }
}

bool weather_provider_from_string(
    const char *text,
    weather_provider_t *provider)
{
    if ((text == NULL) ||
        (provider == NULL))
    {
        return false;
    }

    if (strcmp(text, "SIMULATOR") == 0)
    {
        *provider = WEATHER_PROVIDER_SIMULATOR;
        return true;
    }

    if (strcmp(text, "OPENMETEO") == 0)
    {
        *provider = WEATHER_PROVIDER_OPENMETEO;
        return true;
    }

    return false;
}

bool weather_provider_set(
    weather_provider_t provider)
{
    bool result = false;


    switch(provider)
    {

    case WEATHER_PROVIDER_SIMULATOR:

        result =
            weather_provider_simulator_init();

        break;


    case WEATHER_PROVIDER_OPENMETEO:

        result =
            weather_provider_openmeteo_init();

        break;


    default:

        LOG_ERROR("WEATHER_PROVIDER",
                  "Unknown provider");

        return false;
    }


    if (result)
    {
        s_active_provider = provider;
        s_initialized = true;


        LOG_INFO("WEATHER_PROVIDER",
                 "Active provider : %s",
                 weather_provider_to_string(provider));
    }


    return result;
}

uint32_t weather_provider_get_fetch_count(void)
{
    return s_fetch_count;
}


void weather_provider_reset_fetch_count(void)
{
    s_fetch_count = 0;
}

