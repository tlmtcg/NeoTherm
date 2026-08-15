#ifndef API_THERMAL_PREDICTION_H
#define API_THERMAL_PREDICTION_H

#ifdef _WIN32

#include "webserver.h"
#include <winsock2.h>

void api_thermal_prediction_handle_status(
    SOCKET client_socket);

void api_thermal_prediction_handle_current(
    SOCKET client_socket);

void api_thermal_prediction_handle_next_tick(
    SOCKET client_socket);

void api_thermal_prediction_handle_valid(
    SOCKET client_socket);

void api_thermal_prediction_handle_heated(
    SOCKET client_socket);

void api_thermal_prediction_handle_natural(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_THERMAL_PREDICTION_H */
