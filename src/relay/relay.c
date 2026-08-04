#include "relay.h"
#include "logger.h"
#include "clock.h"
#include "runtime.h"


static bool     s_relay_state      = false;
static uint32_t s_switch_count     = 0;
static uint32_t s_min_switch_delay = 0;
static uint32_t s_last_switch_time = 0;
static bool     s_first_switch     = true;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool relay_init(void)
{
    s_relay_state = false;
    s_switch_count = 0;
    s_last_switch_time = 0;
    s_first_switch = true;

    s_min_switch_delay =
        runtime_get()->relay_delay;

    LOG_INFO("RELAY",
             "Relay initialized : OFF (delay=%u s)",
             s_min_switch_delay);

    return true;
}

/*==========================================================
 * Commande
 *=========================================================*/

bool relay_set(bool state)
{
    /* Pas de changement inutile */
    if (s_relay_state == state)
    {
        return true;
    }

    /* Protection anti-cycles courts */
    if (!relay_can_switch())
    {
        LOG_WARN("RELAY", "Switch refused : minimum delay not elapsed");
        return false;
    }

    s_relay_state = state;
    s_switch_count++;
    s_last_switch_time = clock_seconds_today();
    s_first_switch = false;

    LOG_INFO("RELAY", "Relay switched at %u", s_last_switch_time);
    LOG_INFO("RELAY", "Relay %s", state ? "ON" : "OFF");

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
    /* 
     * Si c'est le tout premier basculement, l'anti-cycle est désactivé.
     * On initialise directement s_last_switch_time à l'heure courante 
     * pour éviter un décalage à 0 si l'horloge a déjà été réglée.
     */
    if (s_first_switch)
    {
        return true;
    }

    uint32_t now = clock_seconds_today();
    uint32_t delta;

    if (now >= s_last_switch_time)
    {
        delta = now - s_last_switch_time;
    }
    else
    {
        delta = (SECONDS_PER_DAY - s_last_switch_time) + now;
    }

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

    LOG_INFO("RELAY", "Min switch delay set : %u s", seconds);
}

uint32_t relay_get_min_switch_delay(void)
{
    return s_min_switch_delay;
}


void relay_test_reset(void)
{
    relay_init();

    relay_set_min_switch_delay(0);

    relay_reset_switch_count();
}

/* API */
void relay_get_status(relay_status_t *status)
{
    if (status == NULL)
    {
        return;
    }

    status->state = s_relay_state;
    status->switch_count = s_switch_count;
    status->last_switch_time = s_last_switch_time;
    status->min_switch_delay = s_min_switch_delay;

    if (s_first_switch)
    {
        status->elapsed_delay = 0;
        status->remaining_delay = 0;
        status->can_switch = true;
        return;
    }

    uint32_t now = clock_seconds_today();
    uint32_t elapsed;

    if (now >= s_last_switch_time)
    {
        elapsed = now - s_last_switch_time;
    }
    else
    {
        elapsed = (SECONDS_PER_DAY - s_last_switch_time) + now;
    }

    status->elapsed_delay = elapsed;

    if (elapsed >= s_min_switch_delay)
    {
        status->remaining_delay = 0;
        status->can_switch = true;
    }
    else
    {
        status->remaining_delay =
            s_min_switch_delay - elapsed;

        status->can_switch = false;
    }
}

uint32_t relay_get_elapsed_delay(void)
{
    if (s_first_switch)
    {
        return 0;
    }

    uint32_t now = clock_seconds_today();

    if (now >= s_last_switch_time)
    {
        return now - s_last_switch_time;
    }

    return (SECONDS_PER_DAY - s_last_switch_time) + now;
}

uint32_t relay_get_remaining_delay(void)
{
    uint32_t elapsed = relay_get_elapsed_delay();

    if (elapsed >= s_min_switch_delay)
    {
        return 0;
    }

    return s_min_switch_delay - elapsed;
}

