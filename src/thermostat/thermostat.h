#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <stdbool.h>

#define HORS_GEL_SETPOINT 7.0f
#define HORS_GEL_HYSTERESIS 1.0f

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        THERMOSTAT_OFF = 0,
        THERMOSTAT_MANUAL,
        THERMOSTAT_AUTO,
        THERMOSTAT_HORS_GEL

    } thermostat_mode_t;

    typedef struct
    {
        thermostat_mode_t mode;

        float temperature;

        float setpoint;

        float hysteresis;

        bool relay_state;

        bool heating_request;

    } thermostat_status_t;

    const thermostat_status_t *thermostat_get_status(void);

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

    const char *thermostat_mode_name(
        thermostat_mode_t mode);

    const char *thermostat_mode_to_string(
        thermostat_mode_t mode);

    bool thermostat_string_to_mode(
        const char *text,
        thermostat_mode_t *mode);

    void thermostat_set_hysteresis(float value);

#ifdef __cplusplus
}
#endif

#endif /* THERMOSTAT_H */
