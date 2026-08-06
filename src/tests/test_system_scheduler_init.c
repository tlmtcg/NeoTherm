#include "test_system_scheduler_init.h"

#include <stdio.h>

#include "system_init.h"
#include "scheduler.h"
#include "scheduler_debug.h"

#include "test_utils.h"

bool test_system_scheduler_init_run(void)
{
    printf("\n=============== SYSTEM SCHEDULER INIT TEST ===============\n");

    /*
     * Initialisation système complète
     */
    ASSERT_TRUE(
        system_init());

    /*
     * Affichage debug optionnel
     */
    scheduler_dump();

    /*
     * Vérification des tâches attendues
     *
     * Ces noms doivent correspondre
     * aux scheduler_register()
     */

    ASSERT_TRUE(
        scheduler_exists("Climate"));

    ASSERT_TRUE(
        scheduler_exists("Thermostat"));

    ASSERT_TRUE(
        scheduler_exists("WeatherService"));

    ASSERT_TRUE(
        scheduler_exists("HistorySave"));

    ASSERT_TRUE(
        scheduler_exists("HistoryCsv"));

    ASSERT_TRUE(
        scheduler_exists("AlarmHistorySave"));

    ASSERT_FALSE(
        scheduler_exists("Bidon"));

    printf(
        "SYSTEM SCHEDULER INIT TEST PASS\n");

    return true;
}
