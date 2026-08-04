#include "scheduler.h"

#include "climate.h"
#include "logger.h"
#include <string.h>
#include "console_utils.h"
#include <stdio.h>

static uint32_t s_tick = 0;

static uint32_t s_climate_tick = 0;

typedef struct
{
    const char *name;

    scheduler_task_t callback;

    uint32_t period;

    uint32_t counter;

    bool enabled;

} scheduler_entry_t;

static scheduler_entry_t s_tasks[SCHEDULER_MAX_TASKS];

static uint32_t s_task_count = 0;

/*==========================================================
 * Initialisation
 *=========================================================*/

void scheduler_init(void)
{
    s_task_count = 0;

    LOG_INFO("SCHED",
             "Scheduler initialized");
}

bool scheduler_register(
    const char *name,
    scheduler_task_t task,
    uint32_t period_ticks)
{
    if ((name == NULL) ||
        (task == NULL) ||
        (period_ticks == 0))
    {
        return false;
    }

    if (s_task_count >= SCHEDULER_MAX_TASKS)
    {
        LOG_ERROR("SCHED",
                  "Maximum number of tasks reached.");

        return false;
    }

    scheduler_entry_t *entry = &s_tasks[s_task_count];

    entry->name = name;
    entry->callback = task;
    entry->period = period_ticks;
    entry->counter = 0;
    entry->enabled = true;

    LOG_INFO("SCHED",
             "Registered %-12s period=%u ticks",
             name,
             period_ticks);

    s_task_count++;

    return true;
}

/*==========================================================
 * Mise à jour
 *=========================================================*/

void scheduler_update(void)
{
    for (uint32_t i = 0; i < s_task_count; i++)
    {
        if (!s_tasks[i].enabled)
        {
            continue;
        }

        s_tasks[i].counter++;

        if (s_tasks[i].counter >= s_tasks[i].period)
        {
            s_tasks[i].counter = 0;

            s_tasks[i].callback();
        }
    }
}

static scheduler_entry_t *scheduler_find(
    const char *name)
{
    if (name == NULL)
    {
        return NULL;
    }

    for (uint32_t i = 0; i < s_task_count; i++)
    {
        if (strcmp(s_tasks[i].name, name) == 0)
        {
            return &s_tasks[i];
        }
    }

    return NULL;
}

bool scheduler_enable(const char *name)
{
    scheduler_entry_t *task = scheduler_find(name);

    if (task == NULL)
    {
        return false;
    }

    task->enabled = true;

    LOG_INFO("SCHED",
             "Task '%s' enabled",
             name);

    return true;
}

bool scheduler_disable(const char *name)
{
    scheduler_entry_t *task = scheduler_find(name);

    if (task == NULL)
    {
        return false;
    }

    task->enabled = false;

    LOG_INFO("SCHED",
             "Task '%s' disabled",
             name);

    return true;
}

bool scheduler_remove(const char *name)
{
    for (uint32_t i = 0; i < s_task_count; i++)
    {
        if (strcmp(s_tasks[i].name, name) == 0)
        {
            for (uint32_t j = i; j < s_task_count - 1; j++)
            {
                s_tasks[j] = s_tasks[j + 1];
            }

            s_task_count--;

            LOG_INFO("SCHED",
                     "Task '%s' removed",
                     name);

            return true;
        }
    }

    return false;
}

uint32_t scheduler_task_count(void)
{
    return s_task_count;
}

bool scheduler_get_info(
    uint32_t index,
    scheduler_info_t *info)
{
    if ((info == NULL) ||
        (index >= s_task_count))
    {
        return false;
    }

    info->name = s_tasks[index].name;
    info->period = s_tasks[index].period;
    info->counter = s_tasks[index].counter;
    info->enabled = s_tasks[index].enabled;

    return true;
}

void scheduler_dump(void)
{
    console_print_header("Scheduler Tasks");

    printf("%-3s %-16s %-8s %-8s %s\n",
           "Id",
           "Name",
           "Period",
           "Next",
           "Enabled");

    console_print_separator();

    for (uint32_t i = 0; i < s_task_count; i++)
    {
        printf("%-3u %-16s %-8u %-8u %s\n",
               i,
               s_tasks[i].name,
               s_tasks[i].period,
               s_tasks[i].period - s_tasks[i].counter,
               s_tasks[i].enabled ? "YES" : "NO");
    }

    printf("\nTotal tasks : %u\n",
           s_task_count);

    console_print_separator();
}
