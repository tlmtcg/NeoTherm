#include "weather_parser_openmeteo.h"

#include <string.h>

#include "../../external/cjson/cJSON.h"
#include "logger.h"

/*==========================================================
 * Extraction valeur float
 *=========================================================*/

static bool json_get_float(
    cJSON *object,
    const char *name,
    float *value)
{
    cJSON *item =
        cJSON_GetObjectItem(
            object,
            name);

    if ((item == NULL) ||
        !cJSON_IsNumber(item))
    {
        return false;
    }

    *value =
        (float)item->valuedouble;

    return true;
}

/*==========================================================
 * Parse Open-Meteo
 *=========================================================*/

bool weather_parse_openmeteo(
    const char *json,
    weather_t *weather)
{
    if ((json == NULL) ||
        (weather == NULL))
    {
        return false;
    }

    cJSON *root =
        cJSON_Parse(json);

    if (root == NULL)
    {
        LOG_ERROR("OPEN_METEO",
                  "Invalid JSON");

        return false;
    }

    memset(weather,
           0,
           sizeof(*weather));

    cJSON *current =
        cJSON_GetObjectItem(
            root,
            "current");

    if (current == NULL)
    {
        LOG_ERROR("OPEN_METEO",
                  "Missing current object");

        cJSON_Delete(root);

        return false;
    }

    /*
     * Température obligatoire
     */

    if (!json_get_float(
            current,
            "temperature_2m",
            &weather->temperature))
    {
        LOG_ERROR("OPEN_METEO",
                  "Temperature missing");

        cJSON_Delete(root);

        return false;
    }

    /*
     * Champs optionnels
     */

    json_get_float(
        current,
        "relative_humidity_2m",
        &weather->humidity);

    json_get_float(
        current,
        "surface_pressure",
        &weather->pressure);

    json_get_float(
        current,
        "wind_speed_10m",
        &weather->wind_speed);

    json_get_float(
        current,
        "wind_direction_10m",
        &weather->wind_direction);

    json_get_float(
        current,
        "wind_gusts_10m",
        &weather->wind_gust);

    json_get_float(
        current,
        "rain",
        &weather->rain);

    weather->valid = true;

    cJSON_Delete(root);

    LOG_INFO("OPEN_METEO",
             "Parsed : %.1f C %.1f %% %.1f hPa",
             weather->temperature,
             weather->humidity,
             weather->pressure);

    return true;
}
