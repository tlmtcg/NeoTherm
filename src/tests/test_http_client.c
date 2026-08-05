#include "test_http_client.h"

#include <stdio.h>
#include <string.h>

#include "http_client.h"
#include "test_utils.h"


bool test_http_client_run(void)
{
    printf("\n================ HTTP CLIENT TEST ================\n");


    /*
     * Initialisation
     */

    ASSERT_TRUE(
        http_client_init());


    ASSERT_TRUE(
        http_client_is_available());



    /*
     * Requête HTTP réelle
     */

    char buffer[8192];


    memset(buffer,
           0,
           sizeof(buffer));


    const char *url =
        "https://api.open-meteo.com/v1/forecast?"
        "latitude=50.6333&"
        "longitude=3.0667&"
        "current=temperature_2m";



    ASSERT_TRUE(
        http_client_get(
            url,
            buffer,
            sizeof(buffer)));



    /*
     * Vérification réponse
     */

    ASSERT_TRUE(
        strlen(buffer) > 0);



    printf("\nHTTP response:\n");

    printf("%.500s\n",
           buffer);



    /*
     * Debug
     */

    http_client_dump();



    printf("HTTP CLIENT TEST PASS\n");


    return true;
}
