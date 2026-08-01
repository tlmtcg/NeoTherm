#include "app_config_internal.h"

#include "ini.h"

void app_config_load_climate(void)
{
    /*
     * Valeur par défaut
     */

    if (!ini_key_exists(
            "climate",
            "initial_temperature"))
    {
        ini_set_float(
            "climate",
            "initial_temperature",
            20.5f);
    }


    /*
     * Chargement configuration
     */

    (void)ini_get_float(
        "climate",
        "initial_temperature",
        &s_config.climate_initial_temperature);
}
