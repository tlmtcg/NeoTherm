#include "api_relay.h"

#ifdef _WIN32

#include <stdio.h>

#include "../../external/cjson/cJSON.h"

#include "relay.h"
#include "webserver_http.h"
#include "webserver_request.h"

/*==========================================================
 * GET /api/relay
 *=========================================================*/

void api_relay_handle_status(
    SOCKET client_socket)
{
    relay_status_t status;

    relay_get_status(
        &status);

    char body[512];

    snprintf(
        body,
        sizeof(body),

        "{"
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

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

/*==========================================================
 * POST /api/relay
 *=========================================================*/

void api_relay_handle_set(
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
        webserver_request_get_body(
            request);

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

    cJSON *state =
        cJSON_GetObjectItem(
            json,
            "state");

    if (!cJSON_IsBool(state))
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing state\"}");

        return;
    }

    bool requested_state =
        cJSON_IsTrue(state);

    cJSON_Delete(json);

    /*
     * Commande du relais.
     */

    if (!relay_set(requested_state))
    {
        webserver_http_send_response(
            client_socket,
            409,
            "Conflict",
            "application/json",
            "{\"error\":\"relay switch refused\"}");

        return;
    }

    /*
     * Relire l'état réel après la commande.
     */

    relay_status_t status;

    relay_get_status(
        &status);

    char response[128];

    snprintf(
        response,
        sizeof(response),

        "{"
        "\"state\":%s,"
        "\"can_switch\":%s"
        "}",

        status.state
            ? "true"
            : "false",

        status.can_switch
            ? "true"
            : "false");

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

#endif /* _WIN32 */
