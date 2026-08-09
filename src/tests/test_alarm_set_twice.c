#include "test_alarm_set_twice.h"

#include <stdio.h>

#include "alarm.h"
#include "alarm_history.h"
#include "alarm_storage.h"
#include "clock.h"
#include "test_utils.h"

bool test_alarm_set_twice_run(void)
{
    printf("\n=============== ALARM SET TWICE TEST ===============\n");

    clock_init();

    alarm_storage_clear();

    alarm_history_init();

    /*
     * Première activation
     */

    ASSERT_TRUE(
        alarm_set(
            ALARM_TEMP_HIGH,
            35.0f));

    ASSERT_EQ_INT(
        1,
        alarm_history_count());

    /*
     * Deuxième activation
     */

    ASSERT_TRUE(
        alarm_set(
            ALARM_TEMP_HIGH,
            42.0f));

    /*
     * Toujours une seule entrée historique
     */

    ASSERT_EQ_INT(
        1,
        alarm_history_count());

    /*
     * La valeur de l'alarme a changé
     */

    ASSERT_EQ_FLOAT(
        42.0f,
        alarm_get_value(ALARM_TEMP_HIGH));

    printf("ALARM SET TWICE TEST PASS\n");

    return true;
}
