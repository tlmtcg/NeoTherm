#include "api_alarm.h"

#ifdef _WIN32

#include <stdio.h>
#include <stddef.h>

#include "alarm.h"
#include "webserver_http.h"
#include <string.h>
#include "../../external/cjson/cJSON.h"
#include "webserver_request.h"

/*==========================================================
 * GET /api/alarms
 *=========================================================*/

void api_alarm_handle(
    SOCKET client_socket)
{
    /*
     * Buffer JSON.
     */

    static char response[8192];

    size_t offset = 0;

    /*
     * Nombre d'alarmes non CLEAR.
     */

    uint32_t count =
        alarm_get_active_count();

    int written =
        snprintf(
            response + offset,
            sizeof(response) - offset,
            "{\"count\":%u,\"alarms\":[",
            count);

    if (written < 0)
    {
        return;
    }

    offset +=
        (size_t)written;

    /*
     * Parcours des alarmes.
     */

    bool first = true;

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
         * On ne retourne que les alarmes
         * qui ne sont pas CLEAR.
         */

        if (alarm->state ==
            ALARM_STATE_CLEAR)
        {
            continue;
        }

        if (offset >=
            sizeof(response))
        {
            break;
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
                break;
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
            break;
        }

        offset +=
            (size_t)written;
    }

    /*
     * Fin du tableau JSON.
     */

    if (offset <
        sizeof(response))
    {
        snprintf(
            response + offset,
            sizeof(response) - offset,
            "]}");
    }
    else
    {
        response[
            sizeof(response) - 1] =
            '\0';
    }

    /*
     * Réponse HTTP.
     */

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * Conversion nom commande -> type alarme
 *=========================================================*/

static alarm_type_t api_alarm_get_type(
    const char *name)
{
    if (name == NULL)
    {
        return ALARM_NONE;
    }

    for (alarm_type_t type = ALARM_TEMP_HIGH;
         type < ALARM_COUNT;
         type++)
    {
        const char *command_name =
            alarm_get_command_name(type);

        if (command_name != NULL &&
            strcmp(command_name, name) == 0)
        {
            return type;
        }
    }

    return ALARM_NONE;
}

/*==========================================================
 * Extraction du type depuis la requête
 *=========================================================*/

static bool api_alarm_parse_type(
    const char *request,
    alarm_type_t *type)
{
    if (request == NULL ||
        type == NULL)
    {
        return false;
    }

    const char *body =
        webserver_request_get_body(request);

    if (body == NULL)
    {
        return false;
    }

    cJSON *json =
        cJSON_Parse(body);

    if (json == NULL)
    {
        return false;
    }

    cJSON *type_json =
        cJSON_GetObjectItem(
            json,
            "type");

    if (!cJSON_IsString(type_json) ||
        type_json->valuestring == NULL)
    {
        cJSON_Delete(json);

        return false;
    }

    alarm_type_t alarm_type =
        api_alarm_get_type(
            type_json->valuestring);

    cJSON_Delete(json);

    if (alarm_type == ALARM_NONE)
    {
        return false;
    }

    *type = alarm_type;

    return true;
}

/*==========================================================
 * POST /api/alarm/ack
 *=========================================================*/

void api_alarm_handle_ack(
    SOCKET client_socket,
    const char *request)
{
    alarm_type_t type;

    if (!api_alarm_parse_type(
            request,
            &type))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid alarm type\"}");

        return;
    }

    if (!alarm_ack(type))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"unable to acknowledge alarm\"}");

        return;
    }

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"type\":\"%s\",\"state\":\"ACK\"}",
        alarm_get_command_name(type));

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * POST /api/alarm/clear
 *=========================================================*/

void api_alarm_handle_clear(
    SOCKET client_socket,
    const char *request)
{
    alarm_type_t type;

    if (!api_alarm_parse_type(
            request,
            &type))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid alarm type\"}");

        return;
    }

    if (!alarm_clear(type))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"unable to clear alarm\"}");

        return;
    }

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"type\":\"%s\",\"state\":\"CLEAR\"}",
        alarm_get_command_name(type));

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

#endif /* _WIN32 */
