#include "api_runtime.h"

#ifdef _WIN32

#include <stdio.h>
#include <string.h>

#include "../../external/cjson/cJSON.h"

#include "runtime.h"
#include "thermostat.h"
#include "clock.h"
#include "webserver_http.h"
#include "webserver_request.h"
#include "storage.h"

/*==========================================================
 * GET /api/runtime
 *=========================================================*/

void api_runtime_handle_status(
    SOCKET client_socket)
{
    const runtime_config_t *cfg =
        runtime_get();

    if (cfg == NULL)
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"runtime unavailable\"}");

        return;
    }

    char body[1024];

    snprintf(
        body,
        sizeof(body),

        "{"
        "\"mode\":\"%s\","
        "\"setpoint\":%.2f,"
        "\"hysteresis\":%.2f,"
        "\"relay_delay\":%u,"
        "\"latitude\":%.6f,"
        "\"longitude\":%.6f,"
        "\"weather_provider\":%d,"
        "\"weather_update_period_sec\":%u,"
        "\"date_time\":{"
            "\"year\":%u,"
            "\"month\":%u,"
            "\"day\":%u,"
            "\"hour\":%u,"
            "\"minute\":%u,"
            "\"second\":%u"
        "},"
        "\"alarm_history_save_period\":%u"
        "}",

        thermostat_mode_to_string(
            cfg->mode),

        cfg->setpoint,

        cfg->hysteresis,

        cfg->relay_delay,

        cfg->latitude,

        cfg->longitude,

        (int)cfg->weather_provider,

        cfg->weather_update_period_sec,

        cfg->date_time.year,
        cfg->date_time.month,
        cfg->date_time.day,
        cfg->date_time.hour,
        cfg->date_time.minute,
        cfg->date_time.second,

        cfg->alarm_history_save_period);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}


/*==========================================================
 * POST /api/runtime
 *=========================================================*/

void api_runtime_handle_update(
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

    const runtime_config_t *current =
        runtime_get();

    if (current == NULL)
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"runtime unavailable\"}");

        return;
    }

    /*
     * Copie de la configuration actuelle.
     */

    runtime_config_t cfg =
        *current;

    /*------------------------------------------------------
     * Mode
     *-----------------------------------------------------*/

    cJSON *item =
        cJSON_GetObjectItem(
            json,
            "mode");

    if (item != NULL)
    {
        if (!cJSON_IsString(item) ||
            item->valuestring == NULL)
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid mode\"}");

            return;
        }

        thermostat_mode_t mode;

        if (!thermostat_string_to_mode(
                item->valuestring,
                &mode))
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid mode\"}");

            return;
        }

        cfg.mode = mode;
    }

    /*------------------------------------------------------
     * Setpoint
     *-----------------------------------------------------*/

    item =
        cJSON_GetObjectItem(
            json,
            "setpoint");

    if (item != NULL)
    {
        if (!cJSON_IsNumber(item) ||
            item->valuedouble < 5.0 ||
            item->valuedouble > 35.0)
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid setpoint\"}");

            return;
        }

        cfg.setpoint =
            (float)item->valuedouble;
    }

    /*------------------------------------------------------
     * Hysteresis
     *-----------------------------------------------------*/

    item =
        cJSON_GetObjectItem(
            json,
            "hysteresis");

    if (item != NULL)
    {
        if (!cJSON_IsNumber(item) ||
            item->valuedouble < 0.0)
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid hysteresis\"}");

            return;
        }

        cfg.hysteresis =
            (float)item->valuedouble;
    }

    /*------------------------------------------------------
     * Relay delay
     *-----------------------------------------------------*/

    item =
        cJSON_GetObjectItem(
            json,
            "relay_delay");

    if (item != NULL)
    {
        if (!cJSON_IsNumber(item) ||
            item->valuedouble <= 0.0)
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid relay_delay\"}");

            return;
        }

        cfg.relay_delay =
            (uint32_t)item->valuedouble;
    }

    /*------------------------------------------------------
     * Latitude
     *-----------------------------------------------------*/

    item =
        cJSON_GetObjectItem(
            json,
            "latitude");

    if (item != NULL)
    {
        if (!cJSON_IsNumber(item) ||
            item->valuedouble < -90.0 ||
            item->valuedouble > 90.0)
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid latitude\"}");

            return;
        }

        cfg.latitude =
            (float)item->valuedouble;
    }

    /*------------------------------------------------------
     * Longitude
     *-----------------------------------------------------*/

    item =
        cJSON_GetObjectItem(
            json,
            "longitude");

    if (item != NULL)
    {
        if (!cJSON_IsNumber(item) ||
            item->valuedouble < -180.0 ||
            item->valuedouble > 180.0)
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid longitude\"}");

            return;
        }

        cfg.longitude =
            (float)item->valuedouble;
    }

    /*------------------------------------------------------
     * Weather provider
     *-----------------------------------------------------*/

    item =
        cJSON_GetObjectItem(
            json,
            "weather_provider");

    if (item != NULL)
    {
        if (!cJSON_IsNumber(item))
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid weather_provider\"}");

            return;
        }

        cfg.weather_provider =
            (weather_provider_t)item->valueint;
    }

    /*------------------------------------------------------
     * Weather update period
     *-----------------------------------------------------*/

    item =
        cJSON_GetObjectItem(
            json,
            "weather_update_period_sec");

    if (item != NULL)
    {
        if (!cJSON_IsNumber(item) ||
            item->valuedouble <= 0.0)
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid weather_update_period_sec\"}");

            return;
        }

        cfg.weather_update_period_sec =
            (uint32_t)item->valuedouble;
    }

    /*------------------------------------------------------
     * Alarm history period
     *-----------------------------------------------------*/

    item =
        cJSON_GetObjectItem(
            json,
            "alarm_history_save_period");

    if (item != NULL)
    {
        if (!cJSON_IsNumber(item) ||
            item->valuedouble <= 0.0)
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid alarm_history_save_period\"}");

            return;
        }

        cfg.alarm_history_save_period =
            (uint32_t)item->valuedouble;
    }

    cJSON_Delete(json);

    /*
     * Sauvegarde complète.
     */

    if (!storage_save_runtime(&cfg))
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"runtime save failed\"}");

        return;
    }

    /*
     * Recharge la configuration en mémoire
     * via runtime_load().
     */

    if (!runtime_load())
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"runtime reload failed\"}");

        return;
    }

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        "{\"updated\":true}");
}

#endif /* _WIN32 */
