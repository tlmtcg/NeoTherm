#include "app_config.h"

#include <string.h>

#include "ini.h"
#include "logger.h"
#include "app_config_internal.h"
#include "console_utils.h"

#include <stdio.h>

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
        .thermal_outside_temperature = 5.0f,
        .thermal_heat_power = 0.25f,
        .thermal_loss_factor = 0.01f,
        .thermal_mass = 8.0f,

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

void app_config_dump(void)
{
    const app_config_t *cfg = app_config_get();

    console_print_header("Application Configuration");

    console_print_string(
        "Logger level",
        cfg->logger_level);

    console_print_uint(
        "Relay GPIO",
        cfg->relay_gpio);

    console_print_uint(
        "Relay delay",
        cfg->relay_default_switch_delay);

    console_print_uint(
        "Climate period",
        cfg->climate_period);

    console_print_uint(
        "Thermostat period",
        cfg->thermostat_period);

    console_print_uint(
        "History save",
        cfg->history_save_period);

    console_print_uint(
        "History CSV",
        cfg->history_csv_period);

    console_print_string(
        "Runtime file",
        cfg->runtime_file);

    console_print_string(
        "Schedule file",
        cfg->schedule_file);

    console_print_string(
        "History file",
        cfg->history_file);

    console_print_string(
        "History CSV",
        cfg->history_csv_file);

    console_print_uint(
        "History max records",
        cfg->history_max_records);

    console_print_uint(
        "Schedule max points",
        cfg->schedule_max_points);

    console_print_float(
        "Default setpoint",
        cfg->schedule_default_setpoint,
        "C");

    console_print_float(
        "Initial temperature",
        cfg->climate_initial_temperature,
        "C");

    console_print_bool(
        "Debug enabled",
        cfg->debug_enabled);

    console_print_bool(
        "Dump config",
        cfg->debug_dump_config);

    console_print_float(
        "Outside temperature",
        cfg->thermal_outside_temperature,
        "C");

    console_print_float(
        "Heat power",
        cfg->thermal_heat_power,
        "C/tick");

    console_print_float(
        "Loss factor",
        cfg->thermal_loss_factor,
        "");

    console_print_float(
        "Thermal mass",
        cfg->thermal_mass,
        "");

    console_print_separator();
}
