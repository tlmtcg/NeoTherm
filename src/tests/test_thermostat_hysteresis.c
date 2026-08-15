#include "test_thermostat_hysteresis.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "history.h"
#include "relay.h"
#include "thermostat.h"
#include "program.h"

#include "../thermal_learning/thermal_learning.h"
#include "../thermal_prediction/thermal_prediction.h"

#include "test_utils.h"
#include "../infra/console/console_utils.h"


/*
 * ==========================================================
 * TEST D'UN CAS D'HYSTERESIS
 * ==========================================================
 */

static bool test_temperature(
    float program_setpoint,
    float temperature,
    bool initial_relay,
    bool expected_request,
    bool expected_relay)
{
    printf(
        "\n--- Temperature %.2f C ---\n",
        temperature);

    /*
     * ------------------------------------------------------
     * Consigne programme
     * ------------------------------------------------------
     */

    ASSERT_EQ_FLOAT(
        program_setpoint,
        program_get_setpoint());

    /*
     * ------------------------------------------------------
     * Relais
     * ------------------------------------------------------
     *
     * On impose explicitement l'état initial du relais.
     */

    relay_test_reset();
    relay_set_min_switch_delay(0);

    if (initial_relay)
    {
        ASSERT_TRUE(
            relay_set(true));
    }

    /*
     * ------------------------------------------------------
     * Température
     * ------------------------------------------------------
     */

    climate_test_set_temperature(
        temperature);

    /*
     * ------------------------------------------------------
     * Prédiction invalide
     * ------------------------------------------------------
     *
     * Ce test vérifie uniquement l'hystérésis classique.
     */

    ASSERT_TRUE(
        !thermal_prediction_is_valid());

    /*
     * ------------------------------------------------------
     * Thermostat
     * ------------------------------------------------------
     */

    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    /*
     * ------------------------------------------------------
     * Affichage
     * ------------------------------------------------------
     */

    printf(
        "Setpoint programme : %.2f C\n",
        program_setpoint);

    printf(
        "Setpoint thermostat: %.2f C\n",
        status->setpoint);

    printf(
        "Temperature        : %.2f C\n",
        temperature);

    printf(
        "Request            : %s\n",
        status->heating_request
            ? "ON"
            : "OFF");

    printf(
        "Relay              : %s\n",
        relay_get()
            ? "ON"
            : "OFF");

    printf(
        "Expected request   : %s\n",
        expected_request
            ? "ON"
            : "OFF");

    printf(
        "Expected relay     : %s\n",
        expected_relay
            ? "ON"
            : "OFF");

    /*
     * ------------------------------------------------------
     * Vérifications
     * ------------------------------------------------------
     */

    ASSERT_EQ_FLOAT(
        program_setpoint,
        status->setpoint);

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
 * TEST PRINCIPAL
 * ==========================================================
 */

bool test_thermostat_hysteresis_run(void)
{
    printf(
        "\n=============== THERMOSTAT HYSTERESIS TEST ===============\n");

    /*
     * ======================================================
     * INITIALISATION
     * ======================================================
     */

    console_print_header(
        "INITIALISATION");

    clock_init();
    climate_init();
    relay_init();
    history_init();

    thermal_learning_init();
    thermal_prediction_init();

    relay_test_reset();
    relay_set_min_switch_delay(0);

    thermostat_init();

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    /*
     * ------------------------------------------------------
     * Heure du test
     * ------------------------------------------------------
     */

    clock_time_t test_time =
    {
        .year = 2026,
        .month = 8,
        .day = 3,
        .hour = 17,
        .minute = 0,
        .second = 0
    };

    ASSERT_TRUE(
        clock_set_time(&test_time));

    clock_sync_to_runtime();

    /*
     * ------------------------------------------------------
     * Consigne programme
     * ------------------------------------------------------
     */

    const float setpoint =
        program_get_setpoint();

    printf(
        "Setpoint programme : %.2f C\n",
        setpoint);

    /*
     * ------------------------------------------------------
     * Hystérésis
     * ------------------------------------------------------
     */

    const float hysteresis =
        thermostat_get_hysteresis();

    printf(
        "Hysteresis         : %.2f C\n",
        hysteresis);

    const float on_threshold =
        setpoint - hysteresis;

    const float off_threshold =
        setpoint + hysteresis;

    printf(
        "Seuil ON           : %.2f C\n",
        on_threshold);

    printf(
        "Seuil OFF          : %.2f C\n",
        off_threshold);


    /*
     * ======================================================
     * TEST 1
     * ======================================================
     *
     * Sous le seuil ON.
     *
     * => demande ON
     * => relais ON
     */

    console_print_header(
        "SOUS SEUIL ON");

    ASSERT_TRUE(
        test_temperature(
            setpoint,
            on_threshold - 0.01f,
            false,
            true,
            true));


    /*
     * ======================================================
     * TEST 2
     * ======================================================
     *
     * Exactement sur le seuil ON.
     *
     * <= seuil ON
     *
     * => demande ON
     * => relais ON
     */

    console_print_header(
        "SEUIL ON");

    ASSERT_TRUE(
        test_temperature(
            setpoint,
            on_threshold,
            false,
            true,
            true));


    /*
     * ======================================================
     * TEST 3
     * ======================================================
     *
     * Dans la zone de maintien avec relais ON.
     *
     * Le relais était ON.
     *
     * => la demande reste ON
     * => le relais reste ON
     */

    console_print_header(
        "ZONE DE MAINTIEN - RELAIS ON");

    ASSERT_TRUE(
        test_temperature(
            setpoint,
            setpoint,
            true,
            true,
            true));


    /*
     * ======================================================
     * TEST 4
     * ======================================================
     *
     * Dans la zone de maintien avec relais OFF.
     *
     * Le relais était OFF.
     *
     * => la demande reste OFF
     * => le relais reste OFF
     */

    console_print_header(
        "ZONE DE MAINTIEN - RELAIS OFF");

    ASSERT_TRUE(
        test_temperature(
            setpoint,
            setpoint + 0.10f,
            false,
            false,
            false));


    /*
     * ======================================================
     * TEST 5
     * ======================================================
     *
     * Exactement sur le seuil OFF.
     *
     * >= seuil OFF
     *
     * => demande OFF
     * => relais OFF
     */

    console_print_header(
        "SEUIL OFF");

    ASSERT_TRUE(
        test_temperature(
            setpoint,
            off_threshold,
            true,
            false,
            false));


    /*
     * ======================================================
     * TEST 6
     * ======================================================
     *
     * Au-dessus du seuil OFF.
     *
     * => demande OFF
     * => relais OFF
     */

    console_print_header(
        "AU-DESSUS SEUIL OFF");

    ASSERT_TRUE(
        test_temperature(
            setpoint,
            off_threshold + 0.01f,
            true,
            false,
            false));


    /*
     * ======================================================
     * FIN
     * ======================================================
     */

    printf(
        "\nPASS : Thermostat hysteresis\n");

    return true;
}
