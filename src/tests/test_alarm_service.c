#include "../services/alarm_service/alarm_service.h"

#include <stdio.h>

#include "test_utils.h"


bool test_alarm_service_run(void)
{
    printf("\n=============== ALARM SERVICE TEST ===============\n");


    ASSERT_TRUE(
        alarm_service_init());


    ASSERT_EQ_UINT32(
        0,
        alarm_service_count_active());


    alarm_set(
        ALARM_TEMP_HIGH,
        35.0f);


    ASSERT_EQ_UINT32(
        1,
        alarm_service_count_active());


    ASSERT_TRUE(
        alarm_service_has_error());


    const alarm_t *alarm =
        alarm_service_get_first_active();


    ASSERT_NOT_NULL(alarm);


    ASSERT_TRUE(
        alarm->type ==
        ALARM_TEMP_HIGH);


    alarm_clear(
        ALARM_TEMP_HIGH);


    ASSERT_EQ_UINT32(
        0,
        alarm_service_count_active());


    alarm_service_dump();


    printf("ALARM SERVICE TEST PASS\n");


    return true;
}
