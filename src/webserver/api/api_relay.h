#ifndef API_RELAY_H
#define API_RELAY_H

#ifdef _WIN32

#include <winsock2.h>

void api_relay_handle_status(
    SOCKET client_socket);

void api_relay_handle_set(
    SOCKET client_socket,
    const char *request);

#endif /* _WIN32 */

#endif /* API_RELAY_H */
