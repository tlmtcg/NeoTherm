#include "webserver.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#include "webserver_router.h"
#include "websocket/websocket_server.h"
#include "logger.h"

#define WEBSERVER_PORT 8080
#define WEBSERVER_BUFFER_SIZE 4096
#define WEBSERVER_BACKLOG 4

#define WEBSERVER_CLIENT_WAIT_MS 2000

/*==========================================================
 * Etat privé
 *=========================================================*/

static SOCKET s_server_socket =
    INVALID_SOCKET;

static SOCKET s_websocket_socket =
    INVALID_SOCKET;

static bool s_running =
    false;

static bool s_winsock_initialized =
    false;


/*==========================================================
 * Utilitaires socket
 *=========================================================*/

static bool webserver_set_nonblocking(
    SOCKET socket)
{
    u_long mode = 1;

    if (ioctlsocket(
            socket,
            FIONBIO,
            &mode) != 0)
    {
        LOG_ERROR(
            "WEBSERVER",
            "Unable to set socket non-blocking "
            "(socket=%llu, error=%d)",
            (unsigned long long)socket,
            WSAGetLastError());

        return false;
    }

    return true;
}

/*----------------------------------------------------------*/

static bool webserver_set_blocking(
    SOCKET socket)
{
    u_long mode = 0;

    if (ioctlsocket(
            socket,
            FIONBIO,
            &mode) != 0)
    {
        LOG_ERROR(
            "WEBSERVER",
            "Unable to set socket blocking "
            "(socket=%llu, error=%d)",
            (unsigned long long)socket,
            WSAGetLastError());

        return false;
    }

    return true;
}

/*----------------------------------------------------------*/

static void webserver_close_socket(
    SOCKET socket)
{
    if (socket == INVALID_SOCKET)
    {
        return;
    }

    LOG_DEBUG(
        "WEBSERVER",
        "Closing socket=%llu",
        (unsigned long long)socket);

    closesocket(socket);
}


/*==========================================================
 * Attente de données
 *=========================================================*/

static bool webserver_wait_for_client_data(
    SOCKET client_socket)
{
    fd_set read_set;

    struct timeval timeout;

    FD_ZERO(&read_set);

    FD_SET(
        client_socket,
        &read_set);

    timeout.tv_sec =
        WEBSERVER_CLIENT_WAIT_MS / 1000;

    timeout.tv_usec =
        (WEBSERVER_CLIENT_WAIT_MS % 1000) * 1000;

    int result =
        select(
            0,
            &read_set,
            NULL,
            NULL,
            &timeout);

    if (result == SOCKET_ERROR)
    {
        LOG_ERROR(
            "WEBSERVER",
            "select() failed "
            "(socket=%llu, error=%d)",
            (unsigned long long)client_socket,
            WSAGetLastError());

        return false;
    }

    if (result == 0)
    {
        LOG_WARN(
            "WEBSERVER",
            "Timeout waiting for TCP client data "
            "(socket=%llu)",
            (unsigned long long)client_socket);

        return false;
    }

    if (!FD_ISSET(
            client_socket,
            &read_set))
    {
        return false;
    }

    LOG_DEBUG(
        "WEBSERVER",
        "TCP client data available "
        "(socket=%llu)",
        (unsigned long long)client_socket);

    return true;
}


/*==========================================================
 * Détection WebSocket
 *=========================================================*/

static bool webserver_is_websocket_request(
    SOCKET client_socket)
{
    char buffer[WEBSERVER_BUFFER_SIZE];

    memset(
        buffer,
        0,
        sizeof(buffer));

    int received =
        recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            MSG_PEEK);

    if (received == SOCKET_ERROR)
    {
        int error =
            WSAGetLastError();

        LOG_ERROR(
            "WEBSERVER",
            "MSG_PEEK failed "
            "(socket=%llu, error=%d)",
            (unsigned long long)client_socket,
            error);

        return false;
    }

    if (received == 0)
    {
        LOG_WARN(
            "WEBSERVER",
            "MSG_PEEK returned zero "
            "(socket=%llu)",
            (unsigned long long)client_socket);

        return false;
    }

    buffer[received] =
        '\0';

    LOG_DEBUG(
        "WEBSERVER",
        "PEEK socket=%llu received=%d data=[%s]",
        (unsigned long long)client_socket,
        received,
        buffer);

    /*
     * Une requête HTTP/WebSocket commence par GET.
     */

    if (received < 4 ||
        strncmp(
            buffer,
            "GET ",
            4) != 0)
    {
        return false;
    }

    /*
     * Recherche du header Upgrade: websocket
     */

    const char *p =
        buffer;

    while (*p != '\0')
    {
        const char *line_end =
            strstr(
                p,
                "\r\n");

        if (line_end == NULL)
        {
            break;
        }

        /*
         * Fin des headers.
         */

        if (line_end == p)
        {
            break;
        }

        const char *colon =
            p;

        while (
            colon < line_end &&
            *colon != ':')
        {
            ++colon;
        }

        if (colon < line_end)
        {
            size_t name_length =
                (size_t)(colon - p);

            if (name_length == 7 &&
                _strnicmp(
                    p,
                    "Upgrade",
                    7) == 0)
            {
                const char *value =
                    colon + 1;

                while (
                    value < line_end &&
                    (*value == ' ' ||
                     *value == '\t'))
                {
                    ++value;
                }

                size_t value_length =
                    (size_t)(line_end - value);

                while (
                    value_length > 0 &&
                    (value[value_length - 1] == ' ' ||
                     value[value_length - 1] == '\t'))
                {
                    --value_length;
                }

                if (value_length == 9 &&
                    _strnicmp(
                        value,
                        "websocket",
                        9) == 0)
                {
                    LOG_DEBUG(
                        "WEBSERVER",
                        "TCP client identified as WebSocket "
                        "(socket=%llu)",
                        (unsigned long long)client_socket);

                    return true;
                }
            }
        }

        p =
            line_end + 2;
    }

    LOG_DEBUG(
        "WEBSERVER",
        "TCP client identified as HTTP "
        "(socket=%llu)",
        (unsigned long long)client_socket);

    return false;
}


/*==========================================================
 * Initialisation
 *=========================================================*/

bool webserver_init(void)
{
    if (s_server_socket != INVALID_SOCKET)
    {
        return true;
    }

    WSADATA wsa_data;

    int result =
        WSAStartup(
            MAKEWORD(2, 2),
            &wsa_data);

    if (result != 0)
    {
        LOG_ERROR(
            "WEBSERVER",
            "WSAStartup failed (%d)",
            result);

        return false;
    }

    s_winsock_initialized =
        true;

    s_server_socket =
        socket(
            AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP);

    if (s_server_socket == INVALID_SOCKET)
    {
        LOG_ERROR(
            "WEBSERVER",
            "Socket creation failed (%d)",
            WSAGetLastError());

        WSACleanup();

        s_winsock_initialized =
            false;

        return false;
    }

    /*
     * Le socket serveur doit être non-bloquant :
     * webserver_update() est appelé périodiquement.
     */

    if (!webserver_set_nonblocking(
            s_server_socket))
    {
        closesocket(
            s_server_socket);

        s_server_socket =
            INVALID_SOCKET;

        WSACleanup();

        s_winsock_initialized =
            false;

        return false;
    }

    s_websocket_socket =
        INVALID_SOCKET;

    s_running =
        false;

    LOG_DEBUG(
        "WEBSERVER",
        "Webserver initialized");

    return true;
}


/*==========================================================
 * Adresse serveur
 *=========================================================*/

static void webserver_build_address(
    struct sockaddr_in *address)
{
    memset(
        address,
        0,
        sizeof(*address));

    address->sin_family =
        AF_INET;

    address->sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    address->sin_port =
        htons(WEBSERVER_PORT);
}


/*==========================================================
 * Démarrage
 *=========================================================*/

bool webserver_start(void)
{
    if (s_server_socket == INVALID_SOCKET)
    {
        LOG_ERROR(
            "WEBSERVER",
            "Cannot start: invalid server socket");

        return false;
    }

    if (s_running)
    {
        return true;
    }

    struct sockaddr_in address;

    webserver_build_address(
        &address);

    if (bind(
            s_server_socket,
            (struct sockaddr *)&address,
            sizeof(address)) == SOCKET_ERROR)
    {
        LOG_ERROR(
            "WEBSERVER",
            "Bind failed (%d)",
            WSAGetLastError());

        webserver_stop();

        return false;
    }

    if (listen(
            s_server_socket,
            WEBSERVER_BACKLOG) == SOCKET_ERROR)
    {
        LOG_ERROR(
            "WEBSERVER",
            "Listen failed (%d)",
            WSAGetLastError());

        webserver_stop();

        return false;
    }

    s_running =
        true;

    LOG_DEBUG(
        "WEBSERVER",
        "Listening on http://localhost:%d",
        WEBSERVER_PORT);

    return true;
}


/*==========================================================
 * HTTP
 *=========================================================*/

static void webserver_process_http_client(
    SOCKET client_socket)
{
    char buffer[WEBSERVER_BUFFER_SIZE];

    memset(
        buffer,
        0,
        sizeof(buffer));

    LOG_DEBUG(
        "WEBSERVER",
        "Reading HTTP request "
        "(socket=%llu)",
        (unsigned long long)client_socket);

    int received =
        recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            0);

    if (received == SOCKET_ERROR)
    {
        LOG_ERROR(
            "WEBSERVER",
            "HTTP recv failed "
            "(socket=%llu, error=%d)",
            (unsigned long long)client_socket,
            WSAGetLastError());

        closesocket(
            client_socket);

        return;
    }

    if (received == 0)
    {
        closesocket(
            client_socket);

        return;
    }

    buffer[received] =
        '\0';

    LOG_DEBUG(
        "WEBSERVER",
        "HTTP request received "
        "(socket=%llu, bytes=%d)",
        (unsigned long long)client_socket,
        received);

    LOG_DEBUG(
        "WEBSERVER",
        "HTTP request: %.200s",
        buffer);

    webserver_router_handle(
        client_socket,
        buffer);

    closesocket(
        client_socket);

    LOG_DEBUG(
        "WEBSERVER",
        "HTTP client closed "
        "(socket=%llu)",
        (unsigned long long)client_socket);
}


/*==========================================================
 * WebSocket
 *=========================================================*/

static void webserver_process_websocket_client(
    SOCKET client_socket)
{
    LOG_DEBUG(
        "WEBSERVER",
        "Starting WebSocket handshake "
        "(socket=%llu)",
        (unsigned long long)client_socket);

    /*
     * IMPORTANT :
     *
     * Le socket accepté peut hériter du comportement
     * du socket serveur.
     *
     * Pour le handshake, on le remet explicitement
     * en mode bloquant.
     */

    if (!webserver_set_blocking(
            client_socket))
    {
        closesocket(
            client_socket);

        return;
    }

    /*
     * Le handshake consomme maintenant la requête
     * HTTP Upgrade complète.
     */

    if (!websocket_server_accept(
            client_socket))
    {
        LOG_ERROR(
            "WEBSERVER",
            "WebSocket handshake failed "
            "(socket=%llu)",
            (unsigned long long)client_socket);

        closesocket(
            client_socket);

        return;
    }

    LOG_DEBUG(
        "WEBSERVER",
        "WebSocket handshake successful "
        "(socket=%llu)",
        (unsigned long long)client_socket);

    /*
     * Une fois le handshake terminé, le socket WebSocket
     * doit redevenir non-bloquant.
     *
     * Cela est indispensable car webserver_update()
     * est appelé dans la boucle principale.
     */

    if (!webserver_set_nonblocking(
            client_socket))
    {
        LOG_ERROR(
            "WEBSERVER",
            "Unable to make WebSocket non-blocking "
            "(socket=%llu)",
            (unsigned long long)client_socket);

        websocket_server_close(
            client_socket);

        return;
    }

    s_websocket_socket =
        client_socket;

    LOG_DEBUG(
        "WEBSERVER",
        "WebSocket connection established "
        "(socket=%llu)",
        (unsigned long long)client_socket);
}


/*==========================================================
 * Mise à jour WebSocket
 *=========================================================*/

static void webserver_update_websocket(void)
{
    if (s_websocket_socket == INVALID_SOCKET)
    {
        return;
    }

    if (!websocket_server_is_connected())
    {
        LOG_DEBUG(
            "WEBSERVER",
            "WebSocket server reports disconnected");

        s_websocket_socket =
            INVALID_SOCKET;

        return;
    }

    /*
     * websocket_server_receive() doit être non-bloquant.
     *
     * Une absence de données n'est PAS une déconnexion.
     */

    if (!websocket_server_receive(
            s_websocket_socket))
    {
        /*
         * Ici on considère que websocket_server_receive()
         * retourne false uniquement en cas de vraie
         * déconnexion ou d'erreur fatale.
         */

        LOG_DEBUG(
            "WEBSERVER",
            "WebSocket connection lost "
            "(socket=%llu)",
            (unsigned long long)s_websocket_socket);

        websocket_server_close(
            s_websocket_socket);

        s_websocket_socket =
            INVALID_SOCKET;
    }
}


/*==========================================================
 * Mise à jour principale
 *=========================================================*/

void webserver_update(void)
{
    if (!s_running)
    {
        return;
    }

    if (s_server_socket == INVALID_SOCKET)
    {
        return;
    }

    /*
     * -----------------------------------------------------
     * 1. WebSocket existant
     * -----------------------------------------------------
     */

    if (s_websocket_socket != INVALID_SOCKET)
    {
        webserver_update_websocket();
    }

    /*
     * -----------------------------------------------------
     * 2. Nouvelle connexion TCP
     * -----------------------------------------------------
     */

    SOCKET client_socket =
        accept(
            s_server_socket,
            NULL,
            NULL);

    if (client_socket == INVALID_SOCKET)
    {
        int error =
            WSAGetLastError();

        if (error != WSAEWOULDBLOCK &&
            error != WSAEINPROGRESS)
        {
            LOG_ERROR(
                "WEBSERVER",
                "accept() failed (%d)",
                error);
        }

        return;
    }

    LOG_DEBUG(
        "WEBSERVER",
        "New TCP client accepted "
        "(socket=%llu)",
        (unsigned long long)client_socket);

    /*
     * -----------------------------------------------------
     * 3. Attendre les données initiales
     * -----------------------------------------------------
     */

    if (!webserver_wait_for_client_data(
            client_socket))
    {
        closesocket(
            client_socket);

        return;
    }

    /*
     * -----------------------------------------------------
     * 4. Détection HTTP / WebSocket
     * -----------------------------------------------------
     */

    if (webserver_is_websocket_request(
            client_socket))
    {
        /*
         * Une seule connexion WebSocket.
         */

        if (s_websocket_socket != INVALID_SOCKET)
        {
            LOG_DEBUG(
                "WEBSERVER",
                "Closing previous WebSocket client "
                "(socket=%llu)",
                (unsigned long long)s_websocket_socket);

            websocket_server_close(
                s_websocket_socket);

            s_websocket_socket =
                INVALID_SOCKET;
        }

        webserver_process_websocket_client(
            client_socket);

        return;
    }

    /*
     * -----------------------------------------------------
     * 5. HTTP classique
     * -----------------------------------------------------
     */

    /*
     * Le socket est actuellement disponible et les données
     * ont été confirmées par select().
     *
     * On peut donc traiter immédiatement la requête.
     */

    webserver_process_http_client(
        client_socket);
}


/*==========================================================
 * Arrêt
 *=========================================================*/

void webserver_stop(void)
{
    LOG_DEBUG(
        "WEBSERVER",
        "Stopping webserver");

    if (s_websocket_socket != INVALID_SOCKET)
    {
        websocket_server_close(
            s_websocket_socket);

        s_websocket_socket =
            INVALID_SOCKET;
    }

    if (s_server_socket != INVALID_SOCKET)
    {
        webserver_close_socket(
            s_server_socket);

        s_server_socket =
            INVALID_SOCKET;
    }

    if (s_winsock_initialized)
    {
        WSACleanup();

        s_winsock_initialized =
            false;
    }

    s_running =
        false;

    LOG_DEBUG(
        "WEBSERVER",
        "Webserver stopped");
}


/*==========================================================
 * Etat
 *=========================================================*/

bool webserver_is_running(void)
{
    return s_running;
}


#else

/*==========================================================
 * ESP32
 *=========================================================*/

bool webserver_init(void)
{
    return true;
}

bool webserver_start(void)
{
    return true;
}

void webserver_update(void)
{
}

void webserver_stop(void)
{
}

bool webserver_is_running(void)
{
    return false;
}

#endif
