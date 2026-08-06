#include "test_alarm_service_history.h"

#include "alarm.h"
#include "alarm_history.h"
#include "stdio.h"
#include "test_utils.h"

bool test_alarm_service_history_run(void)
{
    printf("\n=============== ALARM SERVICE HISTORY TEST ===============\n");


    alarm_history_init();
    alarm_init();


    alarm_set(
        ALARM_TEMP_HIGH,
        35.0f);


    ASSERT_EQ_INT(
        1,
        alarm_history_count());


    const alarm_history_entry_t *entry =
        alarm_history_get(0);


    ASSERT_EQ_INT(
        ALARM_STATE_ACTIVE,
        entry->state);



    alarm_ack(
        ALARM_TEMP_HIGH);


    ASSERT_EQ_INT(
        2,
        alarm_history_count());



    alarm_clear(
        ALARM_TEMP_HIGH);


    ASSERT_EQ_INT(
        3,
        alarm_history_count());


    printf(
        "ALARM SERVICE HISTORY TEST PASS\n");


    return true;
}
