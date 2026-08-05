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

static thermostat_status_t s_status =
    {
        .mode = THERMOSTAT_AUTO,
        .temperature = 0.0f,
        .setpoint = 20.0f,
        .hysteresis = 0.2f,
        .relay_state = false,
        .heating_request = false};

static bool s_manual_relay = false;

/*==========================================================
 * Fonctions utiles
 *=========================================================*/

const char *thermostat_mode_name(
    thermostat_mode_t mode)
{
    switch (mode)
    {
    case THERMOSTAT_OFF:
        return "OFF";

    case THERMOSTAT_MANUAL:
        return "MANUAL";

    case THERMOSTAT_AUTO:
        return "AUTO";

    case THERMOSTAT_HORS_GEL:
        return "HORS_GEL";

    default:
        return "UNKNOWN";
    }
}

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
             thermostat_mode_name(s_status.mode));

    return true;
}

/*==========================================================
 * Mise à jour thermostat
 *=========================================================*/

void thermostat_update(void)
{

    s_status.temperature =
        climate_get_temperature();

    const weather_t *weather =
        weather_get();

    if (weather != NULL &&
        weather->valid)
    {
        s_status.outside_temperature =
            weather->temperature;

        s_status.outside_humidity =
            weather->humidity;

        s_status.weather_valid = true;
    }
    else
    {
        s_status.outside_temperature =
            thermal_model_get_outside_temperature();

        s_status.outside_humidity = 0;

        s_status.weather_valid = false;
    }

    float temperature =
        s_status.temperature;

    switch (s_status.mode)
    {

        /*
         * Arrêt forcé
         */

    case THERMOSTAT_OFF:

        relay_set(false);

        s_status.heating_request = false;

        break;

        /*
         * Mode manuel
         */

    case THERMOSTAT_MANUAL:

        relay_set(s_manual_relay);

        s_status.heating_request = s_manual_relay;

        break;

        /*
         * Régulation automatique
         */

    case THERMOSTAT_AUTO:

        s_status.setpoint = program_get_setpoint();

        if (temperature <
            (s_status.setpoint -
             s_status.hysteresis))
        {
            bool switched = relay_set(true);

            if (!switched)
            {
                LOG_DEBUG("THERMO",
                          "Heating requested but relay blocked "
                          "(anti-cycle delay)");
            }

            s_status.heating_request = true;
        }

        else if (temperature >
                 (s_status.setpoint +
                  s_status.hysteresis))
        {
            bool switched = relay_set(false);

            if (!switched)
            {
                LOG_DEBUG("THERMO",
                          "Relay OFF request blocked");
            }

            s_status.heating_request = false;
        }

        break;

        /*
         * Hors gel
         */

    case THERMOSTAT_HORS_GEL:

        if (temperature <
            HORS_GEL_SETPOINT)
        {
            relay_set(true);

            s_status.heating_request = true;
        }

        else if (temperature >
                 (HORS_GEL_SETPOINT +
                  HORS_GEL_HYSTERESIS))
        {
            relay_set(false);

            s_status.heating_request = false;
        }

        break;

    default:

        LOG_ERROR("THERMO",
                  "Unknown mode %d",
                  s_status.mode);

        relay_set(false);

        s_status.heating_request = false;

        break;
    }

    s_status.relay_state =
        relay_get();

    LOG_DEBUG("THERMO",
              "Mode=%s Inside=%.2f Outside=%.2f Set=%.2f Relay=%s HeatReq=%s",
              thermostat_mode_name(s_status.mode),
              s_status.temperature,
              s_status.outside_temperature, // modif suite à ajout weather et status structure
              s_status.setpoint,
              s_status.relay_state ? "ON" : "OFF",
              s_status.heating_request ? "YES" : "NO");

    history_add(
        temperature,
        s_status.outside_temperature, // modif suite à ajout weather et status structure
        thermostat_get_setpoint(),
        thermostat_get_mode(),
        relay_get(),
        s_status.heating_request);

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

bool thermostat_set_mode(
    thermostat_mode_t mode)
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
     * Aucun changement
     */
    if (s_status.mode == mode)
    {
        return true;
    }

    s_status.mode = mode;

    runtime_config_t cfg;

    if (storage_service_load_runtime(&cfg) != STORAGE_LOAD_ERROR)
    {
        cfg.mode = mode;

        if (!storage_save_runtime(&cfg))
        {
            LOG_ERROR("THERMO",
                      "Failed to save runtime mode");

            return false;
        }
    }

    LOG_INFO("THERMO",
             "Mode changed : %s",
             thermostat_mode_name(mode));

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
                 thermostat_mode_name(
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

bool thermostat_set_setpoint(
    float value)
{
    if (value < 5.0f || value > 35.0f)
    {
        LOG_ERROR("THERMO",
                  "Invalid setpoint %.1f C",
                  value);

        return false;
    }

    s_status.setpoint = value;

    runtime_config_t cfg;

    if (storage_service_load_runtime(&cfg) != STORAGE_LOAD_ERROR)
    {
        cfg.setpoint = value;

        if (!storage_save_runtime(&cfg))
        {
            LOG_ERROR("THERMO",
                      "Failed to save runtime setpoint");

            return false;
        }
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
