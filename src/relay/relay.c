#include "relay.h"

#include "logger.h"
#include "clock.h"

#define RELAY_DEFAULT_SWITCH_DELAY 180

static bool s_relay_state = false;

static uint32_t s_switch_count = 0;

static uint32_t s_min_switch_delay = RELAY_DEFAULT_SWITCH_DELAY;

static uint32_t s_last_switch_time = 0;

static bool s_first_switch = true;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool relay_init(void)
{
    s_relay_state = false;

    s_switch_count = 0;

    s_min_switch_delay =
        RELAY_DEFAULT_SWITCH_DELAY;

    s_last_switch_time = 0;

    s_first_switch = true;

    LOG_INFO("RELAY",
             "Relay initialized : OFF");

    return true;
}

/*==========================================================
 * Commande
 *=========================================================*/

bool relay_set(bool state)
{
    /*
     * Pas de changement inutile
     */
    if (s_relay_state == state)
    {
        return true;
    }

    /*
     * Protection anti-cycles courts
     */
    if (!relay_can_switch())
    {
        LOG_WARN("RELAY",
                 "Switch refused : minimum delay not elapsed");

        return false;
    }

    s_relay_state = state;

    s_switch_count++;

    s_last_switch_time =
        clock_seconds_today();

    s_first_switch = false;

    LOG_INFO("RELAY",
             "Relay switched at %u",
             s_last_switch_time);

    LOG_INFO("RELAY",
             "Relay %s",
             state ? "ON" : "OFF");

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

uint32_t relay_get_switch_count(void)
{
    return s_switch_count;
}

void relay_reset_switch_count(void)
{
    s_switch_count = 0;
}

/*==========================================================
 * Protection
 *=========================================================*/

bool relay_can_switch(void)
{
    if (s_first_switch)
    {
        return true;
    }

    uint32_t now =
        clock_seconds_today();

    uint32_t delta =
        now - s_last_switch_time;

    LOG_DEBUG("RELAY",
              "now=%u last=%u delta=%u delay=%u",
              now,
              s_last_switch_time,
              delta,
              s_min_switch_delay);

    return delta >= s_min_switch_delay;
}

void relay_set_min_switch_delay(uint32_t seconds)
{
    s_min_switch_delay = seconds;

    LOG_INFO("RELAY",
             "Min switch delay set : %u s",
             seconds);
}

uint32_t relay_get_min_switch_delay(void)
{
    return s_min_switch_delay;
}
