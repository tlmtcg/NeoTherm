#include "app_config_internal.h"

#include "ini.h"

void app_config_load_history(void)
{
    int value;


    /*
     * Valeur par défaut
     */

    if (!ini_key_exists(
            "history",
            "max_records"))
    {
        ini_set_int(
            "history",
            "max_records",
            1000);
    }


    /*
     * Chargement configuration
     */

    if (ini_get_int(
            "history",
            "max_records",
            &value))
    {
        s_config.history_max_records =
            (uint32_t)value;
    }
}
