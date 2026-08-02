#include "test_alarm.h"

#include <stdio.h>

#include "alarm.h"
#include "test_utils.h"

bool test_alarm_run(void)
{
    bool result = true;

    printf("\n--- Test Alarm ---\n");

    /*
     * Initialisation
     */
    alarm_init();

    const alarm_t *alarm =
        alarm_get(ALARM_TEMP_HIGH);

    if (alarm == NULL)
    {
        TEST_FAIL("alarm_get returned NULL");
        result = false;
    }

    if (alarm->state != ALARM_STATE_CLEAR)
    {
        TEST_FAIL("Alarm not clear after init");
        result = false;
    }

    /*
     * Activation alarme
     */
    if (!alarm_set(
            ALARM_TEMP_HIGH,
            35.0f))
    {
        TEST_FAIL("alarm_set failed");
        result = false;
    }

    if (!alarm_is_active(ALARM_TEMP_HIGH))
    {
        TEST_FAIL("Alarm should be active");
        result = false;
    }

    alarm =
        alarm_get(ALARM_TEMP_HIGH);

    if (alarm->value != 35.0f)
    {
        TEST_FAIL("Wrong alarm value");
        result = false;
    }

    /*
     * Effacement
     */
    if (!alarm_clear(ALARM_TEMP_HIGH))
    {
        TEST_FAIL("alarm_clear failed");
        result = false;
    }

    if (alarm_is_active(ALARM_TEMP_HIGH))
    {
        TEST_FAIL("Alarm should be cleared");
        result = false;
    }

    if (result)
    {
        ASSERT_SUCCESS("Alarm manager OK");
    }

    return result;
}
