#include "api_event.h"

#ifdef _WIN32

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../../external/cjson/cJSON.h"

#include "event.h"
#include "webserver_http.h"
#include "webserver_request.h"
#include "event_dispatcher.h"

/*==========================================================
 * GET /api/events
 *=========================================================*/

void api_event_handle_status(
    SOCKET client_socket)
{
    char response[4096];

    int offset = 0;

    offset += snprintf(
        response + offset,
        sizeof(response) - (size_t)offset,
        "{"
        "\"total\":%u,"
        "\"queue_count\":%u,"
        "\"queue_size\":%u,"
        "\"queue_empty\":%s,"
        "\"queue_full\":%s,"
        "\"events\":{",

        event_get_total_count(),

        (unsigned int)event_get_queue_count(),

        (unsigned int)EVENT_QUEUE_SIZE,

        event_is_empty()
            ? "true"
            : "false",

        event_is_full()
            ? "true"
            : "false");

    bool first = true;

    for (int i = 0;
         i < EVENT_COUNT;
         i++)
    {
        uint32_t count =
            event_get_count(
                (event_type_t)i);

        if (count == 0)
        {
            continue;
        }

        if (!first)
        {
            offset += snprintf(
                response + offset,
                sizeof(response) - (size_t)offset,
                ",");
        }

        first = false;

        offset += snprintf(
            response + offset,
            sizeof(response) - (size_t)offset,
            "\"%s\":%u",
            event_name(
                (event_type_t)i),
            count);
    }

    offset += snprintf(
        response + offset,
        sizeof(response) - (size_t)offset,
        "}"
        "}");

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * POST /api/events
 *=========================================================*/

void api_event_handle_post(
    SOCKET client_socket,
    const char *request)
{
    if (request == NULL)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid request\"}");

        return;
    }

    /*
     * Recherche du corps HTTP.
     */

    const char *body =
        webserver_request_get_body(request);

    if (body == NULL)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing body\"}");

        return;
    }

    /*
     * Analyse JSON.
     */

    cJSON *json =
        cJSON_Parse(body);

    if (json == NULL)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid json\"}");

        return;
    }

    /*
     * Récupération du nom de l'événement.
     */

    cJSON *event_json =
        cJSON_GetObjectItem(
            json,
            "event");

    if (!cJSON_IsString(event_json) ||
        event_json->valuestring == NULL)
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing event\"}");

        return;
    }

    /*
     * Copie avant cJSON_Delete().
     */

    char event_name_text[64];

    snprintf(
        event_name_text,
        sizeof(event_name_text),
        "%s",
        event_json->valuestring);

    cJSON_Delete(json);

    /*
     * Recherche de l'événement.
     */

    event_type_t event_type =
        EVENT_NONE;

    bool found = false;

    for (int i = 0;
         i < EVENT_COUNT;
         i++)
    {
        const char *name =
            event_name(
                (event_type_t)i);

        if (name == NULL)
        {
            continue;
        }

        if (strcmp(
                event_name_text,
                name) == 0)
        {
            event_type =
                (event_type_t)i;

            found = true;

            break;
        }
    }

    /*
     * Événement inconnu.
     */

    if (!found)
    {
        char response[128];

        snprintf(
            response,
            sizeof(response),
            "{\"error\":\"invalid event\",\"event\":\"%s\"}",
            event_name_text);

        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            response);

        return;
    }

    /*
     * EVENT_NONE ne doit pas être injecté.
     */

    if (event_type == EVENT_NONE)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"EVENT_NONE cannot be posted\"}");

        return;
    }

    /*
     * Construction de l'événement.
     */

    event_t event =
        {
            .type = event_type};

    /*
     * Ajout dans la queue.
     */

    if (!event_post(&event))
    {
        webserver_http_send_response(
            client_socket,
            503,
            "Service Unavailable",
            "application/json",
            "{\"error\":\"event queue full\"}");

        return;
    }

    /*
     * Réponse.
     */

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{"
        "\"event\":\"%s\","
        "\"posted\":true,"
        "\"queue_count\":%u"
        "}",
        event_name_text,
        (unsigned int)event_get_queue_count());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * POST /api/event/dispatch
 *
 * Exemple :
 * {
 *     "event": "ALARM_ACTIVE",
 *     "value": 1
 * }
 *=========================================================*/

void api_event_handle_dispatch(
    SOCKET client_socket,
    const char *request)
{
    if (request == NULL)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid request\"}");

        return;
    }

    const char *body =
        webserver_request_get_body(request);

    if (body == NULL)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing body\"}");

        return;
    }

    cJSON *json =
        cJSON_Parse(body);

    if (json == NULL)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid json\"}");

        return;
    }

    /*------------------------------------------------------
     * Event
     *-----------------------------------------------------*/

    cJSON *event_json =
        cJSON_GetObjectItem(
            json,
            "event");

    if (!cJSON_IsString(event_json) ||
        event_json->valuestring == NULL)
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing event\"}");

        return;
    }

    char event_name_text[64];

    snprintf(
        event_name_text,
        sizeof(event_name_text),
        "%s",
        event_json->valuestring);

    /*------------------------------------------------------
     * Conversion texte -> enum
     *-----------------------------------------------------*/

    event_type_t event_type =
        EVENT_NONE;

    bool found = false;

    for (int i = 0;
         i < EVENT_COUNT;
         i++)
    {
        if (strcmp(
                event_name_text,
                event_name((event_type_t)i)) == 0)
        {
            event_type =
                (event_type_t)i;

            found = true;

            break;
        }
    }

    if (!found)
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid event\"}");

        return;
    }

    /*------------------------------------------------------
     * Valeur optionnelle
     *-----------------------------------------------------*/

    int value = 0;

    cJSON *value_json =
        cJSON_GetObjectItem(
            json,
            "value");

    if (value_json != NULL)
    {
        if (!cJSON_IsNumber(value_json))
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid value\"}");

            return;
        }

        value =
            value_json->valueint;
    }

    cJSON_Delete(json);

    /*------------------------------------------------------
     * Construction événement
     *-----------------------------------------------------*/

    event_t event;

    memset(
        &event,
        0,
        sizeof(event));

    event.type =
        event_type;

    event.data.value =
        value;

    /*------------------------------------------------------
     * Dispatch immédiat
     *-----------------------------------------------------*/

    event_dispatcher_dispatch(
        &event);

    /*------------------------------------------------------
     * Réponse
     *-----------------------------------------------------*/

    char response[256];

    snprintf(
        response,
        sizeof(response),
        "{"
        "\"dispatched\":true,"
        "\"event\":\"%s\","
        "\"value\":%d"
        "}",
        event_name(event_type),
        value);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}


#endif /* _WIN32 */
