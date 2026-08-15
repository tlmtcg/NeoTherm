#ifndef API_THERMAL_H
#define API_THERMAL_H

#ifdef _WIN32

#include <winsock2.h>

/*==========================================================
 * GET /api/thermal
 *=========================================================*/

void api_thermal_handle_status(
    SOCKET client_socket);

/*==========================================================
 * GET /api/thermal/outside_temperature
 *=========================================================*/

void api_thermal_handle_outside_temperature(
    SOCKET client_socket);

/*==========================================================
 * GET /api/thermal/heat_power
 *=========================================================*/

void api_thermal_handle_heat_power(
    SOCKET client_socket);

/*==========================================================
 * GET /api/thermal/loss_factor
 *=========================================================*/

void api_thermal_handle_loss_factor(
    SOCKET client_socket);

/*==========================================================
 * GET /api/thermal/mass
 *=========================================================*/

void api_thermal_handle_mass(
    SOCKET client_socket);

/*==========================================================
 * POST /api/thermal/outside_temperature
 *=========================================================*/

void api_thermal_handle_set_outside_temperature(
    SOCKET client_socket,
    const char *request);

/*==========================================================
 * POST /api/thermal/heat_power
 *=========================================================*/

void api_thermal_handle_set_heat_power(
    SOCKET client_socket,
    const char *request);

/*==========================================================
 * POST /api/thermal/loss_factor
 *=========================================================*/

void api_thermal_handle_set_loss_factor(
    SOCKET client_socket,
    const char *request);

/*==========================================================
 * POST /api/thermal/mass
 *=========================================================*/

void api_thermal_handle_set_mass(
    SOCKET client_socket,
    const char *request);

#endif /* _WIN32 */

#endif /* API_THERMAL_H */