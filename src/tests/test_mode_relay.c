#include "test_mode_relay.h"

#include <stdio.h>

#include "climate.h"
#include "clock.h"
#include "relay.h"
#include "thermostat.h"
#include "test_utils.h"
#include "../console/console_utils.h"


bool test_mode_relay_run(void)
{
    console_print_header(
        "THERMOSTAT MODE / RELAY");


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

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_AUTO));


    /*
     * ==================================================
     * AUTO -> RELAY ON
     * ==================================================
     *
     * Température largement sous la consigne.
     *
     * Le thermostat doit demander le chauffage.
     */

    climate_test_set_temperature(
        16.0f);

    thermostat_update();

    ASSERT_TRUE(
        relay_get());

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    printf(
        "AUTO  : relay ON\n");


    /*
     * ==================================================
     * AUTO -> OFF
     * ==================================================
     *
     * Le mode OFF doit couper le relais,
     * quelle que soit la température.
     */

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_OFF));

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    ASSERT_FALSE(
        thermostat_get_status()->heating_request);

    printf(
        "OFF   : relay OFF\n");


    /*
     * ==================================================
     * OFF -> AUTO
     * ==================================================
     *
     * La température est toujours basse.
     *
     * En revenant en AUTO, le chauffage doit
     * redémarrer.
     */

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_AUTO));

    thermostat_update();

    ASSERT_TRUE(
        relay_get());

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    printf(
        "AUTO  : relay ON again\n");


    /*
     * ==================================================
     * AUTO -> OFF AVEC TEMPÉRATURE HAUTE
     * ==================================================
     */

    climate_test_set_temperature(
        30.0f);

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    ASSERT_FALSE(
        thermostat_get_status()->heating_request);

    printf(
        "AUTO  : relay OFF at high temperature\n");


    /*
     * ==================================================
     * OFF À TEMPÉRATURE BASSE
     * ==================================================
     *
     * Même avec une température basse,
     * OFF doit rester OFF.
     */

    climate_test_set_temperature(
        5.0f);

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_OFF));

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    ASSERT_FALSE(
        thermostat_get_status()->heating_request);

    printf(
        "OFF   : relay remains OFF\n");


    printf(
        "PASS : Thermostat mode / relay\n");

    return true;
}
