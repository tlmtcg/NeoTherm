#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include <stdbool.h>

/*==========================================================
 * Initialisation du système
 *=========================================================*/

/*
 * Initialise l'ensemble des modules du système (logger, debug,
 * app_config, storage, runtime, clock, event, thermal, climate,
 * relay, history, program, thermostat, scheduler).
 * Retourne true en cas de succès, false sinon.
 */
bool system_init(void);

#endif /* SYSTEM_INIT_H */