#include "test_thermostat_prediction_boundary.h"

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

#define PREDICTION_MINUTES   10.0f
#define PREDICTION_MARGIN    0.10f

/*
 * ==========================================================
 * TEST D'UNE TEMPERATURE
 * ==========================================================
 *
 * IMPORTANT :
 *
 * La consigne de référence est TOUJOURS celle du programme :
 *
 *     program_get_setpoint()
 *
 * On ne modifie jamais la consigne du thermostat avec
 * thermostat_set_setpoint().
 *
 * La température testée est construite par rapport à cette
 * consigne.
 */
static bool test_temperature(
    float program_setpoint,
    float temperature,
    bool expected_request)
{
    printf(
        "\n--- Temperature %.2f C ---\n",
        temperature);

    /*
     * ------------------------------------------------------
     * Vérification de la consigne du programme
     * ------------------------------------------------------
     */

    float current_program_setpoint =
        program_get_setpoint();

    ASSERT_EQ_FLOAT(
        program_setpoint,
        current_program_setpoint);

    /*
     * ------------------------------------------------------
     * Relais
     * ------------------------------------------------------
     *
     * Chaque cas doit commencer relais OFF.
     */

    relay_test_reset();
    relay_set_min_switch_delay(0);

    /*
     * ------------------------------------------------------
     * Température
     * ------------------------------------------------------
     */

    climate_test_set_temperature(
        temperature);

    /*
     * ------------------------------------------------------
     * Historique
     * ------------------------------------------------------
     *
     * La dernière mesure sert à la prédiction.
     */

    history_add(
        temperature,
        5.0f,
        program_setpoint,
        THERMOSTAT_AUTO,
        false,
        false);

    /*
     * ------------------------------------------------------
     * Mise à jour prédiction
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    /*
     * ------------------------------------------------------
     * Thermostat
     * ------------------------------------------------------
     *
     * En AUTO, thermostat_update() relit lui-même :
     *
     *     program_get_setpoint()
     */

    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    /*
     * ------------------------------------------------------
     * Vérification de la consigne réellement utilisée
     * ------------------------------------------------------
     */

    ASSERT_EQ_FLOAT(
        program_setpoint,
        status->setpoint);

    /*
     * ------------------------------------------------------
     * Affichage
     * ------------------------------------------------------
     */

    printf(
        "Setpoint programme : %.2f C\n",
        current_program_setpoint);

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
        "Expected            : %s\n",
        expected_request
            ? "ON"
            : "OFF");

    /*
     * ------------------------------------------------------
     * Vérification
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        status->heating_request ==
        expected_request);

    return true;
}


/*
 * ==========================================================
 * TEST PRINCIPAL
 * ==========================================================
 */

bool test_thermostat_prediction_boundary_run(void)
{
    printf(
        "\n=============== THERMOSTAT PREDICTION BOUNDARY TEST ===============\n");

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
     * Fixer l'heure du test
     * ------------------------------------------------------
     *
     * La consigne du programme dépend de l'heure.
     * On fixe donc l'heure AVANT de lire la consigne.
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
     * CONSIGNE DU PROGRAMME
     * ------------------------------------------------------
     *
     * C'est LA référence du test.
     *
     * On ne prend pas thermostat_get_setpoint().
     * On ne fait pas thermostat_set_setpoint().
     */

    const float program_setpoint =
        program_get_setpoint();

    printf(
        "Setpoint programme : %.2f C\n",
        program_setpoint);

    /*
     * ======================================================
     * THERMAL LEARNING
     * ======================================================
     */

    console_print_header(
        "THERMAL LEARNING");

    history_clear();

    /*
     * ------------------------------------------------------
     * Phase chauffage
     * ------------------------------------------------------
     */

    float temperature = 10.0f;

    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temperature,
            5.0f,
            program_setpoint,
            THERMOSTAT_AUTO,
            true,
            true);

        temperature += 0.20f;
    }

    /*
     * ------------------------------------------------------
     * Phase refroidissement
     * ------------------------------------------------------
     */

    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temperature,
            5.0f,
            program_setpoint,
            THERMOSTAT_AUTO,
            false,
            false);

        temperature -= 0.10f;
    }

    /*
     * ------------------------------------------------------
     * Revenir à l'heure du test
     * ------------------------------------------------------
     *
     * Important :
     * la consigne du programme doit être lue à la même
     * heure que celle utilisée pour les tests.
     */

    ASSERT_TRUE(
        clock_set_time(&test_time));

    clock_sync_to_runtime();

    /*
     * ------------------------------------------------------
     * Apprentissage
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    /*
     * ------------------------------------------------------
     * Vérifier que la consigne programme n'a pas changé
     * ------------------------------------------------------
     */

    ASSERT_EQ_FLOAT(
        program_setpoint,
        program_get_setpoint());

    /*
     * ------------------------------------------------------
     * Validation prédiction
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    /*
     * ======================================================
     * PREDICTION BOUNDARY
     * ======================================================
     *
     * Refroidissement :
     *
     *     0.10 C/min
     *
     * Sur 10 minutes :
     *
     *     1.00 C
     *
     * Marge :
     *
     *     0.10 C
     *
     * Donc la prédiction demande du chauffage lorsque :
     *
     *     temperature - 1.00 < setpoint - 0.10
     *
     * soit :
     *
     *     temperature < setpoint + 0.90
     *
     * A exactement +0.90 :
     *
     *     natural_10min = setpoint - 0.10
     *
     * La comparaison du thermostat est STRICTEMENT <
     * donc la demande prédictive doit disparaître.
     */

    console_print_header(
        "PREDICTION BOUNDARY");

    const float cooling_rate =
        thermal_learning_get_cooling_rate();

    const float boundary =
        program_setpoint +
        (cooling_rate * PREDICTION_MINUTES) -
        PREDICTION_MARGIN;

    printf(
        "Setpoint           : %.2f C\n",
        program_setpoint);

    printf(
        "Cooling rate       : %.2f C/min\n",
        cooling_rate);

    printf(
        "Prediction horizon : %.0f min\n",
        PREDICTION_MINUTES);

    printf(
        "Prediction margin  : %.2f C\n",
        PREDICTION_MARGIN);

    printf(
        "Boundary           : %.2f C\n",
        boundary);

    /*
     * ------------------------------------------------------
     * Sous la consigne
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_temperature(
            program_setpoint,
            program_setpoint - 0.10f,
            true));

    /*
     * ------------------------------------------------------
     * A la consigne
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_temperature(
            program_setpoint,
            program_setpoint,
            true));

    /*
     * ------------------------------------------------------
     * +0.30
     *
     * Ici la température est encore sous le seuil
     * normal ON si l'hystérésis est de 0.30.
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_temperature(
            program_setpoint,
            program_setpoint + 0.30f,
            false));

    /*
     * ------------------------------------------------------
     * +0.50
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_temperature(
            program_setpoint,
            program_setpoint + 0.50f,
            false));

    /*
     * ------------------------------------------------------
     * +0.80
     * ------------------------------------------------------
     *
     * La prédiction naturelle est encore :
     *
     *     setpoint - 0.20
     *
     * mais la logique normale ne demande déjà plus
     * nécessairement de chauffage.
     *
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_temperature(
            program_setpoint,
            program_setpoint + 0.80f,
            false));

    /*
     * ------------------------------------------------------
     * +0.90 : FRONTIERE PREDICTIVE
     * ------------------------------------------------------
     *
     * Avec :
     *
     *     cooling = 0.10 C/min
     *     horizon = 10 min
     *     margin  = 0.10 C
     *
     * on obtient :
     *
     *     setpoint + 0.90
     *
     * A cette valeur :
     *
     *     natural_10min =
     *         setpoint - 0.10
     *
     * donc :
     *
     *     natural_10min <
     *         setpoint - 0.10
     *
     * est FAUX.
     *
     * La demande prédictive doit donc être OFF.
     */

    ASSERT_TRUE(
        test_temperature(
            program_setpoint,
            program_setpoint + 0.90f,
            false));

    /*
     * ------------------------------------------------------
     * Au-dessus de la frontière
     * ------------------------------------------------------
     */

    ASSERT_TRUE(
        test_temperature(
            program_setpoint,
            program_setpoint + 1.00f,
            false));

    /*
     * ======================================================
     * FIN
     * ======================================================
     */

    printf(
        "\nPASS : Thermostat prediction boundary\n");

    return true;
}

