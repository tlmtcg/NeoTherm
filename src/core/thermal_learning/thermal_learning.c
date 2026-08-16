#include "thermal_learning.h"

#include <stdio.h>
#include <string.h>

#include "history.h"
#include "logger.h"

#define THERMAL_LEARNING_MAX_RATE 1.0f
#define THERMAL_LEARNING_MIN_SAMPLES 10

/*==========================================================
 * Variables privées
 *=========================================================*/

static thermal_learning_state_t s_learning;

/*==========================================================
 * Calcul durée entre deux timestamps
 *=========================================================*/

static float thermal_learning_delta_minutes(
    const clock_time_t *previous,
    const clock_time_t *current)
{
    if (previous == NULL ||
        current == NULL)
    {
        return 0.0f;
    }

    int previous_seconds =
        previous->hour * 3600 +
        previous->minute * 60 +
        previous->second;

    int current_seconds =
        current->hour * 3600 +
        current->minute * 60 +
        current->second;

    int delta_seconds =
        current_seconds -
        previous_seconds;

    /*
     * Passage à minuit.
     */
    if (delta_seconds < 0)
    {
        delta_seconds += 24 * 3600;
    }

    return (float)delta_seconds / 60.0f;
}

/*==========================================================
 * Initialisation
 *=========================================================*/

bool thermal_learning_init(void)
{
    memset(
        &s_learning,
        0,
        sizeof(s_learning));

    LOG_INFO(
        "LEARNING",
        "Thermal learning initialized");

    return true;
}

/*==========================================================
 * Analyse historique
 *=========================================================*/

bool thermal_learning_analyze(void)
{
    uint32_t count =
        history_count();

    if (count < 2)
    {
        LOG_WARN(
            "LEARNING",
            "Not enough history");

        memset(
            &s_learning,
            0,
            sizeof(s_learning));

        return false;
    }

    /*
     * Analyse complète de l'historique.
     */
    memset(
        &s_learning,
        0,
        sizeof(s_learning));

    history_record_t previous;

    if (!history_get(
            0,
            &previous))
    {
        return false;
    }

    float heat_sum = 0.0f;
    float cool_sum = 0.0f;
    float warming_sum = 0.0f;

    bool overshoot_active = false;

    /*======================================================
     * Parcours historique
     *=====================================================*/

    for (uint32_t i = 1;
         i < count;
         i++)
    {
        history_record_t current;

        if (!history_get(
                i,
                &current))
        {
            continue;
        }

        /*==================================================
         * Durée
         *==================================================*/

        float dt_minutes =
            thermal_learning_delta_minutes(
                &previous.timestamp,
                &current.timestamp);

        if (dt_minutes <= 0.0f)
        {
            previous = current;
            continue;
        }

        /*==================================================
         * Variation température
         *==================================================*/

        float delta_temperature =
            current.inside_temperature -
            previous.inside_temperature;

        float rate =
            delta_temperature /
            dt_minutes;

        LOG_DEBUG(
            "LEARNING",
            "dt=%.3f min delta=%.3f rate=%+.4f "
            "heating=%s",
            dt_minutes,
            delta_temperature,
            rate,
            previous.heating ? "YES" : "NO");

        /*==================================================
         * CHAUFFAGE
         *==================================================*/

        if (previous.heating)
        {
            if (rate > 0.0f)
            {
                if (rate <=
                    THERMAL_LEARNING_MAX_RATE)
                {
                    heat_sum += rate;

                    s_learning.heating_samples++;
                }
                else
                {
                    LOG_WARN(
                        "LEARNING",
                        "Heating sample rejected: "
                        "dt=%.3f delta=%.3f "
                        "rate=%.3f C/min",
                        dt_minutes,
                        delta_temperature,
                        rate);
                }
            }
        }

        /*==================================================
         * TRANSITION CHAUFFAGE -> OFF
         *==================================================*/

        if (previous.heating &&
            !current.heating)
        {
            overshoot_active = true;
        }

        /*==================================================
         * OVERSHOOT
         *==================================================*/

        if (overshoot_active &&
            !current.heating)
        {
            if (current.inside_temperature >
                current.setpoint)
            {
                float overshoot =
                    current.inside_temperature -
                    current.setpoint;

                if (overshoot >
                    s_learning.overshoot)
                {
                    s_learning.overshoot =
                        overshoot;
                }
            }
        }

        /*==================================================
         * CHAUFFAGE OFF
         *==================================================*/

        if (!previous.heating)
        {
            /*
             * ----------------------------------------------
             * Refroidissement naturel
             * ----------------------------------------------
             */

            if (rate < 0.0f)
            {
                float cooling_rate =
                    -rate;

                if (cooling_rate <=
                    THERMAL_LEARNING_MAX_RATE)
                {
                    cool_sum += cooling_rate;

                    s_learning.cooling_samples++;
                }
                else
                {
                    LOG_WARN(
                        "LEARNING",
                        "Cooling sample rejected: "
                        "dt=%.3f delta=%.3f "
                        "rate=%.3f C/min",
                        dt_minutes,
                        delta_temperature,
                        cooling_rate);
                }
            }

            /*
             * ----------------------------------------------
             * Réchauffement naturel
             * ----------------------------------------------
             */

            else if (rate > 0.0f)
            {
                float warming_rate =
                    rate;

                if (warming_rate <=
                    THERMAL_LEARNING_MAX_RATE)
                {
                    warming_sum += warming_rate;

                    s_learning.warming_samples++;
                }
                else
                {
                    LOG_WARN(
                        "LEARNING",
                        "Warming sample rejected: "
                        "dt=%.3f delta=%.3f "
                        "rate=%.3f C/min",
                        dt_minutes,
                        delta_temperature,
                        warming_rate);
                }
            }
        }

        /*==================================================
         * NOUVEAU DÉMARRAGE CHAUFFAGE
         *==================================================*/

        if (!previous.heating &&
            current.heating)
        {
            overshoot_active = false;
        }

        previous = current;
    }

    /*======================================================
     * Moyennes
     *=====================================================*/

    if (s_learning.heating_samples > 0)
    {
        s_learning.heat_rate =
            heat_sum /
            (float)s_learning.heating_samples;
    }

    if (s_learning.cooling_samples > 0)
    {
        s_learning.cooling_rate =
            cool_sum /
            (float)s_learning.cooling_samples;
    }

    if (s_learning.warming_samples > 0)
    {
        s_learning.warming_rate =
            warming_sum /
            (float)s_learning.warming_samples;
    }

    /*======================================================
     * Log
     *=====================================================*/

    LOG_INFO(
        "LEARNING",
        "Heat=%.4f C/min "
        "Cool=%.4f C/min "
        "Warm=%.4f C/min "
        "Overshoot=%.2f C",
        s_learning.heat_rate,
        s_learning.cooling_rate,
        s_learning.warming_rate,
        s_learning.overshoot);

    return true;
}

/*==========================================================
 * Accès à l'état
 *=========================================================*/

const thermal_learning_state_t *
thermal_learning_get_state(void)
{
    return &s_learning;
}

/*==========================================================
 * Getters
 *=========================================================*/

float thermal_learning_get_heat_rate(void)
{
    return s_learning.heat_rate;
}

float thermal_learning_get_cooling_rate(void)
{
    return s_learning.cooling_rate;
}

float thermal_learning_get_warming_rate(void)
{
    return s_learning.warming_rate;
}

float thermal_learning_get_overshoot(void)
{
    return s_learning.overshoot;
}

/*==========================================================
 * Debug
 *=========================================================*/

void thermal_learning_dump(void)
{
    printf("\n");

    printf(
        "Thermal learning\n");

    printf(
        "------------------------------\n");

    printf(
        "Heat rate      : %.5f C/min\n",
        s_learning.heat_rate);

    printf(
        "Cooling rate   : %.5f C/min\n",
        s_learning.cooling_rate);

    printf(
        "Warming rate   : %.5f C/min\n",
        s_learning.warming_rate);

    printf(
        "Overshoot      : %.2f C\n",
        s_learning.overshoot);

    printf(
        "Heat samples   : %u\n",
        s_learning.heating_samples);

    printf(
        "Cool samples   : %u\n",
        s_learning.cooling_samples);

    printf(
        "Warm samples   : %u\n",
        s_learning.warming_samples);

    printf("\n");
}

/*==========================================================
 * Scheduler
 *=========================================================*/

void thermal_learning_task_callback(void)
{
    thermal_learning_analyze();
}

/*==========================================================
 * Update
 *=========================================================*/

bool thermal_learning_update(void)
{
    return thermal_learning_analyze();
}

/*==========================================================
 * Validation
 *=========================================================*/

bool thermal_learning_is_valid(void)
{
    return
        thermal_learning_is_heat_rate_valid() ||
        thermal_learning_is_cooling_rate_valid() ||
        thermal_learning_is_warming_rate_valid();
}

bool thermal_learning_is_heat_rate_valid(void)
{
    return
        s_learning.heating_samples >=
            THERMAL_LEARNING_MIN_SAMPLES &&
        s_learning.heat_rate > 0.0f;
}

bool thermal_learning_is_cooling_rate_valid(void)
{
    return
        s_learning.cooling_samples >=
            THERMAL_LEARNING_MIN_SAMPLES &&
        s_learning.cooling_rate > 0.0f;
}

bool thermal_learning_is_warming_rate_valid(void)
{
    return
        s_learning.warming_samples >=
            THERMAL_LEARNING_MIN_SAMPLES &&
        s_learning.warming_rate > 0.0f;
}
