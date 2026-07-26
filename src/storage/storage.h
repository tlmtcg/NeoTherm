#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>

#include "thermostat.h"

#ifdef __cplusplus
extern "C" {
#endif


bool storage_init(void);


bool storage_save_mode(
        thermostat_mode_t mode);


bool storage_load_mode(
        thermostat_mode_t *mode);



bool storage_save_setpoint(
        float value);


bool storage_load_setpoint(
        float *value);


#ifdef __cplusplus
}
#endif

#endif