#include "api_climate.h"

#ifdef _WIN32

#include <stdio.h>

#include "climate.h"
#include "webserver_http.h"

/*==========================================================
 * GET /api/climate
 *=========================================================*/

void api_climate_handle(
    SOCKET client_socket)
{
    float temperature =
        climate_get_temperature();

    char body[128];

    snprintf(
        body,
        sizeof(body),
        "{\"temperature\":%.2f}",
        temperature);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

#endif /* _WIN32 */
