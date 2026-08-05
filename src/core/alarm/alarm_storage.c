#include "alarm_storage.h"

#include "alarm_history.h"

#include <string.h>

#include "logger.h"

static alarm_history_t s_storage;


/*
 * Initialisation
 */
bool alarm_storage_init(void)
{
    memset(&s_storage,
           0,
           sizeof(s_storage));


    LOG_INFO("ALARM_STORAGE",
             "Alarm storage initialized");


    return true;
}


/*
 * Sauvegarde
 */
bool alarm_storage_save(
    const alarm_history_t *history)
{
    if (history == NULL)
    {
        return false;
    }


    memcpy(&s_storage,
           history,
           sizeof(alarm_history_t));


    LOG_DEBUG("ALARM_STORAGE",
              "Alarm history saved");


    return true;
}


/*
 * Chargement
 */
bool alarm_storage_load(
    alarm_history_t *history)
{
    if (history == NULL)
    {
        return false;
    }


    memcpy(history,
           &s_storage,
           sizeof(alarm_history_t));


    LOG_DEBUG("ALARM_STORAGE",
              "Alarm history loaded");


    return true;
}


/*
 * Effacement
 */
bool alarm_storage_clear(void)
{
    memset(&s_storage,
           0,
           sizeof(s_storage));


    LOG_INFO("ALARM_STORAGE",
             "Alarm storage cleared");


    return true;
}
