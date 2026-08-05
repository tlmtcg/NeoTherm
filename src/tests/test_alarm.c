#include "test_alarm.h"

#include <stdio.h>
#include <math.h>

#include "alarm.h"
#include "test_utils.h"


#define FLOAT_EPSILON 0.01f


static bool float_equal(
    float a,
    float b)
{
    return fabsf(a - b) < FLOAT_EPSILON;
}



bool test_alarm_run(void)
{
    bool result = true;


    printf("\n--- Test Alarm ---\n");


    /*
     * Initialisation
     */
    alarm_init();



    /*
     * Vérification état initial
     */
    const alarm_t *alarm =
        alarm_get(ALARM_TEMP_HIGH);


    if (alarm == NULL)
    {
        TEST_FAIL("alarm_get returned NULL");
        result = false;
    }
    else if (alarm->state != ALARM_STATE_CLEAR)
    {
        TEST_FAIL("Alarm not clear after init");
        result = false;
    }



    /*
     * Activation TEMP_HIGH
     */
    if (!alarm_set(
            ALARM_TEMP_HIGH,
            35.0f))
    {
        TEST_FAIL("alarm_set TEMP_HIGH failed");
        result = false;
    }


    if (!alarm_is_active(
            ALARM_TEMP_HIGH))
    {
        TEST_FAIL("TEMP_HIGH should be active");
        result = false;
    }


    alarm =
        alarm_get(ALARM_TEMP_HIGH);


    if (alarm == NULL ||
        !float_equal(alarm->value, 35.0f))
    {
        TEST_FAIL("Wrong TEMP_HIGH value");
        result = false;
    }



    /*
     * Réactivation avec nouvelle valeur
     */
    if (!alarm_set(
            ALARM_TEMP_HIGH,
            36.0f))
    {
        TEST_FAIL("Second alarm_set failed");
        result = false;
    }


    alarm =
        alarm_get(ALARM_TEMP_HIGH);


    if (alarm == NULL ||
        !float_equal(alarm->value, 36.0f))
    {
        TEST_FAIL("Alarm value not updated");
        result = false;
    }



    /*
     * ACK alarme ACTIVE
     */
    if (!alarm_ack(
            ALARM_TEMP_HIGH))
    {
        TEST_FAIL("alarm_ack failed");
        result = false;
    }


    alarm =
        alarm_get(ALARM_TEMP_HIGH);


    if (alarm == NULL ||
        alarm->state != ALARM_STATE_ACK)
    {
        TEST_FAIL("Alarm should be ACK");
        result = false;
    }



    /*
     * ACK d'une alarme déjà CLEAR
     */
    if (alarm_ack(ALARM_TEMP_LOW))
    {
        TEST_FAIL("ACK should fail on clear alarm");
        result = false;
    }



    /*
     * Effacement après ACK
     */
    if (!alarm_clear(
            ALARM_TEMP_HIGH))
    {
        TEST_FAIL("alarm_clear failed");
        result = false;
    }


    if (alarm_is_active(
            ALARM_TEMP_HIGH))
    {
        TEST_FAIL("Alarm should be cleared");
        result = false;
    }



    /*
     * Test autre alarme
     */
    if (!alarm_set(
            ALARM_SENSOR_SHT31_ERROR,
            -1.0f))
    {
        TEST_FAIL("SHT31 alarm failed");
        result = false;
    }


    if (!alarm_is_active(
            ALARM_SENSOR_SHT31_ERROR))
    {
        TEST_FAIL("SHT31 alarm not active");
        result = false;
    }


    alarm =
        alarm_get(ALARM_SENSOR_SHT31_ERROR);


    if (alarm == NULL ||
        !float_equal(alarm->value, -1.0f))
    {
        TEST_FAIL("Wrong SHT31 alarm value");
        result = false;
    }



    /*
     * Clear all
     */
    for (alarm_type_t type = ALARM_TEMP_HIGH;
         type < ALARM_COUNT;
         type++)
    {
        alarm_clear(type);
    }


    for (alarm_type_t type = ALARM_TEMP_HIGH;
         type < ALARM_COUNT;
         type++)
    {
        if (alarm_is_active(type))
        {
            TEST_FAIL("Alarm still active after clear all");
            result = false;
        }
    }



    /*
     * Test réactivation après CLEAR
     */
    if (!alarm_set(
            ALARM_TEMP_LOW,
            2.0f))
    {
        TEST_FAIL("TEMP_LOW activation failed");
        result = false;
    }


    if (!alarm_is_active(
            ALARM_TEMP_LOW))
    {
        TEST_FAIL("TEMP_LOW should be active");
        result = false;
    }



    /*
     * Résultat
     */
    if (result)
    {
        ASSERT_SUCCESS("Alarm manager OK");
    }


    return result;
}
