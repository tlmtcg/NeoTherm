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
    ASSERT_EQ_STRING("INFO", cfg->logger_level);
    ASSERT_EQ_UINT32(4, cfg->relay_gpio);
    ASSERT_EQ_UINT32(10, cfg->climate_period);
    ASSERT_EQ_UINT32(10, cfg->thermostat_period);

    printf("\nPASS : App Config\n");

    return true;
}