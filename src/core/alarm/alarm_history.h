#ifndef ALARM_HISTORY_H
#define ALARM_HISTORY_H

#include <stdint.h>
#include <stdbool.h>

#include "alarm.h"

#define ALARM_HISTORY_SIZE 50

typedef struct
{
    alarm_type_t type;

    alarm_state_t state;

    float value;

    uint32_t timestamp;

} alarm_history_entry_t;



typedef struct alarm_history_t
{
    alarm_history_entry_t entries[ALARM_HISTORY_SIZE];
    uint32_t head;
    uint32_t count;
    bool dirty;

} alarm_history_t;

/* Initialisation */
void alarm_history_init(void);


/* Ajout d'une entrée */
void alarm_history_add(
    alarm_type_t type,
    alarm_state_t state,
    float value);


/* Nombre d'entrées */
uint32_t alarm_history_count(void);


/* Lecture d'une entrée */
const alarm_history_entry_t *alarm_history_get(
    uint32_t index);


/* Affichage console */
void alarm_history_dump(void);

bool alarm_history_clear(void);

bool alarm_history_save(void);

bool alarm_history_load(void);

void alarm_history_task(void);

bool alarm_history_is_dirty(void);

#endif
