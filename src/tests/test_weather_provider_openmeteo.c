#include "test_weather_provider_openmeteo.h"

#include <stdio.h>

#include "test_utils.h"

#include "http_client.h"

#include "weather.h"
#include "weather_provider_openmeteo.h"



bool test_weather_provider_openmeteo_run(void)
{
    printf("\n================ OPENMETEO PROVIDER TEST ================\n");


    /*
     * Initialisation HTTP
     */

    ASSERT_TRUE(
        http_client_init());



    /*
     * Initialisation provider
     */

    ASSERT_TRUE(
        weather_provider_openmeteo_init());



    /*
     * Récupération météo
     */

    weather_t weather;


    ASSERT_TRUE(
        weather_provider_openmeteo_fetch(
            &weather));

    ASSERT_TRUE(
    weather_set(&weather));


    /*
     * Vérification données
     */

    ASSERT_TRUE(
        weather.valid);



    ASSERT_GT_FLOAT(
        -50.0f,
        weather.temperature
        );


    ASSERT_LT_FLOAT(
        60.0f,
        weather.temperature
        );



    ASSERT_GE_FLOAT(
        0.0f,
        weather.humidity);


    ASSERT_LE_FLOAT(
        100.0f,
        weather.humidity);



    /*
     * Affichage
     */

    weather_dump();



    printf("OPENMETEO PROVIDER TEST PASS\n");


    return true;
}
