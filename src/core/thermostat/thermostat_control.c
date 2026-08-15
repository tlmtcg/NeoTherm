#include "thermostat_internal.h"
#include "relay.h"
#include "program.h"
#include "logger.h"

void thermostat_compute_request(
    float natural_10min,
    float heated_10min,
    bool prediction_valid)
{
    float temperature = s_status.temperature;

    switch (s_status.mode)
    {
    case THERMOSTAT_OFF:

        s_status.heating_request = false;
        s_normal_heating_request = false;

        break;

    case THERMOSTAT_MANUAL:

        s_status.heating_request = s_manual_relay;
        s_normal_heating_request = false;

        break;

    case THERMOSTAT_AUTO:
    {
        s_status.setpoint =
            program_get_setpoint();

        const float heating_on_threshold =
            s_status.setpoint -
            s_status.hysteresis;

        const float heating_off_threshold =
            s_status.setpoint +
            s_status.hysteresis;

        /*
         * --------------------------------------------------
         * DEMANDE NORMALE
         * --------------------------------------------------
         *
         * La demande est ON uniquement sous le seuil ON.
         *
         * Dans la bande d'hystérésis :
         *     => demande normale OFF
         *
         * Le relais, lui, reste ON tant que le seuil OFF
         * n'est pas atteint.
         */

        bool normal_heating_request;

        if (temperature <= heating_on_threshold)
        {
            normal_heating_request = true;
        }
        else if (temperature >= heating_off_threshold)
        {
            normal_heating_request = false;
        }
        else
        {
            /*
             * Dans la bande d'hystérésis,
             * conserver l'état actuel du relais.
             */
            normal_heating_request = relay_get();
        }

        /*
         * --------------------------------------------------
         * DEMANDE PREDICTIVE
         * --------------------------------------------------
         */

        bool predictive_heating_request =
            prediction_valid &&
            !relay_get() &&
            temperature < heating_off_threshold &&
            natural_10min <
                (s_status.setpoint -
                 THERMOSTAT_PREDICTION_MARGIN);

        /*
         * --------------------------------------------------
         * DEMANDE FINALE
         * --------------------------------------------------
         */

        s_status.heating_request =
            normal_heating_request ||
            predictive_heating_request;

        LOG_INFO(
            "THERMO",
            "AUTO: temp=%.2f set=%.2f "
            "ON<=%.2f OFF>=%.2f "
            "natural+%.0f=%.2f heated+%.0f=%.2f",
            temperature,
            s_status.setpoint,
            heating_on_threshold,
            heating_off_threshold,
            THERMOSTAT_PREDICTION_MINUTES,
            natural_10min,
            THERMOSTAT_PREDICTION_MINUTES,
            heated_10min);

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

        s_status.setpoint =
            HORS_GEL_SETPOINT;

        /*
         * HORS GEL possède sa propre hystérésis.
         * On ne doit pas utiliser l'état normal AUTO.
         */

        if (temperature <=
            (s_status.setpoint -
             HORS_GEL_HYSTERESIS))
        {
            s_status.heating_request = true;
        }
        else if (temperature >=
                 (s_status.setpoint +
                  HORS_GEL_HYSTERESIS))
        {
            s_status.heating_request = false;
        }

        break;

    default:

        LOG_ERROR(
            "THERMO",
            "Unknown mode %d",
            s_status.mode);

        s_status.heating_request = false;
        s_normal_heating_request = false;

        break;
    }
}


void thermostat_apply_relay(void)
{
    float temperature =
        s_status.temperature;

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
        const float heating_off_threshold =
            s_status.setpoint +
            s_status.hysteresis;

        /*
         * --------------------------------------------------
         * DEMANDE DE CHAUFFAGE
         * --------------------------------------------------
         *
         * Une demande ON allume le relais.
         */

        if (s_status.heating_request)
        {
            if (!relay_get())
            {
                if (!relay_set(true))
                {
                    LOG_DEBUG(
                        "THERMO",
                        "Heating requested but relay ON blocked by anti-cycle delay");
                }
            }

            break;
        }

        /*
         * --------------------------------------------------
         * ARRET DU CHAUFFAGE
         * --------------------------------------------------
         *
         * Le relais ne s'arrête que lorsque le seuil OFF
         * est atteint.
         */

        if (temperature >= heating_off_threshold)
        {
            if (relay_get())
            {
                if (!relay_set(false))
                {
                    LOG_DEBUG(
                        "THERMO",
                        "Heating stop requested but relay OFF blocked by anti-cycle delay");
                }
            }
        }

        /*
         * --------------------------------------------------
         * BANDE D'HYSTERESIS
         * --------------------------------------------------
         *
         * Si :
         *
         *     heating_request == false
         *
         * et
         *
         *     temperature < heating_off_threshold
         *
         * alors on ne touche PAS au relais.
         *
         * Le relais conserve son état.
         */

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

void thermostat_update_status(void)
{
    /*
     * Synchronisation de l'état réel du relais.
     */
    s_status.relay_state = relay_get();

    LOG_DEBUG(
        "THERMO",
        "Mode=%s Inside=%.2f Outside=%.2f "
        "Set=%.2f Relay=%s HeatReq=%s",
        thermostat_mode_to_string(s_status.mode),
        s_status.temperature,
        s_status.outside_temperature,
        s_status.setpoint,
        s_status.relay_state ? "ON" : "OFF",
        s_status.heating_request ? "YES" : "NO");
}
