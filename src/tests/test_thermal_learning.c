#include "test_thermal_learning.h"

#include <stdio.h>

#include "clock.h"
#include "history.h"
#include "../core/thermal_learning/thermal_learning.h"
#include "test_utils.h"
#include "../console/console_utils.h"

/*==========================================================
 * TEST : HISTORIQUE INSUFFISANT
 *=========================================================*/

static bool test_insufficient_history(void)
{
    console_print_header(
        "LEARNING INSUFFICIENT HISTORY");

    history_clear();

    ASSERT_TRUE(
        thermal_learning_update() == false);

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_heat_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_warming_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_overshoot());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : CHAUFFAGE UNIQUEMENT
 *=========================================================*/

static bool test_heating_only(void)
{
    console_print_header(
        "LEARNING HEATING ONLY");

    history_clear();

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

    float temp = 18.0f;

    /*
     * Chauffage physique actif.
     *
     * +0.15 C/min
     */

    for (int i = 0; i < 30; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            true,
            true);

        temp += 0.15f;
    }

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    printf(
        "Warming rate : %.4f C/min\n",
        thermal_learning_get_warming_rate());

    printf(
        "Overshoot    : %.2f C\n",
        thermal_learning_get_overshoot());

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_warming_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_overshoot());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : REFROIDISSEMENT UNIQUEMENT
 *=========================================================*/

static bool test_cooling_only(void)
{
    console_print_header(
        "LEARNING COOLING ONLY");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 13,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    float temp = 22.0f;

    /*
     * Chauffage physique arrêté.
     *
     * -0.12 C/min
     */

    for (int i = 0; i < 30; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            false,
            false);

        temp -= 0.12f;
    }

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    printf(
        "Warming rate : %.4f C/min\n",
        thermal_learning_get_warming_rate());

    printf(
        "Overshoot    : %.2f C\n",
        thermal_learning_get_overshoot());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_heat_rate());

    ASSERT_TRUE(
        thermal_learning_get_cooling_rate() > 0.0f);

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_warming_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_overshoot());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : RECHAUFFEMENT NATUREL UNIQUEMENT
 *=========================================================*/

static bool test_warming_only(void)
{
    console_print_header(
        "LEARNING WARMING ONLY");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 14,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    float temp = 20.0f;

    /*
     * Chauffage physique arrêté.
     *
     * La température monte naturellement.
     *
     * +0.05 C/min
     */

    for (int i = 0; i < 20; i++)
    {
        clock_tick(60);

        history_add(
            temp,
            26.6f,
            18.0f,
            THERMOSTAT_AUTO,
            false,
            false);

        temp += 0.05f;
    }

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    printf(
        "Warming rate : %.4f C/min\n",
        thermal_learning_get_warming_rate());

    printf(
        "Overshoot    : %.2f C\n",
        thermal_learning_get_overshoot());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_heat_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    ASSERT_TRUE(
        thermal_learning_get_warming_rate() > 0.0f);

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_overshoot());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : TRANSITION ON -> OFF
 *=========================================================*/

static bool test_transition_heating_to_off(void)
{
    console_print_header(
        "LEARNING TRANSITION ON -> OFF");

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

    /*
     * Chauffage actif.
     *
     * 18.0 -> 18.2
     * 18.2 -> 18.4
     *
     * puis arrêt.
     */

    history_add(
        18.0f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.2f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.4f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Intervalle 18.4 -> 18.6.
     *
     * previous.relay == true
     *
     * Il reste donc physiquement
     * un intervalle de chauffage.
     */

    clock_tick(60);

    history_add(
        18.6f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    /*
     * Maintenant :
     *
     * 18.6 -> 18.5
     *
     * previous.relay == false
     *
     * C'est un vrai refroidissement.
     */

    clock_tick(60);

    history_add(
        18.5f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    ASSERT_TRUE(
        thermal_learning_get_cooling_rate() > 0.0f);

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : TRANSITION ON -> OFF SANS REFROIDISSEMENT
 *=========================================================*/

static bool test_transition_does_not_create_cooling(void)
{
    console_print_header(
        "TRANSITION ON -> OFF WITHOUT COOLING");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 16,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    history_add(
        18.0f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.2f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Chauffage arrêté.
     *
     * La température continue de monter.
     */

    clock_tick(60);

    history_add(
        18.4f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    clock_tick(60);

    history_add(
        18.6f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    printf(
        "Warming rate : %.4f C/min\n",
        thermal_learning_get_warming_rate());

    /*
     * Aucun refroidissement.
     *
     * Les intervalles OFF sont du
     * réchauffement naturel.
     */

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    ASSERT_TRUE(
        thermal_learning_get_heat_rate() > 0.0f);

    ASSERT_TRUE(
        thermal_learning_get_warming_rate() > 0.0f);

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : OVERSHOOT APRES TRANSITION ON -> OFF
 *=========================================================*/

static bool test_overshoot_after_heating_stop(void)
{
    console_print_header(
        "OVERSHOOT AFTER HEATING STOP");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 17,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * Consigne : 18.0 C
     */

    history_add(
        17.8f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.0f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.2f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Arrêt du chauffage.
     */

    clock_tick(60);

    history_add(
        18.4f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    clock_tick(60);

    history_add(
        18.6f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    clock_tick(60);

    history_add(
        18.5f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate    : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    printf(
        "Overshoot    : %.2f C\n",
        thermal_learning_get_overshoot());

    /*
     * Maximum :
     *
     * 18.6 - 18.0 = 0.6 C
     */

    ASSERT_EQ_FLOAT(
        0.6f,
        thermal_learning_get_overshoot());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : NOUVEAU CHAUFFAGE REINITIALISE L'OVERSHOOT
 *=========================================================*/

static bool test_new_heating_resets_overshoot(void)
{
    console_print_header(
        "NEW HEATING RESETS OVERSHOOT");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 18,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * Première phase :
     *
     * chauffage -> arrêt -> overshoot
     */

    history_add(
        17.8f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.2f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(60);

    history_add(
        18.4f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    clock_tick(60);

    history_add(
        18.6f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    /*
     * Nouveau démarrage du chauffage.
     *
     * Cela doit désactiver l'ancien
     * état overshoot_active.
     */

    clock_tick(60);

    history_add(
        18.5f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Nouveau arrêt.
     *
     * La température redescend et ne crée
     * donc pas de nouvel overshoot.
     */

    clock_tick(60);

    history_add(
        18.4f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Overshoot : %.2f C\n",
        thermal_learning_get_overshoot());

    /*
     * Le premier overshoot ne doit pas
     * contaminer le second cycle.
     */

    ASSERT_EQ_FLOAT(
        0.4f,
        thermal_learning_get_overshoot());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : SAMPLE CHAUFFAGE TROP RAPIDE REJETE
 *=========================================================*/

static bool test_invalid_heating_sample_rejected(void)
{
    console_print_header(
        "INVALID HEATING SAMPLE REJECTED");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 19,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * 10 secondes seulement.
     *
     * +2 C en 0.167 min
     *
     * => environ 12 C/min
     *
     * Doit être rejeté.
     */

    history_add(
        18.0f,
        5.0f,
        21.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    clock_tick(10);

    history_add(
        20.0f,
        5.0f,
        21.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_heat_rate());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : SAMPLE REFROIDISSEMENT TROP RAPIDE REJETE
 *=========================================================*/

static bool test_invalid_cooling_sample_rejected(void)
{
    console_print_header(
        "INVALID COOLING SAMPLE REJECTED");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 20,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * 10 secondes.
     *
     * -2 C en 0.167 min
     *
     * => environ 12 C/min
     *
     * Doit être rejeté.
     */

    history_add(
        20.0f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    clock_tick(10);

    history_add(
        18.0f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Cooling rate : %.4f C/min\n",
        thermal_learning_get_cooling_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_cooling_rate());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : SAMPLE WARMING TROP RAPIDE REJETE
 *=========================================================*/

static bool test_invalid_warming_sample_rejected(void)
{
    console_print_header(
        "INVALID WARMING SAMPLE REJECTED");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 21,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * Chauffage physique OFF.
     *
     * +2 C en 10 secondes.
     *
     * Doit être rejeté.
     */

    history_add(
        20.0f,
        26.6f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    clock_tick(10);

    history_add(
        22.0f,
        26.6f,
        18.0f,
        THERMOSTAT_AUTO,
        false,
        false);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Warming rate : %.4f C/min\n",
        thermal_learning_get_warming_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_warming_rate());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST : DT NUL OU NEGATIF
 *=========================================================*/

static bool test_invalid_dt_ignored(void)
{
    console_print_header(
        "INVALID DT IGNORED");

    history_clear();

    clock_time_t t =
        {
            .year = 2026,
            .month = 8,
            .day = 3,
            .hour = 22,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    /*
     * Deux mesures avec exactement
     * le même timestamp.
     */

    history_add(
        18.0f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    /*
     * Aucun clock_tick().
     *
     * Même timestamp.
     */

    history_add(
        18.5f,
        5.0f,
        18.0f,
        THERMOSTAT_AUTO,
        true,
        true);

    ASSERT_TRUE(
        thermal_learning_update());

    printf(
        "Heat rate : %.4f C/min\n",
        thermal_learning_get_heat_rate());

    ASSERT_EQ_FLOAT(
        0.0f,
        thermal_learning_get_heat_rate());

    printf("PASS\n");

    return true;
}

/*==========================================================
 * TEST GLOBAL
 *=========================================================*/

bool test_thermal_learning_run(void)
{
    printf(
        "\n=============== THERMAL LEARNING TEST ===============\n");

    ASSERT_TRUE(
        history_init());

    ASSERT_TRUE(
        thermal_learning_init());

    /*
     * ----------------------------------------------------
     * Historique insuffisant
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_insufficient_history());

    /*
     * ----------------------------------------------------
     * Chauffage uniquement
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_heating_only());

    /*
     * ----------------------------------------------------
     * Refroidissement uniquement
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_cooling_only());

    /*
     * ----------------------------------------------------
     * Réchauffement naturel uniquement
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_warming_only());

    /*
     * ----------------------------------------------------
     * Transition chauffage -> arrêt -> refroidissement
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_transition_heating_to_off());

    /*
     * ----------------------------------------------------
     * Transition chauffage -> arrêt sans refroidissement
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_transition_does_not_create_cooling());

    /*
     * ----------------------------------------------------
     * Overshoot
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_overshoot_after_heating_stop());

    /*
     * ----------------------------------------------------
     * Nouveau chauffage = reset overshoot
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_new_heating_resets_overshoot());

    /*
     * ----------------------------------------------------
     * Rejet chauffage trop rapide
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_invalid_heating_sample_rejected());

    /*
     * ----------------------------------------------------
     * Rejet refroidissement trop rapide
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_invalid_cooling_sample_rejected());

    /*
     * ----------------------------------------------------
     * Rejet réchauffement naturel trop rapide
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_invalid_warming_sample_rejected());

    /*
     * ----------------------------------------------------
     * dt invalide
     * ----------------------------------------------------
     */

    ASSERT_TRUE(
        test_invalid_dt_ignored());

    printf(
        "\nPASS : Thermal learning\n");

    return true;
}
