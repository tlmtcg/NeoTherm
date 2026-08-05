#include "test_alarm_history.h"

#include <stdio.h>

#include "alarm_history.h"
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
    alarm_history_init();

    /*
     * Historique vide
     */
    ASSERT_EQ_INT(0, alarm_history_count());

    /*
     * ----- ACTIVE -----
     */
    uint32_t t0 = clock_get_timestamp();

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACTIVE,
        35.0f);

    ASSERT_EQ_INT(1, alarm_history_count());

    const alarm_history_entry_t *entry =
        alarm_history_get(0);

    if (entry == NULL)
    {
        TEST_FAIL("History entry NULL");
        return false;
    }

    ASSERT_EQ_INT(ALARM_TEMP_HIGH, entry->type);
    ASSERT_EQ_INT(ALARM_STATE_ACTIVE, entry->state);
    ASSERT_EQ_FLOAT(35.0f, entry->value);

    ASSERT_TRUE(entry->timestamp >= t0);

    /*
     * ----- ACK -----
     */

    clock_tick(1);

    uint32_t t1 = clock_get_timestamp();

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACK,
        35.0f);

    ASSERT_EQ_INT(2, alarm_history_count());

    entry = alarm_history_get(1);

    ASSERT_EQ_INT(ALARM_STATE_ACK,
                  entry->state);

    ASSERT_TRUE(entry->timestamp >= t1);

    /*
     * ----- CLEAR -----
     */

    clock_tick(1);

    uint32_t t2 = clock_get_timestamp();

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_CLEAR,
        35.0f);

    ASSERT_EQ_INT(3, alarm_history_count());

    entry = alarm_history_get(2);

    ASSERT_EQ_INT(ALARM_STATE_CLEAR,
                  entry->state);

    ASSERT_TRUE(entry->timestamp >= t2);

    /*
     * Vérifie que les timestamps sont
     * chronologiques.
     */

    const alarm_history_entry_t *e0 =
        alarm_history_get(0);

    const alarm_history_entry_t *e1 =
        alarm_history_get(1);

    const alarm_history_entry_t *e2 =
        alarm_history_get(2);

    ASSERT_TRUE(e0->timestamp <= e1->timestamp);
    ASSERT_TRUE(e1->timestamp <= e2->timestamp);

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

    /*
     * Vérifie qu'une entrée est toujours accessible
     */

    entry = alarm_history_get(
        alarm_history_count() - 1);

    ASSERT_TRUE(entry != NULL);

    if (result)
    {
        printf("ALARM HISTORY TEST PASS\n");
    }

    return result;
}
