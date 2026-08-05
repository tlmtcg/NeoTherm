#include "weather_service.h"

#include <stdio.h>
#include <time.h>

#include "logger.h"
#include "weather.h"
#include "runtime.h"

/*==========================================================
 * Configuration
 *=========================================================*/

static weather_provider_t s_provider =
    WEATHER_PROVIDER_SIMULATOR;

/*==========================================================
 * Etat
 *=========================================================*/

static bool s_initialized = false;
static bool s_available = false;

static uint32_t s_last_update = 0;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool weather_service_init(void)
{
    s_initialized = true;
    s_available = false;
    s_last_update = 0;

    const runtime_config_t *runtime =
        runtime_get();

    if (runtime == NULL)
    {
        LOG_ERROR("WEATHER_SERVICE",
                  "Runtime unavailable");

        return false;
    }

    s_provider =
        runtime->weather_provider;

    if (!weather_provider_set(
            s_provider))
    {
        LOG_ERROR("WEATHER_SERVICE",
                  "Provider initialization failed");

        return false;
    }

    LOG_INFO("WEATHER_SERVICE",
             "Weather service initialized");

    return true;
}

/*==========================================================
 * Mise à jour
 *=========================================================*/

bool weather_service_update(void)
{
    if (!s_initialized)
    {
        return false;
    }

    weather_t weather;

    LOG_INFO("WEATHER_SERVICE",
             "Fetching weather data");

    if (!weather_provider_fetch(

            &weather))
    {
        s_available = false;

        LOG_WARN("WEATHER_SERVICE",
                 "Weather unavailable");

        return false;
    }

    if (!weather_set(&weather))
    {
        s_available = false;

        LOG_ERROR("WEATHER_SERVICE",
                  "Unable to update weather");

        return false;
    }

    s_available = true;

    s_last_update =
        (uint32_t)time(NULL);

    LOG_INFO("WEATHER_SERVICE",
             "Weather updated");

    return true;
}

/*==========================================================
 * Tick
 *=========================================================*/

void weather_service_tick(void)
{
    const runtime_config_t *runtime =
        runtime_get();

    if (runtime == NULL)
    {
        return;
    }


    if (!s_available)
    {
        weather_service_update();
        return;
    }


    if (weather_service_age() >=
        runtime->weather_update_period_sec)
    {
        weather_service_update();
    }
}

/*==========================================================
 * Provider
 *=========================================================*/

bool weather_service_set_provider(
    weather_provider_t provider)
{
    if (!weather_provider_set(provider))
    {
        return false;
    }

    s_provider = provider;

    return runtime_set_weather_provider(
        provider);
}

weather_provider_t weather_service_get_provider(void)
{
    return s_provider;
}

/*==========================================================
 * Position
 *=========================================================*/

bool weather_service_set_location(
    float latitude,
    float longitude)
{
    if (!runtime_set_location(
            latitude,
            longitude))
    {
        return false;
    }

    LOG_INFO("WEATHER_SERVICE",
             "Location %.4f %.4f",
             latitude,
             longitude);

    return true;
}

bool weather_service_get_location(
    float *latitude,
    float *longitude)
{
    if ((latitude == NULL) ||
        (longitude == NULL))
    {
        return false;
    }

    const runtime_config_t *runtime =
        runtime_get();

    if (runtime == NULL)
    {
        return false;
    }

    *latitude = runtime->latitude;
    *longitude = runtime->longitude;

    return true;
}

/*==========================================================
 * Etat
 *=========================================================*/

bool weather_service_is_available(void)
{
    return s_available;
}

uint32_t weather_service_age(void)
{
    if (!s_available)
    {
        return UINT32_MAX;
    }

    uint32_t now =
        (uint32_t)time(NULL);

    return now - s_last_update;
}

/*==========================================================
 * Debug
 *=========================================================*/

void weather_service_dump(void)
{

    const runtime_config_t *runtime =
        runtime_get();

    printf("\n");
    printf("==============================\n");
    printf("Weather service\n");
    printf("==============================\n");

    printf("Provider     : %s\n",
           weather_provider_to_string(
               s_provider));

    if (runtime != NULL)
    {
        printf("Latitude     : %.4f\n",
               runtime->latitude);

        printf("Longitude    : %.4f\n",
               runtime->longitude);
    }

    printf("Available    : %s\n",
           s_available ? "YES" : "NO");

    printf("Age          : %u s\n",
           weather_service_age());

    printf("------------------------------\n");
}
