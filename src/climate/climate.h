#ifndef CLIMATE_H
#define CLIMATE_H

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Initialisation du simulateur climatique
 */
void climate_init(void);


/*
 * Mise à jour du climat
 * Appelée périodiquement
 */
void climate_update(void);


/*
 * Retourne la température actuelle
 */
float climate_get_temperature(void);

void climate_test_set_temperature(float temperature);

#ifdef __cplusplus
}
#endif

#endif /* CLIMATE_H */