#ifndef API_HISTORY_H
#define API_HISTORY_H

#ifdef _WIN32

#include <winsock2.h>

void api_history_handle(
    SOCKET client_socket,
    const char *request);

#endif /* _WIN32 */

#endif /* API_HISTORY_H */
