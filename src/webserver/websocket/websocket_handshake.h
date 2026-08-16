#ifndef WEBSOCKET_HANDSHAKE_H
#define WEBSOCKET_HANDSHAKE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef _WIN32

#include <winsock2.h>

typedef SOCKET websocket_socket_t;

#else

typedef int websocket_socket_t;

#endif

/*
 * Taille maximale de la requête HTTP utilisée
 * pour le handshake WebSocket.
 */
#define WEBSOCKET_HANDSHAKE_BUFFER_SIZE 8192

/**
 * Effectue le WebSocket handshake côté serveur.
 *
 * La socket doit déjà être connectée.
 *
 * Le client doit envoyer une requête HTTP contenant :
 *
 *   Upgrade: websocket
 *   Connection: Upgrade
 *   Sec-WebSocket-Key: ...
 *   Sec-WebSocket-Version: 13
 *
 * Le serveur répond :
 *
 *   HTTP/1.1 101 Switching Protocols
 *   Upgrade: websocket
 *   Connection: Upgrade
 *   Sec-WebSocket-Accept: ...
 *
 * @param socket Socket TCP connectée au client.
 *
 * @return true si le handshake réussit.
 */
bool websocket_handshake(
    websocket_socket_t socket);

#endif /* WEBSOCKET_HANDSHAKE_H */
