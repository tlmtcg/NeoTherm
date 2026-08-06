#include "test_scheduler_enable_disable.h"
#include "stdio.h"
#include "scheduler.h"
#include "test_utils.h"

static uint32_t s_task_counter = 0;

static void task(void)
{
    s_task_counter++;
}

bool test_scheduler_enable_disable_run(void)
{
    // enregistrer une tâche période = 1 tick ;
    printf("\n=============== SCHEDULER ENABLE DISABLE TEST ===============\n");

    scheduler_init();

    ASSERT_TRUE(
        scheduler_register(
            "task",
            task,
            1));

    // appeler scheduler_update() 10 fois → compteur = 10 ;

    for (uint8_t i = 0; i < 10; i++)
    {
        scheduler_update();
    }
    ASSERT_EQ_INT(10, s_task_counter);
    
    // désactiver la tâche ;
    ASSERT_TRUE(scheduler_disable("task"));

    // appeler scheduler_update() 10 fois → compteur reste à 10 ;
    for (uint8_t i = 0; i < 10; i++)
    {
        scheduler_update();
    }

    ASSERT_EQ_INT(10, s_task_counter);

    // réactiver ;
    ASSERT_TRUE(scheduler_enable("task"));
    // appeler scheduler_update() 5 fois → compteur = 15.

    for (uint8_t i = 0; i < 5; i++)
    {
        scheduler_update();
    }
    ASSERT_EQ_INT(15,  s_task_counter);

    printf(
        "SCHEDULER ENABLE DISABLE TEST PASS\n");

    return true;
}