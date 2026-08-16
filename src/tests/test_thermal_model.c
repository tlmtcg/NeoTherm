#include <stdio.h>
#include <math.h>

#include "thermal_model.h"
#include "test_utils.h"
#include "../infra/console/console_utils.h"

#define ASSERT_FLOAT_NEAR(expected, actual, epsilon) \
    ASSERT_TRUE( \
        fabsf((expected) - (actual)) < (epsilon))

bool test_thermal_model_run(void)
{
    printf(
        "\n================ THERMAL MODEL TEST ================\n");

    ASSERT_TRUE(
        thermal_model_init());

    /*======================================================
     * TEMPÉRATURE EXTÉRIEURE
     *=====================================================*/

    console_print_header(
        "OUTSIDE TEMPERATURE");

    thermal_model_set_outside_temperature(
        5.0f);

    ASSERT_FLOAT_NEAR(
        5.0f,
        thermal_model_get_outside_temperature(),
        0.001f);

    printf(
        "Outside temperature : %.2f C\n",
        thermal_model_get_outside_temperature());

    /*======================================================
     * PARAMÈTRES DE BASE
     *=====================================================*/

    thermal_model_set_heat_power(
        0.25f);

    thermal_model_set_loss_factor(
        0.01f);

    thermal_model_set_thermal_mass(
        8.0f);

    /*======================================================
     * REFROIDISSEMENT SANS CHAUFFAGE
     *=====================================================*/

    console_print_header(
        "COOLING WITHOUT HEATING");

    float t =
        thermal_model_update(
            20.0f,
            false);

    printf(
        "20.00 C -> %.4f C\n",
        t);

    ASSERT_TRUE(
        t < 20.0f);

    /*======================================================
     * CHAUFFAGE
     *=====================================================*/

    console_print_header(
        "HEATING");

    float t_heat =
        thermal_model_update(
            20.0f,
            true);

    printf(
        "20.00 C -> %.4f C\n",
        t_heat);

    /*
     * Avec :
     *
     * outside    = 5 C
     * inside     = 20 C
     * loss       = 0.01
     * heat       = 0.25
     * mass       = 8
     *
     * delta =
     * ((5 - 20) * 0.01 + 0.25) / 8
     *
     *       = 0.0125 C
     *
     * résultat = 20.0125 C
     */

    ASSERT_FLOAT_NEAR(
        20.0125f,
        t_heat,
        0.0001f);

    ASSERT_TRUE(
        t_heat > t);

    /*======================================================
     * EXTÉRIEUR PLUS CHAUD
     *=====================================================*/

    console_print_header(
        "WARM OUTSIDE");

    thermal_model_set_outside_temperature(
        30.0f);

    t =
        thermal_model_update(
            20.0f,
            false);

    printf(
        "20.00 C -> %.4f C\n",
        t);

    ASSERT_TRUE(
        t > 20.0f);

    /*======================================================
     * CHAUFFAGE PLUS PUISSANT
     *=====================================================*/

    console_print_header(
        "HEATING POWER");

    thermal_model_set_outside_temperature(
        5.0f);

    thermal_model_set_heat_power(
        1.0f);

    float t1 =
        thermal_model_update(
            20.0f,
            true);

    thermal_model_set_heat_power(
        3.0f);

    float t2 =
        thermal_model_update(
            20.0f,
            true);

    printf(
        "Heat 1.0 -> %.4f C\n",
        t1);

    printf(
        "Heat 3.0 -> %.4f C\n",
        t2);

    ASSERT_TRUE(
        t2 > t1);

    /*======================================================
     * PLUS DE PERTES THERMIQUES
     *=====================================================*/

    console_print_header(
        "THERMAL LOSSES");

    thermal_model_set_heat_power(
        0.0f);

    thermal_model_set_loss_factor(
        0.01f);

    t1 =
        thermal_model_update(
            20.0f,
            false);

    thermal_model_set_loss_factor(
        0.10f);

    t2 =
        thermal_model_update(
            20.0f,
            false);

    printf(
        "Loss 0.01 -> %.4f C\n",
        t1);

    printf(
        "Loss 0.10 -> %.4f C\n",
        t2);

    ASSERT_TRUE(
        t2 < t1);

    /*======================================================
     * INERTIE THERMIQUE
     *=====================================================*/

    console_print_header(
        "THERMAL MASS");

    thermal_model_set_loss_factor(
        0.01f);

    thermal_model_set_heat_power(
        1.0f);

    thermal_model_set_thermal_mass(
        2.0f);

    t1 =
        thermal_model_update(
            20.0f,
            true);

    thermal_model_set_thermal_mass(
        20.0f);

    t2 =
        thermal_model_update(
            20.0f,
            true);

    printf(
        "Mass 2  -> %.4f C\n",
        t1);

    printf(
        "Mass 20 -> %.4f C\n",
        t2);

    ASSERT_TRUE(
        (t1 - 20.0f) >
        (t2 - 20.0f));

    /*======================================================
     * CHAUFFAGE > PERTES
     *=====================================================*/

    console_print_header(
        "HEATING GREATER THAN LOSSES");

    thermal_model_set_outside_temperature(
        0.0f);

    thermal_model_set_loss_factor(
        0.10f);

    thermal_model_set_thermal_mass(
        10.0f);

    thermal_model_set_heat_power(
        3.0f);

    t =
        thermal_model_update(
            20.0f,
            true);

    printf(
        "20.00 C -> %.4f C\n",
        t);

    ASSERT_TRUE(
        t > 20.0f);

    /*======================================================
     * CHAUFFAGE < PERTES
     *=====================================================*/

    console_print_header(
        "HEATING LESS THAN LOSSES");

    thermal_model_set_heat_power(
        1.0f);

    t =
        thermal_model_update(
            20.0f,
            true);

    printf(
        "20.00 C -> %.4f C\n",
        t);

    ASSERT_TRUE(
        t < 20.0f);

    /*======================================================
     * ÉQUILIBRE THERMIQUE SANS CHAUFFAGE
     *=====================================================*/

    console_print_header(
        "THERMAL EQUILIBRIUM");

    thermal_model_set_outside_temperature(
        20.0f);

    thermal_model_set_heat_power(
        1.0f);

    thermal_model_set_loss_factor(
        0.01f);

    thermal_model_set_thermal_mass(
        8.0f);

    t =
        thermal_model_update(
            20.0f,
            false);

    ASSERT_FLOAT_NEAR(
        20.0f,
        t,
        0.001f);

    printf(
        "Equilibrium : %.4f C\n",
        t);

    /*======================================================
     * CHAUFFAGE COMPENSE EXACTEMENT LES PERTES
     *=====================================================*/

    console_print_header(
        "HEATING EXACTLY COMPENSATES LOSSES");

    thermal_model_set_outside_temperature(
        0.0f);

    thermal_model_set_loss_factor(
        0.10f);

    thermal_model_set_thermal_mass(
        10.0f);

    thermal_model_set_heat_power(
        2.0f);

    t =
        thermal_model_update(
            20.0f,
            true);

    /*
     * pertes :
     *
     * (0 - 20) * 0.10 = -2.0
     *
     * chauffage :
     *
     * +2.0
     *
     * delta = 0
     */

    ASSERT_FLOAT_NEAR(
        20.0f,
        t,
        0.001f);

    printf(
        "Exact compensation : %.4f C\n",
        t);

    /*======================================================
     * RESULTAT
     *=====================================================*/

    printf(
        "\nPASS : Thermal model\n");

    return true;
}
