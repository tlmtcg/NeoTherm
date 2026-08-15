#ifndef API_CLOCK_H
#define API_CLOCK_H

#ifdef _WIN32

#include "webserver_http.h"

void api_clock_handle_status(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_CLOCK_H */
