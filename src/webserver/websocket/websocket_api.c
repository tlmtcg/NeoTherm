#include "websocket_api.h"

#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "websocket_server.h"
#include "thermostat.h"
#include "relay.h"
#include "alarm.h"

/*==========================================================
 * Constantes
 *=========================================================*/

#define WEBSOCKET_API_RESPONSE_SIZE 512

/*==========================================================
 * Utilitaires
 *=========================================================*/

static bool websocket_api_send_status(void)
{
    char response[WEBSOCKET_API_RESPONSE_SIZE];

    const thermostat_status_t *status =
        thermostat_get_status();

    if (status == NULL)
    {
        LOG_ERROR(
            "WEBSOCKET_API",
            "Thermostat status unavailable");

        return false;
    }

    snprintf(
        response,
        sizeof(response),
        "{"
        "\"type\":\"status\","
        "\"temperature\":%.2f,"
        "\"setpoint\":%.2f,"
        "\"hysteresis\":%.2f,"
        "\"relay\":%s,"
        "\"heating_request\":%s,"
        "\"mode\":\"%s\""
        "}",
        status->temperature,
        status->setpoint,
        status->hysteresis,
        status->relay_state ? "true" : "false",
        status->heating_request ? "true" : "false",
        thermostat_mode_to_string(status->mode));

    LOG_INFO(
        "WEBSOCKET_API",
        "Sending status: %s",
        response);

    return websocket_server_send_text_active(
        response);
}

/*==========================================================
 * GET RELAY
 *=========================================================*/

static bool websocket_api_send_relay(void)
{
    relay_status_t status;

    relay_get_status(
        &status);

    char response[WEBSOCKET_API_RESPONSE_SIZE];

    snprintf(
        response,
        sizeof(response),

        "{"
        "\"type\":\"relay\","
        "\"state\":%s,"
        "\"switch_count\":%u,"
        "\"last_switch_time\":%u,"
        "\"min_switch_delay\":%u,"
        "\"elapsed_delay\":%u,"
        "\"remaining_delay\":%u,"
        "\"can_switch\":%s"
        "}",

        status.state
            ? "true"
            : "false",

        status.switch_count,

        status.last_switch_time,

        status.min_switch_delay,

        status.elapsed_delay,

        status.remaining_delay,

        status.can_switch
            ? "true"
            : "false");

    LOG_INFO(
        "WEBSOCKET_API",
        "Sending relay: %s",
        response);

    return websocket_server_send_text_active(
        response);
}

/*==========================================================
 * GET ALARMS
 *=========================================================*/

static bool websocket_api_handle_alarms(void)
{
    /*
     * Même format que GET /api/alarms.
     */

    static char response[8192];

    size_t offset = 0;

    uint32_t count =
        alarm_get_active_count();

    int written =
        snprintf(
            response + offset,
            sizeof(response) - offset,
            "{"
            "\"type\":\"alarms\","
            "\"count\":%u,"
            "\"alarms\":[",
            count);

    if (written < 0)
    {
        return false;
    }

    offset +=
        (size_t)written;

    bool first = true;

    /*
     * Parcours des alarmes.
     */

    for (alarm_type_t type =
             ALARM_TEMP_HIGH;
         type < ALARM_COUNT;
         type++)
    {
        const alarm_t *alarm =
            alarm_get(type);

        if (alarm == NULL)
        {
            continue;
        }

        /*
         * Comme l'API HTTP :
         *
         * seules les alarmes non CLEAR
         * sont retournées.
         */

        if (alarm->state ==
            ALARM_STATE_CLEAR)
        {
            continue;
        }

        /*
         * Protection buffer.
         */

        if (offset >=
            sizeof(response))
        {
            LOG_ERROR(
                "WEBSOCKET_API",
                "Alarm response buffer full");

            return false;
        }

        /*
         * Séparateur JSON.
         */

        if (!first)
        {
            written =
                snprintf(
                    response + offset,
                    sizeof(response) - offset,
                    ",");

            if (written < 0)
            {
                return false;
            }

            offset +=
                (size_t)written;
        }

        first = false;

        /*
         * Informations de l'alarme.
         */

        const char *command_name =
            alarm_get_command_name(type);

        const char *name =
            alarm_get_name(type);

        const char *state =
            alarm_state_name(
                alarm->state);

        written =
            snprintf(
                response + offset,
                sizeof(response) - offset,

                "{"
                "\"type\":\"%s\","
                "\"name\":\"%s\","
                "\"state\":\"%s\","
                "\"value\":%.2f,"
                "\"timestamp\":%u"
                "}",

                command_name,
                name,
                state,
                alarm->value,
                alarm->timestamp);

        if (written < 0)
        {
            return false;
        }

        /*
         * Vérification de dépassement.
         */

        if ((size_t)written >=
            sizeof(response) - offset)
        {
            LOG_ERROR(
                "WEBSOCKET_API",
                "Alarm response too large");

            return false;
        }

        offset +=
            (size_t)written;
    }

    /*
     * Fin du tableau JSON.
     */

    if (offset >=
        sizeof(response))
    {
        return false;
    }

    written =
        snprintf(
            response + offset,
            sizeof(response) - offset,
            "]}");

    if (written < 0)
    {
        return false;
    }

    LOG_INFO(
        "WEBSOCKET_API",
        "Sending alarm status");

    return websocket_server_send_text_active(
        response);
}

/*==========================================================
 * API WebSocket
 *=========================================================*/

bool websocket_api_handle(
    const char *json)
{
    if (json == NULL)
    {
        return false;
    }

    LOG_INFO(
        "WEBSOCKET_API",
        "Request: %s",
        json);

    /*------------------------------------------------------
     * GET STATUS
     *-----------------------------------------------------*/

    if (strstr(
            json,
            "\"type\":\"get_status\"") != NULL)
    {
        return websocket_api_send_status();
    }

    /*------------------------------------------------------
     * GET RELAY
     *-----------------------------------------------------*/

    if (strstr(
            json,
            "\"type\":\"get_relay\"") != NULL)
    {
        return websocket_api_send_relay();
    }

    /*------------------------------------------------------
     * GET ALARMS
     *-----------------------------------------------------*/

    if (strstr(
            json,
            "\"type\":\"get_alarms\"") != NULL)
    {
        return websocket_api_handle_alarms();
    }

    /*------------------------------------------------------
     * Requête inconnue
     *-----------------------------------------------------*/

    LOG_WARN(
        "WEBSOCKET_API",
        "Unknown request: %s",
        json);

    return false;
}
