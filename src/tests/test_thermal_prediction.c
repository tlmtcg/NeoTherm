#include "test_thermal_prediction.h"

#include <stdio.h>

#include "clock.h"
#include "history.h"
#include "../thermal_learning/thermal_learning.h"
#include "../thermal_prediction/thermal_prediction.h"
#include "thermostat.h"
#include "test_utils.h"
#include "../console/console_utils.h"

bool test_thermal_prediction_run(void)
{
    printf(
        "\n=============== THERMAL PREDICTION TEST ===============\n");

    /*
     * ======================================================
     * INITIALISATION
     * ======================================================
     */

    ASSERT_TRUE(
        history_init());

    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_TRUE(
        thermal_prediction_init());

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 12,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * ======================================================
     * PHASE CHAUFFAGE
     * ======================================================
     *
     * +0.20 °C / minute
     */

    float temp = 18.0f;

    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            true,
            true);

        temp += 0.20f;
    }

    /*
     * ======================================================
     * PHASE REFROIDISSEMENT
     * ======================================================
     *
     * -0.10 °C / minute
     */

    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            false,
            false);

        temp -= 0.10f;
    }

    /*
     * ======================================================
     * ANALYSE LEARNING
     * ======================================================
     */

    ASSERT_TRUE(
        thermal_learning_update());

    ASSERT_TRUE(
        thermal_learning_is_valid());

    printf(
        "Heat rate      : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate   : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    /*
     * ======================================================
     * PREDICTION COMPLETE
     * ======================================================
     *
     * Les deux taux sont disponibles.
     */

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    /*
     * La dernière température enregistrée est
     * proche de 20.00 °C.
     */

    float current =
        thermal_prediction_get_temperature_minutes(
            0.0f);

    float prediction_1 =
        thermal_prediction_get_temperature_minutes_state(
            1.0f,
            false);

    float prediction_5 =
        thermal_prediction_get_temperature_minutes_state(
            5.0f,
            false);

    printf(
        "Current         : %.4f C\n",
        current);

    printf(
        "Natural +1 min  : %.4f C\n",
        prediction_1);

    printf(
        "Natural +5 min  : %.4f C\n",
        prediction_5);

    /*
     * ======================================================
     * VERIFICATION REFROIDISSEMENT
     * ======================================================
     */

    ASSERT_TRUE(
        current > 19.0f);

    ASSERT_TRUE(
        current < 21.0f);

    ASSERT_TRUE(
        prediction_1 < current);

    ASSERT_TRUE(
        prediction_5 < prediction_1);

    /*
     * ======================================================
     * TEST CHAUFFAGE UNIQUEMENT
     * ======================================================
     *
     * Le chauffage est appris mais aucun refroidissement
     * n'est disponible.
     *
     * La prédiction globale reste valide car au moins
     * un scénario est disponible.
     */

    console_print_header(
        "HEATING ONLY");

    history_clear();

    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_TRUE(
        thermal_prediction_init());

    t.hour = 13;
    t.minute = 0;
    t.second = 0;

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    temp = 18.0f;

    /*
     * Première mesure.
     */
    history_add(
        temp,
        5.0f,
        21.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Mesures de chauffage :
     *
     * +0.15 °C / minute
     */
    for (int i = 1; i < 30; i++)
    {
        clock_tick(60);

        temp += 0.15f;

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            true,
            true);
    }

    /*
     * ======================================================
     * ANALYSE LEARNING
     * ======================================================
     */

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate      : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate   : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    printf(
        "Learning valid : %s\n",
        thermal_learning_is_valid()
            ? "YES"
            : "NO");

    /*
     * Le chauffage doit être correctement appris.
     */
    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    /*
     * Aucun refroidissement n'est disponible.
     */
    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    /*
     * Le learning global reste valide grâce
     * au scénario chauffage.
     */
    ASSERT_TRUE(
        thermal_learning_is_valid());
        

    /*
     * Le chauffage est appris.
     */

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    /*
     * Aucun refroidissement n'a été appris.
     */

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    /*
     * La prédiction globale est valide :
     * le scénario chauffage est disponible.
     */

    ASSERT_TRUE(
        thermal_prediction_update());

    ASSERT_TRUE(
        thermal_prediction_is_valid());

    /*
     * La prédiction chauffage doit fonctionner.
     */

    float heating_prediction =
        thermal_prediction_get_heated_temperature_minutes(
            10.0f);

    printf(
        "Heat only      : +10 min = %.4f C\n",
        heating_prediction);

    ASSERT_TRUE(
        heating_prediction > temp);

    /*
     * La prédiction naturelle n'est pas disponible
     * car aucun cooling_rate n'a été appris.
     *
     * Elle doit donc rester à la température courante.
     */

    float natural_prediction =
        thermal_prediction_get_temperature_minutes_state(
            10.0f,
            false);

    ASSERT_EQ_FLOAT(
        temp,
        natural_prediction);

    printf(
        "Natural only   : unavailable\n");

    /*
     * ======================================================
     * TEST LEARNING INVALIDE
     * ======================================================
     */

    console_print_header(
        "INVALID LEARNING");

    history_clear();

    ASSERT_TRUE(
        thermal_learning_init());

    ASSERT_TRUE(
        thermal_prediction_init());

    ASSERT_FALSE(
        thermal_learning_is_valid());

    ASSERT_FALSE(
        thermal_prediction_update());

    ASSERT_FALSE(
        thermal_prediction_is_valid());

    printf(
        "Invalid learning : prediction disabled\n");

    /*
     * ======================================================
     * RESULTAT
     * ======================================================
     */

    printf(
        "PASS : Thermal prediction\n");

    return true;
}
