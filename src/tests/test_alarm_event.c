#include "test_alarm_event.h"

#include <stdio.h>

#include "alarm.h"
#include "event.h"
#include "test_utils.h"


bool test_alarm_event_run(void)
{
    printf("\n=============== ALARM EVENT TEST ===============\n");


    /*
     * Initialisation
     */

    event_init();

    alarm_init();



    /*
     * Activation alarme
     */

    ASSERT_TRUE(
        alarm_set(
            ALARM_TEMP_HIGH,
            35.0f));


    ASSERT_TRUE(
        alarm_is_active(
            ALARM_TEMP_HIGH));


    /*
     * Lecture événement
     */

    event_t event;


    ASSERT_TRUE(
        event_get(&event));


    ASSERT_TRUE(
        event.type ==
        EVENT_ALARM_ACTIVE);


    ASSERT_TRUE(
        event.data.value ==
        ALARM_TEMP_HIGH);



    printf("Alarm active event : %s\n",
           alarm_get_name(
               (alarm_type_t)event.data.value));



    /*
     * Effacement alarme
     */

    ASSERT_TRUE(
        alarm_clear(
            ALARM_TEMP_HIGH));


    ASSERT_TRUE(
        !alarm_is_active(
            ALARM_TEMP_HIGH));



    /*
     * Vérification événement clear
     */

    ASSERT_TRUE(
        event_get(&event));


    ASSERT_TRUE(
        event.type ==
        EVENT_ALARM_CLEAR);


    ASSERT_TRUE(
        event.data.value ==
        ALARM_TEMP_HIGH);



    printf("Alarm clear event : %s\n",
           alarm_get_name(
               (alarm_type_t)event.data.value));



    /*
     * Vérification compteur
     */

    ASSERT_TRUE(
        event_get_count(
            EVENT_ALARM_ACTIVE) == 1);


    ASSERT_TRUE(
        event_get_count(
            EVENT_ALARM_CLEAR) == 1);



    printf("ALARM EVENT TEST PASS\n");


    return true;
}
