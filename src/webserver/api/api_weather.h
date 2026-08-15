#ifndef API_WEATHER_H
#define API_WEATHER_H

#ifdef _WIN32

#include <winsock2.h>

/*==========================================================
 * GET /api/weather
 *=========================================================*/

void api_weather_handle(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_WEATHER_H */
