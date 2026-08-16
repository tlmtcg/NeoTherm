#include "test_storage_service.h"

#include <stdio.h>

#include "../services/storage_service/storage_service.h"

#include "runtime.h"
#include "test_utils.h"
#include "app_config.h"
#include "storage.h"

bool test_storage_service_run(void)
{
    printf("\n================ STORAGE SERVICE TEST ================\n");

    /*
     * Initialisation
     */

    ASSERT_TRUE(storage_service_init());

    /*
     * Nettoyage du fichier précédent.
     *
     * Le premier chargement doit alors retourner
     * STORAGE_LOAD_DEFAULT.
     */

    storage_test_clear();

    /*
     * Chargement initial
     */

    runtime_config_t runtime;

    ASSERT_EQ_INT(
        storage_service_load_runtime(&runtime),
        STORAGE_LOAD_DEFAULT);

    /*
     * Vérifications des valeurs par défaut
     */

    ASSERT_TRUE(runtime.setpoint >= 5.0f);
    ASSERT_TRUE(runtime.setpoint <= 35.0f);

    ASSERT_TRUE(runtime.hysteresis > 0.0f);

    /*
     * Sauvegarde
     */

    ASSERT_TRUE(
        storage_service_save_runtime(&runtime));

    /*
     * Rechargement
     *
     * Maintenant le fichier existe :
     * le résultat attendu est STORAGE_LOAD_OK.
     */

    runtime_config_t loaded;

    ASSERT_EQ_INT(
        storage_service_load_runtime(&loaded),
        STORAGE_LOAD_OK);

    /*
     * Vérification que les valeurs ont été conservées.
     */

    ASSERT_TRUE(
        loaded.setpoint == runtime.setpoint);

    ASSERT_TRUE(
        loaded.hysteresis == runtime.hysteresis);

    ASSERT_TRUE(
        loaded.relay_delay == runtime.relay_delay);

    /*
     * Dump
     */

    storage_service_dump();

    printf("\n");
    printf("Runtime\n");
    printf("------------------------------\n");

    printf(
        "Mode         : %d\n",
        loaded.mode);

    printf(
        "Setpoint     : %.1f C\n",
        loaded.setpoint);

    printf(
        "Hysteresis   : %.2f C\n",
        loaded.hysteresis);

    printf(
        "Relay delay  : %u s\n",
        loaded.relay_delay);

    printf("\n");

    printf("STORAGE SERVICE TEST PASS\n");

    return true;
}
