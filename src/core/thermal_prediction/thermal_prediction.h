#ifndef THERMAL_PREDICTION_H
#define THERMAL_PREDICTION_H

#include <stdbool.h>
#include <stdint.h>

/*==========================================================
 * Initialisation
 *=========================================================*/

bool thermal_prediction_init(void);

/*==========================================================
 * Calcul de la prédiction
 *=========================================================*/

/*
 * Met à jour les données de prédiction à partir :
 * - de la température actuelle
 * - de l'état du chauffage
 * - du thermal learning validé
 */
bool thermal_prediction_update(void);

/*==========================================================
 * Lecture
 *=========================================================*/

/*
 * Retourne la température prévue dans X minutes.
 *
 * Retourne une valeur calculée uniquement si le learning
 * est valide.
 */
float thermal_prediction_get_temperature_minutes(
    float minutes);

float thermal_prediction_get_heated_temperature_minutes(
    float minutes);

float thermal_prediction_get_temperature_minutes_state(
    float minutes,
    bool heating);
    
/*
 * Indique si la prédiction courante est valide.
 */
bool thermal_prediction_is_valid(void);

/*==========================================================
 * Debug
 *=========================================================*/

void thermal_prediction_dump(void);

/*==========================================================
 * Scheduler
 *=========================================================*/

void thermal_prediction_task_callback(void);

#endif /* THERMAL_PREDICTION_H */
