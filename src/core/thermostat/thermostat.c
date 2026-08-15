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
#include "thermostat_internal.h"
#include "thermostat_inputs.h"
#include "thermostat_prediction.h"
#include "thermostat_control.h"

thermostat_status_t s_status =
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

bool s_manual_relay = false;

bool s_normal_heating_request = false; 

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

    s_manual_relay = false;

    s_status.mode =
        cfg->mode;

    if (cfg->mode == THERMOSTAT_HORS_GEL)
    {
        s_status.setpoint = HORS_GEL_SETPOINT;
    }
    else
    {
        s_status.setpoint = cfg->setpoint;
    }

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
