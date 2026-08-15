#include "api_weather.h"

#ifdef _WIN32

#include <stdio.h>

#include "weather.h"
#include "webserver_http.h"

/*==========================================================
 * GET /api/weather
 *=========================================================*/

void api_weather_handle(
    SOCKET client_socket)
{
    const weather_t *weather =
        weather_get();

    /*
     * Aucune donnée météo valide.
     */

    if (weather == NULL)
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"weather unavailable\"}");

        return;
    }

    /*
     * Construction JSON.
     */

    char body[512];

    snprintf(
        body,
        sizeof(body),

        "{"
        "\"temperature\":%.2f,"
        "\"humidity\":%.2f,"
        "\"pressure\":%.2f,"
        "\"wind_speed\":%.2f,"
        "\"wind_gust\":%.2f,"
        "\"wind_direction\":%.2f,"
        "\"rain\":%.2f"
        "}",

        weather->temperature,
        weather->humidity,
        weather->pressure,
        weather->wind_speed,
        weather->wind_gust,
        weather->wind_direction,
        weather->rain);

    /*
     * Réponse HTTP.
     */

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

#endif /* _WIN32 */
