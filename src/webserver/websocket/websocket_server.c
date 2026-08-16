#include "websocket_server.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32

#include <winsock2.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

#include "logger.h"
#include "websocket_handshake.h"
#include "websocket_frame.h"

/*==========================================================
 * Etat
 *==========================================================*/

static bool s_initialized = false;
static bool s_connected = false;

static websocket_server_socket_t s_socket =
#ifdef _WIN32
    INVALID_SOCKET;
#else
    -1;
#endif

/*
 * Buffer TCP.
 *
 * Il contient les données reçues mais pas encore
 * complètement consommées par le décodeur WebSocket.
 */
static uint8_t s_receive_buffer[
    WEBSOCKET_SERVER_BUFFER_SIZE];

static size_t s_receive_length = 0;

/*==========================================================
 * Utilitaires
 *==========================================================*/

static bool websocket_server_socket_valid(
    websocket_server_socket_t socket)
{
#ifdef _WIN32

    return socket != INVALID_SOCKET;

#else

    return socket >= 0;

#endif
}

/*----------------------------------------------------------
 * Envoi complet
 *----------------------------------------------------------*/

static bool websocket_server_send_buffer(
    websocket_server_socket_t socket,
    const uint8_t *buffer,
    size_t length)
{
    size_t total = 0;

    if (!websocket_server_socket_valid(socket))
    {
        return false;
    }

    if (buffer == NULL ||
        length == 0)
    {
        return false;
    }

    while (total < length)
    {
#ifdef _WIN32

        int sent =
            send(
                socket,
                (const char *)buffer + total,
                (int)(length - total),
                0);

#else

        ssize_t sent =
            send(
                socket,
                buffer + total,
                length - total,
                0);

#endif

        if (sent <= 0)
        {
            LOG_ERROR(
                "WEBSOCKET_SERVER",
                "Socket send failed");

            return false;
        }

        total +=
            (size_t)sent;
    }

    return true;
}

/*----------------------------------------------------------
 * Réception TCP
 *----------------------------------------------------------*/

static int websocket_server_receive_data(
    websocket_server_socket_t socket,
    uint8_t *buffer,
    size_t buffer_size)
{
#ifdef _WIN32

    return recv(
        socket,
        (char *)buffer,
        (int)buffer_size,
        0);

#else

    return (int)recv(
        socket,
        buffer,
        buffer_size,
        0);

#endif
}

/*----------------------------------------------------------
 * Reset du buffer
 *----------------------------------------------------------*/

static void websocket_server_reset_receive_buffer(void)
{
    s_receive_length = 0;

    memset(
        s_receive_buffer,
        0,
        sizeof(s_receive_buffer));
}

/*==========================================================
 * Traitement TEXT
 *==========================================================*/

static void websocket_server_handle_text(
    const websocket_frame_t *frame)
{
    char text[
        WEBSOCKET_SERVER_BUFFER_SIZE];

    size_t length;

    if (frame == NULL)
    {
        return;
    }

    length =
        (size_t)frame->payload_length;

    if (length >= sizeof(text))
    {
        LOG_WARN(
            "WEBSOCKET_SERVER",
            "TEXT frame too large");

        return;
    }

    memcpy(
        text,
        frame->payload,
        length);

    text[length] =
        '\0';

    LOG_INFO(
        "WEBSOCKET_SERVER",
        "TEXT received: %s",
        text);

    /*
     * C'est ici que nous brancherons ensuite
     * le traitement JSON de l'interface Web.
     */
}

/*==========================================================
 * Traitement d'une trame
 *==========================================================*/

static bool websocket_server_handle_frame(
    websocket_server_socket_t socket,
    websocket_frame_t *frame)
{
    if (frame == NULL)
    {
        return true;
    }

    switch (frame->opcode)
    {
        /*--------------------------------------------------
         * TEXT
         *--------------------------------------------------*/

        case WEBSOCKET_OPCODE_TEXT:
        {
            websocket_server_handle_text(
                frame);

            break;
        }

        /*--------------------------------------------------
         * PING
         *--------------------------------------------------*/

        case WEBSOCKET_OPCODE_PING:
        {
            LOG_DEBUG(
                "WEBSOCKET_SERVER",
                "PING received");

            if (!websocket_server_send_pong(
                    socket,
                    frame->payload,
                    (size_t)frame->payload_length))
            {
                return false;
            }

            break;
        }

        /*--------------------------------------------------
         * PONG
         *--------------------------------------------------*/

        case WEBSOCKET_OPCODE_PONG:
        {
            LOG_DEBUG(
                "WEBSOCKET_SERVER",
                "PONG received");

            break;
        }

        /*--------------------------------------------------
         * CLOSE
         *--------------------------------------------------*/

        case WEBSOCKET_OPCODE_CLOSE:
        {
            LOG_INFO(
                "WEBSOCKET_SERVER",
                "CLOSE received");

            /*
             * Répondre avec CLOSE.
             */
            websocket_server_send_close(
                socket,
                1000,
                "Normal closure");

            return false;
        }

        /*--------------------------------------------------
         * BINARY
         *--------------------------------------------------*/

        case WEBSOCKET_OPCODE_BINARY:
        {
            LOG_WARN(
                "WEBSOCKET_SERVER",
                "BINARY frame ignored");

            break;
        }

        /*--------------------------------------------------
         * CONTINUATION
         *--------------------------------------------------*/

        case WEBSOCKET_OPCODE_CONTINUATION:
        {
            LOG_WARN(
                "WEBSOCKET_SERVER",
                "Continuation frame not supported");

            break;
        }

        /*--------------------------------------------------
         * Inconnu
         *--------------------------------------------------*/

        default:
        {
            LOG_WARN(
                "WEBSOCKET_SERVER",
                "Unknown opcode: 0x%02X",
                frame->opcode);

            break;
        }
    }

    return true;
}

/*==========================================================
 * Traitement du buffer
 *==========================================================*/

static bool websocket_server_process_buffer(
    websocket_server_socket_t socket)
{
    while (s_receive_length > 0)
    {
        websocket_frame_t frame;

        size_t consumed = 0;

        /*
         * Tentative de décodage.
         */
        if (!websocket_frame_decode(
                s_receive_buffer,
                s_receive_length,
                &frame,
                &consumed))
        {
            /*
             * Deux possibilités :
             *
             * 1. La trame est incomplète.
             * 2. La trame est invalide.
             *
             * Le décodeur actuel ne distingue pas les deux.
             *
             * On conserve donc les données et on attend
             * davantage de données TCP.
             */
            return true;
        }

        /*
         * Protection contre un décodeur qui retournerait
         * une longueur incohérente.
         */
        if (consumed == 0 ||
            consumed > s_receive_length)
        {
            LOG_ERROR(
                "WEBSOCKET_SERVER",
                "Invalid consumed length");

            return false;
        }

        /*
         * Traiter la trame.
         */
        if (!websocket_server_handle_frame(
                socket,
                &frame))
        {
            return false;
        }

        /*
         * Retirer la trame du buffer.
         *
         * Les éventuelles trames suivantes sont déplacées
         * au début du buffer.
         */
        size_t remaining =
            s_receive_length - consumed;

        if (remaining > 0)
        {
            memmove(
                s_receive_buffer,
                &s_receive_buffer[consumed],
                remaining);
        }

        s_receive_length =
            remaining;
    }

    return true;
}

/*==========================================================
 * Initialisation
 *==========================================================*/

bool websocket_server_init(void)
{
    if (s_initialized)
    {
        return true;
    }

    s_connected = false;

#ifdef _WIN32

    s_socket = INVALID_SOCKET;

#else

    s_socket = -1;

#endif

    websocket_server_reset_receive_buffer();

    s_initialized = true;

    LOG_INFO(
        "WEBSOCKET_SERVER",
        "WebSocket server initialized");

    return true;
}

/*==========================================================
 * Shutdown
 *==========================================================*/

void websocket_server_shutdown(void)
{
    if (s_connected)
    {
        websocket_server_close(
            s_socket);
    }

    websocket_server_reset_receive_buffer();

#ifdef _WIN32

    s_socket = INVALID_SOCKET;

#else

    s_socket = -1;

#endif

    s_initialized = false;

    LOG_INFO(
        "WEBSOCKET_SERVER",
        "WebSocket server shutdown");
}

/*==========================================================
 * Acceptation
 *==========================================================*/

bool websocket_server_accept(
    websocket_server_socket_t socket)
{
    if (!s_initialized)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Server not initialized");

        return false;
    }

    if (!websocket_server_socket_valid(socket))
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Invalid socket");

        return false;
    }

    /*
     * Si une autre connexion WebSocket était active,
     * on la ferme.
     */
    if (s_connected)
    {
        websocket_server_close(
            s_socket);
    }

    /*
     * Le socket TCP est déjà connecté.
     *
     * Effectuer maintenant le handshake WebSocket.
     */
    if (!websocket_handshake(socket))
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "WebSocket handshake failed");

        return false;
    }

    s_socket =
        socket;

    s_connected =
        true;

    websocket_server_reset_receive_buffer();

    LOG_INFO(
        "WEBSOCKET_SERVER",
        "WebSocket client connected");

    return true;
}

/*==========================================================
 * Fermeture
 *==========================================================*/

void websocket_server_close(
    websocket_server_socket_t socket)
{
    if (!websocket_server_socket_valid(socket))
    {
        return;
    }

    /*
     * Envoyer CLOSE si la connexion est encore active.
     */
    if (s_connected)
    {
        websocket_server_send_close(
            socket,
            1000,
            "Normal closure");
    }

#ifdef _WIN32

    closesocket(socket);

#else

    close(socket);

#endif

    if (socket == s_socket)
    {
#ifdef _WIN32

        s_socket = INVALID_SOCKET;

#else

        s_socket = -1;

#endif

        s_connected = false;

        websocket_server_reset_receive_buffer();
    }

    LOG_INFO(
        "WEBSOCKET_SERVER",
        "WebSocket client disconnected");
}

/*==========================================================
 * Etat
 *==========================================================*/

bool websocket_server_is_connected(void)
{
    return s_connected;
}

/*==========================================================
 * Réception
 *==========================================================*/

bool websocket_server_receive(
    websocket_server_socket_t socket)
{
    uint8_t temporary[
        WEBSOCKET_SERVER_BUFFER_SIZE];

    int received;

    if (!s_initialized ||
        !s_connected)
    {
        return false;
    }

    if (socket != s_socket)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Socket does not match active client");

        return false;
    }

    /*
     * Espace disponible dans le buffer d'accumulation.
     */
    size_t available =
        sizeof(s_receive_buffer) -
        s_receive_length;

    if (available == 0)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "WebSocket receive buffer full");

        return false;
    }

    /*
     * Lecture TCP.
     */
    received =
        websocket_server_receive_data(
            socket,
            temporary,
            sizeof(temporary));

    if (received <= 0)
    {
        LOG_INFO(
            "WEBSOCKET_SERVER",
            "WebSocket client disconnected");

        websocket_server_close(
            socket);

        return false;
    }

    LOG_DEBUG(
        "WEBSOCKET_SERVER",
        "Received %d bytes",
        received);

    /*
     * Protection contre un dépassement.
     */
    if ((size_t)received > available)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "WebSocket receive buffer overflow");

        websocket_server_close(
            socket);

        return false;
    }

    /*
     * Ajouter les données au buffer d'accumulation.
     */
    memcpy(
        &s_receive_buffer[s_receive_length],
        temporary,
        (size_t)received);

    s_receive_length +=
        (size_t)received;

    /*
     * Traiter autant de trames complètes que possible.
     */
    if (!websocket_server_process_buffer(
            socket))
    {
        websocket_server_close(
            socket);

        return false;
    }

    return s_connected;
}

/*==========================================================
 * Envoi TEXT
 *==========================================================*/

bool websocket_server_send_text(
    websocket_server_socket_t socket,
    const char *json)
{
    uint8_t buffer[
        WEBSOCKET_SERVER_BUFFER_SIZE];

    size_t encoded;

    if (!s_connected ||
        socket != s_socket)
    {
        return false;
    }

    if (json == NULL)
    {
        return false;
    }

    if (!websocket_frame_encode_text(
            json,
            strlen(json),
            buffer,
            sizeof(buffer),
            &encoded))
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Failed to encode TEXT frame");

        return false;
    }

    return websocket_server_send_buffer(
        socket,
        buffer,
        encoded);
}

/*==========================================================
 * Envoi PING
 *==========================================================*/

bool websocket_server_send_ping(
    websocket_server_socket_t socket)
{
    uint8_t buffer[256];

    size_t encoded;

    if (!s_connected ||
        socket != s_socket)
    {
        return false;
    }

    if (!websocket_frame_encode_ping(
            NULL,
            0,
            buffer,
            sizeof(buffer),
            &encoded))
    {
        return false;
    }

    return websocket_server_send_buffer(
        socket,
        buffer,
        encoded);
}

/*==========================================================
 * Envoi PONG
 *==========================================================*/

bool websocket_server_send_pong(
    websocket_server_socket_t socket,
    const uint8_t *payload,
    size_t length)
{
    uint8_t buffer[
        WEBSOCKET_SERVER_BUFFER_SIZE];

    size_t encoded;

    if (!s_connected ||
        socket != s_socket)
    {
        return false;
    }

    if (!websocket_frame_encode_pong(
            payload,
            length,
            buffer,
            sizeof(buffer),
            &encoded))
    {
        return false;
    }

    return websocket_server_send_buffer(
        socket,
        buffer,
        encoded);
}

/*==========================================================
 * Envoi CLOSE
 *==========================================================*/

bool websocket_server_send_close(
    websocket_server_socket_t socket,
    uint16_t status_code,
    const char *reason)
{
    uint8_t buffer[256];

    size_t encoded;

    if (!websocket_server_socket_valid(socket))
    {
        return false;
    }

    if (!websocket_frame_encode_close(
            status_code,
            reason,
            buffer,
            sizeof(buffer),
            &encoded))
    {
        return false;
    }

    return websocket_server_send_buffer(
        socket,
        buffer,
        encoded);
}
