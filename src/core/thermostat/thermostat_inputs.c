#include "thermostat.h"
#include "thermostat_internal.h"
#include <stdlib.h>
#include "climate.h"
#include "thermal_model.h"

void thermostat_read_inputs(void)
{
    /* Lecture de la température intérieure */
    s_status.temperature = climate_get_temperature();

    /* Lecture de la météo extérieure */
    const weather_t *weather = weather_get();

    if (weather != NULL && weather->valid)
    {
        s_status.outside_temperature = weather->temperature;
        s_status.outside_humidity = weather->humidity;
        s_status.weather_valid = true;
    }
    else
    {
        s_status.outside_temperature = thermal_model_get_outside_temperature();
        s_status.outside_humidity = 0.0f;
        s_status.weather_valid = false;
    }
}
