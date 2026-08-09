#include "thermal_prediction.h"

#include <stdio.h>
#include <string.h>

#include "history.h"
#include "logger.h"
#include "../thermal_learning/thermal_learning.h"

/*==========================================================
 * Variables privées
 *=========================================================*/

static float s_current_temperature = 0.0f;

static bool s_heating = false;

static float s_predicted_temperature = 0.0f;

static bool s_valid = false;

/*
 * Taux d'apprentissage mémorisés.
 *
 * Les deux scénarios sont indépendants :
 *
 *   - chauffage
 *   - refroidissement
 */
static float s_heat_rate = 0.0f;
static bool s_heat_rate_valid = false;

static float s_cooling_rate = 0.0f;
static bool s_cooling_rate_valid = false;


/*==========================================================
 * Initialisation
 *=========================================================*/

bool thermal_prediction_init(void)
{
    s_current_temperature = 0.0f;
    s_heating = false;
    s_predicted_temperature = 0.0f;

    s_valid = false;

    s_heat_rate = 0.0f;
    s_heat_rate_valid = false;

    s_cooling_rate = 0.0f;
    s_cooling_rate_valid = false;

    LOG_INFO(
        "PREDICTION",
        "Thermal prediction initialized");

    return true;
}


/*==========================================================
 * Mise à jour
 *=========================================================*/

bool thermal_prediction_update(void)
{
    history_record_t record;

    /*
     * ======================================================
     * RECUPERATION DE LA DERNIERE MESURE
     * ======================================================
     */

    if (!history_get_latest(&record))
    {
        s_valid = false;

        LOG_WARN(
            "PREDICTION",
            "No history available");

        return false;
    }


    /*
     * ======================================================
     * ETAT ACTUEL
     * ======================================================
     */

    s_current_temperature =
        record.inside_temperature;

    s_heating =
        record.heating;


    /*
     * ======================================================
     * TAUX D'APPRENTISSAGE
     * ======================================================
     */

    float heat_rate =
        thermal_learning_get_heat_rate();

    float cooling_rate =
        thermal_learning_get_cooling_rate();


    /*
     * ======================================================
     * VALIDITE
     * ======================================================
     *
     * Une prédiction complète nécessite les deux taux :
     *
     *   - cooling_rate pour natural
     *   - heat_rate    pour heated
     *
     */

    if (heat_rate <= 0.0f ||
        cooling_rate <= 0.0f)
    {
        s_valid = false;

        LOG_DEBUG(
            "PREDICTION",
            "Learning incomplete : "
            "Heat=%.4f Cool=%.4f",
            heat_rate,
            cooling_rate);

        return false;
    }


    /*
     * ======================================================
     * PREDICTION A 1 TICK
     * ======================================================
     *
     * Les taux sont en °C/minute.
     *
     * 1 tick = 1 seconde.
     */

    if (s_heating)
    {
        s_predicted_temperature =
            s_current_temperature +
            (heat_rate / 60.0f);
    }
    else
    {
        s_predicted_temperature =
            s_current_temperature -
            (cooling_rate / 60.0f);
    }


    /*
     * ======================================================
     * PREDICTION VALIDE
     * ======================================================
     */

    s_valid = true;


    LOG_DEBUG(
        "PREDICTION",
        "Current=%.2f Heating=%s "
        "HeatRate=%.4f CoolRate=%.4f "
        "NextTick=%.2f",
        s_current_temperature,
        s_heating ? "YES" : "NO",
        heat_rate,
        cooling_rate,
        s_predicted_temperature);


    return true;
}

/*==========================================================
 * Température prévue
 *=========================================================*/

float thermal_prediction_get_temperature_minutes(
    float minutes)
{
    if (!s_valid)
    {
        return s_current_temperature;
    }

    return thermal_prediction_get_temperature_minutes_state(
        minutes,
        s_heating);
}

/*==========================================================
 * Validité
 *=========================================================*/

bool thermal_prediction_is_valid(void)
{
    return s_valid;
}


/*==========================================================
 * Debug
 *=========================================================*/

void thermal_prediction_dump(void)
{
    printf("\n");

    printf("Thermal prediction\n");
    printf("------------------------------\n");

    printf(
        "Valid             : %s\n",
        s_valid ? "YES" : "NO");

    printf(
        "Current temp      : %.2f C\n",
        s_current_temperature);

    printf(
        "Heating           : %s\n",
        s_heating ? "YES" : "NO");

    printf(
        "Heat rate         : %.4f C/min (%s)\n",
        s_heat_rate,
        s_heat_rate_valid ? "VALID" : "INVALID");

    printf(
        "Cooling rate      : %.4f C/min (%s)\n",
        s_cooling_rate,
        s_cooling_rate_valid ? "VALID" : "INVALID");

    printf(
        "Next tick         : %.2f C\n",
        s_predicted_temperature);

    printf("\n");
}


/*==========================================================
 * Scheduler
 *=========================================================*/

void thermal_prediction_task_callback(void)
{
    thermal_prediction_update();
}


/*==========================================================
 * Température avec chauffage
 *=========================================================*/

float thermal_prediction_get_heated_temperature_minutes(
    float minutes)
{
    return thermal_prediction_get_temperature_minutes_state(
        minutes,
        true);
}

/*==========================================================
 * Température selon l'état demandé
 *=========================================================*/

float thermal_prediction_get_temperature_minutes_state(
    float minutes,
    bool heating)
{
    /*
     * Aucun déplacement temporel.
     */

    if (minutes <= 0.0f)
    {
        return s_current_temperature;
    }


    /*
     * La prédiction complète doit être valide.
     */

    if (!s_valid)
    {
        return s_current_temperature;
    }


    /*
     * ------------------------------------------------------
     * SCENARIO CHAUFFAGE
     * ------------------------------------------------------
     */

    if (heating)
    {
        float heat_rate =
            thermal_learning_get_heat_rate();

        if (heat_rate <= 0.0f)
        {
            return s_current_temperature;
        }

        return s_current_temperature +
               heat_rate * minutes;
    }


    /*
     * ------------------------------------------------------
     * SCENARIO REFROIDISSEMENT NATUREL
     * ------------------------------------------------------
     */

    float cooling_rate =
        thermal_learning_get_cooling_rate();

    if (cooling_rate <= 0.0f)
    {
        return s_current_temperature;
    }

    return s_current_temperature -
           cooling_rate * minutes;
}
