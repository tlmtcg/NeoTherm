#include "history.h"
#include "logger.h"
#include "clock.h"

#include <string.h>

/*==========================================================
 * Variables privées
 *=========================================================*/

static history_record_t s_history[HISTORY_SIZE];

static uint32_t s_head = 0;

static uint32_t s_count = 0;

static uint32_t s_tick = 0;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool history_init(void)
{
    memset(
        s_history,
        0,
        sizeof(s_history));

    s_head = 0;

    s_count = 0;

    s_tick = 0;

    LOG_INFO("HISTORY",
             "History initialized (%u records)",
             HISTORY_SIZE);

    return true;
}

/*==========================================================
 * Ajout d'un enregistrement
 *=========================================================*/

bool history_add(
    float inside_temperature,
    float outside_temperature,
    float setpoint,
    thermostat_mode_t mode,
    bool relay,
    bool heating)
{

    history_record_t *record =
        &s_history[s_head];

    /*
     * Index simulation
     */
    record->tick = s_tick++;

    LOG_INFO("HISTORY",
             "Add tick=%u count=%u head=%u",
             s_tick,
             s_count,
             s_head);
             
    /*
     * Horodatage
     */
    if (!clock_get_time(
            &record->timestamp))
    {
        LOG_ERROR("HISTORY",
                  "Unable to get clock time");

        return false;
    }

    /*
     * Valeurs thermiques
     */
    record->inside_temperature =
        inside_temperature;

    record->outside_temperature =
        outside_temperature;

    record->setpoint =
        setpoint;

    /*
     * Etat thermostat
     */
    record->mode =
        mode;

    record->relay =
        relay;

    record->heating =
        heating;

    /*
     * Avance buffer circulaire
     */
    s_head++;

    if (s_head >= HISTORY_SIZE)
    {
        s_head = 0;
    }

    if (s_count < HISTORY_SIZE)
    {
        s_count++;
    }

    return true;
}

/*==========================================================
 * Lecture d'un enregistrement
 *=========================================================*/

bool history_get(
    uint32_t index,
    history_record_t *record)
{
    if (record == NULL)
    {
        return false;
    }

    if (index >= s_count)
    {
        return false;
    }

    uint32_t oldest;

    if (s_count < HISTORY_SIZE)
    {
        oldest = 0;
    }
    else
    {
        oldest = s_head;
    }

    uint32_t position =
        (oldest + index) % HISTORY_SIZE;

    *record = s_history[position];

    return true;
}

/*==========================================================
 * Informations
 *=========================================================*/

uint32_t history_count(void)
{
    return s_count;
}

/*==========================================================
 * Efface l'historique
 *=========================================================*/

void history_clear(void)
{
    s_head = 0;

    s_count = 0;

    s_tick = 0;

    memset(
        s_history,
        0,
        sizeof(s_history));

    LOG_INFO("HISTORY",
             "History cleared");
}

/*==========================================================
 * Dernier enregistrement
 *=========================================================*/

bool history_get_latest(
    history_record_t *record)
{
    if (record == NULL)
    {
        return false;
    }

    if (s_count == 0)
    {
        return false;
    }

    uint32_t index;

    /*
     * s_head pointe toujours sur la prochaine case libre
     * donc le dernier élément est juste avant
     */
    if (s_head == 0)
    {
        index = HISTORY_SIZE - 1;
    }
    else
    {
        index = s_head - 1;
    }

    *record = s_history[index];

    return true;
}

/*==========================================================
 * Push un enregistrement
 *=========================================================*/

bool history_push(
    const history_record_t *record)
{
    if (record == NULL)
    {
        return false;
    }

    s_history[s_head] = *record;

    s_head = (s_head + 1U) % HISTORY_SIZE;

    if (s_count < HISTORY_SIZE)
    {
        s_count++;
    }

    if (record->tick >= s_tick)
    {
        s_tick = record->tick + 1U;
    }

    return true;
}
