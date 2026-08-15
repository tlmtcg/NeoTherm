#include "webserver.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#include "webserver_router.h"

#define WEBSERVER_PORT 8080
#define WEBSERVER_BUFFER_SIZE 2048
#define WEBSERVER_BACKLOG 4

/*==========================================================
 * Etat privé
 *=========================================================*/

static SOCKET s_server_socket = INVALID_SOCKET;

static bool s_running = false;

static bool s_winsock_initialized = false;

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
        printf(
            "Webserver: WSAStartup failed (%d)\n",
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
        printf(
            "Webserver: socket creation failed\n");

        WSACleanup();

        s_winsock_initialized =
            false;

        return false;
    }

    printf(
        "Webserver initialized\n");

    return true;
}

/*==========================================================
 * Configuration du socket
 *=========================================================*/

static bool webserver_configure_socket(void)
{
    u_long mode = 1;

    if (ioctlsocket(
            s_server_socket,
            FIONBIO,
            &mode) != 0)
    {
        printf(
            "Webserver: unable to set "
            "non-blocking mode\n");

        return false;
    }

    return true;
}

/*==========================================================
 * Création de l'adresse serveur
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

    /*
     * Simulateur PC :
     * accessible uniquement depuis localhost.
     */

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
        return false;
    }

    if (s_running)
    {
        return true;
    }

    struct sockaddr_in address;

    webserver_build_address(
        &address);

    /*
     * Bind.
     */

    if (bind(
            s_server_socket,
            (struct sockaddr *)&address,
            sizeof(address)) == SOCKET_ERROR)
    {
        printf(
            "Webserver: bind failed\n");

        webserver_stop();

        return false;
    }

    /*
     * Listen.
     */

    if (listen(
            s_server_socket,
            WEBSERVER_BACKLOG) == SOCKET_ERROR)
    {
        printf(
            "Webserver: listen failed\n");

        webserver_stop();

        return false;
    }

    /*
     * Socket non bloquant.
     */

    if (!webserver_configure_socket())
    {
        webserver_stop();

        return false;
    }

    s_running =
        true;

    printf(
        "Webserver listening on "
        "http://localhost:%d\n",
        WEBSERVER_PORT);

    return true;
}

/*==========================================================
 * Traitement d'une connexion
 *=========================================================*/

static void webserver_process_client(
    SOCKET client_socket)
{
    char buffer[WEBSERVER_BUFFER_SIZE];

    int received =
        recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            0);

    if (received <= 0)
    {
        closesocket(
            client_socket);

        return;
    }

    /*
     * Terminaison de la chaîne.
     */

    buffer[received] =
        '\0';

    /*
     * Le serveur TCP ne connaît pas
     * la logique HTTP.
     *
     * Il transmet simplement la requête
     * au routeur.
     */

    webserver_router_handle(
        client_socket,
        buffer);

    /*
     * Une requête = une connexion.
     */

    closesocket(
        client_socket);
}

/*==========================================================
 * Mise à jour
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
     * Socket serveur non bloquant.
     *
     * Si aucune connexion n'est disponible,
     * accept() retourne immédiatement.
     */

    SOCKET client_socket =
        accept(
            s_server_socket,
            NULL,
            NULL);

    if (client_socket == INVALID_SOCKET)
    {
        return;
    }

    webserver_process_client(
        client_socket);
}

/*==========================================================
 * Arrêt
 *=========================================================*/

void webserver_stop(void)
{
    if (s_server_socket != INVALID_SOCKET)
    {
        closesocket(
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

    printf(
        "Webserver stopped\n");
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

/*
 * L'implémentation ESP-IDF sera ajoutée ici.
 *
 * L'application conserve la même interface :
 *
 *     webserver_init()
 *     webserver_start()
 *     webserver_update()
 *     webserver_stop()
 *     webserver_is_running()
 */

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
