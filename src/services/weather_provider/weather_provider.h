#ifndef WEATHER_PROVIDER_H
#define WEATHER_PROVIDER_H

#include <stdbool.h>

#include "../core/weather/weather.h"
#include <string.h>
#include <stdint.h>

typedef enum
{
    WEATHER_PROVIDER_SIMULATOR = 0,
    WEATHER_PROVIDER_OPENMETEO

} weather_provider_t;

/*==========================================================
 * Initialisation
 *=========================================================*/

/**
 * @brief Initialise le fournisseur météo.
 *
 * @return true si le fournisseur est prêt.
 */
bool weather_provider_init(void);

/*==========================================================
 * Téléchargement météo
 *=========================================================*/

/**
 * @brief Récupère les dernières données météo.
 *
 * Cette fonction remplit la structure passée en paramètre.
 * Elle ne modifie jamais directement le module weather.
 *
 * @param weather Structure à remplir.
 *
 * @return true si des données valides ont été obtenues.
 */
bool weather_provider_fetch(weather_t *weather);

/*==========================================================
 * Etat
 *=========================================================*/

/**
 * @brief Indique si le fournisseur est disponible.
 */
bool weather_provider_is_available(void);

/*==========================================================
 * Debug
 *=========================================================*/

/**
 * @brief Affiche l'état du fournisseur.
 */
void weather_provider_dump(void);

const char *weather_provider_to_string(
    weather_provider_t provider);

bool weather_provider_from_string(
    const char *text,
    weather_provider_t *provider);

bool weather_provider_set(
    weather_provider_t provider);

uint32_t weather_provider_get_fetch_count(void);

void weather_provider_reset_fetch_count(void);

#endif /* WEATHER_PROVIDER_H */
