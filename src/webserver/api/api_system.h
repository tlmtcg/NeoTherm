#ifndef API_SYSTEM_H
#define API_SYSTEM_H

#ifdef _WIN32

#include <winsock2.h>

void api_system_handle_root(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_SYSTEM_H */
