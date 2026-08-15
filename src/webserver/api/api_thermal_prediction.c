#include "api_thermal_prediction.h"

#ifdef _WIN32

#include <stdio.h>

#include "../thermal_prediction/thermal_prediction.h"
#include "webserver_http.h"

/*==========================================================
 * GET /api/thermal_prediction
 *=========================================================*/

void api_thermal_prediction_handle_status(
    SOCKET client_socket)
{
    char response[512];

    snprintf(
        response,
        sizeof(response),

        "{"
        "\"valid\":%s,"
        "\"current_temperature\":%.2f,"
        "\"heating\":%s,"
        "\"heat_rate\":%.4f,"
        "\"heat_rate_valid\":%s,"
        "\"cooling_rate\":%.4f,"
        "\"cooling_rate_valid\":%s,"
        "\"next_tick\":%.4f"
        "}",

        thermal_prediction_is_valid()
            ? "true"
            : "false",

        thermal_prediction_get_current_temperature(),

        thermal_prediction_is_heating()
            ? "true"
            : "false",

        thermal_prediction_get_heat_rate(),

        thermal_prediction_is_heat_rate_valid()
            ? "true"
            : "false",

        thermal_prediction_get_cooling_rate(),

        thermal_prediction_is_cooling_rate_valid()
            ? "true"
            : "false",

        thermal_prediction_get_next_tick_temperature());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/thermal_prediction/current
 *=========================================================*/

void api_thermal_prediction_handle_current(
    SOCKET client_socket)
{
    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"current_temperature\":%.2f}",
        thermal_prediction_get_current_temperature());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/thermal_prediction/next_tick
 *=========================================================*/

void api_thermal_prediction_handle_next_tick(
    SOCKET client_socket)
{
    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"next_tick\":%.4f}",
        thermal_prediction_get_next_tick_temperature());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/thermal_prediction/valid
 *=========================================================*/

void api_thermal_prediction_handle_valid(
    SOCKET client_socket)
{
    char response[32];

    snprintf(
        response,
        sizeof(response),
        "{\"valid\":%s}",
        thermal_prediction_is_valid()
            ? "true"
            : "false");

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/thermal_prediction/heated
 *
 * Prédiction à 10 minutes avec chauffage.
 *=========================================================*/

void api_thermal_prediction_handle_heated(
    SOCKET client_socket)
{
    float temperature =
        thermal_prediction_get_heated_temperature_minutes(
            10.0f);

    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"minutes\":10,\"temperature\":%.2f}",
        temperature);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/thermal_prediction/natural
 *
 * Prédiction à 10 minutes sans chauffage.
 *=========================================================*/

void api_thermal_prediction_handle_natural(
    SOCKET client_socket)
{
    float temperature =
        thermal_prediction_get_temperature_minutes_state(
            10.0f,
            false);

    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"minutes\":10,\"temperature\":%.2f}",
        temperature);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

#endif /* _WIN32 */
