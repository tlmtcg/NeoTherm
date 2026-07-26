#include "scheduler_debug.h"

#include <stdio.h>

#include "scheduler.h"

/*==========================================================
 * API publique
 *=========================================================*/

void scheduler_dump(void)
{
    printf("\n");
    printf("========== SCHEDULER ==========\n\n");

    printf("%-15s %-8s %-8s %-8s\n",
           "Task",
           "Period",
           "Counter",
           "Enabled");

    printf("---------------------------------------------\n");

    scheduler_info_t info;

    for (uint32_t i = 0; i < scheduler_task_count(); i++)
    {
        if (!scheduler_get_info(i, &info))
        {
            continue;
        }

        printf("%-15s %-8u %-8u %-8s\n",
               info.name,
               info.period,
               info.counter,
               info.enabled ? "YES" : "NO");
    }

    printf("\n");
}
