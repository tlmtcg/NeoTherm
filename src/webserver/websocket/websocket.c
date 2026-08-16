#include "websocket.h"

#ifdef _WIN32

#include <stdio.h>
#include <string.h>

/*==========================================================
 * Etat privé
 *=========================================================*/

static SOCKET s_client_socket =
    INVALID_SOCKET;

static bool s_connected =
    false;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool websocket_init(void)
{
    s_client_socket =
        INVALID_SOCKET;

    s_connected =
        false;

    printf(
        "WebSocket initialized\n");

    return true;
}

/*==========================================================
 * Handshake
 *=========================================================*/

bool websocket_accept(
    SOCKET client_socket,
    const char *request)
{
    if (client_socket ==
        INVALID_SOCKET)
    {
        return false;
    }

    if (request == NULL)
    {
        return false;
    }

    /*
     * Première version :
     *
     * on vérifie simplement que la requête
     * demande bien /ws.
     */

    if (strstr(
            request,
            "GET /ws") == NULL)
    {
        return false;
    }

    /*
     * Pour l'instant nous ne faisons pas
     * encore le véritable handshake WebSocket.
     *
     * Cette étape sera ajoutée juste après.
     */

    s_client_socket =
        client_socket;

    s_connected =
        true;

    printf(
        "WebSocket client accepted\n");

    return true;
}

/*==========================================================
 * Mise à jour
 *=========================================================*/

void websocket_update(void)
{
    if (!s_connected)
    {
        return;
    }

    /*
     * La gestion des frames WebSocket
     * sera ajoutée ici.
     */
}

/*==========================================================
 * Broadcast
 *=========================================================*/

void websocket_broadcast(
    const char *message)
{
    if (!s_connected)
    {
        return;
    }

    if (message == NULL)
    {
        return;
    }

    /*
     * Temporairement :
     * affichage uniquement.
     */

    printf(
        "WebSocket TX: %s\n",
        message);
}

/*==========================================================
 * Arrêt
 *=========================================================*/

void websocket_stop(void)
{
    if (s_client_socket !=
        INVALID_SOCKET)
    {
        closesocket(
            s_client_socket);

        s_client_socket =
            INVALID_SOCKET;
    }

    s_connected =
        false;

    printf(
        "WebSocket stopped\n");
}

/*==========================================================
 * Etat
 *=========================================================*/

bool websocket_is_connected(void)
{
    return s_connected;
}

#else

bool websocket_init(void)
{
    return true;
}

bool websocket_accept(
    int client_socket,
    const char *request)
{
    (void)client_socket;
    (void)request;

    return false;
}

void websocket_update(void)
{
}

void websocket_broadcast(
    const char *message)
{
    (void)message;
}

void websocket_stop(void)
{
}

bool websocket_is_connected(void)
{
    return false;
}

#endif
