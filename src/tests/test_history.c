#include "test_history.h"

#include <stdio.h>
#include <math.h>

#include "clock.h"
#include "history.h"
#include "history_debug.h"
#include "thermostat.h"
#include "test_utils.h"


#define FLOAT_EPSILON 0.01f


bool test_history_run(void)
{
    printf("\n=============== HISTORY TEST ===============\n");


    history_init();


    /*
     * Historique vide après initialisation
     */

    ASSERT_EQ_UINT32(
        0,
        history_count());



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



    /*
     * Création de 10 enregistrements
     */

    for (uint32_t i = 0; i < 10; i++)
    {
        history_add(
            20.0f + (float)i * 0.1f,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            (i & 1U),
            (i & 1U));


        clock_tick(10);
    }



    ASSERT_EQ_UINT32(
        10,
        history_count());



    /*
     * Vérification dernier enregistrement
     */

    history_record_t record;


    ASSERT_TRUE(
        history_get_latest(&record));


    ASSERT_EQ_FLOAT(
        20.9f,
        record.inside_temperature);


    ASSERT_EQ_FLOAT(
        5.0f,
        record.outside_temperature);


    ASSERT_EQ_FLOAT(
        21.0f,
        record.setpoint);


    ASSERT_EQ_UINT32(
        THERMOSTAT_AUTO,
        record.mode);



    /*
     * Sauvegarde fichier
     */

    ASSERT_TRUE(
        history_save("../history.dat"));



    /*
     * Effacement mémoire
     */

    history_clear();


    ASSERT_EQ_UINT32(
        0,
        history_count());



    /*
     * Rechargement
     */

    ASSERT_TRUE(
        history_load("../history.dat"));


    ASSERT_EQ_UINT32(
        10,
        history_count());



    /*
     * Vérification après reload
     */

    ASSERT_TRUE(
        history_get_latest(&record));


    ASSERT_EQ_FLOAT(
        20.9f,
        record.inside_temperature);



    printf("PASS : History\n");


    return true;
}
