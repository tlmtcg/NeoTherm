#include "test_storage.h"

#include <stdio.h>

#include "logger.h"
#include "runtime.h"
#include "storage.h"
#include "test_utils.h"
#include "thermostat.h"

bool test_storage_run(void)
{
    printf("\n=============== STORAGE TEST ===============\n");

    storage_init();

    /*
     * Sauvegarde initiale
     */

    runtime_config_t cfg =
        {
            .mode = THERMOSTAT_MANUAL,
            .setpoint = 19.5f,
            .hysteresis = 0.2f,
            .relay_delay = 180,
            .latitude = 50.681f,
            .longitude = 3.154f};

    printf("\nSaving runtime...\n");

    ASSERT_TRUE(storage_save_runtime(&cfg));

    /*
     * Lecture
     */

    runtime_config_t loaded;

    ASSERT_EQ_INT(
    STORAGE_LOAD_OK,
    storage_load_runtime(&loaded));

    printf("Loaded mode        : %s\n",
           thermostat_mode_to_string(loaded.mode));

    printf("Loaded setpoint    : %.1f\n",
           loaded.setpoint);

    printf("Loaded hysteresis  : %.2f\n",
           loaded.hysteresis);

    printf("Loaded relay delay : %u\n",
           loaded.relay_delay);

    ASSERT_EQ_INT(THERMOSTAT_MANUAL,
                  loaded.mode);

    ASSERT_EQ_FLOAT(19.5f,
                    loaded.setpoint);

    ASSERT_EQ_FLOAT(0.2f,
                    loaded.hysteresis);

    ASSERT_EQ_UINT32(180,
                     loaded.relay_delay);

    ASSERT_EQ_FLOAT(50.681f,
                    loaded.latitude);

    ASSERT_EQ_FLOAT(3.154f,
                    loaded.longitude);

    /*
     * Test écrasement
     */

    cfg.mode = THERMOSTAT_AUTO;
    cfg.setpoint = 22.0f;
    cfg.hysteresis = 0.5f;
    cfg.relay_delay = 300;
    cfg.latitude = 48.8566f;
    cfg.longitude = 2.3522f;

    printf("\nOverwrite runtime...\n");

    ASSERT_TRUE(storage_save_runtime(&cfg));

    ASSERT_EQ_INT(STORAGE_LOAD_OK,storage_load_runtime(&loaded));

    ASSERT_EQ_INT(THERMOSTAT_AUTO,
                  loaded.mode);

    ASSERT_EQ_FLOAT(22.0f,
                    loaded.setpoint);

    ASSERT_EQ_FLOAT(0.5f,
                    loaded.hysteresis);

    ASSERT_EQ_UINT32(300,
                     loaded.relay_delay);

    ASSERT_EQ_FLOAT(48.8566f,
                    loaded.latitude);

    ASSERT_EQ_FLOAT(2.3522f,
                    loaded.longitude);

    printf("\nPASS : Storage\n");

    return true;
}