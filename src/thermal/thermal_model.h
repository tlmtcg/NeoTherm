#ifndef THERMAL_MODEL_H
#define THERMAL_MODEL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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

void thermal_model_set_heat_power(
    float value);

void thermal_model_set_loss_factor(
    float value);

void thermal_model_set_thermal_mass(
    float value);

#ifdef __cplusplus
}
#endif

#endif