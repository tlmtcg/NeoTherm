#include "test_thermostat_prediction_invalid.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "history.h"
#include "relay.h"
#include "thermostat.h"

#include "../thermal_learning/thermal_learning.h"
#include "../thermal_prediction/thermal_prediction.h"

#include "test_utils.h"
#include "../infra/console/console_utils.h"

bool test_thermostat_prediction_invalid_run(void)
{
    printf(
        "\n=============== THERMOSTAT PREDICTION INVALID TEST ===============\n");

    /*
     * --------------------------------------------------
     * INITIALISATION
     * --------------------------------------------------
     */

    console_print_header("INITIALISATION");

    clock_init();
    climate_init();
    relay_init();
    history_init();

    thermal_learning_init();
    thermal_prediction_init();

    relay_test_reset();
    relay_set_min_switch_delay(0);

    thermostat_init();
    thermostat_set_mode(THERMOSTAT_AUTO);

    /*
     * --------------------------------------------------
     * HISTORIQUE THERMIQUE
     * --------------------------------------------------
     *
     * On fournit uniquement des mesures de chauffage.
     *
     * Le taux de chauffage doit donc être valide.
     *
     * Aucun historique de refroidissement n'est fourni.
     *
     * => cooling_rate = 0
     * => prédiction invalide
     */

    console_print_header("THERMAL LEARNING");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 15,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    float temp = 18.0f;

    /*
     * Phase chauffage uniquement.
     */
    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            18.0f,
            THERMOSTAT_AUTO,
            true,
            true);

        temp += 0.20f;
    }

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    /*
     * --------------------------------------------------
     * PREDICTION INVALID
     * --------------------------------------------------
     */

    console_print_header(
        "PREDICTION INVALID");

    /*
     * Aucun apprentissage disponible.
     */
    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_FALSE(
        thermal_learning_is_valid());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_heat_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    /*
     * La dernière mesure de l'historique est utilisée
     * par thermal_prediction_update().
     *
     * Aucun taux n'étant disponible, la prédiction
     * est invalide.
     */
    ASSERT_FALSE(
        thermal_prediction_update());

    ASSERT_FALSE(
        thermal_prediction_is_valid());
        
    /*
     * --------------------------------------------------
     * THERMOSTAT
     * --------------------------------------------------
     *
     * On place la température à 17.90 °C.
     *
     * Cette température est :
     *
     *     > seuil ON = 17.70 °C
     *
     * Donc la logique normale ne demande pas
     * encore le chauffage.
     *
     * Comme la prédiction est invalide, elle ne doit
     * pas provoquer de demande de chauffage.
     */

    climate_test_set_temperature(
        17.90f);

    /*
     * La température de test doit être présente dans
     * l'historique pour que la prédiction utilise cette
     * valeur comme température courante.
     *
     * On conserve cependant l'état "heating=true"
     * de l'historique d'apprentissage afin de ne pas
     * créer artificiellement un taux de refroidissement.
     */
    history_add(
        17.90f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * La prédiction doit rester invalide.
     */
    ASSERT_FALSE(
        thermal_prediction_update());

    ASSERT_FALSE(
        thermal_prediction_is_valid());

    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    /*
     * La prédiction étant invalide, elle ne doit pas
     * déclencher le chauffage.
     */
    ASSERT_FALSE(
        status->heating_request);

    ASSERT_FALSE(
        relay_get());

    printf(
        "Invalid prediction : heating OFF\n");

    /*
     * --------------------------------------------------
     * RESUME
     * --------------------------------------------------
     */

    printf(
        "\nPASS : Thermostat prediction invalid\n");

    return true;
}
