#ifndef ALARM_SERVICE_H
#define ALARM_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "alarm.h"


/*
 * Initialisation
 */
bool alarm_service_init(void);


/*
 * Nombre d'alarmes actives
 */
uint32_t alarm_service_count_active(void);


/*
 * Présence d'une alarme critique
 */
bool alarm_service_has_error(void);


/*
 * Première alarme active
 */
const alarm_t *alarm_service_get_first_active(void);


/*
 * Affichage debug
 */
void alarm_service_dump(void);


#endif