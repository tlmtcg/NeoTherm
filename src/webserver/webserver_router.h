#ifndef WEBSERVER_ROUTER_H
#define WEBSERVER_ROUTER_H

#ifdef _WIN32

#include <winsock2.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================
 * Routage HTTP
 *=========================================================*/

bool webserver_router_handle(
    SOCKET client_socket,
    const char *request);

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */

#endif /* WEBSERVER_ROUTER_H */
