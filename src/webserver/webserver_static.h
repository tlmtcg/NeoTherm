#ifndef WEBSERVER_STATIC_H
#define WEBSERVER_STATIC_H

#ifdef _WIN32

#include <winsock2.h>
#include <stdbool.h>

bool webserver_static_handle(
    SOCKET client_socket,
    const char *path);

#endif /* _WIN32 */

#endif /* WEBSERVER_STATIC_H */
