#include "test_thermostat_thermal_integration.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "history.h"
#include "program.h"
#include "relay.h"
#include "thermostat.h"
#include "thermal_model.h"

#include "test_utils.h"
#include "../infra/console/console_utils.h"


/*
 * ==========================================================
 * TEST D'INTEGRATION THERMIQUE
 * ==========================================================
 *
 * Chaîne testée :
 *
 *     Thermal Model
 *          ↓
 *       Climate
 *          ↓
 *      Thermostat
 *          ↓
 *        Relay
 *
 * Le test laisse réellement évoluer la température
 * avec climate_tick().
 *
 */


/*
 * ==========================================================
 * INITIALISATION
 * ==========================================================
 */

static bool test_thermal_integration_init(void)
{
    console_print_header(
        "INITIALISATION");

    clock_init();

    climate_init();

    relay_init();

    history_init();

    program_init();

    thermal_model_init();

    relay_test_reset();

    thermostat_init();

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    /*
     * thermostat_init() recharge le délai
     * anti-cycle depuis le runtime.
     *
     * On le désactive pour ce test afin de
     * tester uniquement la dynamique thermique.
     */

    relay_set_min_switch_delay(0);

    return true;
}


/*
 * ==========================================================
 * AFFICHAGE
 * ==========================================================
 */

static void print_state(
    const char *label)
{
    const thermostat_status_t *status =
        thermostat_get_status();

    printf(
        "%s\n",
        label);

    printf(
        "  Temperature : %.2f C\n",
        climate_get_temperature());

    printf(
        "  Setpoint    : %.2f C\n",
        status->setpoint);

    printf(
        "  Request     : %s\n",
        status->heating_request
            ? "ON"
            : "OFF");

    printf(
        "  Relay       : %s\n",
        relay_get()
            ? "ON"
            : "OFF");
}


/*
 * ==========================================================
 * DEMARRAGE DU CHAUFFAGE
 * ==========================================================
 */

static bool test_heating_start(void)
{
    console_print_header(
        "DEMARRAGE CHAUFFAGE");

    /*
     * On démarre volontairement sous le seuil ON.
     */

    climate_test_set_temperature(
        17.50f);

    thermostat_update();

    print_state(
        "Etat initial");

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    ASSERT_TRUE(
        relay_get());

    return true;
}


/*
 * ==========================================================
 * EVOLUTION THERMIQUE
 * ==========================================================
 */

static bool test_temperature_rises(void)
{
    console_print_header(
        "EVOLUTION THERMIQUE");

    float initial_temperature =
        climate_get_temperature();

    /*
     * Plusieurs ticks avec le relais ON.
     *
     * climate_tick() utilise le modèle thermique
     * réel pour calculer la nouvelle température.
     */

    for (int i = 0; i < 10; i++)
    {
        climate_tick();
    }

    float final_temperature =
        climate_get_temperature();

    printf(
        "Temperature initiale : %.2f C\n",
        initial_temperature);

    printf(
        "Temperature finale   : %.2f C\n",
        final_temperature);

    /*
     * Avec le chauffage ON, la température
     * doit avoir augmenté.
     */

    ASSERT_TRUE(
        final_temperature >
        initial_temperature);

    ASSERT_TRUE(
        relay_get());

    return true;
}


/*
 * ==========================================================
 * ARRET DU CHAUFFAGE
 * ==========================================================
 */

static bool test_heating_stops(void)
{
    console_print_header(
        "ARRET CHAUFFAGE");

    const thermostat_status_t *status =
        thermostat_get_status();

    float off_threshold =
        status->setpoint +
        status->hysteresis;

    /*
     * On fait évoluer le modèle jusqu'à
     * atteindre le seuil OFF.
     *
     * Limite de sécurité pour éviter une boucle
     * infinie en cas de problème du modèle.
     */

    bool reached_off_threshold = false;

    for (int i = 0; i < 500; i++)
    {
        thermostat_update();

        if (climate_get_temperature() >=
            off_threshold)
        {
            reached_off_threshold = true;
            break;
        }

        climate_tick();
    }

    printf(
        "Temperature : %.2f C\n",
        climate_get_temperature());

    printf(
        "Seuil OFF   : %.2f C\n",
        off_threshold);

    ASSERT_TRUE(
        reached_off_threshold);

    /*
     * Le thermostat doit maintenant demander
     * l'arrêt.
     */

    thermostat_update();

    ASSERT_FALSE(
        thermostat_get_status()->heating_request);

    ASSERT_FALSE(
        relay_get());

    return true;
}


/*
 * ==========================================================
 * REFROIDISSEMENT
 * ==========================================================
 */

static bool test_temperature_falls(void)
{
    console_print_header(
        "REFROIDISSEMENT");

    float initial_temperature =
        climate_get_temperature();

    /*
     * Le relais est maintenant OFF.
     *
     * Le modèle thermique doit donc faire
     * redescendre la température.
     */

    for (int i = 0; i < 10; i++)
    {
        climate_tick();
    }

    float final_temperature =
        climate_get_temperature();

    printf(
        "Temperature initiale : %.2f C\n",
        initial_temperature);

    printf(
        "Temperature finale   : %.2f C\n",
        final_temperature);

    ASSERT_TRUE(
        final_temperature <
        initial_temperature);

    ASSERT_FALSE(
        relay_get());

    return true;
}


/*
 * ==========================================================
 * TEST PRINCIPAL
 * ==========================================================
 */

bool test_thermostat_thermal_integration_run(void)
{
    printf(
        "\n=============== "
        "THERMOSTAT THERMAL INTEGRATION TEST "
        "===============\n");

    /*
     * ------------------------------------------------------
     * Initialisation
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_thermal_integration_init());

    /*
     * ------------------------------------------------------
     * Démarrage chauffage
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_heating_start());

    /*
     * ------------------------------------------------------
     * Montée de température
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_temperature_rises());

    /*
     * ------------------------------------------------------
     * Arrêt au seuil OFF
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_heating_stops());

    /*
     * ------------------------------------------------------
     * Refroidissement
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_temperature_falls());

    printf(
        "\nPASS : Thermostat thermal integration\n");

    return true;
}
