#ifndef API_THERMAL_LEARNING_H
#define API_THERMAL_LEARNING_H

#ifdef _WIN32

#include <winsock2.h>

/*==========================================================
 * GET /api/thermal/learning
 *=========================================================*/

void api_thermal_learning_handle_status(
    SOCKET client_socket);

/*==========================================================
 * GET /api/thermal/learning/heat_rate
 *=========================================================*/

void api_thermal_learning_handle_heat_rate(
    SOCKET client_socket);

/*==========================================================
 * GET /api/thermal/learning/cooling_rate
 *=========================================================*/

void api_thermal_learning_handle_cooling_rate(
    SOCKET client_socket);

/*==========================================================
 * GET /api/thermal/learning/overshoot
 *=========================================================*/

void api_thermal_learning_handle_overshoot(
    SOCKET client_socket);

/*==========================================================
 * GET /api/thermal/learning/valid
 *=========================================================*/

void api_thermal_learning_handle_valid(
    SOCKET client_socket);

#endif /* _WIN32 */

#endif /* API_THERMAL_LEARNING_H */
