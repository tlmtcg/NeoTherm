#include "runtime.h"

#include "storage.h"
#include "logger.h"
#include "schedule.h"
#include "console_utils.h"
#include "climate.h"
#include "relay.h"
#include "thermal_model.h"
#include "../weather_service/weather_service.h"

#include <stdlib.h>
const runtime_config_t runtime_default_config =
    {
        /*
         * Thermostat
         */
        .mode = THERMOSTAT_AUTO,

        .setpoint = 20.5f,

        .hysteresis = 0.2f,

        .relay_delay = 180,

        /*
         * Weather
         */
        .latitude = 50.681f,

        .longitude = 3.154f,

        .weather_provider =
            WEATHER_PROVIDER_OPENMETEO,

        .weather_update_period_sec =
            15 * 60,

        /*
         * Clock
         */
        .date_time =
            {
                .year = 2026,
                .month = 1,
                .day = 1,
                .hour = 0,
                .minute = 0,
                .second = 0},
            
         /* Divers */
        .alarm_history_save_period = 600,

        };


static runtime_config_t s_runtime;

bool runtime_set_alarm_history_save_period(uint32_t period)
{
    s_runtime.alarm_history_save_period = period;

    return true;
}

bool runtime_init(void)
{
    s_runtime = runtime_default_config;

    return runtime_load();
}

const runtime_config_t *runtime_get(void)
{
    return &s_runtime;
}

bool runtime_set_setpoint(float value)
{
    if (value < 5.0f ||
        value > 35.0f)
    {
        return false;
    }

    s_runtime.setpoint = value;

    return true;
}

bool runtime_set_mode(thermostat_mode_t mode)
{
    if ((mode < THERMOSTAT_OFF) ||
        (mode > THERMOSTAT_HORS_GEL))
    {
        return false;
    }

    s_runtime.mode = mode;

    return true;
}

bool runtime_set_hysteresis(float value)
{
    if (value < 0)
    {
        return false;
    }

    s_runtime.hysteresis = value;

    return true;
}

bool runtime_set_relay_delay(uint32_t seconds)
{
    if (seconds == 0)
    {
        return false;
    }

    s_runtime.relay_delay = seconds;

    return true;
}

bool runtime_set_location(float latitude,
                          float longitude)
{
    if (latitude < -90 ||
        latitude > 90)
    {
        return false;
    }

    if (longitude < -180 ||
        longitude > 180)
    {
        return false;
    }

    s_runtime.latitude = latitude;
    s_runtime.longitude = longitude;

    return true;
}

bool runtime_set_weather_provider(
    weather_provider_t provider)
{
    s_runtime.weather_provider = provider;


    return runtime_save();
}

bool runtime_set_weather_update_period(
    uint32_t seconds)
{
    if (seconds == 0)
    {
        return false;
    }


    s_runtime.weather_update_period_sec =
        seconds;


    return runtime_save();
}

bool runtime_save(void)
{
    LOG_INFO("RUNTIME",
             "Saving datetime %04u-%02u-%02u %02u:%02u:%02u",
             s_runtime.date_time.year,
             s_runtime.date_time.month,
             s_runtime.date_time.day,
             s_runtime.date_time.hour,
             s_runtime.date_time.minute,
             s_runtime.date_time.second);

    return storage_save_runtime(
        &s_runtime);
}

bool runtime_load(void)
{
    runtime_config_t cfg = runtime_default_config;

    storage_load_result_t result =
        storage_load_runtime(&cfg);

    switch (result)
    {
    case STORAGE_LOAD_OK:

        s_runtime = cfg;

        LOG_INFO("RUNTIME",
                 "Runtime file loaded");

        return true;

    case STORAGE_LOAD_DEFAULT:

        s_runtime = cfg;

        LOG_WARN("RUNTIME",
                 "Using default runtime configuration");

        return true;

    case STORAGE_LOAD_ERROR:

    default:

        LOG_ERROR("RUNTIME",
                  "Unable to load runtime configuration");

        return false;
    }
}

bool runtime_set_datetime(
    const clock_time_t *time)
{
    if (time == NULL)
    {
        return false;
    }

    s_runtime.date_time = *time;

    return true;
}

void runtime_dump(void)
{
    clock_time_t now;
    schedule_next_t next;

    clock_get_time(&now);
    schedule_get_next(&next);

    console_print_header("Runtime");

    console_print_string(
        "Mode",
        thermostat_mode_name(thermostat_get_mode()));

    console_print_float(
        "Temperature",
        climate_get_temperature(),
        "C");

    console_print_float(
        "Setpoint",
        thermostat_get_setpoint(),
        "C");

    console_print_float(
        "Next setpoint",
        next.setpoint,
        "C");

    console_print_bool(
        "Heating request",
        thermostat_get_status()->heating_request);

    console_print_string(
        "Relay",
        relay_get() ? "ON" : "OFF");

    console_print_float(
        "Hysteresis",
        thermostat_get_hysteresis(),
        "C");

    console_print_uint(
        "Relay delay",
        relay_get_min_switch_delay());

    console_print_float(
        "Outside temp",
        thermal_model_get_outside_temperature(),
        "C");

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

    console_print_datetime(
        "Date/Time",
        &now);

      console_print_uint(
        "alarm_history_save period",
        s_runtime.alarm_history_save_period);

    console_print_separator();
}
