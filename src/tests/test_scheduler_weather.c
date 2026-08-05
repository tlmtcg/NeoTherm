#include "test_scheduler_weather.h"

#include <stdio.h>

#include "scheduler.h"
#include "weather_service.h"
#include "weather_provider.h"
#include "runtime.h"

#include "test_utils.h"



bool test_scheduler_weather_run(void)
{
    printf("\n=============== SCHEDULER WEATHER TEST ===============\n");


    ASSERT_TRUE(
        runtime_init());


    ASSERT_TRUE(
        runtime_set_weather_update_period(
            2));


    weather_provider_reset_fetch_count();


    ASSERT_TRUE(
        weather_service_init());


    scheduler_init();



    ASSERT_TRUE(
        scheduler_register(
            "WeatherService",
            weather_service_tick,
            1));



    /*
     * Premier passage scheduler
     */

    scheduler_update();



    uint32_t count =
        weather_provider_get_fetch_count();


    printf("Fetch after first tick : %u\n",
           count);



    ASSERT_EQ_UINT32(
        1,
        count);



    /*
     * Tick suivant avant période
     */

    scheduler_update();


    count =
        weather_provider_get_fetch_count();


    printf("Fetch before period : %u\n",
           count);



    ASSERT_EQ_UINT32(
        1,
        count);


        runtime_set_weather_update_period(60);

    printf("SCHEDULER WEATHER TEST PASS\n");


    return true;
}
