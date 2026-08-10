#include "thermostat.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "climate.h"
#include "relay.h"
#include "app_config.h"
#include "storage.h"
#include "history.h"
#include "thermal_model.h"
#include "program.h"
#include "../services/storage_service/storage_service.h"
#include "alarm_runtime.h"
#include "../thermal_prediction/thermal_prediction.h"

#define THERMOSTAT_PREDICTION_MINUTES 10.0f
#define THERMOSTAT_PREDICTION_MARGIN 0.10f

static thermostat_status_t s_status =
    {
        .mode = THERMOSTAT_AUTO,
        .temperature = 0.0f,
        .setpoint = 20.0f,
        .hysteresis = 0.2f,
        .relay_state = false,
        .heating_request = false,
        .outside_temperature = 0.0f,
        .outside_humidity = 0.0f,
        .weather_valid = false,
        .temp_forecast_1h = 0.0f,
        .temperature_valid = false};

static bool s_manual_relay = false;


/*==========================================================
 * Initialisation
 *=========================================================*/

bool thermostat_init(void)
{
    const runtime_config_t *cfg =
        runtime_get();

    if (cfg == NULL)
    {
        LOG_ERROR("THERMO",
                  "Runtime configuration unavailable");

        return false;
    }

    s_status.mode =
        cfg->mode;

    s_status.setpoint =
        cfg->setpoint;

    s_status.hysteresis =
        cfg->hysteresis;

    relay_set_min_switch_delay(
        cfg->relay_delay);

    LOG_INFO("THERMO",
             "Thermostat initialized : %.2f C +/- %.2f Mode=%s",
             s_status.setpoint,
             s_status.hysteresis,
             thermostat_mode_to_string(s_status.mode));

    return true;
}

/*==========================================================
 * Mise à jour thermostat
 *=========================================================*/

/* ======================================================
 * SOUS-FONCTIONS
 * ====================================================== */

static void thermostat_read_inputs(void)
{
    /* Lecture de la température intérieure */
    s_status.temperature = climate_get_temperature();

    /* Lecture de la météo extérieure */
    const weather_t *weather = weather_get();

    if (weather != NULL && weather->valid)
    {
        s_status.outside_temperature = weather->temperature;
        s_status.outside_humidity = weather->humidity;
        s_status.weather_valid = true;
    }
    else
    {
        s_status.outside_temperature = thermal_model_get_outside_temperature();
        s_status.outside_humidity = 0.0f;
        s_status.weather_valid = false;
    }
}

static void thermostat_update_prediction(float *natural_10min, float *heated_10min, bool *prediction_valid)
{
    *prediction_valid = thermal_prediction_is_valid();
    float temperature = s_status.temperature;

    *natural_10min = temperature;
    *heated_10min = temperature;
    float predicted_1h = temperature;

    if (*prediction_valid)
    {
        *natural_10min = thermal_prediction_get_temperature_minutes_state(
            THERMOSTAT_PREDICTION_MINUTES, false);

        *heated_10min = thermal_prediction_get_temperature_minutes_state(
            THERMOSTAT_PREDICTION_MINUTES, true);

        predicted_1h = thermal_prediction_get_temperature_minutes_state(60.0f, true);
    }

    s_status.temp_forecast_1h = predicted_1h;
}

static void thermostat_compute_request(float natural_10min, float heated_10min, bool prediction_valid)
{
    float temperature = s_status.temperature;

    switch (s_status.mode)
    {
    case THERMOSTAT_OFF:
        s_status.heating_request = false;
        break;

    case THERMOSTAT_MANUAL:
        s_status.heating_request = s_manual_relay;
        break;

    case THERMOSTAT_AUTO:
    {
        s_status.setpoint = program_get_setpoint();

        const float heating_on_threshold = s_status.setpoint - s_status.hysteresis;
        const float heating_off_threshold = s_status.setpoint + s_status.hysteresis;

        bool normal_heating_request = (temperature <= heating_on_threshold);

        bool predictive_heating_request =
            prediction_valid &&
            !relay_get() &&
            temperature < heating_off_threshold &&
            natural_10min < (s_status.setpoint - THERMOSTAT_PREDICTION_MARGIN);

        s_status.heating_request = normal_heating_request || predictive_heating_request;

        LOG_INFO(
            "THERMO",
            "AUTO: temp=%.2f set=%.2f ON<=%.2f OFF>=%.2f natural+%.0f=%.2f heated+%.0f=%.2f",
            temperature, s_status.setpoint, heating_on_threshold, heating_off_threshold,
            THERMOSTAT_PREDICTION_MINUTES, natural_10min, THERMOSTAT_PREDICTION_MINUTES, heated_10min);

        LOG_INFO(
            "THERMO",
            "Normal=%s Predictive=%s Request=%s Valid=%s",
            normal_heating_request ? "YES" : "NO",
            predictive_heating_request ? "YES" : "NO",
            s_status.heating_request ? "YES" : "NO",
            prediction_valid ? "YES" : "NO");
        break;
    }

    case THERMOSTAT_HORS_GEL:
        s_status.setpoint = HORS_GEL_SETPOINT;

        if (temperature <= (s_status.setpoint - HORS_GEL_HYSTERESIS))
        {
            s_status.heating_request = true;
        }
        else if (temperature >= (s_status.setpoint + HORS_GEL_HYSTERESIS))
        {
            s_status.heating_request = false;
        }
        break;

    default:
        LOG_ERROR("THERMO", "Unknown mode %d", s_status.mode);
        s_status.heating_request = false;
        break;
    }
}

static void thermostat_apply_relay(void)
{
    float temperature = s_status.temperature;

    switch (s_status.mode)
    {
    case THERMOSTAT_OFF:
        relay_set(false);
        break;

    case THERMOSTAT_MANUAL:
        relay_set(s_manual_relay);
        break;

    case THERMOSTAT_AUTO:
    {
        const float heating_off_threshold = s_status.setpoint + s_status.hysteresis;

        if (s_status.heating_request)
        {
            if (!relay_get())
            {
                if (!relay_set(true))
                {
                    LOG_DEBUG("THERMO", "Heating requested but relay ON blocked by anti-cycle delay");
                }
            }
        }
        else if (temperature >= heating_off_threshold)
        {
            if (relay_get())
            {
                if (!relay_set(false))
                {
                    LOG_DEBUG("THERMO", "Heating stop requested but relay OFF blocked by anti-cycle delay");
                }
            }
        }
        break;
    }

    case THERMOSTAT_HORS_GEL:
        if (s_status.heating_request)
        {
            relay_set(true);
        }
        else
        {
            relay_set(false);
        }
        break;

    default:
        relay_set(false);
        break;
    }
}

static void thermostat_update_status(void)
{
    s_status.relay_state = relay_get();

    LOG_DEBUG(
        "THERMO",
        "Mode=%s Inside=%.2f Outside=%.2f Set=%.2f Relay=%s HeatReq=%s",
        thermostat_mode_to_string(s_status.mode),
        s_status.temperature,
        s_status.outside_temperature,
        s_status.setpoint,
        s_status.relay_state ? "ON" : "OFF",
        s_status.heating_request ? "YES" : "NO");
}

/* ======================================================
 * FONCTION PRINCIPALE
 * ====================================================== */

void thermostat_update(void)
{
    float natural_10min = 0.0f;
    float heated_10min = 0.0f;
    bool prediction_valid = false;

    thermostat_read_inputs();

    thermostat_update_prediction(&natural_10min, &heated_10min, &prediction_valid);

    thermostat_compute_request(natural_10min, heated_10min, prediction_valid);

    thermostat_apply_relay();

    thermostat_update_status();

    history_add(
        s_status.temperature,
        s_status.outside_temperature,
        s_status.setpoint,
        thermostat_get_mode(),
        relay_get(),
        s_status.heating_request);

    LOG_INFO(
        "THERMO",
        "Prediction +%.0fmin : natural=%.2f C heated=%.2f C",
        THERMOSTAT_PREDICTION_MINUTES,
        natural_10min,
        heated_10min);

    LOG_INFO(
        "THERMO",
        "Prediction valid=%s",
        prediction_valid ? "YES" : "NO");

    alarm_runtime_update(&s_status);
}
/*==========================================================
 * Accesseurs
 *=========================================================*/

float thermostat_get_setpoint(void)
{
    return s_status.setpoint;
}

float thermostat_get_hysteresis(void)
{
    return s_status.hysteresis;
}

thermostat_mode_t thermostat_get_mode(void)
{
    return s_status.mode;
}

const thermostat_status_t *thermostat_get_status(void)
{
    return &s_status;
}

void thermostat_set_hysteresis(float value)
{
    s_status.hysteresis = value;
}

/*==========================================================
 * Gestion mode
 *=========================================================*/

bool thermostat_set_mode(thermostat_mode_t mode)
{
    if ((mode < THERMOSTAT_OFF) ||
        (mode > THERMOSTAT_HORS_GEL))
    {
        LOG_ERROR("THERMO",
                  "Invalid mode %d",
                  mode);

        return false;
    }

    /*
     * Aucun changement.
     */
    if (s_status.mode == mode)
    {
        return true;
    }

    runtime_config_t cfg;

    if (storage_service_load_runtime(&cfg) == STORAGE_LOAD_ERROR)
    {
        LOG_ERROR("THERMO",
                  "Failed to load runtime configuration");

        return false;
    }

    /*
     * Mise à jour du mode.
     */
    s_status.mode = mode;
    cfg.mode = mode;

    /*
     * La consigne effective dépend du mode.
     *
     * HORS GEL :
     *     consigne forcée à HORS_GEL_SETPOINT.
     *
     * AUTRES MODES :
     *     retour à la consigne normale mémorisée.
     */
    if (mode == THERMOSTAT_HORS_GEL)
    {
        s_status.setpoint = HORS_GEL_SETPOINT;
    }
    else
    {
        s_status.setpoint = cfg.setpoint;
    }

    /*
     * On sauvegarde uniquement le mode.
     * cfg.setpoint reste inchangé.
     */
    if (!storage_save_runtime(&cfg))
    {
        LOG_ERROR("THERMO",
                  "Failed to save runtime mode");

        return false;
    }

    LOG_INFO("THERMO",
             "Mode changed : %s",
             thermostat_mode_to_string(mode));

    LOG_INFO("THERMO",
             "Effective setpoint : %.2f C",
             s_status.setpoint);

    return true;
}

/*==========================================================
 * Commande manuelle
 *=========================================================*/

bool thermostat_manual_set_relay(
    bool state)
{

    if (s_status.mode != THERMOSTAT_MANUAL)
    {
        LOG_WARN("THERMO",
                 "Manual command ignored (mode=%s)",
                 thermostat_mode_to_string(
                     s_status.mode));

        return false;
    }

    s_manual_relay = state;

    relay_set(state);

    LOG_INFO("THERMO",
             "Manual relay = %s",
             state ? "ON" : "OFF");

    return true;
}

/*==========================================================
 * Consigne
 *=========================================================*/

bool thermostat_set_setpoint(float value)
{
    if (value < 5.0f || value > 35.0f)
    {
        LOG_ERROR("THERMO",
                  "Invalid setpoint %.1f C",
                  value);

        return false;
    }

    runtime_config_t cfg;

    if (storage_service_load_runtime(&cfg) == STORAGE_LOAD_ERROR)
    {
        LOG_ERROR("THERMO",
                  "Failed to load runtime configuration");

        return false;
    }

    /*
     * La consigne utilisateur est toujours mémorisée.
     */
    cfg.setpoint = value;

    if (!storage_save_runtime(&cfg))
    {
        LOG_ERROR("THERMO",
                  "Failed to save runtime setpoint");

        return false;
    }

    /*
     * En HORS GEL, la consigne effective reste forcée.
     */
    if (s_status.mode == THERMOSTAT_HORS_GEL)
    {
        s_status.setpoint = HORS_GEL_SETPOINT;
    }
    else
    {
        s_status.setpoint = value;
    }

    LOG_INFO("THERMO",
             "Setpoint changed : %.1f C",
             value);

    return true;
}

const char *thermostat_mode_to_string(
    thermostat_mode_t mode)
{
    switch (mode)
    {
    case THERMOSTAT_AUTO:
        return "AUTO";

    case THERMOSTAT_MANUAL:
        return "MANUAL";

    case THERMOSTAT_HORS_GEL:
        return "HORS_GEL";

    case THERMOSTAT_OFF:
        return "OFF";

    default:
        return "UNKNOWN";
    }
}

bool thermostat_string_to_mode(
    const char *text,
    thermostat_mode_t *mode)
{
    if ((text == NULL) ||
        (mode == NULL))
    {
        return false;
    }

    if (strcmp(text, "AUTO") == 0)
    {
        *mode = THERMOSTAT_AUTO;
        return true;
    }

    if (strcmp(text, "MANUAL") == 0)
    {
        *mode = THERMOSTAT_MANUAL;
        return true;
    }

    if (strcmp(text, "HORS_GEL") == 0)
    {
        *mode = THERMOSTAT_HORS_GEL;
        return true;
    }

    if (strcmp(text, "OFF") == 0)
    {
        *mode = THERMOSTAT_OFF;
        return true;
    }

    LOG_WARN("STORAGE",
             "Unknown thermostat mode \"%s\"",
             text);

    return false;
}
