#include "test_runtime.h"

#include <stdio.h>

#include "clock.h"
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

    ASSERT_EQ_UINT32(
        2026,
        cfg->date_time.year);

    ASSERT_EQ_UINT32(
        1,
        cfg->date_time.month);

    ASSERT_EQ_UINT32(
        1,
        cfg->date_time.day);

    ASSERT_EQ_UINT32(
        0,
        cfg->date_time.hour);

    ASSERT_EQ_UINT32(
        0,
        cfg->date_time.minute);

    ASSERT_EQ_UINT32(
        0,
        cfg->date_time.second);

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

    /*
     * Date / heure
     */

    clock_time_t dt =
    {
        .year   = 2026,
        .month  = 7,
        .day    = 31,
        .hour   = 14,
        .minute = 25,
        .second = 42
    };

    ASSERT_TRUE(
        runtime_set_datetime(
            &dt));

    /*
     * Vérifie que runtime copie bien la structure
     */

    dt.second = 59;

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

    ASSERT_EQ_UINT32(
        2026,
        cfg->date_time.year);

    ASSERT_EQ_UINT32(
        7,
        cfg->date_time.month);

    ASSERT_EQ_UINT32(
        31,
        cfg->date_time.day);

    ASSERT_EQ_UINT32(
        14,
        cfg->date_time.hour);

    ASSERT_EQ_UINT32(
        25,
        cfg->date_time.minute);

    ASSERT_EQ_UINT32(
        42,
        cfg->date_time.second);

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

    ASSERT_TRUE(
        runtime_set_hysteresis(
            1.0f));

    ASSERT_TRUE(
        runtime_set_relay_delay(
            999));

    ASSERT_TRUE(
        runtime_set_location(
            0.0f,
            0.0f));

    clock_time_t dt2 =
    {
        .year   = 2030,
        .month  = 1,
        .day    = 1,
        .hour   = 0,
        .minute = 0,
        .second = 0
    };

    ASSERT_TRUE(
        runtime_set_datetime(
            &dt2));

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

    ASSERT_EQ_UINT32(
        2026,
        cfg->date_time.year);

    ASSERT_EQ_UINT32(
        7,
        cfg->date_time.month);

    ASSERT_EQ_UINT32(
        31,
        cfg->date_time.day);

    ASSERT_EQ_UINT32(
        14,
        cfg->date_time.hour);

    ASSERT_EQ_UINT32(
        25,
        cfg->date_time.minute);

    ASSERT_EQ_UINT32(
        42,
        cfg->date_time.second);

    /*
     * Vérifie la synchronisation avec l'horloge
     */

    clock_time_t now =
    {
        .year   = 2026,
        .month  = 12,
        .day    = 24,
        .hour   = 18,
        .minute = 30,
        .second = 15
    };

    ASSERT_TRUE(
        clock_set_time(
            &now));

    ASSERT_TRUE(
        runtime_set_datetime(
            &now));

    ASSERT_TRUE(
        runtime_save());

    ASSERT_TRUE(
        runtime_load());

    cfg = runtime_get();

    ASSERT_EQ_UINT32(
        2026,
        cfg->date_time.year);

    ASSERT_EQ_UINT32(
        12,
        cfg->date_time.month);

    ASSERT_EQ_UINT32(
        24,
        cfg->date_time.day);

    ASSERT_EQ_UINT32(
        18,
        cfg->date_time.hour);

    ASSERT_EQ_UINT32(
        30,
        cfg->date_time.minute);

    ASSERT_EQ_UINT32(
        15,
        cfg->date_time.second);

    /*
     * Tests d'erreur
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

    ASSERT_FALSE(
        runtime_set_datetime(
            NULL));

    printf("\nPASS : Runtime\n");

    return true;
}
