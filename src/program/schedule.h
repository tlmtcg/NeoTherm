#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <stdint.h>
#include <stdbool.h>

#include "clock.h"


#define SCHEDULE_MAX_POINTS 8


typedef struct
{
    uint8_t hour;
    uint8_t minute;

    float setpoint;

} schedule_point_t;


typedef struct
{
    uint8_t day;

    uint8_t count;

    schedule_point_t points[SCHEDULE_MAX_POINTS];

} schedule_day_t;



bool schedule_init(void);


float schedule_get_setpoint(void);


#endif