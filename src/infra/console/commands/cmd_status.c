#include "cmd_status.h"

#include <stdio.h>

#include "alarm.h"
#include "climate.h"
#include "clock.h"
#include "console_utils.h"
#include "relay.h"
#include "schedule.h"
#include "thermal_model.h"
#include "thermostat.h"

bool cmd_status(const char *args)
{
    (void)args;

    clock_time_t now;
    schedule_next_t next;

    console_print_header("System Status");

    /*
     * Time
     */
    if (clock_get_time(&now))
    {
        console_print_datetime(
            "Time",
            &now);
    }

    console_print_separator();

    /*
     * Thermostat
     */
    console_print_string(
        "Mode",
        thermostat_mode_to_string(
            thermostat_get_mode()));

    console_print_float(
        "Temperature",
        climate_get_temperature(),
        "C");

    console_print_float(
        "Outside",
        thermal_model_get_outside_temperature(),
        "C");

    console_print_float(
        "Setpoint",
        thermostat_get_setpoint(),
        "C");

    console_print_float(
        "Program",
        schedule_get_setpoint(),
        "C");

    if (schedule_get_next(&next))
    {
        char text[64];

        snprintf(text,
                 sizeof(text),
                 "%s %02u:%02u -> %.1f C",
                 next.day,
                 next.hour,
                 next.minute,
                 next.setpoint);

        console_print_string(
            "Next schedule",
            text);
    }
    else
    {
        console_print_string(
            "Next schedule",
            "None");
    }

    console_print_bool(
        "Heating request",
        thermostat_get_status()->heating_request);

    console_print_separator();

    /*
     * Relay
     */
    console_print_string(
        "Relay",
        relay_get() ? "ON" : "OFF");

    console_print_uint(
        "Switch count",
        relay_get_switch_count());

    console_print_uint(
        "Relay delay",
        relay_get_min_switch_delay());

    console_print_uint(
        "Elapsed delay",
        relay_get_elapsed_delay());

    console_print_uint(
        "Remaining delay",
        relay_get_remaining_delay());

    console_print_separator();

    /*
     * Thermal model
     */
    console_print_float(
        "Heat power",
        thermal_model_get_heat_power(),
        "C/tick");

    console_print_float(
        "Loss factor",
        thermal_model_get_loss_factor(),
        "");

    console_print_float(
        "Thermal mass",
        thermal_model_get_thermal_mass(),
        "");

    console_print_separator();

    /*
     * Alarms
     */
    printf("Alarms\n");
    console_print_separator();

    alarm_dump();

    return true;
}
