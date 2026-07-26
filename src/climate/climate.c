#include "climate.h"

#include "event.h"
#include "logger.h"
#include "relay.h"

static float s_temperature = 20.5f;

/*==========================================================
 * Initialisation
 *=========================================================*/

void climate_init(void)
{
    s_temperature = 20.5f;

    LOG_INFO("CLIMATE",
             "Climate initialized : %.1f C",
             s_temperature);
}

/*==========================================================
 * Mise à jour climat
 *=========================================================*/

void climate_update(void)
{
    /*
     * Simulation simple :
     * la température monte doucement
     */

    bool heating = relay_get();

    if (heating)
    {
        s_temperature += 0.1f;
    }
    else
    {
        s_temperature -= 0.1f;
    }

    event_t event;

    event.type = EVENT_CLIMATE_UPDATE;

    event.data.temperature = s_temperature;

    if (!event_post(&event))
    {
        LOG_ERROR("CLIMATE",
                  "Unable to post climate event");
    }
    else
    {
        LOG_INFO("CLIMATE",
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

void climate_test_set_temperature(float temperature)
{
    s_temperature = temperature;

    LOG_INFO("CLIMATE",
             "TEST temperature forced : %.1f C",
             s_temperature);
}