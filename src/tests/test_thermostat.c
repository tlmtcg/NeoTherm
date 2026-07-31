#include "test_thermostat.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "history.h"
#include "program.h"
#include "relay.h"
#include "storage.h"
#include "thermostat.h"
#include "test_utils.h"

#define HORS_GEL_SETPOINT 8.0f
#define HORS_GEL_HYSTERESIS 0.5f

bool test_thermostat_run(void)
{
    printf("\n=============== THERMOSTAT TEST ===============\n");

    /*
     * Initialisation
     */

    clock_init();

    climate_init();

    relay_init();

    history_init();

    storage_init();

    program_init();

    thermostat_init();

    relay_test_reset();

    /*
     * -----------------------------
     * AUTO
     * -----------------------------
     */

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    printf("\nMode AUTO\n");

/*
 * Température supérieure à la consigne
 */
#ifdef MODE_TEST

    climate_test_set_temperature(
        20.0f);

    thermostat_update();

    ASSERT_FALSE(relay_get());

    printf("PASS\n");

    /*
     * Température inférieure à la consigne
     */

    climate_test_set_temperature(
        16.0f);

    thermostat_update();

    ASSERT_TRUE(relay_get());

    printf("PASS\n");

#endif

    /*
     * -----------------------------
     * OFF
     * -----------------------------
     */

    thermostat_set_mode(
        THERMOSTAT_OFF);

    thermostat_update();

    ASSERT_FALSE(relay_get());

    printf("PASS\n");

    /*
     * -----------------------------
     * MANUAL
     * -----------------------------
     */

    thermostat_set_mode(
        THERMOSTAT_MANUAL);

    thermostat_manual_set_relay(true);

    thermostat_update();

    ASSERT_TRUE(relay_get());

    printf("PASS\n");

    thermostat_manual_set_relay(false);

    thermostat_update();

    ASSERT_FALSE(relay_get());

    printf("PASS\n");

    /*
     * -----------------------------
     * HORS GEL
     * -----------------------------
     */

    thermostat_set_mode(THERMOSTAT_HORS_GEL);

    printf("\nMode HORS GEL\n");

    /* en dessous de 8°C -> ON */

#ifdef MODE_TEST
    climate_test_set_temperature(4.0f);

    thermostat_update();

    ASSERT_TRUE(relay_get());

    printf("PASS\n");

    /* au-dessus de 8.5°C -> OFF */

    climate_test_set_temperature(8.6f);

    thermostat_update();

    ASSERT_FALSE(relay_get());

    printf("PASS\n");

#endif

    /*
     * Résumé
     */

    printf("\nHistory records : %u\n",
           history_count());

    printf("\nPASS : Thermostat\n");

    return true;
}
