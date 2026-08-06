#include "test_cmd_alarm_history.h"

#include <stdio.h>

#include "alarm_history.h"
#include "alarm_storage.h"
#include "../infra/console/commands/cmd_alarm.h"
#include "test_utils.h"


bool test_cmd_alarm_history_run(void)
{
    bool result = true;

    printf("\n=============== CMD ALARM HISTORY TEST ===============\n");


    /*
     * Nettoyage
     */
    alarm_storage_clear();
    alarm_history_init();


    /*
     * Création historique
     */
    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACTIVE,
        35.0f);


    alarm_history_add(
        ALARM_TEMP_HIGH,
        ALARM_STATE_ACK,
        36.0f);


    alarm_history_add(
        ALARM_TEMP_LOW,
        ALARM_STATE_CLEAR,
        10.0f);


    /*
     * Vérification RAM
     */
    ASSERT_EQ_INT(
        3,
        alarm_history_count());


    ASSERT_TRUE(
        alarm_history_is_dirty());


    /*
     * Sauvegarde
     */
    ASSERT_TRUE(
        alarm_history_save());


    ASSERT_TRUE(
        !alarm_history_is_dirty());


    /*
     * Reset RAM
     */
    alarm_history_init();


    ASSERT_EQ_INT(
        0,
        alarm_history_count());


    /*
     * Recharge depuis storage
     */
    ASSERT_TRUE(
        alarm_history_load());


    ASSERT_EQ_INT(
        3,
        alarm_history_count());


    /*
     * Vérifie les données rechargées
     */
    const alarm_history_entry_t *entry;


    entry = alarm_history_get(0);

    if (entry == NULL)
    {
        TEST_FAIL("Entry 0 NULL");
        result = false;
    }
    else
    {
        ASSERT_EQ_INT(
            ALARM_TEMP_HIGH,
            entry->type);

        ASSERT_EQ_INT(
            ALARM_STATE_ACTIVE,
            entry->state);

        ASSERT_EQ_FLOAT(
            35.0f,
            entry->value);
    }


    entry = alarm_history_get(1);

    if (entry == NULL)
    {
        TEST_FAIL("Entry 1 NULL");
        result = false;
    }
    else
    {
        ASSERT_EQ_INT(
            ALARM_STATE_ACK,
            entry->state);

        ASSERT_EQ_FLOAT(
            36.0f,
            entry->value);
    }


    entry = alarm_history_get(2);

    if (entry == NULL)
    {
        TEST_FAIL("Entry 2 NULL");
        result = false;
    }
    else
    {
        ASSERT_EQ_INT(
            ALARM_STATE_CLEAR,
            entry->state);

        ASSERT_EQ_FLOAT(
            10.0f,
            entry->value);
    }


    /*
     * Test commande console
     */
    if (!cmd_alarms("history"))
    {
        TEST_FAIL(
            "cmd alarm history failed");

        result = false;
    }


    if (result)
    {
        ASSERT_SUCCESS(
            "CMD alarm history OK");
    }


    return result;
}
