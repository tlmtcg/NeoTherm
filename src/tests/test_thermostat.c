#include "test_thermostat.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "history.h"
#include "program.h"
#include "relay.h"
#include "storage.h"
#include "thermostat.h"
#include "test_utils.h"
#include "../infra/console/console_utils.h"
#include "schedule.h"

#define MODE_TEST

static bool test_program_run(void)
{
    /*
     * -----------------------------
     * PROGRAM MORNING
     * -----------------------------
     */

    console_print_header("PROGRAM MORNING");

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 7,
            .minute = 0,
            .second = 0};

    clock_set_time(&t);

    float setpoint =
        program_get_setpoint();

    ASSERT_EQ_FLOAT(
        21.0f,
        setpoint);

    printf("PASS\n");

    /*
     * -----------------------------
     * PROGRAM BEFORE MORNING
     * -----------------------------
     */

    console_print_header("PROGRAM BEFORE MORNING");

    t.year = 2026;
    t.month = 8;
    t.day = 3;
    t.hour = 5;
    t.minute = 59;
    t.second = 0;

    clock_set_time(&t);

    setpoint =
        program_get_setpoint();

    ASSERT_EQ_FLOAT(
        DEFAULT_SETPOINT,
        setpoint);

    printf("PASS\n");

    /*
     * -----------------------------
     * PROGRAM EVENING
     * -----------------------------
     */

    console_print_header("PROGRAM EVENING");

    t.year = 2026;
    t.month = 8;
    t.day = 3;
    t.hour = 23;
    t.minute = 0;
    t.second = 0;

    clock_set_time(&t);

    setpoint =
        program_get_setpoint();

    ASSERT_EQ_FLOAT(
        17.0f,
        setpoint);

    /*
     * -----------------------------
     * PROGRAM + THERMOSTAT AUTO
     * -----------------------------
     */

    console_print_header("PROGRAM THERMOSTAT");

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    /*
     * Lundi 07:00
     * Consigne attendue : 21°C
     */
    t.year = 2026;
    t.month = 8;
    t.day = 3;
    t.hour = 7;
    t.minute = 0;
    t.second = 0;

    clock_set_time(&t);

    /*
     * Vérification programme
     */
    ASSERT_EQ_FLOAT(
        21.0f,
        program_get_setpoint());

    /*
     * Température sous consigne
     */
    climate_test_set_temperature(
        18.0f);

    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    ASSERT_EQ_FLOAT(
        21.0f,
        status->setpoint);

    ASSERT_TRUE(
        relay_get());

    /*
     * -----------------------------
     * PROGRAM NIGHT
     * -----------------------------
     */

    console_print_header("PROGRAM NIGHT");

    t.hour = 23;
    t.minute = 0;
    t.second = 0;

    clock_set_time(&t);

    ASSERT_EQ_FLOAT(
        17.0f,
        program_get_setpoint());

    climate_test_set_temperature(
        18.0f);

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    printf("PASS\n");

    return true;
}

bool test_thermostat_run(void)
{
    printf("\n=============== THERMOSTAT TEST ===============\n");

    /*
     * Initialisation
     */
    console_print_header("Initialisation");
    clock_init();
    climate_init();
    relay_init();
    history_init();
    storage_init();
    program_init();
    thermostat_init();
    relay_test_reset();

    thermostat_set_mode(THERMOSTAT_AUTO);

    ASSERT_EQ_INT(
        THERMOSTAT_AUTO,
        thermostat_get_mode());

    ASSERT_FALSE(relay_get());

    printf("PASS\n");

    /*
     * -----------------------------
     * SETPOINT
     * -----------------------------
     */

    console_print_header("SETPOINT");
    thermostat_set_setpoint(21.5f);

    ASSERT_EQ_FLOAT(
        21.5f,
        thermostat_get_setpoint());

    printf("PASS\n");

    /*
     * -----------------------------
     * HYSTERESIS
     * -----------------------------
     */

    console_print_header("HYSTERESIS");

    thermostat_set_mode(THERMOSTAT_AUTO);

    /*
     * Lire la consigne réellement utilisée par AUTO
     */
    float setpoint = program_get_setpoint();

    float hyst = thermostat_get_hysteresis();

    /*
     * En dessous du seuil ON
     */
    relay_set(false);

    climate_test_set_temperature(
        setpoint - hyst - 0.1f);

    clock_add_seconds(180);

    thermostat_update();

    ASSERT_TRUE(relay_get());

    /*
     * Dans la bande d'hystérésis
     */
    climate_test_set_temperature(
        setpoint);

    thermostat_update();

    ASSERT_TRUE(relay_get());

    /*
     * Au-dessus du seuil OFF
     */
    climate_test_set_temperature(
        setpoint + hyst + 0.1f);

    thermostat_update();

    ASSERT_FALSE(relay_get());

    printf("PASS\n");
    /*
     * -----------------------------
     * MODE CHANGE
     * -----------------------------
     */

    console_print_header("MODE CHANGE");
    thermostat_set_mode(
        THERMOSTAT_MANUAL);

    ASSERT_EQ_INT(
        THERMOSTAT_MANUAL,
        thermostat_get_mode());

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    ASSERT_EQ_INT(
        THERMOSTAT_AUTO,
        thermostat_get_mode());

    printf("PASS\n");

    /*
     * -----------------------------
     * INVALID MODE
     * -----------------------------
     */

    console_print_header("INVALID MODE");
    ASSERT_FALSE(
        thermostat_set_mode(
            (thermostat_mode_t)99));

    printf("PASS\n");

    /*
     * -----------------------------
     * INVALID SETPOINT
     * -----------------------------
     */

    console_print_header("INVALID SETPOINT");

    float previous =
        thermostat_get_setpoint();

    ASSERT_FALSE(
        thermostat_set_setpoint(-100));

    ASSERT_EQ_FLOAT(
        previous,
        thermostat_get_setpoint());

    printf("PASS\n");

    /*
     * -----------------------------
     * AUTO
     * -----------------------------
     */

    console_print_header("AUTO");
    thermostat_set_mode(
        THERMOSTAT_AUTO);

    printf("\nMode AUTO\n");

    /*
     * Température supérieure à la consigne
     */

    climate_test_set_temperature(
        20.0f);

    thermostat_update();

    ASSERT_FALSE(relay_get());

    printf("PASS\n");

    /*
     * Température inférieure à la consigne
     */

    climate_test_set_temperature(
        16.0f);

    thermostat_update();

    ASSERT_TRUE(relay_get());

    printf("PASS\n");

    /*
     * -----------------------------
     * OFF
     * -----------------------------
     */

    console_print_header("OFF");
    thermostat_set_mode(
        THERMOSTAT_OFF);

    thermostat_update();

    ASSERT_FALSE(relay_get());

    /* OFF quelque soit la température*/

    climate_test_set_temperature(
        5.0f);

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    climate_test_set_temperature(
        30.0f);

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    printf("PASS\n");

    /*
     * -----------------------------
     * MANUAL
     * -----------------------------
     */

    console_print_header("MANUAL");
    thermostat_set_mode(
        THERMOSTAT_MANUAL);

    thermostat_manual_set_relay(true);

    thermostat_update();

    ASSERT_TRUE(relay_get());

    printf("PASS\n");

    thermostat_manual_set_relay(false);

    thermostat_update();

    ASSERT_FALSE(relay_get());

    printf("PASS\n");

    /*
     * -----------------------------
     * RELAY REQUEST
     * -----------------------------
     */
    console_print_header("RELAY REQUEST");

    thermostat_set_mode(THERMOSTAT_AUTO);

    /*
     * Première mise à jour pour obtenir
     * la consigne effective du programme
     */
    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    setpoint = status->setpoint;
    hyst = status->hysteresis;

    /*
     * Demande de chauffage
     */
    climate_test_set_temperature(
        setpoint - hyst - 1.0f);

    thermostat_update();

    ASSERT_TRUE(relay_get());

    /*
     * Arrêt chauffage
     */
    climate_test_set_temperature(
        setpoint + hyst + 1.0f);

    clock_add_seconds(180);

    thermostat_update();

    ASSERT_FALSE(relay_get());

    printf("PASS\n");

    /*
     * -----------------------------
     * HORS GEL
     * -----------------------------
     */

    console_print_header("HORS GEL");

    thermostat_set_mode(
        THERMOSTAT_HORS_GEL);

    /* Mise à jour du statut */
    thermostat_update();

    ASSERT_EQ_FLOAT(
        HORS_GEL_SETPOINT,
        thermostat_get_setpoint());

    climate_test_set_temperature(
        HORS_GEL_SETPOINT -
        HORS_GEL_HYSTERESIS -
        0.5f);

    thermostat_update();

    ASSERT_TRUE(
        relay_get());

    climate_test_set_temperature(
        HORS_GEL_SETPOINT +
        HORS_GEL_HYSTERESIS +
        0.5f);

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    printf("PASS\n");

    /*
     * -----------------------------
     * STATUS
     * -----------------------------
     */

    console_print_header("STATUS");
    status = thermostat_get_status();

    ASSERT_EQ_INT(
        thermostat_get_mode(),
        status->mode);

    ASSERT_EQ_FLOAT(
        thermostat_get_setpoint(),
        status->setpoint);

    printf("PASS\n");

    /*
     * -----------------------------
     * HISTORY
     * -----------------------------
     */

    console_print_header("HISTORY");
    ASSERT_TRUE(
        history_count() > 0);

    printf("PASS\n");

    /*
     * -----------------------------
     * RELAY ANTI CYCLE
     * -----------------------------
     */

    console_print_header("RELAY DELAY");

    relay_init();

    relay_set_min_switch_delay(60);

    relay_set(true);

    ASSERT_TRUE(
        relay_get());

    clock_add_seconds(10);

    relay_set(false);

    ASSERT_TRUE(
        relay_get()); // encore ON

    clock_add_seconds(60);

    relay_set(false);

    ASSERT_FALSE(
        relay_get());

    printf("PASS\n");

    bool result = test_program_run();

    /*
     * Résumé
     */

    printf("\nHistory records : %u\n",
           history_count());

    printf("\nPASS : Thermostat\n");

    return result;
}
