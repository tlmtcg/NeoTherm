#ifndef API_EVENT_H
#define API_EVENT_H

#ifdef _WIN32

#include "webserver_http.h"

void api_event_handle_status(
    SOCKET client_socket);

void api_event_handle_post(
    SOCKET client_socket,
    const char *request);

void api_event_handle_dispatch(
    SOCKET client_socket,
    const char *request);
    
#endif /* _WIN32 */

#endif /* API_EVENT_H */
