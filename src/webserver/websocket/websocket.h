#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdbool.h>

#ifdef _WIN32

#include <winsock2.h>

bool websocket_init(void);

bool websocket_accept(
    SOCKET client_socket,
    const char *request);

void websocket_update(void);

void websocket_broadcast(
    const char *message);

void websocket_stop(void);

bool websocket_is_connected(void);

#else

bool websocket_init(void);

bool websocket_accept(
    int client_socket,
    const char *request);

void websocket_update(void);

void websocket_broadcast(
    const char *message);

void websocket_stop(void);

bool websocket_is_connected(void);

#endif

#endif
