#ifndef API_CONFIG_H
#define API_CONFIG_H

#ifdef _WIN32

#include <winsock2.h>

void api_config_handle(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_CONFIG_H */
