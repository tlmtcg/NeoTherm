
#include "test_thermostat_integration.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "history.h"
#include "program.h"
#include "relay.h"
#include "thermostat.h"

#include "test_utils.h"
#include "../infra/console/console_utils.h"

/*
 * ==========================================================
 * TEST D'INTEGRATION THERMOSTAT
 * ==========================================================
 *
 * Chaîne testée :
 *
 *     Climate
 *        ↓
 *     Thermostat
 *        ↓
 *      Relay
 *
 * Le test vérifie le comportement global du thermostat
 * AUTO avec hystérésis.
 *
 */

/*
 * ==========================================================
 * INITIALISATION
 * ==========================================================
 */

static bool test_integration_init(void)
{
    console_print_header(
        "INITIALISATION");

    clock_init();

    climate_init();

    relay_init();

    history_init();

    program_init();

    relay_test_reset();

    thermostat_init();

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    /*
     * Le thermostat initialise le relais avec
     * la configuration runtime (300 s).
     *
     * Pour ce test d'intégration, on désactive
     * temporairement l'anti-cycle afin de tester
     * directement la chaîne :
     *
     *     Climate -> Thermostat -> Relay
     */
    relay_set_min_switch_delay(0);
    
    return true;
}

/*
 * ==========================================================
 * VERIFICATION ETAT
 * ==========================================================
 */

static bool check_state(
    float temperature,
    bool expected_request,
    bool expected_relay)
{
    const thermostat_status_t *status =
        thermostat_get_status();

    printf(
        "\nTemperature : %.2f C\n",
        temperature);

    printf(
        "Setpoint    : %.2f C\n",
        status->setpoint);

    printf(
        "Request     : %s\n",
        status->heating_request
            ? "ON"
            : "OFF");

    printf(
        "Relay       : %s\n",
        relay_get()
            ? "ON"
            : "OFF");

    printf(
        "Expected    : request=%s relay=%s\n",
        expected_request
            ? "ON"
            : "OFF",
        expected_relay
            ? "ON"
            : "OFF");

    ASSERT_TRUE(
        status->heating_request ==
        expected_request);

    ASSERT_TRUE(
        relay_get() ==
        expected_relay);

    return true;
}

/*
 * ==========================================================
 * SCENARIO 1
 * TEMPERATURE SOUS LE SEUIL ON
 * ==========================================================
 */

static bool test_heating_start(void)
{
    console_print_header(
        "HEATING START");

    climate_test_set_temperature(
        17.60f);

    thermostat_update();

    /*
     * Sous le seuil ON :
     *
     * request = ON
     * relay   = ON
     */

    ASSERT_TRUE(
        check_state(
            17.60f,
            true,
            true));

    return true;
}

/*
 * ==========================================================
 * SCENARIO 2
 * TEMPERATURE DANS L'HYSTERESIS
 * ==========================================================
 */

static bool test_hysteresis_hold(void)
{
    console_print_header(
        "HYSTERESIS HOLD");

    climate_test_set_temperature(
        18.00f);

    thermostat_update();

    /*
     * Température dans la bande :
     *
     * le relais reste ON.
     *
     * La demande normale reste également ON
     * dans le fonctionnement intégré.
     */

    ASSERT_TRUE(
        check_state(
            18.00f,
            true,
            true));

    return true;
}

/*
 * ==========================================================
 * SCENARIO 3
 * TEMPERATURE AU-DESSUS DU SEUIL OFF
 * ==========================================================
 */

static bool test_heating_stop(void)
{
    console_print_header(
        "HEATING STOP");

    climate_test_set_temperature(
        18.40f);

    thermostat_update();

    /*
     * Au-dessus du seuil OFF :
     *
     * request = OFF
     * relay   = OFF
     */

    ASSERT_TRUE(
        check_state(
            18.40f,
            false,
            false));

    return true;
}

/*
 * ==========================================================
 * SCENARIO 4
 * REFROIDISSEMENT APRES ARRET
 * ==========================================================
 */

static bool test_heating_restart(void)
{
    console_print_header(
        "HEATING RESTART");

    climate_test_set_temperature(
        17.60f);

    thermostat_update();

    /*
     * Retour sous le seuil ON :
     *
     * request = ON
     * relay   = ON
     */

    ASSERT_TRUE(
        check_state(
            17.60f,
            true,
            true));

    return true;
}

/*
 * ==========================================================
 * TEST PRINCIPAL
 * ==========================================================
 */

bool test_thermostat_integration_run(void)
{
    printf(
        "\n=============== "
        "THERMOSTAT INTEGRATION TEST "
        "===============\n");

    /*
     * ------------------------------------------------------
     * Initialisation
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_integration_init());

    /*
     * ------------------------------------------------------
     * Consigne
     * ------------------------------------------------------
     */

    const thermostat_status_t *status =
        thermostat_get_status();

    printf(
        "Setpoint   : %.2f C\n",
        status->setpoint);

    printf(
        "Hysteresis : %.2f C\n",
        status->hysteresis);

    /*
     * ------------------------------------------------------
     * Scénario complet
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_heating_start());

    ASSERT_TRUE(
        test_hysteresis_hold());

    ASSERT_TRUE(
        test_heating_stop());

    ASSERT_TRUE(
        test_heating_restart());

    printf(
        "\nPASS : Thermostat integration\n");

    return true;
}
