#include "alarm_runtime.h"

#include "alarm.h"


#define TEMP_MAX 30.0f
#define TEMP_MIN 5.0f


void alarm_runtime_init(void)
{

}


void alarm_runtime_update(
    const thermostat_status_t *status
)
{
    if(status == NULL)
    {
        return;
    }


    if(status->temperature > TEMP_MAX)
    {
        alarm_set(
            ALARM_TEMP_HIGH,
            status->temperature
        );
    }
    else
    {
        alarm_clear(ALARM_TEMP_HIGH);
    }


    if(status->temperature < TEMP_MIN)
    {
        alarm_set(
            ALARM_TEMP_LOW,
            status->temperature
        );
    }
    else
    {
        alarm_clear(ALARM_TEMP_LOW);
    }
}