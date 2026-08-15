#include "api_scheduler.h"

#ifdef _WIN32

#include <stdio.h>
#include <string.h>

#include "../../external/cjson/cJSON.h"

#include "scheduler.h"
#include "webserver_http.h"
#include "webserver_request.h"

/*==========================================================
 * GET /api/scheduler
 *=========================================================*/

void api_scheduler_handle_status(
    SOCKET client_socket)
{
    char body[4096];

    int offset = 0;

    offset += snprintf(
        body + offset,
        sizeof(body) - (size_t)offset,
        "{"
        "\"task_count\":%u,"
        "\"max_tasks\":%u,"
        "\"tasks\":[",
        scheduler_task_count(),
        (unsigned int)SCHEDULER_MAX_TASKS);

    for (uint32_t i = 0;
         i < scheduler_task_count();
         i++)
    {
        scheduler_info_t info;

        if (!scheduler_get_info(i, &info))
        {
            continue;
        }

        if (i > 0)
        {
            offset += snprintf(
                body + offset,
                sizeof(body) - (size_t)offset,
                ",");
        }

        uint32_t next =
            info.period > info.counter
                ? info.period - info.counter
                : 0;

        offset += snprintf(
            body + offset,
            sizeof(body) - (size_t)offset,
            "{"
            "\"id\":%u,"
            "\"name\":\"%s\","
            "\"period\":%u,"
            "\"counter\":%u,"
            "\"next\":%u,"
            "\"enabled\":%s"
            "}",

            i,

            info.name,

            info.period,

            info.counter,

            next,

            info.enabled
                ? "true"
                : "false");
    }

    offset += snprintf(
        body + offset,
        sizeof(body) - (size_t)offset,
        "]}");

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}


/*==========================================================
 * Traitement du nom de tâche
 *=========================================================*/

static bool api_scheduler_get_name(
    const char *request,
    char *name,
    size_t name_size)
{
    if ((request == NULL) ||
        (name == NULL) ||
        (name_size == 0))
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
            "name");

    if (!cJSON_IsString(item) ||
        item->valuestring == NULL)
    {
        cJSON_Delete(json);

        return false;
    }

    snprintf(
        name,
        name_size,
        "%s",
        item->valuestring);

    cJSON_Delete(json);

    return true;
}


/*==========================================================
 * POST /api/scheduler/enable
 *=========================================================*/

void api_scheduler_handle_enable(
    SOCKET client_socket,
    const char *request)
{
    char name[64];

    if (!api_scheduler_get_name(
            request,
            name,
            sizeof(name)))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing or invalid name\"}");

        return;
    }

    if (!scheduler_exists(name))
    {
        webserver_http_send_response(
            client_socket,
            404,
            "Not Found",
            "application/json",
            "{\"error\":\"task not found\"}");

        return;
    }

    if (!scheduler_enable(name))
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"unable to enable task\"}");

        return;
    }

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"enabled\":true,\"name\":\"%s\"}",
        name);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}


/*==========================================================
 * POST /api/scheduler/disable
 *=========================================================*/

void api_scheduler_handle_disable(
    SOCKET client_socket,
    const char *request)
{
    char name[64];

    if (!api_scheduler_get_name(
            request,
            name,
            sizeof(name)))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing or invalid name\"}");

        return;
    }

    if (!scheduler_exists(name))
    {
        webserver_http_send_response(
            client_socket,
            404,
            "Not Found",
            "application/json",
            "{\"error\":\"task not found\"}");

        return;
    }

    if (!scheduler_disable(name))
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"unable to disable task\"}");

        return;
    }

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"enabled\":false,\"name\":\"%s\"}",
        name);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}


/*==========================================================
 * POST /api/scheduler/remove
 *=========================================================*/

void api_scheduler_handle_remove(
    SOCKET client_socket,
    const char *request)
{
    char name[64];

    if (!api_scheduler_get_name(
            request,
            name,
            sizeof(name)))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"missing or invalid name\"}");

        return;
    }

    if (!scheduler_exists(name))
    {
        webserver_http_send_response(
            client_socket,
            404,
            "Not Found",
            "application/json",
            "{\"error\":\"task not found\"}");

        return;
    }

    if (!scheduler_remove(name))
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"unable to remove task\"}");

        return;
    }

    char response[128];

    snprintf(
        response,
        sizeof(response),
        "{\"removed\":true,\"name\":\"%s\"}",
        name);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

#endif /* _WIN32 */
