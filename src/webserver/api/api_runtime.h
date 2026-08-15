#ifndef API_RUNTIME_H
#define API_RUNTIME_H

#ifdef _WIN32

#include "webserver_http.h"

void api_runtime_handle_status(
    SOCKET client_socket);

void api_runtime_handle_update(
    SOCKET client_socket,
    const char *request);

#endif /* _WIN32 */

#endif /* API_RUNTIME_H */
