#ifndef ALARM_STORAGE_H
#define ALARM_STORAGE_H

#include <stdbool.h>

#include "alarm_history.h"

typedef struct alarm_history_t alarm_history_t;

/*
 * Initialisation stockage alarmes
 */
bool alarm_storage_init(void);


/*
 * Sauvegarde historique
 */
bool alarm_storage_save(
    const alarm_history_t *history);


/*
 * Chargement historique
 */
bool alarm_storage_load(
    alarm_history_t *history);


/*
 * Effacement stockage
 */
bool alarm_storage_clear(void);


#endif
