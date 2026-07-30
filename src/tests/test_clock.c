#include <stdio.h>

#include "clock.h"
#include "test_utils.h"


bool test_clock_run(void)
{
    printf("\n================ CLOCK TEST ================\n");


    clock_init();


    clock_time_t t =
    {
        .year   = 2026,
        .month  = 1,
        .day    = 1,
        .hour   = 12,
        .minute = 0,
        .second = 0
    };


    ASSERT_TRUE(
        clock_set_time(&t));


    clock_time_t now;


    ASSERT_TRUE(
        clock_get_time(&now));


    /*
     * Vérification heure initiale
     */

    ASSERT_EQ_UINT32(
        43200,
        clock_seconds_today());


    ASSERT_EQ_UINT32(
        2026,
        now.year);

    ASSERT_EQ_UINT32(
        1,
        now.month);

    ASSERT_EQ_UINT32(
        1,
        now.day);

    ASSERT_EQ_UINT32(
        12,
        now.hour);

    ASSERT_EQ_UINT32(
        0,
        now.minute);

    ASSERT_EQ_UINT32(
        0,
        now.second);



    /*
     * +30 secondes
     */

    clock_tick(30);

    ASSERT_TRUE(
        clock_get_time(&now));

    ASSERT_EQ_UINT32(
        43230,
        clock_seconds_today());

    ASSERT_EQ_UINT32(
        30,
        now.second);



    /*
     * +40 secondes
     */

    clock_tick(40);

    ASSERT_TRUE(
        clock_get_time(&now));

    ASSERT_EQ_UINT32(
        43270,
        clock_seconds_today());

    ASSERT_EQ_UINT32(
        1,
        now.minute);

    ASSERT_EQ_UINT32(
        10,
        now.second);



    /*
     * +1 heure
     */

    clock_tick(3600);

    ASSERT_TRUE(
        clock_get_time(&now));

    ASSERT_EQ_UINT32(
        46870,
        clock_seconds_today());

    ASSERT_EQ_UINT32(
        13,
        now.hour);

    ASSERT_EQ_UINT32(
        10,
        now.second);



    /*
     * Test clock_add_second()
     */

    clock_add_second();

    ASSERT_TRUE(
        clock_get_time(&now));

    ASSERT_EQ_UINT32(
        46871,
        clock_seconds_today());



    /*
     * Test clock_add_seconds()
     */

    clock_add_seconds(10);

    ASSERT_TRUE(
        clock_get_time(&now));

    ASSERT_EQ_UINT32(
        46881,
        clock_seconds_today());



    /*
     * Passage à minuit
     */

    clock_time_t presque_minuit =
    {
        .year   = 2026,
        .month  = 1,
        .day    = 1,
        .hour   = 23,
        .minute = 59,
        .second = 50
    };


    ASSERT_TRUE(
        clock_set_time(&presque_minuit));


    ASSERT_EQ_UINT32(
        86390,
        clock_seconds_today());


    /*
     * +20 secondes -> 2 janvier 00:00:10
     */

    clock_tick(20);


    ASSERT_TRUE(
        clock_get_time(&now));


    ASSERT_EQ_UINT32(
        10,
        clock_seconds_today());


    ASSERT_EQ_UINT32(
        2026,
        now.year);

    ASSERT_EQ_UINT32(
        1,
        now.month);

    ASSERT_EQ_UINT32(
        2,
        now.day);

    ASSERT_EQ_UINT32(
        0,
        now.hour);

    ASSERT_EQ_UINT32(
        0,
        now.minute);

    ASSERT_EQ_UINT32(
        10,
        now.second);



    printf("PASS : Clock\n");

    return true;
}
