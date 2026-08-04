#ifndef ALARM_RUNTIME_H
#define ALARM_RUNTIME_H

#include "thermostat.h"


void alarm_runtime_init(void);


void alarm_runtime_update(
    const thermostat_status_t *status
);


#endif