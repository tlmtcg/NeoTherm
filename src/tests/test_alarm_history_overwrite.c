#include "test_alarm_history_overwrite.h"

#include <stdio.h>

#include "alarm_history.h"
#include "alarm_storage.h"
#include "clock.h"
#include "test_utils.h"

bool test_alarm_history_overwrite_run(void)
{
    printf("\n=============== ALARM HISTORY OVERWRITE TEST ===============\n");

    clock_init();

    alarm_storage_clear();
    alarm_history_init();

    /*
     * Remplit le buffer + 10 entrées
     */

    for (uint32_t i = 0;
         i < ALARM_HISTORY_SIZE + 10;
         i++)
    {
        clock_add_second();

        alarm_history_add(
            ALARM_TEMP_HIGH,
            ALARM_STATE_ACTIVE,
            (float)i);
    }

    ASSERT_EQ_INT(
        ALARM_HISTORY_SIZE,
        alarm_history_count());

    /*
     * Première entrée = valeur 10
     */

    const alarm_history_entry_t *first =
        alarm_history_get(0);

    ASSERT_TRUE(first != NULL);

    ASSERT_EQ_FLOAT(
        10.0f,
        first->value);

    /*
     * Dernière entrée = valeur ALARM_HISTORY_SIZE+9
     */

    const alarm_history_entry_t *last =
        alarm_history_get(
            alarm_history_count() - 1);

    ASSERT_TRUE(last != NULL);

    ASSERT_EQ_FLOAT(
        (float)(ALARM_HISTORY_SIZE + 9),
        last->value);

    printf("ALARM HISTORY OVERWRITE TEST PASS\n");

    return true;
}
