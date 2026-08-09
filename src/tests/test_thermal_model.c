#include <stdio.h>
#include <math.h>

#include "thermal_model.h"
#include "test_utils.h"
#include "../infra/console/console_utils.h"

#define ASSERT_FLOAT_NEAR(expected, actual, epsilon) \
    ASSERT_TRUE(fabsf((expected) - (actual)) < (epsilon))

bool test_thermal_model_run(void)
{
    printf("\n================ THERMAL MODEL TEST ================\n");

    ASSERT_TRUE(
        thermal_model_init());



    /*
     * Température extérieure
     */

    thermal_model_set_outside_temperature(5.0f);

    ASSERT_FLOAT_NEAR(
        5.0f,
        thermal_model_get_outside_temperature(),
        0.001f);



    /*
     * Refroidissement sans chauffage
     */

    thermal_model_set_heat_power(0.25f);
    thermal_model_set_loss_factor(0.01f);
    thermal_model_set_thermal_mass(8.0f);

    float t =
        thermal_model_update(
            20.0f,
            false);

    ASSERT_TRUE(
        t < 20.0f);



    /*
     * Chauffage actif
     */

    float t_heat =
        thermal_model_update(
            20.0f,
            true);

    ASSERT_TRUE(
        t_heat > t);



    /*
     * Température extérieure élevée
     */

    thermal_model_set_outside_temperature(30.0f);

    t =
        thermal_model_update(
            20.0f,
            false);

    ASSERT_TRUE(
        t > 20.0f);



    /*
     * Chauffage plus puissant
     */

    thermal_model_set_outside_temperature(5.0f);

    thermal_model_set_heat_power(1.0f);

    float t1 =
        thermal_model_update(
            20.0f,
            true);

    thermal_model_set_heat_power(3.0f);

    float t2 =
        thermal_model_update(
            20.0f,
            true);

    ASSERT_TRUE(
        t2 > t1);



    /*
     * Plus de pertes thermiques
     */

    thermal_model_set_heat_power(0.0f);

    thermal_model_set_loss_factor(0.01f);

    t1 =
        thermal_model_update(
            20.0f,
            false);

    thermal_model_set_loss_factor(0.10f);

    t2 =
        thermal_model_update(
            20.0f,
            false);

    ASSERT_TRUE(
        t2 < t1);



    /*
     * Plus d'inertie thermique
     */

    thermal_model_set_loss_factor(0.01f);

    thermal_model_set_heat_power(1.0f);

    thermal_model_set_thermal_mass(2.0f);

    t1 =
        thermal_model_update(
            20.0f,
            true);

    thermal_model_set_thermal_mass(20.0f);

    t2 =
        thermal_model_update(
            20.0f,
            true);

    ASSERT_TRUE(
        (t1 - 20.0f) >
        (t2 - 20.0f));

/*
 * Equilibre thermique
 */

console_print_header("Equilibre thermique");

thermal_model_set_outside_temperature(20.0f);
thermal_model_set_heat_power(1.0f);
thermal_model_set_loss_factor(0.01f);
thermal_model_set_thermal_mass(8.0f);


t=
    thermal_model_update(
        20.0f,
        false);


ASSERT_FLOAT_NEAR(
    20.0f,
    t,
    0.001f);

printf("PASS : Equilibre thermique\n");

console_print_header("chauffage compense exactement les pertes");
thermal_model_set_outside_temperature(0.0f);
thermal_model_set_loss_factor(0.1f);
thermal_model_set_thermal_mass(10.0f);
thermal_model_set_heat_power(2.0f);


 t =
    thermal_model_update(
        20.0f,
        true);


ASSERT_FLOAT_NEAR(
    20.0f,
    t,
    0.001f);

    printf("PASS : Thermal model\n");

    return true;
}
