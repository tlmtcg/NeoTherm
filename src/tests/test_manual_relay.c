#include "test_manual_relay.h"

#include <stdio.h>

#include "climate.h"
#include "clock.h"
#include "relay.h"
#include "thermostat.h"
#include "test_utils.h"
#include "../console/console_utils.h"


bool test_manual_relay_run(void)
{
    console_print_header(
        "THERMOSTAT MANUAL / RELAY");


    /*
     * ==================================================
     * INITIALISATION
     * ==================================================
     */

    clock_init();
    climate_init();
    relay_init();
    relay_test_reset();
    thermostat_init();

    relay_set_min_switch_delay(0);


    /*
     * ==================================================
     * AUTO -> MANUAL
     * ==================================================
     */

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_MANUAL));

    ASSERT_EQ_INT(
        THERMOSTAT_MANUAL,
        thermostat_get_mode());

    printf(
        "MANUAL : mode active\n");


    /*
     * ==================================================
     * MANUAL -> RELAY ON
     * ==================================================
     */

    ASSERT_TRUE(
        thermostat_manual_set_relay(true));

    thermostat_update();

    ASSERT_TRUE(
        relay_get());

    printf(
        "MANUAL : relay ON\n");


    /*
     * ==================================================
     * MANUAL -> RELAY OFF
     * ==================================================
     */

    ASSERT_TRUE(
        thermostat_manual_set_relay(false));

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    printf(
        "MANUAL : relay OFF\n");


    /*
     * ==================================================
     * MANUAL -> ON SANS THERMOSTAT UPDATE
     * ==================================================
     *
     * La commande manuelle agit immédiatement
     * sur le relais.
     */

    ASSERT_TRUE(
        thermostat_manual_set_relay(true));

    ASSERT_TRUE(
        relay_get());

    printf(
        "MANUAL : immediate relay ON\n");


    /*
     * ==================================================
     * RETOUR AUTO
     * ==================================================
     */

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_AUTO));

    thermostat_update();

    ASSERT_EQ_INT(
        THERMOSTAT_AUTO,
        thermostat_get_mode());

    /*
     * Température élevée :
     * AUTO doit couper le relais.
     */

    climate_test_set_temperature(
        30.0f);

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    printf(
        "AUTO   : manual command no longer controls relay\n");


    /*
     * ==================================================
     * COMMANDE MANUELLE INTERDITE EN AUTO
     * ==================================================
     */

    ASSERT_FALSE(
        thermostat_manual_set_relay(true));

    ASSERT_FALSE(
        relay_get());

    printf(
        "AUTO   : manual command rejected\n");


    /*
     * ==================================================
     * TEST MANUAL À NOUVEAU
     * ==================================================
     */

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_MANUAL));

    ASSERT_TRUE(
        thermostat_manual_set_relay(true));

    ASSERT_TRUE(
        relay_get());

    printf(
        "MANUAL : relay ON again\n");


    printf(
        "PASS : Thermostat manual / relay\n");

    return true;
}
