#ifndef THERMAL_MODEL_H
#define THERMAL_MODEL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float outside_temperature;

    float heat_power;

    float heat_rate;

    float cooling_rate;

    float warming_rate;

    float loss_factor;

    float thermal_mass;

} thermal_model_t;

bool thermal_model_init(void);

/*==========================================================
 * Mise à jour du modèle thermique
 *=========================================================*/

float thermal_model_update(
    float inside_temperature,
    bool heating);

/*==========================================================
 * Température extérieure
 *=========================================================*/

void thermal_model_set_outside_temperature(
    float temperature);

float thermal_model_get_outside_temperature(void);

/*==========================================================
 * Paramètres du modèle
 *=========================================================*/

/*==========================================================
 * Paramètres du modèle
 *=========================================================*/

void thermal_model_set_heat_power(
    float value);

float thermal_model_get_heat_power(void);

float thermal_model_get_heat_rate(void);

float thermal_model_get_cooling_rate(void);

float thermal_model_get_warming_rate(void);

void thermal_model_set_loss_factor(
    float value);

float thermal_model_get_loss_factor(void);

void thermal_model_set_thermal_mass(
    float value);

float thermal_model_get_thermal_mass(void);

void thermal_dump(void);

#ifdef __cplusplus
}
#endif

#endif