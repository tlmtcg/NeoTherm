#include "runtime.h"

#include "storage.h"
#include "logger.h"

const runtime_config_t runtime_default_config =
{
    .mode          = THERMOSTAT_AUTO,
    .setpoint      = 20.5f,
    .hysteresis    = 0.2f,
    .relay_delay   = 180,
    .latitude      = 50.681f,
    .longitude     = 3.154f
};



static runtime_config_t s_runtime;

// bool runtime_init(void)
// {
//     /*
//      * Initialisation avec les valeurs par défaut
//      */
//     s_runtime = runtime_default_config;

//     /*
//      * Chargement de la configuration sauvegardée
//      */
//     if (!runtime_load())
//     {
//         LOG_ERROR("RUNTIME",
//                   "Unable to load runtime configuration");

//         return false;
//     }

//     LOG_INFO("RUNTIME",
//              "Runtime initialized");

//     return true;
// }

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



bool runtime_save(void)
{
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
                     "Runtime configuration loaded");

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
