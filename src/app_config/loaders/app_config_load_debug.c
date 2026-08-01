#include "app_config_internal.h"

#include "ini.h"

void app_config_load_debug(void)
{
    /*
     * Valeurs par défaut
     */

    if (!ini_key_exists(
            "debug",
            "enabled"))
    {
        ini_set_bool(
            "debug",
            "enabled",
            true);
    }


    if (!ini_key_exists(
            "debug",
            "dump_config"))
    {
        ini_set_bool(
            "debug",
            "dump_config",
            false);
    }


    /*
     * Chargement configuration
     */

    (void)ini_get_bool(
        "debug",
        "enabled",
        &s_config.debug_enabled);


    (void)ini_get_bool(
        "debug",
        "dump_config",
        &s_config.debug_dump_config);
}
