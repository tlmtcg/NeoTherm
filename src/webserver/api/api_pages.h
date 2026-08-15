#ifndef API_PAGES_H
#define API_PAGES_H

#ifdef _WIN32

#include "webserver_http.h"

void api_pages_handle_index(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_PAGES_H */