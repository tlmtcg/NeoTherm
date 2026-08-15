#include "api_thermostat.h"

#ifdef _WIN32

#include <stdio.h>
#include <string.h>

#include "../../external/cjson/cJSON.h"

#include "thermostat.h"
#include "webserver_http.h"
#include "webserver_request.h"

/*==========================================================
 * POST /api/mode
 *=========================================================*/

void api_thermostat_handle_mode(
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
     * Récupération du mode.
     */

    cJSON *mode =
        cJSON_GetObjectItem(
            json,
            "mode");

    if (!cJSON_IsString(mode) ||
        mode->valuestring == NULL)
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing mode\"}");

        return;
    }

    /*
     * Copie avant cJSON_Delete().
     */

    char mode_name[32];

    snprintf(
        mode_name,
        sizeof(mode_name),
        "%s",
        mode->valuestring);

    cJSON_Delete(json);

    /*
     * Conversion texte -> enum.
     */

    thermostat_mode_t thermostat_mode;

    if (strcmp(
            mode_name,
            "AUTO") == 0)
    {
        thermostat_mode =
            THERMOSTAT_AUTO;
    }
    else if (strcmp(
                 mode_name,
                 "MANUAL") == 0)
    {
        thermostat_mode =
            THERMOSTAT_MANUAL;
    }
    else if (strcmp(
                 mode_name,
                 "OFF") == 0)
    {
        thermostat_mode =
            THERMOSTAT_OFF;
    }
    else if (strcmp(
                 mode_name,
                 "HORS_GEL") == 0)
    {
        thermostat_mode =
            THERMOSTAT_HORS_GEL;
    }
    else
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid mode\"}");

        return;
    }

    /*
     * Application du mode.
     */

    thermostat_set_mode(
        thermostat_mode);

    /*
     * Réponse.
     */

    char response[64];

    snprintf(
        response,
        sizeof(response),
        "{\"mode\":\"%s\"}",
        mode_name);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/status
 *=========================================================*/

void api_thermostat_handle_status(
    SOCKET client_socket)
{
    const thermostat_status_t *status =
        thermostat_get_status();

    if (status == NULL)
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"thermostat unavailable\"}");

        return;
    }

    /*
     * Conversion enum -> texte.
     */

    const char *mode =
        thermostat_mode_to_string(
            status->mode);

    if (mode == NULL)
    {
        mode = "UNKNOWN";
    }

    /*
     * Construction JSON.
     */

    char body[1024];

    snprintf(
        body,
        sizeof(body),

        "{"
        "\"mode\":\"%s\","
        "\"temperature\":%.2f,"
        "\"setpoint\":%.2f,"
        "\"hysteresis\":%.2f,"
        "\"heating_request\":%s,"
        "\"relay\":%s"
        "}",

        mode,

        status->temperature,

        status->setpoint,

        status->hysteresis,

        status->heating_request
            ? "true"
            : "false",

        status->relay_state
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
 * POST /api/setpoint
 *=========================================================*/

void api_thermostat_handle_setpoint(
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
     * Récupération du setpoint.
     */

    cJSON *setpoint =
        cJSON_GetObjectItem(
            json,
            "setpoint");

    if (!cJSON_IsNumber(setpoint))
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing setpoint\"}");

        return;
    }

    float value =
        (float)setpoint->valuedouble;

    cJSON_Delete(json);

    /*
     * Validation.
     */

    if (value < 5.0f ||
        value > 35.0f)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid setpoint\"}");

        return;
    }

    /*
     * Application.
     */

    thermostat_set_setpoint(
        value);

    /*
     * Réponse.
     */

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"setpoint\":%.2f}",
        value);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * GET /api/temperature
 *=========================================================*/

void api_thermostat_handle_temperature(
    SOCKET client_socket)
{
    const thermostat_status_t *status =
        thermostat_get_status();

    if (status == NULL)
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"thermostat unavailable\"}");

        return;
    }

    char body[128];

    snprintf(
        body,
        sizeof(body),
        "{\"temperature\":%.2f}",
        status->temperature);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

/*==========================================================
 * GET /api/thermostat/weather
 *=========================================================*/

void api_thermostat_handle_weather(
    SOCKET client_socket)
{
    const thermostat_status_t *status =
        thermostat_get_status();

    if (status == NULL)
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"thermostat unavailable\"}");

        return;
    }

    char body[256];

    snprintf(
        body,
        sizeof(body),

        "{"
        "\"outside_temperature\":%.2f,"
        "\"outside_humidity\":%.2f,"
        "\"weather_valid\":%s,"
        "\"temp_forecast_1h\":%.2f"
        "}",

        status->outside_temperature,

        status->outside_humidity,

        status->weather_valid
            ? "true"
            : "false",

        status->temp_forecast_1h);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}


#endif /* _WIN32 */
