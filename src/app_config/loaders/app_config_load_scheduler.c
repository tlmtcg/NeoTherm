#include "app_config_internal.h"

#include "ini.h"

void app_config_load_scheduler(void)
{
    int value;


    /*
     * Valeurs par défaut
     */

    if (!ini_key_exists(
            "scheduler",
            "climate_period"))
    {
        ini_set_int(
            "scheduler",
            "climate_period",
            10);
    }


    if (!ini_key_exists(
            "scheduler",
            "thermostat_period"))
    {
        ini_set_int(
            "scheduler",
            "thermostat_period",
            10);
    }


    if (!ini_key_exists(
            "scheduler",
            "history_save_period"))
    {
        ini_set_int(
            "scheduler",
            "history_save_period",
            60);
    }


    if (!ini_key_exists(
            "scheduler",
            "history_csv_period"))
    {
        ini_set_int(
            "scheduler",
            "history_csv_period",
            300);
    }



    /*
     * Chargement configuration
     */

    if (ini_get_int(
            "scheduler",
            "climate_period",
            &value))
    {
        s_config.climate_period =
            (uint32_t)value;
    }


    if (ini_get_int(
            "scheduler",
            "thermostat_period",
            &value))
    {
        s_config.thermostat_period =
            (uint32_t)value;
    }


    if (ini_get_int(
            "scheduler",
            "history_save_period",
            &value))
    {
        s_config.history_save_period =
            (uint32_t)value;
    }


    if (ini_get_int(
            "scheduler",
            "history_csv_period",
            &value))
    {
        s_config.history_csv_period =
            (uint32_t)value;
    }
}
