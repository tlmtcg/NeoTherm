#include "app_config_internal.h"

#include "ini.h"

void app_config_load_debug(void)
{
    /*
     * Valeurs par défaut
     */

    if (!ini_key_exists(
            "debug",
            "debug_enabled"))
    {
        ini_set_bool(
            "debug",
            "debug_enabled",
            true);
    }


    if (!ini_key_exists(
            "debug",
            "debug_dump_config"))
    {
        ini_set_bool(
            "debug",
            "debugdump_config",
            false);
    }


    /*
     * Chargement configuration
     */

    (void)ini_get_bool(
        "debug",
        "debug_enabled",
        &s_config.debug_enabled);


    (void)ini_get_bool(
        "debug",
        "debug_dump_config",
        &s_config.debug_dump_config);
}
