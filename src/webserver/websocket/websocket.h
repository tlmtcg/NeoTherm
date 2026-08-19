#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdbool.h>

#include "websocket_server.h"

/*==========================================================
 * Initialisation
 *=========================================================*/

bool websocket_init(void);

/*==========================================================
 * Acceptation d'un client
 *=========================================================*/

bool websocket_accept(
    websocket_server_socket_t client_socket,
    const char *request);

/*==========================================================
 * Mise à jour
 *=========================================================*/

void websocket_update(void);

/*==========================================================
 * Envoi
 *=========================================================*/

void websocket_broadcast(
    const char *message);

/*==========================================================
 * Arrêt
 *=========================================================*/

void websocket_stop(void);

/*==========================================================
 * Etat
 *=========================================================*/

bool websocket_is_connected(void);

#endif /* WEBSOCKET_H */
