#include "weather_provider_openmeteo.h"

#include "logger.h"
#include <stdlib.h>
#include "weather_parser_openmeteo.h"
#include "../infra/http/http_client.h"
#include "http_client.h"

static bool s_initialized = false;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool weather_provider_openmeteo_init(void)
{
    if (!http_client_init())
    {
        LOG_ERROR("OPEN_METEO",
                  "HTTP init failed");

        return false;
    }


    s_initialized = true;


    LOG_INFO("OPEN_METEO",
             "Provider initialized");


    return true;
}

/*==========================================================
 * Acquisition météo
 *=========================================================*/

bool weather_provider_openmeteo_fetch(
    weather_t *weather)
{
    if (!s_initialized)
    {
        LOG_ERROR("OPEN_METEO",
                  "Provider not initialized");

        return false;
    }

    if (weather == NULL)
    {
        return false;
    }

    /*
     * A faire :
     *
     * 1) Construire l'URL Open-Meteo
     * 2) Effectuer la requête HTTP
     * 3) Parser le JSON
     * 4) Remplir weather
     */

    char json[8192];

    const char *url =
        "https://api.open-meteo.com/v1/forecast?"
        "latitude=50.6333&"
        "longitude=3.0667&"
        "current="
        "temperature_2m,"
        "relative_humidity_2m,"
        "surface_pressure,"
        "wind_speed_10m,"
        "wind_direction_10m,"
        "wind_gusts_10m,"
        "rain";

    if (!http_client_get(
            url,
            json,
            sizeof(json)))
    {
        return false;
    }

    return weather_parse_openmeteo(
        json,
        weather);

    return false;
}
