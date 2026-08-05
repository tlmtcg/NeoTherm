#include "test_alarm_storage.h"

#include <stdio.h>
#include <string.h>

#include "alarm_storage.h"
#include "alarm_history.h"
#include "alarm.h"
#include "test_utils.h"


bool test_alarm_storage_run(void)
{
    printf("\n=============== ALARM STORAGE TEST ===============\n");


    /*
     * Initialisation
     */
    if (!alarm_storage_init())
    {
        TEST_FAIL("alarm_storage_init failed");
    }


    /*
     * Création historique test
     */
    alarm_history_t history;

    memset(&history,
           0,
           sizeof(history));


    history.count = 1;


    history.entries[0].type =
        ALARM_TEMP_HIGH;

    history.entries[0].state =
        ALARM_STATE_ACTIVE;

    history.entries[0].value =
        35.0f;

    history.entries[0].timestamp =
        1798133438;



    /*
     * Sauvegarde
     */
    if (!alarm_storage_save(&history))
    {
        TEST_FAIL("alarm_storage_save failed");
    }



    /*
     * Chargement
     */
    alarm_history_t loaded;

    memset(&loaded,
           0,
           sizeof(loaded));


    if (!alarm_storage_load(&loaded))
    {
        TEST_FAIL("alarm_storage_load failed");
    }



    /*
     * Vérification
     */
    if (loaded.count != 1)
    {
        TEST_FAIL_VALUE_U32(
            "Wrong history count",
            1,
            loaded.count);
    }


    alarm_history_entry_t *entry =
        &loaded.entries[0];


    if (entry->type != ALARM_TEMP_HIGH)
    {
        TEST_FAIL("Wrong alarm type");
    }


    if (entry->state != ALARM_STATE_ACTIVE)
    {
        TEST_FAIL("Wrong alarm state");
    }


    if (entry->value != 35.0f)
    {
        TEST_FAIL_VALUE_FLOAT(
            "Wrong alarm value",
            35.0f,
            entry->value);
    }


    if (entry->timestamp != 1798133438)
    {
        TEST_FAIL_VALUE_U32(
            "Wrong timestamp",
            1798133438,
            entry->timestamp);
    }



    /*
     * Effacement
     */
    if (!alarm_storage_clear())
    {
        TEST_FAIL("alarm_storage_clear failed");
    }


    memset(&loaded,
           0,
           sizeof(loaded));


    if (!alarm_storage_load(&loaded))
    {
        TEST_FAIL("load after clear failed");
    }


    if (loaded.count != 0)
    {
        TEST_FAIL_VALUE_U32(
            "Storage not empty after clear",
            0,
            loaded.count);
    }



    ASSERT_SUCCESS("Alarm storage OK");


    return true;
}