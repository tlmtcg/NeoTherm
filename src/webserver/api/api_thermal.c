#include "api_thermal.h"

#ifdef _WIN32

#include <stdio.h>

#include "../../external/cjson/cJSON.h"

#include "thermal_model.h"
#include "webserver_http.h"
#include "webserver_request.h"

/*==========================================================
 * GET /api/thermal
 *=========================================================*/

void api_thermal_handle_status(
    SOCKET client_socket)
{
    char body[256];

    snprintf(
        body,
        sizeof(body),

        "{"
        "\"outside_temperature\":%.2f,"
        "\"heat_power\":%.3f,"
        "\"loss_factor\":%.4f,"
        "\"mass\":%.2f"
        "}",

        thermal_model_get_outside_temperature(),
        thermal_model_get_heat_power(),
        thermal_model_get_loss_factor(),
        thermal_model_get_thermal_mass());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

/*==========================================================
 * Lecture d'un float dans le JSON
 *=========================================================*/

static bool api_thermal_get_float(
    const char *request,
    const char *name,
    float *value)
{
    if (request == NULL ||
        name == NULL ||
        value == NULL)
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

    cJSON *item =
        cJSON_GetObjectItem(
            json,
            name);

    if (!cJSON_IsNumber(item))
    {
        cJSON_Delete(json);

        return false;
    }

    *value =
        (float)item->valuedouble;

    cJSON_Delete(json);

    return true;
}

/*==========================================================
 * GET /api/thermal/outside_temperature
 *=========================================================*/

void api_thermal_handle_outside_temperature(
    SOCKET client_socket)
{
    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"outside_temperature\":%.2f}",
        thermal_model_get_outside_temperature());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/thermal/heat_power
 *=========================================================*/

void api_thermal_handle_heat_power(
    SOCKET client_socket)
{
    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"heat_power\":%.3f}",
        thermal_model_get_heat_power());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/thermal/loss_factor
 *=========================================================*/

void api_thermal_handle_loss_factor(
    SOCKET client_socket)
{
    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"loss_factor\":%.4f}",
        thermal_model_get_loss_factor());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/thermal/mass
 *=========================================================*/

void api_thermal_handle_mass(
    SOCKET client_socket)
{
    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"mass\":%.2f}",
        thermal_model_get_thermal_mass());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * POST /api/thermal/outside_temperature
 *=========================================================*/

void api_thermal_handle_set_outside_temperature(
    SOCKET client_socket,
    const char *request)
{
    float value;

    printf("========== RAW REQUEST ==========\n");
    printf("%s\n", request ? request : "NULL");
    printf("=================================\n");

    const char *body =
        webserver_request_get_body(request);

    printf("THERMAL BODY = [%s]\n",
           body ? body : "NULL");

    if (!api_thermal_get_float(
            request,
            "temperature",
            &value))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid temperature\"}");

        return;
    }

    thermal_model_set_outside_temperature(
        value);

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"outside_temperature\":%.2f}",
        thermal_model_get_outside_temperature());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * POST /api/thermal/heat_power
 *=========================================================*/

void api_thermal_handle_set_heat_power(
    SOCKET client_socket,
    const char *request)
{
    float value;

    if (!api_thermal_get_float(
            request,
            "value",
            &value) ||
        value <= 0.0f)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid heat power\"}");

        return;
    }

    thermal_model_set_heat_power(value);

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"heat_power\":%.3f}",
        thermal_model_get_heat_power());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * POST /api/thermal/loss_factor
 *=========================================================*/

void api_thermal_handle_set_loss_factor(
    SOCKET client_socket,
    const char *request)
{
    float value;

    if (!api_thermal_get_float(
            request,
            "value",
            &value) ||
        value <= 0.0f)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid loss factor\"}");

        return;
    }

    thermal_model_set_loss_factor(value);

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"loss_factor\":%.4f}",
        thermal_model_get_loss_factor());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * POST /api/thermal/mass
 *=========================================================*/

void api_thermal_handle_set_mass(
    SOCKET client_socket,
    const char *request)
{
    float value;

    if (!api_thermal_get_float(
            request,
            "value",
            &value) ||
        value <= 0.1f)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid mass\"}");

        return;
    }

    thermal_model_set_thermal_mass(value);

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"mass\":%.2f}",
        thermal_model_get_thermal_mass());

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

#endif /* _WIN32 */
