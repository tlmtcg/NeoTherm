#ifndef WEATHER_PROVIDER_SIMULATOR_H
#define WEATHER_PROVIDER_SIMULATOR_H

#include <stdbool.h>

#include "../core/weather/weather.h"

/*==========================================================
 * Initialisation
 *=========================================================*/

bool weather_provider_simulator_init(void);

/*==========================================================
 * Acquisition météo
 *=========================================================*/

bool weather_provider_simulator_fetch(
    weather_t *weather);

#endif /* WEATHER_PROVIDER_SIMULATOR_H */
