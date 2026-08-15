#include "api_clock.h"

#ifdef _WIN32

#include <stdio.h>

#include "clock.h"
#include "webserver_http.h"

/*==========================================================
 * GET /api/clock
 *=========================================================*/

void api_clock_handle_status(
    SOCKET client_socket)
{
    clock_time_t time;

    if (!clock_get_time(&time))
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"clock unavailable\"}");

        return;
    }

    uint32_t timestamp =
        clock_get_timestamp();

    char response[256];

    snprintf(
        response,
        sizeof(response),

        "{"
        "\"year\":%u,"
        "\"month\":%u,"
        "\"day\":%u,"
        "\"hour\":%u,"
        "\"minute\":%u,"
        "\"second\":%u,"
        "\"datetime\":\"%04u-%02u-%02u %02u:%02u:%02u\","
        "\"timestamp\":%u"
        "}",

        time.year,
        time.month,
        time.day,
        time.hour,
        time.minute,
        time.second,

        time.year,
        time.month,
        time.day,
        time.hour,
        time.minute,
        time.second,

        timestamp);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

#endif /* _WIN32 */
