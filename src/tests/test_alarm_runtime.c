#include "test_alarm_runtime.h"

#include <stdio.h>

#include "alarm.h"
#include "alarm_runtime.h"
#include "thermostat.h"
#include "test_utils.h"


bool test_alarm_runtime_run(void)
{
    bool result = true;


    printf("\n--- Test Alarm Runtime ---\n");


    alarm_init();
    alarm_runtime_init();


    thermostat_status_t status =
    {
        .mode = THERMOSTAT_AUTO,
        .temperature = 20.0f,
        .setpoint = 19.0f,
        .hysteresis = 0.2f,
        .relay_state = false,
        .heating_request = false
    };


    /*
     * Température normale
     */
    alarm_runtime_update(&status);


    if(alarm_is_active(ALARM_TEMP_HIGH))
    {
        TEST_FAIL("High temperature alarm should be OFF");
        result = false;
    }



    /*
     * Température trop haute
     */
    status.temperature = 35.0f;

    alarm_runtime_update(&status);


    if(!alarm_is_active(ALARM_TEMP_HIGH))
    {
        TEST_FAIL("High temperature alarm not triggered");
        result = false;
    }



    /*
     * Retour normal
     */
    status.temperature = 20.0f;

    alarm_runtime_update(&status);


    if(alarm_is_active(ALARM_TEMP_HIGH))
    {
        TEST_FAIL("High temperature alarm not cleared");
        result = false;
    }



    /*
     * Température trop basse
     */
    status.temperature = 2.0f;

    alarm_runtime_update(&status);


    if(!alarm_is_active(ALARM_TEMP_LOW))
    {
        TEST_FAIL("Low temperature alarm not triggered");
        result = false;
    }



    if(result)
    {
        ASSERT_SUCCESS("Alarm runtime OK");
    }


    return result;
}
