#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#include "thermostat.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    thermostat_mode_t mode;

    float setpoint;

    float hysteresis;

    uint32_t relay_delay;

    float latitude;

    float longitude;

} runtime_config_t;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool runtime_init(void);

/*==========================================================
 * Lecture
 *=========================================================*/

const runtime_config_t *runtime_get(void);

/*==========================================================
 * Chargement / Sauvegarde
 *=========================================================*/

bool runtime_load(void);

bool runtime_save(void);

/*==========================================================
 * Modification
 *=========================================================*/

bool runtime_set_mode(thermostat_mode_t mode);

bool runtime_set_setpoint(float value);

bool runtime_set_hysteresis(float value);

bool runtime_set_relay_delay(uint32_t seconds);

bool runtime_set_location(float latitude,
                          float longitude);

#ifdef __cplusplus
}
#endif

#endif
