#include "test_runtime_weather.h"

#include <stdio.h>

#include "runtime.h"
#include "weather_provider.h"
#include "test_utils.h"


bool test_runtime_weather_run(void)
{
    printf("\n=============== RUNTIME WEATHER TEST ===============\n");


    /*
     * Initialisation runtime
     */

    ASSERT_TRUE(
        runtime_init());


    const runtime_config_t *runtime =
        runtime_get();


    ASSERT_NOT_NULL(runtime);



    /*
     * Vérification configuration chargée
     */

    printf("Loaded provider : %s\n",
           weather_provider_to_string(
               runtime->weather_provider));


    /*
     * Le provider chargé doit être valide
     */

    ASSERT_TRUE(
        runtime->weather_provider ==
            WEATHER_PROVIDER_SIMULATOR ||
        runtime->weather_provider ==
            WEATHER_PROVIDER_OPENMETEO);



    ASSERT_TRUE(
        runtime->weather_update_period_sec > 0);



    /*
     * Modification configuration météo
     */

    ASSERT_TRUE(
        runtime_set_weather_provider(
            WEATHER_PROVIDER_OPENMETEO));


    ASSERT_TRUE(
        runtime_set_weather_update_period(
            600));



    /*
     * Modification position
     */

    ASSERT_TRUE(
        runtime_set_location(
            50.6333f,
            3.0667f));



    /*
     * Vérification configuration mémoire
     */

    runtime =
        runtime_get();


    ASSERT_NOT_NULL(runtime);



    ASSERT_TRUE(
        runtime->weather_provider ==
        WEATHER_PROVIDER_OPENMETEO);



    ASSERT_EQ_UINT32(
        600,
        runtime->weather_update_period_sec);



    ASSERT_EQ_FLOAT(
        50.6333f,
        runtime->latitude);



    ASSERT_EQ_FLOAT(
        3.0667f,
        runtime->longitude);



    /*
     * Sauvegarde
     */

    ASSERT_TRUE(
        runtime_save());



    /*
     * Rechargement depuis stockage
     */

    ASSERT_TRUE(
        runtime_load());



    runtime =
        runtime_get();


    ASSERT_NOT_NULL(runtime);



    /*
     * Vérification persistance
     */

    ASSERT_TRUE(
        runtime->weather_provider ==
        WEATHER_PROVIDER_OPENMETEO);



    ASSERT_EQ_UINT32(
        600,
        runtime->weather_update_period_sec);



    ASSERT_EQ_FLOAT(
        50.6333f,
        runtime->latitude);



    ASSERT_EQ_FLOAT(
        3.0667f,
        runtime->longitude);



    runtime_dump();


    printf("RUNTIME WEATHER TEST PASS\n");


    return true;
}
