#ifndef DEBUG_H
#define DEBUG_H

#include "app_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================
 * Initialisation
 *=========================================================*/

void debug_init(void);

/*==========================================================
 * Dump des composants
 *=========================================================*/

/*
 * Affiche la configuration de l'application.
 */
void debug_dump_app_config(
    const app_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */
