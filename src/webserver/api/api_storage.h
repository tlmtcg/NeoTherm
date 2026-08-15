#ifndef API_STORAGE_H
#define API_STORAGE_H

#ifdef _WIN32

#include "webserver_http.h"

void api_storage_handle_get(
    SOCKET client_socket);

void api_storage_handle_post(
    SOCKET client_socket,
    const char *request);

void api_storage_handle_delete(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_STORAGE_H */
