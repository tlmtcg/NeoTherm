#include "test_program.h"

#include <stdio.h>

#include "clock.h"
#include "program.h"
#include "logger.h"
#include "test_utils.h"
#include "schedule.h"

bool test_program_run(void)
{
    LOG_INFO("TEST",
             "Program");

    clock_time_t t =
        {
            .year = 2026,
            .month = 1,
            .day = 1,
            .minute = 0,
            .second = 0};

    t.hour = 5;
    t.minute = 59;
    clock_set_time(&t);

    ASSERT_FLOAT_EQ(17.0f,
                    schedule_get_setpoint());

    t.hour = 6;
    t.minute = 0;
    clock_set_time(&t);

    ASSERT_FLOAT_EQ(21.0f,
                    schedule_get_setpoint());

    t.hour = 23;
    t.minute = 0;
    clock_set_time(&t);

    ASSERT_FLOAT_EQ(17.0f,
                    schedule_get_setpoint());

 
}
