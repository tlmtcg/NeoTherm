#include "test_weather.h"

#include <stdbool.h>
#include <stdio.h>

#include "../weather/weather.h"

#include "test_utils.h"


bool test_weather_run(void)
{
    printf("\n================ WEATHER TEST ================\n");


    /*
     * Init
     */
    ASSERT_TRUE(
        weather_init());


    /*
     * Après init :
     * aucune donnée météo disponible
     */
    ASSERT_FALSE(
        weather_is_valid());


    const weather_t *weather =
        weather_get();


    ASSERT_TRUE(
        weather == NULL);



    /*
     * Injection météo simulée
     */

    weather_t simulated =
    {
        .valid = true,

        .temperature = 18.5f,
        .humidity = 65.0f,
        .pressure = 1015.0f,

        .wind_speed = 12.0f,
        .wind_gust = 25.0f,
        .wind_direction = 270.0f,

        .rain = 0.0f
    };


    ASSERT_TRUE(
        weather_set(&simulated));



    /*
     * Vérification après mise à jour
     */

    ASSERT_TRUE(
        weather_is_valid());


    weather =
        weather_get();


    ASSERT_TRUE(
        weather != NULL);



    ASSERT_EQ_FLOAT(
        weather->temperature,
        18.5f);


    ASSERT_EQ_FLOAT(
        weather->humidity,
        65.0f);


    ASSERT_EQ_FLOAT(
        weather->pressure,
        1015.0f);


    ASSERT_EQ_FLOAT(
        weather->wind_speed,
        12.0f);



    /*
     * Dump debug
     */

    weather_dump();



    /*
     * Invalidation
     */

    weather_clear();


    ASSERT_FALSE(
        weather_is_valid());


    ASSERT_TRUE(
        weather_get() == NULL);



    printf("WEATHER TEST PASS\n");


    return true;
}
