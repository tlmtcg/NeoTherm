#ifndef CLIMATE_H
#define CLIMATE_H

#include "app.h" // pour UNIT_TEST
 
bool climate_init(void);

float climate_get_temperature(void);

/*
 * Mise à jour depuis un capteur
 * ou une simulation externe
 */
void climate_update(float temperature);


/*
 * Evolution du modèle thermique
 */
void climate_tick(void);

/*
 * API TEST uniquement
 */
#ifdef UNIT_TEST
void climate_test_set_temperature(float temperature);
#endif

#endif
