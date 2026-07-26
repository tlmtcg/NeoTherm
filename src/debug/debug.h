#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"

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

void debug_dump_config(const config_runtime_t *runtime);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */