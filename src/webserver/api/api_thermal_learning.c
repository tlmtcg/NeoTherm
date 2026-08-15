#include "api_thermal_learning.h"

#ifdef _WIN32

#include <stdio.h>

#include "thermal_learning.h"
#include "webserver_http.h"

/*==========================================================
 * GET /api/thermal/learning
 *=========================================================*/

void api_thermal_learning_handle_status(
    SOCKET client_socket)
{
    const thermal_learning_state_t *state =
        thermal_learning_get_state();

    if (state == NULL)
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"learning unavailable\"}");

        return;
    }

    char body[256];

    snprintf(
        body,
        sizeof(body),

        "{"
        "\"heat_rate\":%.4f,"
        "\"cooling_rate\":%.4f,"
        "\"overshoot\":%.2f,"
        "\"heating_samples\":%u,"
        "\"cooling_samples\":%u,"
        "\"valid\":%s"
        "}",

        state->heat_rate,
        state->cooling_rate,
        state->overshoot,
        state->heating_samples,
        state->cooling_samples,
        thermal_learning_is_valid()
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
 * GET /api/thermal/learning/heat_rate
 *=========================================================*/

void api_thermal_learning_handle_heat_rate(
    SOCKET client_socket)
{
    char body[64];

    snprintf(
        body,
        sizeof(body),
        "{\"heat_rate\":%.4f}",
        thermal_learning_get_heat_rate());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

/*==========================================================
 * GET /api/thermal/learning/cooling_rate
 *=========================================================*/

void api_thermal_learning_handle_cooling_rate(
    SOCKET client_socket)
{
    char body[64];

    snprintf(
        body,
        sizeof(body),
        "{\"cooling_rate\":%.4f}",
        thermal_learning_get_cooling_rate());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

/*==========================================================
 * GET /api/thermal/learning/overshoot
 *=========================================================*/

void api_thermal_learning_handle_overshoot(
    SOCKET client_socket)
{
    char body[64];

    snprintf(
        body,
        sizeof(body),
        "{\"overshoot\":%.2f}",
        thermal_learning_get_overshoot());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

/*==========================================================
 * GET /api/thermal/learning/valid
 *=========================================================*/

void api_thermal_learning_handle_valid(
    SOCKET client_socket)
{
    const char *valid =
        thermal_learning_is_valid()
            ? "true"
            : "false";

    char body[32];

    snprintf(
        body,
        sizeof(body),
        "{\"valid\":%s}",
        valid);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

#endif /* _WIN32 */
