#ifndef WEBSERVER_HTTP_H
#define WEBSERVER_HTTP_H

#ifdef _WIN32

#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

void webserver_http_send_response(
    SOCKET client_socket,
    int status_code,
    const char *status_text,
    const char *content_type,
    const char *body);

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */

#endif /* WEBSERVER_HTTP_H */
