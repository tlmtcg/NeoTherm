#ifndef THERMAL_LEARNING_H
#define THERMAL_LEARNING_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    /*
     * Résultats appris
     */
    float heat_rate;    // °C / tick
    float cooling_rate; // °C / tick
    float warming_rate; // °C / tick
    float overshoot;    // °C

    /*
     * Statistiques
     */
    uint32_t heating_samples;
    uint32_t cooling_samples;
    uint32_t warming_samples;

    /*
     * Etat interne
     */
    bool was_heating;
    float last_temperature;
    float last_setpoint;

    float confidence;

} thermal_learning_state_t;

bool thermal_learning_init(void);

/*
 * Analyse l'historique complet
 */
bool thermal_learning_analyze(void);

void thermal_learning_task_callback(void);

const thermal_learning_state_t *
thermal_learning_get_state(void);

void thermal_learning_dump(void);

/*
 * Getters
 */
float thermal_learning_get_heat_rate(void);
float thermal_learning_get_cooling_rate(void);
float thermal_learning_get_warming_rate(void);
float thermal_learning_get_overshoot(void);

/*
 * Application au modèle thermique
 */
void thermal_model_apply_learning(void);

/*
 * Analyse / mise à jour du learning
 */
bool thermal_learning_update(void);

/*
 * Validation du learning
 */
bool thermal_learning_is_valid(void);
bool thermal_learning_is_heat_rate_valid(void);
bool thermal_learning_is_cooling_rate_valid(void);
bool thermal_learning_is_warming_rate_valid(void);

#endif
