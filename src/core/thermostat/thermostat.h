// #ifndef THERMOSTAT_H
// #define THERMOSTAT_H

// #include <stdbool.h>
// #include "../weather/weather.h"

// #define HORS_GEL_SETPOINT 7.0f
// #define HORS_GEL_HYSTERESIS 1.0f

// #ifdef __cplusplus
// extern "C"
// {
// #endif

//     typedef enum
//     {
//         THERMOSTAT_OFF = 0,
//         THERMOSTAT_MANUAL,
//         THERMOSTAT_AUTO,
//         THERMOSTAT_HORS_GEL

//     } thermostat_mode_t;

// typedef struct
// {
//     thermostat_mode_t mode;

//     float temperature;

//     float setpoint;

//     float hysteresis;

//     bool relay_state;

//     bool heating_request;

//     float outside_temperature;

//     float outside_humidity;

//     bool weather_valid;

//     float temp_forecast_1h;

//     float predicted_temperature_1h;

//     bool prediction_valid;

//     bool temperature_valid;

// } thermostat_status_t;

//     const thermostat_status_t *thermostat_get_status(void);

//     /*==========================================================
//      * Initialisation
//      *=========================================================*/

//     bool thermostat_init(void);

//     /*==========================================================
//      * Mise à jour
//      *=========================================================*/

//     void thermostat_update(void);

//     /*==========================================================
//      * Lecture
//      *=========================================================*/

//     float thermostat_get_setpoint(void);

//     float thermostat_get_hysteresis(void);

//     bool thermostat_set_mode(
//         thermostat_mode_t mode);

//     thermostat_mode_t thermostat_get_mode(void);

//     bool thermostat_set_setpoint(float value);

//     bool thermostat_manual_set_relay(bool state);

//     const char *thermostat_mode_name(
//         thermostat_mode_t mode);

//     const char *thermostat_mode_to_string(
//         thermostat_mode_t mode);

//     bool thermostat_string_to_mode(
//         const char *text,
//         thermostat_mode_t *mode);

//     void thermostat_set_hysteresis(float value);

//     bool weather_set(
//         const weather_t *data);

// #ifdef __cplusplus
// }
// #endif

// #endif /* THERMOSTAT_H */


#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <stdbool.h>

#include "../weather/weather.h"

#define HORS_GEL_SETPOINT   7.0f
#define HORS_GEL_HYSTERESIS 1.0f
#define THERMOSTAT_PREDICTION_MINUTES 10.0f
#define THERMOSTAT_PREDICTION_MARGIN 0.10f

#ifdef __cplusplus
extern "C"
{
#endif

/*==========================================================
 * Modes
 *=========================================================*/

typedef enum
{
    THERMOSTAT_OFF = 0,
    THERMOSTAT_MANUAL,
    THERMOSTAT_AUTO,
    THERMOSTAT_HORS_GEL

} thermostat_mode_t;

/*==========================================================
 * Etat du thermostat
 *=========================================================*/

typedef struct
{
    thermostat_mode_t mode;

    /* Température intérieure */
    float temperature;
    bool temperature_valid;

    /* Consigne */
    float setpoint;
    float hysteresis;

    /* Chauffage */
    bool relay_state;
    bool heating_request;

    /* Météo extérieure */
    float outside_temperature;
    float outside_humidity;
    bool weather_valid;

    /* Prédiction thermique */
    float temp_forecast_1h;
    bool prediction_valid;

} thermostat_status_t;

/*==========================================================
 * Etat
 *=========================================================*/

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

thermostat_mode_t thermostat_get_mode(void);

/*==========================================================
 * Configuration
 *=========================================================*/

bool thermostat_set_mode(
    thermostat_mode_t mode);

bool thermostat_set_setpoint(
    float value);

void thermostat_set_hysteresis(
    float value);

/*==========================================================
 * Commande manuelle
 *=========================================================*/

bool thermostat_manual_set_relay(
    bool state);

/*==========================================================
 * Conversion du mode
 *=========================================================*/

const char *thermostat_mode_to_string(
    thermostat_mode_t mode);

bool thermostat_string_to_mode(
    const char *text,
    thermostat_mode_t *mode);

#ifdef __cplusplus
}
#endif

#endif /* THERMOSTAT_H */

