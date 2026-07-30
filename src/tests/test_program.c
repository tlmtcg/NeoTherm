#include "test_program.h"

#include <stdio.h>

#include "clock.h"
#include "program.h"
#include "schedule.h"
#include "test_utils.h"


bool test_program_run(void)
{
    printf("\n=============== PROGRAM TEST ===============\n");


    clock_time_t t =
    {
        .year   = 2026,
        .month  = 1,
        .day    = 1,
        .hour   = 5,
        .minute = 59,
        .second = 0
    };


    /*
     * Avant début de journée
     */

    ASSERT_TRUE(
        clock_set_time(&t));


    ASSERT_EQ_FLOAT(
        17.0f,
        schedule_get_setpoint());



    /*
     * Passage en période confort
     */

    t.hour = 6;
    t.minute = 0;


    ASSERT_TRUE(
        clock_set_time(&t));


    ASSERT_EQ_FLOAT(
        21.0f,
        schedule_get_setpoint());



    /*
     * Retour période réduite
     */

    t.hour = 23;
    t.minute = 0;


    ASSERT_TRUE(
        clock_set_time(&t));


    ASSERT_EQ_FLOAT(
        17.0f,
        schedule_get_setpoint());



    printf("PASS : Program\n");


    return true;
}
