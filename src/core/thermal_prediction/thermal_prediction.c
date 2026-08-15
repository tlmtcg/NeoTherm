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
static bool  s_heating = false;

static float s_predicted_temperature = 0.0f;
static bool  s_valid = false;

/*
 * Taux d'apprentissage mémorisés.
 *
 * heat    : température qui monte avec le chauffage
 * warming : température qui monte naturellement, chauffage OFF
 * cooling : température qui descend naturellement, chauffage OFF
 */
static float s_heat_rate = 0.0f;
static bool  s_heat_rate_valid = false;

static float s_warming_rate = 0.0f;
static bool  s_warming_rate_valid = false;

static float s_cooling_rate = 0.0f;
static bool  s_cooling_rate_valid = false;

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

    s_warming_rate = 0.0f;
    s_warming_rate_valid = false;

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

    /*------------------------------------------------------
     * Dernière mesure
     *------------------------------------------------------*/

    if (!history_get_latest(&record))
    {
        s_current_temperature = 0.0f;
        s_heating = false;
        s_predicted_temperature = 0.0f;

        s_valid = false;

        s_heat_rate = 0.0f;
        s_heat_rate_valid = false;

        s_warming_rate = 0.0f;
        s_warming_rate_valid = false;

        s_cooling_rate = 0.0f;
        s_cooling_rate_valid = false;

        LOG_WARN(
            "PREDICTION",
            "No history available");

        return false;
    }

    /*------------------------------------------------------
     * Etat actuel
     *------------------------------------------------------*/

    s_current_temperature =
        record.inside_temperature;

    s_heating =
        record.heating;

    /*------------------------------------------------------
     * Récupération des taux appris
     *------------------------------------------------------*/

    s_heat_rate =
        thermal_learning_get_heat_rate();

    s_warming_rate =
        thermal_learning_get_warming_rate();

    s_cooling_rate =
        thermal_learning_get_cooling_rate();

    /*------------------------------------------------------
     * Validité individuelle
     *------------------------------------------------------*/

    s_heat_rate_valid =
        s_heat_rate > 0.0f;

    s_warming_rate_valid =
        s_warming_rate > 0.0f;

    s_cooling_rate_valid =
        s_cooling_rate > 0.0f;

    /*------------------------------------------------------
     * Validité de la prédiction correspondant à l'état
     *
     * Chauffage ON :
     *     il faut le taux de chauffage.
     *
     * Chauffage OFF :
     *     il faut un taux naturel.
     *------------------------------------------------------*/

    if (s_heating)
    {
        s_valid =
            s_heat_rate_valid;
    }
    else
    {
        s_valid =
            s_warming_rate_valid ||
            s_cooling_rate_valid;
    }

    /*------------------------------------------------------
     * Prédiction à 1 seconde
     *------------------------------------------------------*/

    s_predicted_temperature =
        s_current_temperature;

    if (s_heating)
    {
        if (s_heat_rate_valid)
        {
            s_predicted_temperature =
                s_current_temperature +
                (s_heat_rate / 60.0f);
        }
    }
    else
    {
        if (s_warming_rate_valid)
        {
            s_predicted_temperature =
                s_current_temperature +
                (s_warming_rate / 60.0f);
        }
        else if (s_cooling_rate_valid)
        {
            s_predicted_temperature =
                s_current_temperature -
                (s_cooling_rate / 60.0f);
        }
    }

    /*------------------------------------------------------
     * Log
     *------------------------------------------------------*/

    LOG_INFO(
        "PREDICTION",
        "Current=%.2f Heating=%s "
        "HeatRate=%.4f (%s) "
        "WarmRate=%.4f (%s) "
        "CoolRate=%.4f (%s) "
        "NextTick=%.2f "
        "Valid=%s",

        s_current_temperature,

        s_heating
            ? "YES"
            : "NO",

        s_heat_rate,

        s_heat_rate_valid
            ? "VALID"
            : "INVALID",

        s_warming_rate,

        s_warming_rate_valid
            ? "VALID"
            : "INVALID",

        s_cooling_rate,

        s_cooling_rate_valid
            ? "VALID"
            : "INVALID",

        s_predicted_temperature,

        s_valid
            ? "YES"
            : "NO");

    return s_valid;
}

/*==========================================================
 * Température prévue selon l'état actuel
 *=========================================================*/

float thermal_prediction_get_temperature_minutes(
    float minutes)
{
    return thermal_prediction_get_temperature_minutes_state(
        minutes,
        s_heating);
}

/*==========================================================
 * Température prévue selon un état demandé
 *=========================================================*/

float thermal_prediction_get_temperature_minutes_state(
    float minutes,
    bool heating)
{
    if (minutes <= 0.0f)
    {
        return s_current_temperature;
    }

    /*------------------------------------------------------
     * Scénario chauffage
     *------------------------------------------------------*/

    if (heating)
    {
        if (!s_heat_rate_valid)
        {
            return s_current_temperature;
        }

        return s_current_temperature +
               s_heat_rate * minutes;
    }

    /*------------------------------------------------------
     * Scénario naturel
     *
     * Priorité au réchauffement naturel.
     * Sinon refroidissement naturel.
     *------------------------------------------------------*/

    if (s_warming_rate_valid)
    {
        return s_current_temperature +
               s_warming_rate * minutes;
    }

    if (s_cooling_rate_valid)
    {
        return s_current_temperature -
               s_cooling_rate * minutes;
    }

    return s_current_temperature;
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
 * Validité
 *=========================================================*/

bool thermal_prediction_is_valid(void)
{
    return s_valid;
}

/*==========================================================
 * Getters
 *=========================================================*/

float thermal_prediction_get_current_temperature(void)
{
    return s_current_temperature;
}

bool thermal_prediction_is_heating(void)
{
    return s_heating;
}

float thermal_prediction_get_heat_rate(void)
{
    return s_heat_rate;
}

bool thermal_prediction_is_heat_rate_valid(void)
{
    return s_heat_rate_valid;
}

float thermal_prediction_get_warming_rate(void)
{
    return s_warming_rate;
}

bool thermal_prediction_is_warming_rate_valid(void)
{
    return s_warming_rate_valid;
}

float thermal_prediction_get_cooling_rate(void)
{
    return s_cooling_rate;
}

bool thermal_prediction_is_cooling_rate_valid(void)
{
    return s_cooling_rate_valid;
}

float thermal_prediction_get_next_tick_temperature(void)
{
    return s_predicted_temperature;
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
        "Heat rate         : %.5f C/min (%s)\n",
        s_heat_rate,
        s_heat_rate_valid ? "VALID" : "INVALID");

    printf(
        "Warming rate      : %.5f C/min (%s)\n",
        s_warming_rate,
        s_warming_rate_valid ? "VALID" : "INVALID");

    printf(
        "Cooling rate      : %.5f C/min (%s)\n",
        s_cooling_rate,
        s_cooling_rate_valid ? "VALID" : "INVALID");

    printf(
        "Next tick         : %.5f C\n",
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
