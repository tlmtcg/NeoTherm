#include "cmd_thermostat.h"
#include "console_utils.h"

bool cmd_thermostat(const char *args)
{
    const thermostat_status_t *status =
        thermostat_get_status();

    console_print_header("Thermostat Status");

    console_print_string(
        "Mode",
        thermostat_mode_to_string(status->mode));

    console_print_float(
        "Temperature",
        status->temperature,
        "C");

    console_print_float(
        "Setpoint",
        status->setpoint,
        "C");

    console_print_float(
        "Hysteresis",
        status->hysteresis,
        "C");

    console_print_bool(
        "Relay",
        status->relay_state);

    console_print_bool(
        "Heating",
        status->heating_request);

    console_print_separator();

    return true;
}