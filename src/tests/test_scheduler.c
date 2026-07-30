#include "test_scheduler.h"

#include <stdio.h>
#include <stdint.h>

#include "scheduler.h"
#include "scheduler_debug.h"
#include "test_utils.h"


static uint32_t s_task1_counter = 0;
static uint32_t s_task2_counter = 0;



static void task1(void)
{
    s_task1_counter++;
}



static void task2(void)
{
    s_task2_counter++;
}



bool test_scheduler_run(void)
{
    printf("\n=============== SCHEDULER TEST ===============\n");


    /*
     * Reset compteurs
     */

    s_task1_counter = 0;
    s_task2_counter = 0;



    /*
     * Initialisation
     */

    scheduler_init();



    /*
     * Enregistrement tâches
     */

    ASSERT_TRUE(
        scheduler_register(
            "Task1",
            task1,
            2));



    ASSERT_TRUE(
        scheduler_register(
            "Task2",
            task2,
            5));



    /*
     * Debug optionnel
     */

    scheduler_dump();



    /*
     * Exécution 10 ticks
     */

    printf("\nRunning scheduler...\n");


    for (uint32_t i = 0; i < 10; i++)
    {
        scheduler_update();
    }



    /*
     * Vérification
     *
     * Task1 période 2 :
     * 10 ticks -> 5 exécutions
     *
     * Task2 période 5 :
     * 10 ticks -> 2 exécutions
     */

    ASSERT_EQ_UINT32(
        5,
        s_task1_counter);


    ASSERT_EQ_UINT32(
        2,
        s_task2_counter);



    printf("\nTask1 executions : %u\n",
           s_task1_counter);

    printf("Task2 executions : %u\n",
           s_task2_counter);



    printf("\nPASS : Scheduler\n");


    return true;
}
