#ifndef API_SCHEDULE_H
#define API_SCHEDULE_H

#ifdef _WIN32

#include <winsock2.h>

void api_schedule_handle(
    SOCKET client_socket);

void api_schedule_handle_set(
    SOCKET client_socket,
    const char *request);

void api_schedule_handle_remove(
    SOCKET client_socket,
    const char *request);

#endif /* _WIN32 */

#endif /* API_SCHEDULE_H */
