#include "api_storage.h"

#ifdef _WIN32

#include <stdio.h>
#include <string.h>

#include "../../external/cjson/cJSON.h"

#include "storage.h"
#include "runtime.h"
#include "thermostat.h"
#include "weather_provider.h"

#include "webserver_http.h"
#include "webserver_request.h"

/*==========================================================
 * GET /api/storage
 *=========================================================*/

void api_storage_handle_get(
    SOCKET client_socket)
{
    runtime_config_t cfg;

    storage_load_result_t result =
        storage_load_runtime(&cfg);

    if (result == STORAGE_LOAD_ERROR)
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"unable to load runtime\"}");

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
        "\"weather_provider\":\"%s\","
        "\"weather_update_period\":%u,"
        "\"date_time\":{"
            "\"year\":%u,"
            "\"month\":%u,"
            "\"day\":%u,"
            "\"hour\":%u,"
            "\"minute\":%u,"
            "\"second\":%u"
        "},"
        "\"source\":\"%s\""
        "}",

        thermostat_mode_to_string(cfg.mode),

        cfg.setpoint,

        cfg.hysteresis,

        cfg.relay_delay,

        cfg.latitude,

        cfg.longitude,

        weather_provider_to_string(
            cfg.weather_provider),

        cfg.weather_update_period_sec,

        cfg.date_time.year,
        cfg.date_time.month,
        cfg.date_time.day,
        cfg.date_time.hour,
        cfg.date_time.minute,
        cfg.date_time.second,

        result == STORAGE_LOAD_DEFAULT
            ? "default"
            : "file");

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

/*==========================================================
 * POST /api/storage
 *=========================================================*/

void api_storage_handle_post(
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

    /*
     * On part de la configuration actuelle.
     */
    runtime_config_t cfg;

    storage_load_result_t result =
        storage_load_runtime(&cfg);

    if (result == STORAGE_LOAD_ERROR)
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"unable to load runtime\"}");

        return;
    }

    /*
     * Mode
     */
    cJSON *mode =
        cJSON_GetObjectItem(json, "mode");

    if (mode != NULL)
    {
        if (!cJSON_IsString(mode) ||
            !thermostat_string_to_mode(
                mode->valuestring,
                &cfg.mode))
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
    }

    /*
     * Setpoint
     */
    cJSON *setpoint =
        cJSON_GetObjectItem(json, "setpoint");

    if (setpoint != NULL)
    {
        if (!cJSON_IsNumber(setpoint) ||
            setpoint->valuedouble < 5.0 ||
            setpoint->valuedouble > 35.0)
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
            (float)setpoint->valuedouble;
    }

    /*
     * Hystérésis
     */
    cJSON *hysteresis =
        cJSON_GetObjectItem(json, "hysteresis");

    if (hysteresis != NULL)
    {
        if (!cJSON_IsNumber(hysteresis) ||
            hysteresis->valuedouble < 0.0)
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
            (float)hysteresis->valuedouble;
    }

    /*
     * Relay delay
     */
    cJSON *relay_delay =
        cJSON_GetObjectItem(
            json,
            "relay_delay");

    if (relay_delay != NULL)
    {
        if (!cJSON_IsNumber(relay_delay) ||
            relay_delay->valuedouble <= 0.0)
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
            (uint32_t)relay_delay->valuedouble;
    }

    /*
     * Latitude
     */
    cJSON *latitude =
        cJSON_GetObjectItem(json, "latitude");

    if (latitude != NULL)
    {
        if (!cJSON_IsNumber(latitude) ||
            latitude->valuedouble < -90.0 ||
            latitude->valuedouble > 90.0)
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
            (float)latitude->valuedouble;
    }

    /*
     * Longitude
     */
    cJSON *longitude =
        cJSON_GetObjectItem(json, "longitude");

    if (longitude != NULL)
    {
        if (!cJSON_IsNumber(longitude) ||
            longitude->valuedouble < -180.0 ||
            longitude->valuedouble > 180.0)
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
            (float)longitude->valuedouble;
    }

    /*
     * Weather provider
     */
    cJSON *provider =
        cJSON_GetObjectItem(
            json,
            "weather_provider");

    if (provider != NULL)
    {
        if (!cJSON_IsString(provider))
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

        if (strcmp(
                provider->valuestring,
                "SIMULATOR") == 0)
        {
            cfg.weather_provider =
                WEATHER_PROVIDER_SIMULATOR;
        }
        else if (strcmp(
                     provider->valuestring,
                     "OPENMETEO") == 0)
        {
            cfg.weather_provider =
                WEATHER_PROVIDER_OPENMETEO;
        }
        else
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
    }

    /*
     * Weather update period
     */
    cJSON *weather_period =
        cJSON_GetObjectItem(
            json,
            "weather_update_period");

    if (weather_period != NULL)
    {
        if (!cJSON_IsNumber(weather_period) ||
            weather_period->valuedouble <= 0.0)
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid weather_update_period\"}");

            return;
        }

        cfg.weather_update_period_sec =
            (uint32_t)weather_period->valuedouble;
    }

    /*
     * Date / heure
     */
    cJSON *date_time =
        cJSON_GetObjectItem(
            json,
            "date_time");

    if (date_time != NULL)
    {
        if (!cJSON_IsObject(date_time))
        {
            cJSON_Delete(json);

            webserver_http_send_response(
                client_socket,
                400,
                "Bad Request",
                "application/json",
                "{\"error\":\"invalid date_time\"}");

            return;
        }

        cJSON *item;

        item = cJSON_GetObjectItem(
            date_time, "year");

        if (cJSON_IsNumber(item))
            cfg.date_time.year =
                (uint32_t)item->valuedouble;

        item = cJSON_GetObjectItem(
            date_time, "month");

        if (cJSON_IsNumber(item))
            cfg.date_time.month =
                (uint32_t)item->valuedouble;

        item = cJSON_GetObjectItem(
            date_time, "day");

        if (cJSON_IsNumber(item))
            cfg.date_time.day =
                (uint32_t)item->valuedouble;

        item = cJSON_GetObjectItem(
            date_time, "hour");

        if (cJSON_IsNumber(item))
            cfg.date_time.hour =
                (uint32_t)item->valuedouble;

        item = cJSON_GetObjectItem(
            date_time, "minute");

        if (cJSON_IsNumber(item))
            cfg.date_time.minute =
                (uint32_t)item->valuedouble;

        item = cJSON_GetObjectItem(
            date_time, "second");

        if (cJSON_IsNumber(item))
            cfg.date_time.second =
                (uint32_t)item->valuedouble;
    }

    cJSON_Delete(json);

    /*
     * Sauvegarde.
     */
    if (!storage_save_runtime(&cfg))
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"unable to save runtime\"}");

        return;
    }

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        "{\"saved\":true}");
}

/*==========================================================
 * DELETE /api/storage
 *=========================================================*/

void api_storage_handle_delete(
    SOCKET client_socket)
{
    if (!storage_test_clear())
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"unable to clear storage\"}");

        return;
    }

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        "{\"cleared\":true}");
}

#endif /* _WIN32 */
