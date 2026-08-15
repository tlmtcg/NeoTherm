#ifndef API_SCHEDULER_H
#define API_SCHEDULER_H

#ifdef _WIN32

#include "webserver_http.h"

void api_scheduler_handle_status(
    SOCKET client_socket);

void api_scheduler_handle_enable(
    SOCKET client_socket,
    const char *request);

void api_scheduler_handle_disable(
    SOCKET client_socket,
    const char *request);

void api_scheduler_handle_remove(
    SOCKET client_socket,
    const char *request);

#endif /* _WIN32 */

#endif /* API_SCHEDULER_H */
