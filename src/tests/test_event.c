#include <stdio.h>

#include "test_event.h"
#include "event.h"
#include "test_utils.h"


bool test_event_run(void)
{
    printf("\n================ EVENT TEST ================\n");


    event_init();


    /*
     * Etat initial
     */

    ASSERT_TRUE(
        event_is_empty());

    ASSERT_FALSE(
        event_is_full());



    /*
     * Paramètres NULL
     */

    ASSERT_FALSE(
        event_post(NULL));

    ASSERT_FALSE(
        event_get(NULL));



    /*
     * Lecture sur file vide
     */

    event_t evt;

    ASSERT_FALSE(
        event_get(&evt));



    /*
     * Ajout d'un événement simple
     */

    event_t tx =
    {
        .type = EVENT_TIMER_1S
    };


    ASSERT_TRUE(
        event_post(&tx));

    ASSERT_FALSE(
        event_is_empty());



    /*
     * Lecture
     */

    ASSERT_TRUE(
        event_get(&evt));


    ASSERT_EQ_UINT32(
        EVENT_TIMER_1S,
        evt.type);


    ASSERT_TRUE(
        event_is_empty());



    /*
     * Test transport des données
     */

    event_t climate =
    {
        .type = EVENT_CLIMATE_UPDATE,
        .data.temperature = 21.5f
    };


    ASSERT_TRUE(
        event_post(&climate));


    ASSERT_TRUE(
        event_get(&evt));


    ASSERT_EQ_UINT32(
        EVENT_CLIMATE_UPDATE,
        evt.type);


    ASSERT_EQ_FLOAT(
        21.5f,
        evt.data.temperature);



    /*
     * Vérification FIFO
     */

    event_t e1 =
    {
        .type = EVENT_TIMER_1S
    };

    event_t e2 =
    {
        .type = EVENT_CLIMATE_UPDATE
    };

    event_t e3 =
    {
        .type = EVENT_QUIT
    };


    ASSERT_TRUE(event_post(&e1));
    ASSERT_TRUE(event_post(&e2));
    ASSERT_TRUE(event_post(&e3));


    ASSERT_TRUE(event_get(&evt));

    ASSERT_EQ_UINT32(
        EVENT_TIMER_1S,
        evt.type);



    ASSERT_TRUE(event_get(&evt));

    ASSERT_EQ_UINT32(
        EVENT_CLIMATE_UPDATE,
        evt.type);



    ASSERT_TRUE(event_get(&evt));

    ASSERT_EQ_UINT32(
        EVENT_QUIT,
        evt.type);



    ASSERT_TRUE(
        event_is_empty());



    /*
     * Test buffer circulaire
     */

    event_t fill =
    {
        .type = EVENT_TIMER_1S
    };


    for (size_t i = 0;
         i < EVENT_QUEUE_SIZE - 1;
         i++)
    {
        ASSERT_TRUE(
            event_post(&fill));
    }


    for (size_t i = 0;
         i < EVENT_QUEUE_SIZE - 1;
         i++)
    {
        ASSERT_TRUE(
            event_get(&evt));

        ASSERT_EQ_UINT32(
            EVENT_TIMER_1S,
            evt.type);
    }



    ASSERT_TRUE(
        event_post(&e1));

    ASSERT_TRUE(
        event_post(&e2));



    ASSERT_TRUE(
        event_get(&evt));

    ASSERT_EQ_UINT32(
        EVENT_TIMER_1S,
        evt.type);



    ASSERT_TRUE(
        event_get(&evt));

    ASSERT_EQ_UINT32(
        EVENT_CLIMATE_UPDATE,
        evt.type);



    ASSERT_TRUE(
        event_is_empty());



    /*
     * Remplissage complet
     */

    for (size_t i = 0;
         i < EVENT_QUEUE_SIZE;
         i++)
    {
        ASSERT_TRUE(
            event_post(&fill));
    }


    ASSERT_TRUE(
        event_is_full());


    ASSERT_FALSE(
        event_is_empty());



    /*
     * Dépassement de capacité
     */

    ASSERT_FALSE(
        event_post(&fill));



    /*
     * Vidage complet
     */

    for (size_t i = 0;
         i < EVENT_QUEUE_SIZE;
         i++)
    {
        ASSERT_TRUE(
            event_get(&evt));


        ASSERT_EQ_UINT32(
            EVENT_TIMER_1S,
            evt.type);
    }


    ASSERT_TRUE(
        event_is_empty());


    ASSERT_FALSE(
        event_is_full());



    /*
     * Test event_clear()
     */

    ASSERT_TRUE(
        event_post(&e1));

    ASSERT_TRUE(
        event_post(&e2));


    ASSERT_FALSE(
        event_is_empty());


    event_clear();


    ASSERT_TRUE(
        event_is_empty());


    ASSERT_FALSE(
        event_is_full());



    /*
     * Test event_init() après utilisation
     */

    ASSERT_TRUE(
        event_post(&e1));


    ASSERT_FALSE(
        event_is_empty());


    event_init();


    ASSERT_TRUE(
        event_is_empty());



    /*
     * event_type_to_string()
     */

    ASSERT_EQ_STRING(
        "NONE",
        event_type_to_string(EVENT_NONE));


    ASSERT_EQ_STRING(
        "CLIMATE_UPDATE",
        event_type_to_string(EVENT_CLIMATE_UPDATE));


    ASSERT_EQ_STRING(
        "TIMER_1S",
        event_type_to_string(EVENT_TIMER_1S));


    ASSERT_EQ_STRING(
        "QUIT",
        event_type_to_string(EVENT_QUIT));


    ASSERT_EQ_STRING(
        "UNKNOWN",
        event_type_to_string((event_type_t)255));



    printf("PASS : Event\n");


    return true;
}
