#include "alarm_checks.h"
#include "weather_service.h"

#include "alarm.h"

#include <stdlib.h>
#define TEMP_MAX 30.0f
#define TEMP_MIN 5.0f


void alarm_check_temperature(
    const thermostat_status_t *status)
{
    if (status == NULL)
    {
        return;
    }


    if (status->temperature > TEMP_MAX)
    {
        alarm_set(
            ALARM_TEMP_HIGH,
            status->temperature);
    }
    else
    {
        alarm_clear(
            ALARM_TEMP_HIGH);
    }



    if (status->temperature < TEMP_MIN)
    {
        alarm_set(
            ALARM_TEMP_LOW,
            status->temperature);
    }
    else
    {
        alarm_clear(
            ALARM_TEMP_LOW);
    }
}



void alarm_check_heating(
    const thermostat_status_t *status)
{
    if (status == NULL)
    {
        return;
    }


    /*
     * Sera ajouté ensuite :
     *
     * ALARM_HEATING_TIMEOUT
     *
     */
}



void alarm_check_sensor(
    const thermostat_status_t *status)
{
    if (status == NULL)
    {
        return;
    }


    /*
     * Sera ajouté ensuite :
     *
     * ALARM_SENSOR_INVALID
     *
     */
}

void alarm_check_weather(void)
{
    if (!weather_service_is_available())
    {
        alarm_set(
            ALARM_WEATHER_ERROR,
            0);
    }
    else
    {
        alarm_clear(
            ALARM_WEATHER_ERROR);
    }
}
