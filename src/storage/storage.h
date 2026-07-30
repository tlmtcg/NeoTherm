#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>

#include "thermostat.h"
#include "runtime.h"

#ifdef __cplusplus
extern "C" {
#endif



bool storage_init(void);

void storage_dump(void);

bool storage_save_runtime(
        const runtime_config_t *cfg);

storage_load_result_t storage_load_runtime(
    runtime_config_t *cfg);

/**
 * @brief Supprime le fichier runtime.ini.
 *
 * Utilisé uniquement par les tests.
 */
bool storage_test_clear(void);

#ifdef __cplusplus
}
#endif

#endif