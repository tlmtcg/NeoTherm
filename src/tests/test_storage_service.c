#include "test_storage_service.h"

#include <stdio.h>

#include "../services/storage_service/storage_service.h"
#include "runtime.h"
#include "test_utils.h"
#include "app_config.h"
#include "runtime.h"
#include "storage.h"

bool test_storage_service_run(void)
{
    printf("\n================ STORAGE SERVICE TEST ================\n");

    /*
     * Initialisation
     */

    // ASSERT_TRUE(app_config_init("../app.ini"));
    // ASSERT_TRUE(runtime_init());
    // ASSERT_TRUE(storage_init());

    ASSERT_TRUE(storage_service_init());

    /*
     * Chargement de la configuration runtime
     */

    runtime_config_t runtime;

    ASSERT_EQ_INT(
        storage_service_load_runtime(&runtime),
        STORAGE_LOAD_OK);

    /*
     * Vérifications
     */

    ASSERT_TRUE(runtime.setpoint >= 5.0f);
    ASSERT_TRUE(runtime.setpoint <= 35.0f);

    ASSERT_TRUE(runtime.hysteresis > 0.0f);

    /*
     * Sauvegarde (sans modification)
     */

    ASSERT_TRUE(
        storage_service_save_runtime(&runtime));

    /*
     * Dump
     */

    storage_service_dump();

    printf("Runtime\n");
    printf("------------------------------\n");
    printf("Mode         : %d\n", runtime.mode);
    printf("Setpoint     : %.1f C\n", runtime.setpoint);
    printf("Hysteresis   : %.2f C\n", runtime.hysteresis);
    printf("Relay delay  : %u s\n", runtime.relay_delay);
    printf("\n");

    printf("STORAGE SERVICE TEST PASS\n");

    return true;
}