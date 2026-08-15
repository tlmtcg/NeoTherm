#ifndef API_THERMOSTAT_H
#define API_THERMOSTAT_H

#ifdef _WIN32

#include <winsock2.h>

void api_thermostat_handle_status(
    SOCKET client_socket);

void api_thermostat_handle_temperature(
    SOCKET client_socket);

void api_thermostat_handle_mode(
    SOCKET client_socket,
    const char *request);

void api_thermostat_handle_setpoint(
    SOCKET client_socket,
    const char *request);

void api_thermostat_handle_weather(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_THERMOSTAT_H */
