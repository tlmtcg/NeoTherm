#include "thermostat.h"

#include "logger.h"
#include "climate.h"
#include "relay.h"
#include "config.h"
#include "storage.h"

#define HORS_GEL_SETPOINT 7.0f
#define HORS_GEL_HYSTERESIS 1.0f

static float s_setpoint = 20.0f;

static float s_hysteresis = 0.2f;

static thermostat_mode_t s_mode = THERMOSTAT_AUTO;

static bool s_manual_relay = false;

/*==========================================================
 * Fonctions privées
 *=========================================================*/

static const char *thermostat_mode_name(
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
    if (!storage_load_setpoint(&s_setpoint))
    {
        config_get_float(
            "thermostat",
            "consigne",
            &s_setpoint);
    }

    if (!storage_load_mode(&s_mode))
    {
        s_mode = THERMOSTAT_AUTO;
    }

    config_get_float(
        "thermostat",
        "hysteresis",
        &s_hysteresis);

    s_mode = THERMOSTAT_AUTO;

    s_manual_relay = false;

    LOG_INFO("THERMO",
             "Thermostat initialized : %.1f C +/- %.1f",
             s_setpoint,
             s_hysteresis);

    return true;
}

/*==========================================================
 * Mise à jour thermostat
 *=========================================================*/

void thermostat_update(void)
{
    float temperature =
        climate_get_temperature();

    switch (s_mode)
    {

        /*----------------------------------
         * Arrêt complet
         *---------------------------------*/

    case THERMOSTAT_OFF:

        relay_set(false);

        break;

        /*----------------------------------
         * Commande manuelle
         *---------------------------------*/

    case THERMOSTAT_MANUAL:

        relay_set(s_manual_relay);

        break;

        /*----------------------------------
         * Régulation normale
         *---------------------------------*/

    case THERMOSTAT_AUTO:

        if (temperature <
            (s_setpoint - s_hysteresis))
        {
            relay_set(true);
        }
        else if (temperature >
                 (s_setpoint + s_hysteresis))
        {
            relay_set(false);
        }

        break;

        /*----------------------------------
         * Protection hors gel
         *---------------------------------*/

    case THERMOSTAT_HORS_GEL:

        if (temperature < HORS_GEL_SETPOINT)
        {
            relay_set(true);
        }
        else if (temperature >
                 (HORS_GEL_SETPOINT +
                  HORS_GEL_HYSTERESIS))
        {
            relay_set(false);
        }

        break;

        /*----------------------------------
         * Sécurité
         *---------------------------------*/

    default:

        LOG_ERROR("THERMO",
                  "Unknown mode %d",
                  s_mode);

        relay_set(false);

        break;
    }

    LOG_INFO("THERMO",
             "Mode=%s Temp=%.1f Relay=%s",
             thermostat_mode_name(s_mode),
             temperature,
             relay_get() ? "ON" : "OFF");
}

/*==========================================================
 * Accesseurs
 *=========================================================*/

float thermostat_get_setpoint(void)
{
    return s_setpoint;
}

float thermostat_get_hysteresis(void)
{
    return s_hysteresis;
}

/*==========================================================
 * Gestion des modes
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

    s_mode = mode;

    storage_save_mode(mode);

    LOG_INFO("THERMO",
             "Mode changed : %s",
             thermostat_mode_name(mode));

    return true;
}

thermostat_mode_t thermostat_get_mode(void)
{
    return s_mode;
}

/*==========================================================
 * Commande manuelle
 *=========================================================*/

bool thermostat_manual_set_relay(
    bool state)
{
    s_manual_relay = state;

    if (s_mode != THERMOSTAT_MANUAL)
    {
        LOG_WARN("THERMO",
                 "Manual command ignored (mode=%s)",
                 thermostat_mode_name(s_mode));

        return false;
    }

    relay_set(state);

    LOG_INFO("THERMO",
             "Manual relay = %s",
             state ? "ON" : "OFF");

    return true;
}

bool thermostat_set_setpoint(float value)
{
    s_setpoint = value;

    storage_save_setpoint(value);

    LOG_INFO("THERMO",
             "Setpoint changed : %.1f C",
             value);

    return true;
}
