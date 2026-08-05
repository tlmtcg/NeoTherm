#include "storage_service.h"

#include "storage.h"
#include "logger.h"

static bool s_initialized = false;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool storage_service_init(void)
{
    s_initialized = true;

    LOG_INFO("STORAGE_SERVICE",
             "Storage service initialized");

    return true;
}

/*==========================================================
 * Runtime
 *=========================================================*/

storage_load_result_t storage_service_load_runtime(runtime_config_t *cfg)
{
    if (!s_initialized)
        return STORAGE_LOAD_ERROR;

    return storage_load_runtime(cfg);
}


bool storage_service_save_runtime(
    const runtime_config_t *cfg)
{
    if (!s_initialized)
    {
        LOG_ERROR("STORAGE_SERVICE",
                  "Service not initialized");

        return false;
    }

    return storage_save_runtime(cfg);
}

/*==========================================================
 * Debug
 *=========================================================*/

void storage_service_dump(void)
{
    LOG_INFO("STORAGE_SERVICE",
             "Initialized : %s",
             s_initialized ? "YES" : "NO");
}
