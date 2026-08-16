#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32

#include <winsock2.h>

typedef SOCKET websocket_server_socket_t;

#else

typedef int websocket_server_socket_t;

#endif

/*==========================================================
 * Configuration
 *==========================================================*/

#define WEBSOCKET_SERVER_BUFFER_SIZE 16384

/*==========================================================
 * Initialisation
 *==========================================================*/

/**
 * Initialise le serveur WebSocket.
 */
bool websocket_server_init(void);

/**
 * Arrête le serveur WebSocket.
 */
void websocket_server_shutdown(void);

/*==========================================================
 * Connexion
 *==========================================================*/

/**
 * Accepte une connexion WebSocket sur une socket TCP
 * déjà acceptée par le webserver.
 *
 * La fonction effectue le WebSocket handshake.
 *
 * @param socket Socket TCP connectée au navigateur.
 *
 * @return true si le handshake réussit.
 */
bool websocket_server_accept(
    websocket_server_socket_t socket);

/**
 * Ferme une connexion WebSocket.
 */
void websocket_server_close(
    websocket_server_socket_t socket);

/**
 * Indique si une connexion WebSocket est active.
 */
bool websocket_server_is_connected(void);

/*==========================================================
 * Réception
 *==========================================================*/

/**
 * Traite les données reçues depuis le navigateur.
 *
 * Cette fonction :
 *
 * - décode les trames WebSocket ;
 * - démasque les données client ;
 * - traite TEXT ;
 * - répond aux PING ;
 * - traite CLOSE.
 *
 * @param socket Socket du client.
 *
 * @return true si la connexion reste active.
 */
bool websocket_server_receive(
    websocket_server_socket_t socket);

/*==========================================================
 * Envoi
 *==========================================================*/

/**
 * Envoie une chaîne JSON au navigateur.
 *
 * @param socket Socket du client.
 * @param json JSON à envoyer.
 *
 * @return true si l'envoi réussit.
 */
bool websocket_server_send_text(
    websocket_server_socket_t socket,
    const char *json);

/**
 * Envoie un PING.
 */
bool websocket_server_send_ping(
    websocket_server_socket_t socket);

/**
 * Envoie un PONG.
 */
bool websocket_server_send_pong(
    websocket_server_socket_t socket,
    const uint8_t *payload,
    size_t length);

/**
 * Envoie une fermeture WebSocket.
 */
bool websocket_server_send_close(
    websocket_server_socket_t socket,
    uint16_t status_code,
    const char *reason);

#endif /* WEBSOCKET_SERVER_H */
