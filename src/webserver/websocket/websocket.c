#include "websocket.h"

#include "websocket_server.h"
#include "websocket_handshake.h"

/*==========================================================
 * Initialisation
 *=========================================================*/

bool websocket_init(void)
{
    return websocket_server_init();
}

/*==========================================================
 * Acceptation
 *=========================================================*/

bool websocket_accept(
    websocket_socket_t client_socket,
    const char *request)
{
    /*
     * Le handshake est maintenant entièrement géré
     * par websocket_server_accept().
     *
     * request n'est plus utilisé :
     * websocket_handshake() lit directement la requête
     * HTTP depuis le socket.
     */

    (void)request;

    return websocket_server_accept(
        client_socket);
}

/*==========================================================
 * Mise à jour
 *=========================================================*/

void websocket_update(void)
{
    if (!websocket_server_is_connected())
    {
        return;
    }

    /*
     * websocket_server_receive() utilise maintenant
     * le socket actif enregistré par le serveur.
     *
     * Le socket n'est cependant pas exposé par
     * websocket_server.h.
     *
     * Cette fonction sera donc complétée lorsque
     * l'API publique du serveur fournira une fonction
     * receive() sans argument.
     */
}

/*==========================================================
 * Broadcast
 *=========================================================*/

void websocket_broadcast(
    const char *message)
{
    /*
     * Le serveur ne gère actuellement qu'un seul
     * client WebSocket actif.
     *
     * Le socket actif n'est pas encore exposé ici.
     *
     * Cette fonction sera donc reliée à
     * websocket_server_send_text() lorsque l'API
     * sera finalisée.
     */

    (void)message;
}

/*==========================================================
 * Arrêt
 *=========================================================*/

void websocket_stop(void)
{
    websocket_server_shutdown();
}

/*==========================================================
 * Etat
 *=========================================================*/

bool websocket_is_connected(void)
{
    return websocket_server_is_connected();
}
