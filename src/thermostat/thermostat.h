#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    THERMOSTAT_OFF = 0,
    THERMOSTAT_MANUAL,
    THERMOSTAT_AUTO,
    THERMOSTAT_HORS_GEL

} thermostat_mode_t;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool thermostat_init(void);


/*==========================================================
 * Mise à jour
 *=========================================================*/

void thermostat_update(void);


/*==========================================================
 * Lecture
 *=========================================================*/

float thermostat_get_setpoint(void);

float thermostat_get_hysteresis(void);

bool thermostat_set_mode(
    thermostat_mode_t mode);


thermostat_mode_t thermostat_get_mode(void);

bool thermostat_set_setpoint(float value);

bool thermostat_manual_set_relay(bool state);

#ifdef __cplusplus
}
#endif

#endif /* THERMOSTAT_H */
