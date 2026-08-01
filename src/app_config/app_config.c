#include "app_config.h"

#include <string.h>

#include "ini.h"
#include "logger.h"
#include "app_config_internal.h"

app_config_t s_config =
    {
        /* Logger */
        .logger_level = "INFO",

        /* Relay */
        .relay_gpio = 4,
        .relay_default_switch_delay = 180,

        /* Scheduler */
        .climate_period = 10,
        .thermostat_period = 10,
        .history_save_period = 60,
        .history_csv_period = 300,

        /* Files */
        .runtime_file = "runtime.ini",
        .schedule_file = "program.ini",
        .history_file = "history.dat",
        .history_csv_file = "history.csv",

        /* History */
        .history_max_records = 1000,

        /* Schedule */
        .schedule_max_points = 10,
        .schedule_default_setpoint = 17.0f,

        /* Climate */
        .climate_initial_temperature = 20.5f,

        /* Debug */
        .debug_enabled = true,
        .debug_dump_config = false,

        /* Thermal */
        .thermal_outside_temperature=5.0f,
        .thermal_heat_power=0.25f,
        .thermal_loss_factor=0.01f,
        .thermal_mass=8.0f,

};

bool app_config_init(const char *filename)
{
    if (filename == NULL)
    {
        LOG_ERROR("APP_CONFIG",
                  "Filename is NULL");
        return false;
    }

    if (!ini_init(filename))
    {
        LOG_ERROR("APP_CONFIG",
                  "Unable to load %s",
                  filename);
        return false;
    }

    app_config_load_logger();
    app_config_load_relay();
    app_config_load_scheduler();
    app_config_load_files();
    app_config_load_history();
    app_config_load_schedule();
    app_config_load_climate();
    app_config_load_debug();
    app_config_load_thermal_model();

    LOG_INFO("APP_CONFIG",
             "Application configuration loaded");

    return true;
}

const app_config_t *app_config_get(void)
{
    return &s_config;
}

bool app_config_set_float(
    const char *section,
    const char *key,
    float value)
{
    if ((section == NULL) || (key == NULL))
    {
        return false;
    }

    return ini_set_float(
        section,
        key,
        value);
}
