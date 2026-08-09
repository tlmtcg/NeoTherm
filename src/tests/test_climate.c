#include "test_climate.h"

#include <stdio.h>
#include <math.h>

#include "climate.h"
#include "thermal_model.h"
#include "event.h"
#include "runtime.h"
#include "test_utils.h"
#include "../infra/console/console_utils.h"
#include "relay.h"
#include "program.h"

bool test_climate_run(void)
{
    printf("\n=============== CLIMATE TEST ===============\n");

    /*
     * Initialisation environnement
     */

    event_init();

    thermal_model_init();

    /*
     * Initialisation climate
     */

    climate_init();

    float temp =
        climate_get_temperature();

    printf("Initial : %.2f C\n",
           temp);

    /*
     * Après initialisation, la température
     * doit être initialisée avec la consigne
     * courante du runtime.
     */
    ASSERT_EQ_FLOAT(
        runtime_get()->setpoint,
        temp);

    /*
     * Test modification température
     */

    climate_update(
        18.75f);

    temp =
        climate_get_temperature();

    printf("Updated : %.2f C\n",
           temp);

    ASSERT_EQ_FLOAT(
        18.75f,
        temp);

    printf("\nPASS : Climate\n");

    /*
     * -----------------------------
     * CLIMATE HEATING
     * -----------------------------
     */

    console_print_header("CLIMATE HEATING");

    relay_set_min_switch_delay(0);

    /*
     * Modèle thermique
     */

    thermal_model_set_outside_temperature(
        5.0f);

    thermal_model_set_heat_power(
        8.0f);

    thermal_model_set_loss_factor(
        0.01f);

    thermal_model_set_thermal_mass(
        8.0f);

    /*
     * Chauffage forcé
     */

    relay_set(true);

    climate_test_set_temperature(
        18.0f);

    float before =
        climate_get_temperature();

    for (int i = 0; i < 10; i++)
    {
        climate_tick();
    }

    float after =
        climate_get_temperature();

    printf("Before %.2f After %.2f\n",
           before,
           after);

    ASSERT_TRUE(
        after > before);

    printf("PASS\n");

    /*
     * -----------------------------
     * CLIMATE COOLING
     * -----------------------------
     */

    console_print_header("CLIMATE COOLING");

    relay_set(false);

    climate_test_set_temperature(
        20.0f);

    before =
        climate_get_temperature();

    for (int i = 0; i < 10; i++)
    {
        climate_tick();
    }

    after =
        climate_get_temperature();

    ASSERT_TRUE(
        after < before);

    printf("PASS\n");

    /*
     * -----------------------------
     * CLIMATE THERMAL MODEL
     * -----------------------------
     */

    console_print_header("CLIMATE THERMAL MODEL");

    relay_set_min_switch_delay(0);

    /*
     * Configuration thermique
     */

    thermal_model_set_outside_temperature(5.0f);
    thermal_model_set_heat_power(8.0f);
    thermal_model_set_loss_factor(0.01f);
    thermal_model_set_thermal_mass(8.0f);

    /*
     * Chauffage ON
     */

    relay_set(true);

    climate_test_set_temperature(18.0f);

    before =
        climate_get_temperature();

    for (int i = 0; i < 20; i++)
    {
        climate_tick();
    }

    after =
        climate_get_temperature();

    printf("Before %.2f After %.2f\n",
           before,
           after);

    ASSERT_TRUE(after > before);

    printf("PASS\n");

    /*
     * -----------------------------
     * FULL THERMOSTAT LOOP
     * -----------------------------
     */

    console_print_header("FULL THERMOSTAT LOOP");

    relay_set_min_switch_delay(0);

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    /*
     * Récupération vraie consigne programme
     */

    float setpoint =
        program_get_setpoint();

    thermal_model_set_outside_temperature(
        5.0f);

    thermal_model_set_heat_power(
        0.5f);

    thermal_model_set_loss_factor(
        0.01f);

    thermal_model_set_thermal_mass(
        20.0f);

    climate_test_set_temperature(
        setpoint - 2.0f);

    for (int i = 0; i < 200; i++)
    {
        thermostat_update();
        climate_tick();
    }

    printf("Setpoint %.2f\n", setpoint);
    printf("Temperature %.2f\n",
           climate_get_temperature());

    printf("Relay %s\n",
           relay_get() ? "ON" : "OFF");


    return true;
}
