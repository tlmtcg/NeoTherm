#include "test_time_service.h"

#include <stdio.h>

#include "../services/time_service/time_service.h"
#include "../clock/clock.h"

#include "test_utils.h"

bool test_time_service_run(void)
{
    printf("\n================ TIME SERVICE TEST ================\n");


    /*
     * Initialisation
     */

    ASSERT_TRUE(
        time_service_init());


    ASSERT_FALSE(
        time_service_is_available());


    /*
     * Synchronisation
     */

    ASSERT_TRUE(
        time_service_sync());


    ASSERT_TRUE(
        time_service_is_available());


    /*
     * Vérification de l'heure
     */

    clock_time_t now;

    ASSERT_TRUE(
        clock_get_time(&now));


    ASSERT_TRUE(now.year >= 2025);
    ASSERT_TRUE(now.month >= 1 && now.month <= 12);
    ASSERT_TRUE(now.day >= 1 && now.day <= 31);
    ASSERT_TRUE(now.hour <= 23);
    ASSERT_TRUE(now.minute <= 59);
    ASSERT_TRUE(now.second <= 59);


    printf("Current NeoTherm time : "
           "%04u-%02u-%02u %02u:%02u:%02u\n",
           now.year,
           now.month,
           now.day,
           now.hour,
           now.minute,
           now.second);


    /*
     * Dump
     */

    time_service_dump();

    return true;
}
