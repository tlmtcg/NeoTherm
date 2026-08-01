#include "app_config_internal.h"

#include "ini.h"

void app_config_load_relay(void)
{
    int value;


    /*
     * Valeurs par défaut
     */

    if (!ini_key_exists(
            "relay",
            "gpio"))
    {
        ini_set_int(
            "relay",
            "gpio",
            4);
    }


    if (!ini_key_exists(
            "relay",
            "default_switch_delay"))
    {
        ini_set_int(
            "relay",
            "default_switch_delay",
            180);
    }



    /*
     * Chargement configuration
     */

    if (ini_get_int(
            "relay",
            "gpio",
            &value))
    {
        s_config.relay_gpio =
            (uint32_t)value;
    }


    if (ini_get_int(
            "relay",
            "default_switch_delay",
            &value))
    {
        s_config.relay_default_switch_delay =
            (uint32_t)value;
    }
}
