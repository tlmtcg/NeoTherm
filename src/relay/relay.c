#include "relay.h"

#include "logger.h"
#include "event.h"


static bool s_relay_state = false;


/*==========================================================
 * Initialisation
 *=========================================================*/

bool relay_init(void)
{
    s_relay_state = false;

    LOG_INFO("RELAY",
             "Relay initialized : OFF");

    return true;
}


/*==========================================================
 * Commande
 *=========================================================*/

bool relay_set(bool state)
{
    if (s_relay_state == state)
    {
        return true;
    }

    s_relay_state = state;


    if (state)
    {
        LOG_INFO("RELAY",
                 "Relay ON");

        event_t event;

        event.type = EVENT_RELAY_ON;

        event_post(&event);
    }
    else
    {
        LOG_INFO("RELAY",
                 "Relay OFF");

        event_t event;

        event.type = EVENT_RELAY_OFF;

        event_post(&event);
    }


    return true;
}


bool relay_toggle(void)
{
    return relay_set(!s_relay_state);
}


/*==========================================================
 * Lecture
 *=========================================================*/

bool relay_get(void)
{
    return s_relay_state;
}
