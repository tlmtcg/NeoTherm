#include "test_runtime.h"

#include <stdio.h>

#include "runtime.h"
#include "storage.h"
#include "test_utils.h"
#include "thermostat.h"



bool test_runtime_run(void)
{
    printf("\n=============== RUNTIME TEST ===============\n");


    /*
     * Premier démarrage
     */

    printf("\nFirst boot test\n");


    ASSERT_TRUE(
        storage_test_clear());


    ASSERT_TRUE(
        storage_init());


    ASSERT_TRUE(
        runtime_init());


    const runtime_config_t *cfg =
        runtime_get();


    ASSERT_TRUE(cfg != NULL);


    ASSERT_EQ_INT(
        THERMOSTAT_AUTO,
        cfg->mode);


    ASSERT_EQ_FLOAT(
        20.5f,
        cfg->setpoint);


    ASSERT_EQ_FLOAT(
        0.2f,
        cfg->hysteresis);


    ASSERT_EQ_UINT32(
        180,
        cfg->relay_delay);


    ASSERT_EQ_FLOAT(
        50.681f,
        cfg->latitude);


    ASSERT_EQ_FLOAT(
        3.154f,
        cfg->longitude);


    printf("First boot OK\n");



    /*
     * Modification configuration
     */


    ASSERT_TRUE(
        runtime_set_mode(
            THERMOSTAT_MANUAL));


    ASSERT_TRUE(
        runtime_set_setpoint(
            21.5f));


    ASSERT_TRUE(
        runtime_set_hysteresis(
            0.3f));


    ASSERT_TRUE(
        runtime_set_relay_delay(
            300));


    ASSERT_TRUE(
        runtime_set_location(
            48.8566f,
            2.3522f));



    cfg = runtime_get();


    ASSERT_EQ_INT(
        THERMOSTAT_MANUAL,
        cfg->mode);


    ASSERT_EQ_FLOAT(
        21.5f,
        cfg->setpoint);


    ASSERT_EQ_FLOAT(
        0.3f,
        cfg->hysteresis);


    ASSERT_EQ_UINT32(
        300,
        cfg->relay_delay);


    ASSERT_EQ_FLOAT(
        48.8566f,
        cfg->latitude);


    ASSERT_EQ_FLOAT(
        2.3522f,
        cfg->longitude);



    /*
     * Sauvegarde
     */


    ASSERT_TRUE(
        runtime_save());



    /*
     * Modification volontaire en RAM
     */

    ASSERT_TRUE(
        runtime_set_mode(
            THERMOSTAT_OFF));


    ASSERT_TRUE(
        runtime_set_setpoint(
            10.0f));



    cfg = runtime_get();


    ASSERT_EQ_INT(
        THERMOSTAT_OFF,
        cfg->mode);


    ASSERT_EQ_FLOAT(
        10.0f,
        cfg->setpoint);



    /*
     * Recharge depuis stockage
     */


    ASSERT_TRUE(
        runtime_load());



    cfg = runtime_get();



    ASSERT_EQ_INT(
        THERMOSTAT_MANUAL,
        cfg->mode);


    ASSERT_EQ_FLOAT(
        21.5f,
        cfg->setpoint);


    ASSERT_EQ_FLOAT(
        0.3f,
        cfg->hysteresis);


    ASSERT_EQ_UINT32(
        300,
        cfg->relay_delay);


    ASSERT_EQ_FLOAT(
        48.8566f,
        cfg->latitude);


    ASSERT_EQ_FLOAT(
        2.3522f,
        cfg->longitude);



    /*
     * Test erreurs
     */


    ASSERT_FALSE(
        runtime_set_mode(
            99));


    ASSERT_FALSE(
        runtime_set_location(
            200.0f,
            0.0f));


    ASSERT_FALSE(
        runtime_set_location(
            0.0f,
            300.0f));



    printf("\nPASS : Runtime\n");


    return true;
}
