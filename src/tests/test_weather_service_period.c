#include "test_weather_service_period.h"

#include <stdio.h>
#include <time.h>


#include "weather_service.h"
#include "weather.h"
#include "runtime.h"
#include "weather_provider.h"

#include "test_utils.h"



static void test_sleep_seconds(
    unsigned seconds)
{
    time_t start =
        time(NULL);

    while ((unsigned)(time(NULL) - start) < seconds)
    {
        /*
         * attente volontaire
         */
    }
}



bool test_weather_service_period_run(void)
{
    printf("\n=============== WEATHER SERVICE PERIOD TEST ===============\n");


    /*
     * Initialisation runtime
     */

    ASSERT_TRUE(
        runtime_init());


    /*
     * Reset compteur provider
     */

    weather_provider_reset_fetch_count();



    /*
     * Configuration période courte
     */

    ASSERT_TRUE(
        runtime_set_weather_update_period(
            2));


    ASSERT_TRUE(
        runtime_set_weather_provider(
            WEATHER_PROVIDER_OPENMETEO));



    /*
     * Initialisation météo
     */

    ASSERT_TRUE(
        weather_service_init());



    ASSERT_TRUE(
        weather_service_set_location(
            50.6333f,
            3.0667f));



    /*
     * Première mise à jour forcée
     */

    ASSERT_TRUE(
        weather_service_update());


    ASSERT_TRUE(
        weather_service_is_available());



    const weather_t *weather =
        weather_get();


    ASSERT_NOT_NULL(weather);


    ASSERT_TRUE(
        weather->valid);



    uint32_t count =
        weather_provider_get_fetch_count();


    printf("Fetch count initial : %u\n",
           count);



    ASSERT_EQ_UINT32(
        1,
        count);



    uint32_t age =
        weather_service_age();


    printf("Initial age : %u s\n",
           age);



    /*
     * Tick immédiat
     *
     * La période n'est pas dépassée.
     * Aucun nouvel appel provider attendu.
     */

    weather_service_tick();



    count =
        weather_provider_get_fetch_count();


    printf("Fetch count immediate tick : %u\n",
           count);



    ASSERT_EQ_UINT32(
        1,
        count);



    age =
        weather_service_age();


    printf("Age after immediate tick : %u s\n",
           age);



    ASSERT_TRUE(
        age <= 1);



    /*
     * Attente dépassement période
     */

    printf("Waiting update period...\n");


    test_sleep_seconds(3);



    /*
     * Le tick doit déclencher
     * une nouvelle récupération
     */

    weather_service_tick();



    count =
        weather_provider_get_fetch_count();


    printf("Fetch count after period : %u\n",
           count);



    ASSERT_EQ_UINT32(
        2,
        count);



    age =
        weather_service_age();


    printf("Age after period tick : %u s\n",
           age);



    ASSERT_TRUE(
        age <= 1);



    /*
     * Vérification données météo
     */

    weather =
        weather_get();


    ASSERT_NOT_NULL(weather);


    ASSERT_TRUE(
        weather->valid);



    weather_service_dump();



    printf("WEATHER SERVICE PERIOD TEST PASS\n");


    return true;
}
