#ifndef API_CLIMATE_H
#define API_CLIMATE_H

#ifdef _WIN32

#include <winsock2.h>

void api_climate_handle(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_CLIMATE_H */
