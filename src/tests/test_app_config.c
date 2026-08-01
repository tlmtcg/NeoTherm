#include "test_app_config.h"

#include <stdio.h>

#include "app_config.h"
#include "ini.h"
#include "test_utils.h"

bool test_app_config_run(void)
{
    printf("\n=============== APP CONFIG TEST ===============\n");

    /*
     * Test d'initialisation avec un fichier inexistant
     */
    printf("\nNon-existent file test\n");
    ASSERT_FALSE(app_config_init("non_existent_app.ini"));

    /*
     * Test avec un fichier de configuration valide
     */
    printf("\nValid config file test\n");

    // Initialisation du parser INI et chargement de la configuration applicative
    ASSERT_TRUE(app_config_init("../app.ini"));

    const app_config_t *cfg = app_config_get();
    ASSERT_TRUE(cfg != NULL);

    /*
     * Vérification des valeurs chargées
     */

    ASSERT_EQ_STRING("INFO",
                     cfg->logger_level);

    /*----------------------------------------------------------
     * Relay
     *---------------------------------------------------------*/

    ASSERT_EQ_UINT32(4,
                     cfg->relay_gpio);

    ASSERT_EQ_UINT32(180,
                     cfg->relay_default_switch_delay);

    /*----------------------------------------------------------
     * Scheduler
     *---------------------------------------------------------*/

    ASSERT_EQ_UINT32(10,
                     cfg->climate_period);

    ASSERT_EQ_UINT32(10,
                     cfg->thermostat_period);

    ASSERT_EQ_UINT32(60,
                     cfg->history_save_period);

    ASSERT_EQ_UINT32(300,
                     cfg->history_csv_period);

    /*----------------------------------------------------------
     * Files
     *---------------------------------------------------------*/

    ASSERT_EQ_STRING("../runtime.ini",
                     cfg->runtime_file);

    ASSERT_EQ_STRING("../program.ini",
                     cfg->schedule_file);

    ASSERT_EQ_STRING("../history.dat",
                     cfg->history_file);

    ASSERT_EQ_STRING("../history.csv",
                     cfg->history_csv_file);

    /*----------------------------------------------------------
     * History
     *---------------------------------------------------------*/

    ASSERT_EQ_UINT32(1000,
                     cfg->history_max_records);

    /*----------------------------------------------------------
     * Schedule
     *---------------------------------------------------------*/

    ASSERT_EQ_UINT32(10,
                     cfg->schedule_max_points);

    ASSERT_EQ_FLOAT(17.0f,
                    cfg->schedule_default_setpoint);

    /*----------------------------------------------------------
     * Climate
     *---------------------------------------------------------*/

    ASSERT_EQ_FLOAT(20.5f,
                    cfg->climate_initial_temperature);

    /*----------------------------------------------------------
     * Debug
     *---------------------------------------------------------*/

    ASSERT_EQ_BOOL(true,
                   cfg->debug_enabled);

    ASSERT_EQ_BOOL(false,
                   cfg->debug_dump_config);

    printf("\nPASS : App Config\n");

    return true;
}