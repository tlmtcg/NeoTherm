#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include <stdbool.h>

/*
 *==========================================================
 * Initialisation
 *==========================================================
 */

bool time_service_init(void);

/*
 *==========================================================
 * Synchronisation
 *==========================================================
 */

/*
 * Synchronise l'horloge NeoTherm avec
 * la source de temps (PC aujourd'hui,
 * NTP/RTC sur ESP32).
 */
bool time_service_sync(void);

/*
 *==========================================================
 * Etat
 *==========================================================
 */

bool time_service_is_available(void);

/*
 *==========================================================
 * Debug
 *==========================================================
 */

void time_service_dump(void);

#endif