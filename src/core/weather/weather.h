#ifndef WEATHER_H
#define WEATHER_H

#include <stdbool.h>


typedef struct
{
    bool valid;

    float temperature;
    float humidity;
    float pressure;

    float wind_speed;
    float wind_gust;
    float wind_direction;

    float rain;

} weather_t;



bool weather_init(void);


bool weather_set(
        const weather_t *data);


const weather_t *weather_get(void);


bool weather_is_valid(void);


void weather_clear(void);

void weather_dump(void);

void weather_invalidate(void);

#endif
