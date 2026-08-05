#ifndef WEATHER_PROVIDER_OPENMETEO_H
#define WEATHER_PROVIDER_OPENMETEO_H

#include <stdbool.h>

#include "../core/weather/weather.h"

/*==========================================================
 * Initialisation
 *=========================================================*/

bool weather_provider_openmeteo_init(void);

/*==========================================================
 * Acquisition météo
 *=========================================================*/

bool weather_provider_openmeteo_fetch(
    weather_t *weather);

#endif
