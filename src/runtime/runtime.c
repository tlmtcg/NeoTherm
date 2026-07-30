#include "runtime.h"
#include "storage.h"

static runtime_config_t s_runtime =
    {
        .mode = THERMOSTAT_AUTO,
        .setpoint = 19.5f,
        .hysteresis = 0.2f,
        .relay_delay = 180,
        .latitude = 50.681f,
        .longitude = 3.154f};

bool runtime_init(void)
{
    runtime_load();

    return true;
}

// getter
const runtime_config_t *runtime_get(void)
{
    return &s_runtime;
}

// setters
bool runtime_set_setpoint(float value)
{
    s_runtime.setpoint = value;

    return true;
}

bool runtime_set_mode(thermostat_mode_t mode)
{
    s_runtime.mode = mode;
}

bool runtime_set_hysteresis(float value)
{
    s_runtime.hysteresis = value;
}

bool runtime_set_relay_delay(uint32_t seconds)
{
    s_runtime.relay_delay = seconds;
}

bool runtime_set_location(float latitude,
                          float longitude)
{
    s_runtime.latitude = latitude;
    s_runtime.longitude = longitude;
}

bool runtime_save(void)
{
    storage_save_runtime(&s_runtime);
}

bool runtime_load(void)
{
    runtime_config_t cfg;

    if (storage_load_runtime(&cfg))
    {
        s_runtime = cfg;
    }
}