#ifndef API_ALARM_H
#define API_ALARM_H

#ifdef _WIN32

#include <winsock2.h>

/*==========================================================
 * GET /api/alarms
 *=========================================================*/

void api_alarm_handle(
    SOCKET client_socket);

/*==========================================================
 * POST /api/alarm/ack
 *=========================================================*/

void api_alarm_handle_ack(
    SOCKET client_socket,
    const char *request);

/*==========================================================
 * POST /api/alarm/clear
 *=========================================================*/

void api_alarm_handle_clear(
    SOCKET client_socket,
    const char *request);

#endif /* _WIN32 */

#endif /* API_ALARM_H */
