#ifndef STORAGE_SERVICE_H
#define STORAGE_SERVICE_H

#include <stdbool.h>

#include "runtime.h"

bool storage_service_init(void);

storage_load_result_t storage_service_load_runtime(runtime_config_t *cfg);

bool storage_service_save_runtime(const runtime_config_t *cfg);

void storage_service_dump(void);

#endif
