#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>

#include "thermostat.h"
#include "runtime.h"

#ifdef __cplusplus
extern "C" {
#endif



bool storage_init(void);


bool storage_save_mode(
        thermostat_mode_t mode);


bool storage_load_mode(
        thermostat_mode_t *mode);

void storage_dump(void);

bool storage_save_setpoint(
        float value);


bool storage_load_setpoint(
        float *value);

bool storage_save_runtime(
        const runtime_config_t *cfg);

bool storage_load_runtime(
        runtime_config_t *cfg);
        
#ifdef __cplusplus
}
#endif

#endif