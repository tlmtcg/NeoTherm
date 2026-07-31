#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#define SCHEDULER_MAX_TASKS 16

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*scheduler_task_t)(void);

/*==========================================================
 * Informations publiques sur une tâche
 *=========================================================*/

typedef struct
{
    const char *name;
    uint32_t period;
    uint32_t counter;
    bool enabled;

} scheduler_info_t;

/*==========================================================
 * Initialisation
 *=========================================================*/

void scheduler_init(void);

bool scheduler_register(
    const char *name,
    scheduler_task_t task,
    uint32_t period_ticks);

void scheduler_update(void);

/*==========================================================
 * Gestion des tâches
 *=========================================================*/

bool scheduler_enable(const char *name);

bool scheduler_disable(const char *name);

bool scheduler_remove(const char *name);

/*==========================================================
 * Informations
 *=========================================================*/

uint32_t scheduler_task_count(void);

bool scheduler_get_info(
    uint32_t index,
    scheduler_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULER_H */
