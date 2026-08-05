#include "weather.h"

#include <string.h>

#include "console_utils.h"
#include "logger.h"

static weather_t s_weather;

/*
 *==========================================================
 * Initialisation
 *==========================================================
 */

bool weather_init(void)
{
    memset(&s_weather,
           0,
           sizeof(s_weather));


    s_weather.valid = false;


    LOG_INFO("WEATHER",
             "Weather initialized");


    return true;
}

/*
 *==========================================================
 * Lecture
 *==========================================================
 */

const weather_t *weather_get(void)
{
    if (!s_weather.valid)
    {
        return NULL;
    }


    return &s_weather;
}

/*
 *==========================================================
 * Mise à jour
 *==========================================================
 */

bool weather_set(
        const weather_t *data)
{
    if (data == NULL)
    {
        LOG_ERROR("WEATHER",
                  "Invalid weather data");

        return false;
    }


    /*
     * Copie atomique de la structure
     */

    s_weather = *data;


    s_weather.valid = true;


    LOG_INFO("WEATHER",
             "Updated : Temp=%.1f C Hum=%.1f %%",
             s_weather.temperature,
             s_weather.humidity);


    return true;
}

void weather_invalidate(void)
{
    s_weather.valid = false;

    LOG_WARN("WEATHER",
             "Weather invalidated");
}

/*
 *==========================================================
 * Debug
 *==========================================================
 */

void weather_dump(void)
{
    console_print_header("Weather");

    console_print_bool(
        "Valid",
        s_weather.valid);

    console_print_float(
        "Temperature",
        s_weather.temperature,
        "C");

    console_print_float(
        "Humidity",
        s_weather.humidity,
        "%");

    console_print_float(
        "Pressure",
        s_weather.pressure,
        "hPa");

    console_print_float(
        "Wind speed",
        s_weather.wind_speed,
        "km/h");

    console_print_float(
        "Wind gust",
        s_weather.wind_gust,
        "km/h");

    console_print_float(
        "Wind direction",
        s_weather.wind_direction,
        "deg");

    console_print_float(
        "Rain",
        s_weather.rain,
        "mm");

    console_print_separator();
}

/*
 *==========================================================
 * Etat
 *==========================================================
 */

bool weather_is_valid(void)
{
    return s_weather.valid;
}


void weather_clear(void)
{
    memset(&s_weather,
           0,
           sizeof(s_weather));


    s_weather.valid = false;
}
