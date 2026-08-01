#include "app_config.h"

#include <string.h>

#include "ini.h"
#include "logger.h"

static app_config_t s_config =
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

static void app_config_load_logger(void)
{
    (void)ini_get_string(
        "logger",
        "level",
        s_config.logger_level,
        sizeof(s_config.logger_level));
}

static void app_config_load_relay(void)
{
    int value;

    if (ini_get_int("relay", "gpio", &value))
    {
        s_config.relay_gpio = (uint32_t)value;
    }

    if (ini_get_int("relay", "default_switch_delay", &value))
    {
        s_config.relay_default_switch_delay = (uint32_t)value;
    }
}

static void app_config_load_scheduler(void)
{
    int value;

    if (ini_get_int("scheduler", "climate_period", &value))
        s_config.climate_period = (uint32_t)value;

    if (ini_get_int("scheduler", "thermostat_period", &value))
        s_config.thermostat_period = (uint32_t)value;

    if (ini_get_int("scheduler", "history_save_period", &value))
        s_config.history_save_period = (uint32_t)value;

    if (ini_get_int("scheduler", "history_csv_period", &value))
        s_config.history_csv_period = (uint32_t)value;
}

static void app_config_load_files(void)
{
    (void)ini_get_string(
        "files",
        "runtime",
        s_config.runtime_file,
        sizeof(s_config.runtime_file));

    (void)ini_get_string(
        "files",
        "schedule",
        s_config.schedule_file,
        sizeof(s_config.schedule_file));

    (void)ini_get_string(
        "files",
        "history",
        s_config.history_file,
        sizeof(s_config.history_file));

    (void)ini_get_string(
        "files",
        "history_csv",
        s_config.history_csv_file,
        sizeof(s_config.history_csv_file));
}

/*==========================================================
 * History
 *=========================================================*/

static void app_config_load_history(void)
{
    int value;

    if (ini_get_int("history",
                    "max_records",
                    &value))
    {
        s_config.history_max_records = (uint32_t)value;
    }
}

/*==========================================================
 * Schedule
 *=========================================================*/

static void app_config_load_schedule(void)
{
    int value;

    if (ini_get_int("schedule",
                    "max_points_per_day",
                    &value))
    {
        s_config.schedule_max_points = (uint32_t)value;
    }

    (void)ini_get_float(
        "schedule",
        "default_setpoint",
        &s_config.schedule_default_setpoint);
}

/*==========================================================
 * Climate
 *=========================================================*/

static void app_config_load_climate(void)
{
    (void)ini_get_float(
        "climate",
        "initial_temperature",
        &s_config.climate_initial_temperature);
}

/*==========================================================
 * Debug
 *=========================================================*/

static void app_config_load_debug(void)
{
    (void)ini_get_bool(
        "debug",
        "enabled",
        &s_config.debug_enabled);

    (void)ini_get_bool(
        "debug",
        "dump_config",
        &s_config.debug_dump_config);
}

static void app_config_load_thermal_model(void)
{
    float value;

    if (ini_get_float("thermal", "outside_temperature", &value))
        s_config.thermal_outside_temperature = value;

    if (ini_get_float("thermal", "heat_power", &value))
        s_config.thermal_heat_power = value;

    if (ini_get_float("thermal", "loss_factor", &value))
        s_config.thermal_loss_factor = value;

    if (ini_get_float("thermal", "thermal_mass", &value))
        s_config.thermal_mass = value;
}

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