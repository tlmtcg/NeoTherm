#include "test_scheduler.h"

#include <stdio.h>

#include "scheduler.h"
#include "scheduler_debug.h"

static uint32_t s_task1_counter = 0;
static uint32_t s_task2_counter = 0;

static void task1(void)
{
    s_task1_counter++;

    printf("Task1 executed (%u)\n",
           s_task1_counter);
}

static void task2(void)
{
    s_task2_counter++;

    printf("Task2 executed (%u)\n",
           s_task2_counter);
}

bool test_scheduler_run(void)
{
    printf("\n=============== SCHEDULER TEST ===============\n");


    /*
     * Reset des compteurs de test
     */
    s_task1_counter = 0;
    s_task2_counter = 0;


    /*
     * Réinitialisation scheduler
     */
    scheduler_init();


    if (!scheduler_register(
            "Task1",
            task1,
            2))
    {
        printf("FAIL : register Task1\n");
        return false;
    }


    if (!scheduler_register(
            "Task2",
            task2,
            5))
    {
        printf("FAIL : register Task2\n");
        return false;
    }


    scheduler_dump();


    printf("\nRunning scheduler...\n\n");


    for (uint32_t i = 0; i < 10; i++)
    {
        printf("Tick %2u\n", i + 1);

        scheduler_update();
    }


    printf("\n");

    printf("Task1 executions : %u\n",
           s_task1_counter);

    printf("Task2 executions : %u\n",
           s_task2_counter);


    /*
     * Vérification
     *
     * période 2 sur 10 ticks -> 5 exécutions
     * période 5 sur 10 ticks -> 2 exécutions
     */

    if (s_task1_counter != 5)
    {
        printf("FAIL : Task1 expected=5 got=%u\n",
               s_task1_counter);

        return false;
    }


    if (s_task2_counter != 2)
    {
        printf("FAIL : Task2 expected=2 got=%u\n",
               s_task2_counter);

        return false;
    }


    printf("\nPASS : Scheduler\n");

    return true;
}
