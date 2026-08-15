#include "api_schedule.h"

#ifdef _WIN32

#include <stdio.h>

#include "schedule.h"
#include "webserver_http.h"
#include "webserver_request.h"
#include "../../external/cjson/cJSON.h"

/*==========================================================
 * GET /api/schedule
 *=========================================================*/

void api_schedule_handle(
    SOCKET client_socket)
{
    static const char *day_names[] =
    {
        "MONDAY",
        "TUESDAY",
        "WEDNESDAY",
        "THURSDAY",
        "FRIDAY",
        "SATURDAY",
        "SUNDAY"
    };

    char response[8192];

    size_t offset = 0;

    int written =
        snprintf(
            response + offset,
            sizeof(response) - offset,
            "{\"days\":["
        );

    if (written < 0)
    {
        return;
    }

    offset += (size_t)written;

    /*
     * Parcours des 7 jours.
     */

    for (uint8_t day = 0;
         day < 7;
         day++)
    {
        schedule_day_t schedule;

        if (!schedule_get_day(
                day,
                &schedule))
        {
            continue;
        }

        /*
         * Séparateur entre les jours.
         */

        if (day > 0)
        {
            written =
                snprintf(
                    response + offset,
                    sizeof(response) - offset,
                    ","
                );

            if (written < 0)
            {
                return;
            }

            offset += (size_t)written;
        }

        /*
         * Début du jour.
         */

        written =
            snprintf(
                response + offset,
                sizeof(response) - offset,
                "{\"day\":\"%s\",\"points\":[",
                day_names[day]
            );

        if (written < 0)
        {
            return;
        }

        offset += (size_t)written;

        /*
         * Points du jour.
         */

        for (uint32_t i = 0;
             i < schedule.count;
             i++)
        {
            const schedule_point_t *point =
                &schedule.points[i];

            /*
             * Séparateur entre les points.
             */

            if (i > 0)
            {
                written =
                    snprintf(
                        response + offset,
                        sizeof(response) - offset,
                        ","
                    );

                if (written < 0)
                {
                    return;
                }

                offset += (size_t)written;
            }

            /*
             * Point JSON.
             */

            written =
                snprintf(
                    response + offset,
                    sizeof(response) - offset,

                    "{"
                    "\"hour\":%u,"
                    "\"minute\":%u,"
                    "\"setpoint\":%.2f"
                    "}",

                    point->hour,
                    point->minute,
                    point->setpoint
                );

            if (written < 0)
            {
                return;
            }

            offset += (size_t)written;

            /*
             * Protection contre le débordement.
             */

            if (offset >= sizeof(response))
            {
                return;
            }
        }

        /*
         * Fin du jour.
         */

        written =
            snprintf(
                response + offset,
                sizeof(response) - offset,
                "]}"
            );

        if (written < 0)
        {
            return;
        }

        offset += (size_t)written;

        /*
         * Protection contre le débordement.
         */

        if (offset >= sizeof(response))
        {
            return;
        }
    }

    /*
     * Fin du tableau.
     */

    written =
        snprintf(
            response + offset,
            sizeof(response) - offset,
            "]}"
        );

    if (written < 0)
    {
        return;
    }

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * POST /api/schedule
 *=========================================================*/

void api_schedule_handle_set(
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

    cJSON *day =
        cJSON_GetObjectItem(
            json,
            "day");

    cJSON *hour =
        cJSON_GetObjectItem(
            json,
            "hour");

    cJSON *minute =
        cJSON_GetObjectItem(
            json,
            "minute");

    cJSON *setpoint =
        cJSON_GetObjectItem(
            json,
            "setpoint");

    if (!cJSON_IsNumber(day) ||
        !cJSON_IsNumber(hour) ||
        !cJSON_IsNumber(minute) ||
        !cJSON_IsNumber(setpoint))
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing schedule fields\"}");

        return;
    }

    int day_value =
        day->valueint;

    int hour_value =
        hour->valueint;

    int minute_value =
        minute->valueint;

    float setpoint_value =
        (float)setpoint->valuedouble;

    cJSON_Delete(json);

    /*
     * Validation.
     */

    if (day_value < 0 ||
        day_value > 6)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid day\"}");

        return;
    }

    if (hour_value < 0 ||
        hour_value > 23)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid hour\"}");

        return;
    }

    if (minute_value < 0 ||
        minute_value > 59)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid minute\"}");

        return;
    }

    /*
     * On reprend les limites du thermostat.
     */

    if (setpoint_value < 5.0f ||
        setpoint_value > 35.0f)
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
     * Ajout ou modification.
     */

    if (!schedule_set_point(
            (uint8_t)day_value,
            (uint8_t)hour_value,
            (uint8_t)minute_value,
            setpoint_value))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"unable to set schedule point\"}");

        return;
    }

    /*
     * Sauvegarde.
     */

    if (!schedule_save())
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"unable to save schedule\"}");

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
        "\"day\":%d,"
        "\"hour\":%d,"
        "\"minute\":%d,"
        "\"setpoint\":%.2f"
        "}",
        day_value,
        hour_value,
        minute_value,
        setpoint_value);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

/*==========================================================
 * DELETE /api/schedule
 *=========================================================*/

void api_schedule_handle_remove(
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

    cJSON *day =
        cJSON_GetObjectItem(
            json,
            "day");

    cJSON *hour =
        cJSON_GetObjectItem(
            json,
            "hour");

    cJSON *minute =
        cJSON_GetObjectItem(
            json,
            "minute");

    if (!cJSON_IsNumber(day) ||
        !cJSON_IsNumber(hour) ||
        !cJSON_IsNumber(minute))
    {
        cJSON_Delete(json);

        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing schedule fields\"}");

        return;
    }

    int day_value =
        day->valueint;

    int hour_value =
        hour->valueint;

    int minute_value =
        minute->valueint;

    cJSON_Delete(json);

    /*
     * Validation.
     */

    if (day_value < 0 ||
        day_value > 6)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid day\"}");

        return;
    }

    if (hour_value < 0 ||
        hour_value > 23)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid hour\"}");

        return;
    }

    if (minute_value < 0 ||
        minute_value > 59)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid minute\"}");

        return;
    }

    /*
     * Suppression.
     */

    if (!schedule_remove_point(
            (uint8_t)day_value,
            (uint8_t)hour_value,
            (uint8_t)minute_value))
    {
        webserver_http_send_response(
            client_socket,
            404,
            "Not Found",
            "application/json",
            "{\"error\":\"schedule point not found\"}");

        return;
    }

    /*
     * Sauvegarde.
     */

    if (!schedule_save())
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"unable to save schedule\"}");

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
        "\"day\":%d,"
        "\"hour\":%d,"
        "\"minute\":%d,"
        "\"removed\":true"
        "}",
        day_value,
        hour_value,
        minute_value);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

#endif /* _WIN32 */
