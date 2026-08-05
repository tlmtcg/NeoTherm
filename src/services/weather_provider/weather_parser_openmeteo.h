#ifndef WEATHER_PARSER_OPENMETEO_H
#define WEATHER_PARSER_OPENMETEO_H

#include <stdbool.h>

#include "../core/weather/weather.h"


/*==========================================================
 * Parse une réponse JSON Open-Meteo
 *=========================================================*/

/**
 * @brief Convertit un JSON Open-Meteo en structure weather_t
 *
 * @param json Réponse JSON complète
 * @param weather Structure météo à remplir
 *
 * @return true si parsing réussi
 */
bool weather_parse_openmeteo(
    const char *json,
    weather_t *weather);


#endif /* WEATHER_PARSER_OPENMETEO_H */
