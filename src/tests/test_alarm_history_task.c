#include "test_alarm_history_task.h"

#include <stdio.h>

#include "alarm_history.h"
#include "alarm_history_task.h"
#include "alarm_storage.h"
#include "scheduler.h"
#include "test_utils.h"

bool test_alarm_history_task_run(void)
{
    printf("\n=============== ALARM HISTORY TASK TEST ===============\n");

    /*
     * Initialisation
     */

    alarm_storage_clear();
    alarm_history_init();

    scheduler_init();

    /*
     * Enregistrement de la tâche
     */

    ASSERT_TRUE(
        scheduler_register(
            "AlarmHistorySave",
            alarm_history_task,
            1));

    /*
     * Ajout d'une entrée
     */

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACTIVE,
        35.0f);

    ASSERT_EQ_INT(
        1,
        alarm_history_count());

    ASSERT_TRUE(
        alarm_history_is_dirty());

    /*
     * Exécution du scheduler
     */

    scheduler_update();

    /*
     * La tâche doit avoir sauvegardé
     */

    ASSERT_FALSE(
        alarm_history_is_dirty());

    /*
     * On vide la RAM
     */

    ASSERT_TRUE(
        alarm_history_clear());

    ASSERT_EQ_INT(
        0,
        alarm_history_count());

    /*
     * Rechargement depuis le stockage
     */

    ASSERT_TRUE(
        alarm_history_load());

    ASSERT_EQ_INT(
        1,
        alarm_history_count());

    const alarm_history_entry_t *entry =
        alarm_history_get(0);

    ASSERT_TRUE(
        entry != NULL);

    ASSERT_EQ_INT(
        ALARM_TEMP_HIGH,
        entry->type);

    ASSERT_EQ_INT(
        ALARM_STATE_ACTIVE,
        entry->state);

    ASSERT_EQ_FLOAT(
        35.0f,
        entry->value);

    printf("ALARM HISTORY TASK TEST PASS\n");

    return true;
}