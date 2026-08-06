#include "test_alarm_history_scheduler.h"

#include <stdio.h>

#include "alarm_history.h"
#include "alarm_storage.h"
#include "clock.h"
#include "test_utils.h"


bool test_alarm_history_scheduler_run(void)
{
    bool result = true;


    printf("\n=============== ALARM HISTORY SCHEDULER TEST ===============\n");


    /*
     * Initialisation
     */

    clock_init();

    alarm_storage_clear();

    alarm_history_init();



    /*
     * Historique vide
     */

    ASSERT_EQ_INT(
        0,
        alarm_history_count());


    ASSERT_FALSE(
        alarm_history_is_dirty());



    /*
     * Ajout événement
     */

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACTIVE,
        35.0f);



    ASSERT_EQ_INT(
        1,
        alarm_history_count());


    ASSERT_TRUE(
        alarm_history_is_dirty());



    /*
     * Simulation tâche scheduler
     *
     * AlarmHistorySave
     */

    alarm_history_task();



    /*
     * Après sauvegarde :
     * dirty doit être faux
     */

    ASSERT_FALSE(
        alarm_history_is_dirty());



    /*
     * Vérifie que le stockage
     * contient bien l'historique
     */

    alarm_history_clear();


    ASSERT_EQ_INT(
        0,
        alarm_history_count());


    ASSERT_TRUE(
        alarm_history_load());


    ASSERT_EQ_INT(
        1,
        alarm_history_count());



    const alarm_history_entry_t *entry =
        alarm_history_get(0);



    ASSERT_TRUE(
        entry != NULL);



    ASSERT_EQ_INT(
        ALARM_TEMP_HIGH,
        entry->type);


    ASSERT_EQ_INT(
        ALARM_STATE_ACTIVE,
        entry->state);


    ASSERT_EQ_FLOAT(
        35.0f,
        entry->value);



    ASSERT_FALSE(
        alarm_history_is_dirty());



    /*
     * Test modification multiple avant save
     *
     * Vérifie que la tâche sauvegarde
     * le dernier état complet
     */

    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACK,
        36.0f);


    alarm_history_add(
        ALARM_TEMP_LOW,
        ALARM_STATE_CLEAR,
        10.0f);



    ASSERT_EQ_INT(
        3,
        alarm_history_count());


    ASSERT_TRUE(
        alarm_history_is_dirty());



    /*
     * Nouvelle exécution scheduler
     */

    alarm_history_task();



    ASSERT_FALSE(
        alarm_history_is_dirty());



    /*
     * Reload complet
     */

    alarm_history_clear();


    ASSERT_TRUE(
        alarm_history_load());


    ASSERT_EQ_INT(
        3,
        alarm_history_count());



    if (result)
    {
        printf(
            "ALARM HISTORY SCHEDULER TEST PASS\n");
    }


    return result;
}
