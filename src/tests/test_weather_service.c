#include <stdio.h>

#include "../services/weather_service/weather_service.h"
#include "../weather/weather.h"
#include "test_utils.h"

bool test_weather_service_run(void)
{
    printf("\n");
    printf("================ WEATHER SERVICE TEST ================\n");

    /*
     * Initialisation core météo
     */

    ASSERT_TRUE(
        weather_init());

    /*
     * Initialisation service
     */

    ASSERT_TRUE(
        weather_service_init());

    /*
     * Position
     */

    ASSERT_TRUE(
        weather_service_set_location(
            50.6333f,
            3.0667f));

    float lat;
    float lon;

    ASSERT_TRUE(
        weather_service_get_location(
            &lat,
            &lon));

    ASSERT_EQ_FLOAT(
        lat,
        50.6333f);

    ASSERT_EQ_FLOAT(
        lon,
        3.0667f);

    /*
     * Récupération météo simulée
     */

    ASSERT_TRUE(
        weather_service_update());

    /*
     * Vérification disponibilité
     */

    ASSERT_TRUE(
        weather_service_is_available());

    /*
     * Lecture données météo
     */

    const weather_t *weather =
        weather_get();

    ASSERT_TRUE(
        weather != NULL);

    ASSERT_TRUE(
        weather->temperature > -50.0f &&
        weather->temperature < 60.0f);

    ASSERT_TRUE(
        weather->humidity >= 0.0f &&
        weather->humidity <= 100.0f);

    ASSERT_TRUE(
        weather->pressure > 800.0f &&
        weather->pressure < 1100.0f);

    /*
     * Age des données
     */

    uint32_t age =
        weather_service_age();

    printf("Weather age : %u s\n",
           age);

    /*
     * Dump debug
     */

    weather_service_dump();

    printf("Weather service : PASS\n");

    return true;
}
