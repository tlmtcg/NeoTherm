#include "api_system.h"

#ifdef _WIN32

#include "webserver_http.h"

/*==========================================================
 * GET /
 *=========================================================*/

void api_system_handle_root(
    SOCKET client_socket)
{
    const char *body =
        "NeoTherm Web Server\n"
        "OK\n";

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "text/plain",
        body);
}

#endif /* _WIN32 */
