#include "app_config_internal.h"

#include "ini.h"

void app_config_load_schedule(void)
{
    int value;


    /*
     * Valeurs par défaut
     */

    if (!ini_key_exists(
            "schedule",
            "max_points_per_day"))
    {
        ini_set_int(
            "schedule",
            "max_points_per_day",
            10);
    }


    if (!ini_key_exists(
            "schedule",
            "default_setpoint"))
    {
        ini_set_float(
            "schedule",
            "default_setpoint",
            17.0f);
    }



    /*
     * Chargement configuration
     */

    if (ini_get_int(
            "schedule",
            "max_points_per_day",
            &value))
    {
        s_config.schedule_max_points =
            (uint32_t)value;
    }


    (void)ini_get_float(
        "schedule",
        "default_setpoint",
        &s_config.schedule_default_setpoint);
}
