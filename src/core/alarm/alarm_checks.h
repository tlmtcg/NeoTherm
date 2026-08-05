#ifndef ALARM_CHECKS_H
#define ALARM_CHECKS_H

#include "thermostat.h"


void alarm_check_temperature(
    const thermostat_status_t *status);


void alarm_check_heating(
    const thermostat_status_t *status);


void alarm_check_sensor(
    const thermostat_status_t *status);


void alarm_check_weather(void);

#endif
