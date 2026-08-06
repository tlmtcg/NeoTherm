#include "test_alarm_history.h"

#include <stdio.h>

#include "alarm_history.h"
#include "alarm_storage.h"
#include "clock.h"
#include "test_utils.h"

bool test_alarm_history_run(void)
{
    bool result = true;

    printf("\n=============== ALARM HISTORY TEST ===============\n");

    /*
     * Initialisation
     */
    clock_init();
    alarm_storage_clear();
    alarm_history_init();

    /*
     * Historique vide
     */
    ASSERT_EQ_INT(
        0,
        alarm_history_count());

    ASSERT_FALSE(
        alarm_history_is_dirty());

    /*
     * ACTIVE
     */

    uint32_t t0 =
        clock_get_timestamp();

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACTIVE,
        35.0f);

    ASSERT_EQ_INT(
        1,
        alarm_history_count());

    ASSERT_TRUE(
        alarm_history_is_dirty());

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

    ASSERT_TRUE(
        entry->timestamp >= t0);

    /*
     * ACK
     */

    clock_tick(1);

    uint32_t t1 =
        clock_get_timestamp();

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACK,
        35.0f);

    ASSERT_EQ_INT(
        2,
        alarm_history_count());

    entry =
        alarm_history_get(1);

    ASSERT_TRUE(
        entry != NULL);

    ASSERT_EQ_INT(
        ALARM_STATE_ACK,
        entry->state);

    ASSERT_TRUE(
        entry->timestamp >= t1);

    /*
     * CLEAR
     */

    clock_tick(1);

    uint32_t t2 =
        clock_get_timestamp();

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_CLEAR,
        35.0f);

    ASSERT_EQ_INT(
        3,
        alarm_history_count());

    entry =
        alarm_history_get(2);

    ASSERT_TRUE(
        entry != NULL);

    ASSERT_EQ_INT(
        ALARM_STATE_CLEAR,
        entry->state);

    ASSERT_TRUE(
        entry->timestamp >= t2);

    /*
     * Chronologie
     */

    const alarm_history_entry_t *e0 =
        alarm_history_get(0);

    const alarm_history_entry_t *e1 =
        alarm_history_get(1);

    const alarm_history_entry_t *e2 =
        alarm_history_get(2);

    ASSERT_TRUE(
        e0->timestamp <= e1->timestamp);

    ASSERT_TRUE(
        e1->timestamp <= e2->timestamp);

    /*
     * Sauvegarde des 3 événements
     */

    ASSERT_TRUE(
        alarm_history_save());

    ASSERT_FALSE(
        alarm_history_is_dirty());

    /*
     * Reset RAM
     */

    ASSERT_TRUE(
        alarm_history_clear());

    ASSERT_EQ_INT(
        0,
        alarm_history_count());

    /*
     * Recharge stockage
     */

    ASSERT_TRUE(
        alarm_history_load());

    ASSERT_EQ_INT(
        3,
        alarm_history_count());

    ASSERT_FALSE(
        alarm_history_is_dirty());

    /*
     * Test AlarmHistorySave task
     */

    printf(
        "\n----- Alarm History Task TEST -----\n");

    /*
     * Nouveau cycle propre
     */

    alarm_storage_clear();

    alarm_history_init();

    /*
     * Ajout d'un événement
     */

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACTIVE,
        42.0f);

    ASSERT_EQ_INT(
        1,
        alarm_history_count());

    ASSERT_TRUE(
        alarm_history_is_dirty());

    /*
     * Exécution tâche de sauvegarde
     */

    alarm_history_task();

    ASSERT_FALSE(
        alarm_history_is_dirty());

    /*
     * Effacement RAM
     */

    ASSERT_TRUE(
        alarm_history_clear());

    ASSERT_EQ_INT(
        0,
        alarm_history_count());

    /*
     * Recharge depuis stockage
     */

    ASSERT_TRUE(
        alarm_history_load());

    ASSERT_EQ_INT(
        1,
        alarm_history_count());

    ASSERT_FALSE(
        alarm_history_is_dirty());

    entry =
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
        42.0f,
        entry->value);

    /*
     * Test buffer circulaire
     */

    for (uint32_t i = 0;
         i < ALARM_HISTORY_SIZE + 20;
         i++)
    {
        clock_add_second();

        alarm_history_add(
            ALARM_TEMP_LOW,
            ALARM_STATE_ACTIVE,
            (float)i);
    }

    ASSERT_EQ_INT(
        ALARM_HISTORY_SIZE,
        alarm_history_count());

    entry =
        alarm_history_get(
            alarm_history_count() - 1);

    ASSERT_TRUE(
        entry != NULL);

    if (result)
    {
        printf(
            "ALARM HISTORY TEST PASS\n");
    }

    return result;
}
