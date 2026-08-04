#include "climate.h"

#include "event.h"
#include "logger.h"
#include "relay.h"
#include "thermal_model.h"
#include "runtime.h"

static float s_temperature = 20.5f;

static bool s_simulation_override = false;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool climate_init(void)
{
    s_temperature = runtime_get()->setpoint;

    LOG_INFO("CLIMATE",
             "Climate initialized : %.1f C",
             s_temperature);

    return true;
}

/*==========================================================
 * Mise à jour climat
 *=========================================================*/

void climate_update(float temperature)
{
    s_temperature = temperature;
    s_simulation_override = false;

    event_t event = {0};

    event.type =
        EVENT_CLIMATE_UPDATE;

    event.data.temperature =
        s_temperature;

    if (event_post(&event))
    {
        LOG_DEBUG("CLIMATE",
                 "Temperature = %.2f C",
                 s_temperature);
    }
}

/*==========================================================
 * Lecture température
 *=========================================================*/

float climate_get_temperature(void)
{
    return s_temperature;
}

void climate_tick(void)
{

    if (s_simulation_override)
    {
        return;
    }

    float heat = 0.0f;

    if (relay_get())
    {
        heat = 1.0f;
    }

    s_temperature =
        thermal_model_update(
            s_temperature,
            relay_get());

    event_t event = {0};

    event.type =
        EVENT_CLIMATE_UPDATE;

    event.data.temperature =
        s_temperature;

    event_post(&event);

    LOG_DEBUG("CLIMATE",
             "Temperature = %.2f C",
             s_temperature);
}

#ifdef UNIT_TEST

void climate_test_set_temperature(float temperature)
{
    s_temperature = temperature;

    LOG_INFO("CLIMATE",
             "Test temperature injected : %.2f C",
             temperature);
}

#endif