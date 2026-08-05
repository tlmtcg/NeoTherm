#include "alarm_runtime.h"

#include "alarm_checks.h"

#include <stdlib.h>

void alarm_runtime_init(void)
{
}

void alarm_runtime_update(
    const thermostat_status_t *status)
{
    if (status == NULL)
    {
        return;
    }

    alarm_check_temperature(status);

    alarm_check_heating(status);

    alarm_check_sensor(status);

    alarm_check_weather();
}
