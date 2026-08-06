#include "test_scheduler_limit.h"
#include "stdio.h"
#include "scheduler.h"
#include "test_utils.h"
#include "alarm_history_task.h"
#include "alarm.h"
#include "alarm_storage.h"
#include "alarm_history.h"

static uint32_t s_task1_counter = 0;

static void task1(void)
{
    s_task1_counter++;
}

bool test_scheduler_limit_run(void)
{
    printf("\n=============== SCHEDULER LIMIT TEST ===============\n");

    scheduler_init();

    for (uint32_t i = 0; i < SCHEDULER_MAX_TASKS; i++)
    {
        char name[32];

        snprintf(name,
                 sizeof(name),
                 "Task_%u",
                 i);

        ASSERT_TRUE(
            scheduler_register(
                name,
                task1,
                1));
    }

    /*
     * Une tâche supplémentaire doit échouer
     */

    ASSERT_FALSE(
        scheduler_register(
            "Overflow",
            task1,
            1));

    printf("SCHEDULER LIMIT TEST PASS\n");

    ASSERT_TRUE(scheduler_remove("Task_15"));

    ASSERT_TRUE(scheduler_register(
        "Task1",
        task1,
        1));

    scheduler_update();

    ASSERT_EQ_UINT32(
        16,
        s_task1_counter);

    printf("SCHEDULER REMOVE TASK PASS\n");

    printf("\n=============== SCHEDULER ALARM HISTORY TEST ===============\n");

    alarm_storage_clear();

    alarm_history_init();

    scheduler_init();

    ASSERT_TRUE(
        scheduler_register(
            "AlarmHistorySave",
            alarm_history_task,
            5));

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACTIVE,
        35.0f);

    ASSERT_TRUE(
        alarm_history_is_dirty());

    /*
     * 4 ticks :
     * pas encore sauvegardé
     */

    for (int i = 0; i < 4; i++)
    {
        scheduler_update();
    }

    ASSERT_TRUE(
        alarm_history_is_dirty());

    /*
     * 5ème tick :
     * sauvegarde
     */

    scheduler_update();

    ASSERT_FALSE(
        alarm_history_is_dirty());

    printf(
        "SCHEDULER ALARM HISTORY TEST PASS\n");

    return true;
}
