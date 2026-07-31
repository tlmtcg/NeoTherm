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

/**
 * @brief Modifie ou ajoute un point de consigne pour un jour et une heure donnés.
 * @param day Jour de la semaine (0 = lundi, ..., 6 = dimanche)
 * @param hour Heure (0-23)
 * @param minute Minute (0-59)
 * @param setpoint Température de consigne
 * @return true si succès, false si échec (ex: tableau plein)
 */
bool schedule_set_point(uint8_t day, uint8_t hour, uint8_t minute, float setpoint);


#endif