#ifndef WEATHER_SERVICE_H
#define WEATHER_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "weather_provider.h"

bool weather_service_init(void);

bool weather_service_update(void);

void weather_service_tick(void);

bool weather_service_set_provider(
    weather_provider_t provider);

weather_provider_t weather_service_get_provider(void);

bool weather_service_set_location(
    float latitude,
    float longitude);

bool weather_service_get_location(
    float *latitude,
    float *longitude);

bool weather_service_is_available(void);

uint32_t weather_service_age(void);

void weather_service_dump(void);

#endif
