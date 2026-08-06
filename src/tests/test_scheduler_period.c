#include "test_scheduler_period.h"
#include "stdio.h"
#include "scheduler.h"
#include "test_utils.h"

static uint32_t s_taskA_counter = 0;
static uint32_t s_taskB_counter = 0;
static uint32_t s_taskC_counter = 0;

static void taskA(void) { s_taskA_counter++; }

static void taskB(void) { s_taskB_counter++; }

static void taskC(void) { s_taskC_counter++; }

bool test_scheduler_period_run(void)
{
    printf("\n=============== SCHEDULER PERIOD TEST ===============\n");
    scheduler_init();
    // Tester précisément le respect des périodes :

    // enregistrer une tâche période = 1 tick ;
    // tâche A : période 2
    ASSERT_TRUE( scheduler_register( "taskA", taskA, 2));

    // tâche B : période 3
    ASSERT_TRUE( scheduler_register( "taskB", taskB, 3));

    // tâche C : période 5
    ASSERT_TRUE( scheduler_register( "taskC", taskC, 5));

    // Après 30 ticks, 

    for (uint8_t i=0; i<30;i++)
    {
        scheduler_update();
    }
    
    // vérifier :

    // A → 15 exécutions
     ASSERT_EQ_INT(15, s_taskA_counter);
    // B → 10 exécutions
    ASSERT_EQ_INT(10, s_taskB_counter);
    // C → 6 exécutions
    ASSERT_EQ_INT(6, s_taskC_counter);

    printf(
        "SCHEDULER PERIOD TEST PASS\n");

    return true;
}