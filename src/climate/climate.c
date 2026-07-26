#include "climate.h"

#include "event.h"
#include "logger.h"


static float s_temperature = 20.0f;


/*==========================================================
 * Initialisation
 *=========================================================*/

void climate_init(void)
{
    s_temperature = 20.0f;

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

    s_temperature += 0.1f;


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
                 "Temperature = %.1f C",
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
