#include "app_config_internal.h"

#include "ini.h"

void app_config_load_thermal_model(void)
{
    float value;


    if (!ini_key_exists(
            "thermal",
            "outside_temperature"))
    {
        ini_set_float(
            "thermal",
            "outside_temperature",
            5.0f);
    }


    if (ini_get_float(
            "thermal",
            "outside_temperature",
            &value))
    {
        s_config.thermal_outside_temperature =
            value;
    }



    if (!ini_key_exists(
            "thermal",
            "heat_power"))
    {
        ini_set_float(
            "thermal",
            "heat_power",
            0.25f);
    }


    if (ini_get_float(
            "thermal",
            "heat_power",
            &value))
    {
        s_config.thermal_heat_power =
            value;
    }
}
