#include "websocket_server.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32

#include <winsock2.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#endif

#include "logger.h"
#include "websocket_handshake.h"
#include "websocket_frame.h"
#include "websocket_api.h"

/*==========================================================
 * Etat
 *=========================================================*/

static bool s_initialized = false;
static bool s_connected = false;

/*
 * Un seul client WebSocket actif.
 */
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
static uint8_t s_receive_buffer[WEBSOCKET_SERVER_BUFFER_SIZE];

static size_t s_receive_length = 0;

/*==========================================================
 * Utilitaires
 *=========================================================*/

/*----------------------------------------------------------
 * Vérification socket
 *---------------------------------------------------------*/

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
 * Fermeture bas niveau
 *---------------------------------------------------------*/

static void websocket_server_socket_close(
    websocket_server_socket_t socket)
{
    if (!websocket_server_socket_valid(socket))
    {
        return;
    }

#ifdef _WIN32

    closesocket(socket);

#else

    close(socket);

#endif
}

/*----------------------------------------------------------
 * Reset état socket
 *---------------------------------------------------------*/

static void websocket_server_reset_socket(void)
{
#ifdef _WIN32

    s_socket = INVALID_SOCKET;

#else

    s_socket = -1;

#endif

    s_connected = false;
}

/*----------------------------------------------------------
 * Socket non bloquant
 *---------------------------------------------------------*/

static bool websocket_server_set_nonblocking(
    websocket_server_socket_t socket)
{
#ifdef _WIN32

    u_long mode = 1;

    if (ioctlsocket(
            socket,
            FIONBIO,
            &mode) != 0)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Unable to set socket non-blocking");

        return false;
    }

#else

    int flags =
        fcntl(
            socket,
            F_GETFL,
            0);

    if (flags < 0)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Unable to get socket flags");

        return false;
    }

    if (fcntl(
            socket,
            F_SETFL,
            flags | O_NONBLOCK) < 0)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Unable to set socket non-blocking");

        return false;
    }

#endif

    return true;
}

/*----------------------------------------------------------
 * Envoi d'une trame
 *
 * IMPORTANT :
 *
 * Cette fonction est utilisée sur un socket non bloquant.
 *
 * Pour notre architecture actuelle, les trames envoyées
 * sont petites et nous considérons EWOULDBLOCK comme un
 * échec d'envoi.
 *
 * Si nous voulons supporter des envois volumineux ou une
 * forte charge, il faudra ajouter un buffer TX.
 *---------------------------------------------------------*/

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

        int remaining =
            (int)(length - total);

        int sent =
            send(
                socket,
                (const char *)buffer + total,
                remaining,
                0);

        if (sent == SOCKET_ERROR)
        {
            int error =
                WSAGetLastError();

            if (error == WSAEWOULDBLOCK)
            {
                LOG_WARN(
                    "WEBSOCKET_SERVER",
                    "Socket send would block");

                return false;
            }

            LOG_ERROR(
                "WEBSOCKET_SERVER",
                "Socket send failed (%d)",
                error);

            return false;
        }

#else

        int flags = 0;

#ifdef MSG_NOSIGNAL
        flags |= MSG_NOSIGNAL;
#endif

        ssize_t sent =
            send(
                socket,
                buffer + total,
                length - total,
                flags);

        if (sent < 0)
        {
            if (errno == EAGAIN ||
                errno == EWOULDBLOCK)
            {
                LOG_WARN(
                    "WEBSOCKET_SERVER",
                    "Socket send would block");

                return false;
            }

            LOG_ERROR(
                "WEBSOCKET_SERVER",
                "Socket send failed: %s",
                strerror(errno));

            return false;
        }

#endif

        if (sent == 0)
        {
            LOG_ERROR(
                "WEBSOCKET_SERVER",
                "Socket send returned zero");

            return false;
        }

        total +=
            (size_t)sent;
    }

    return true;
}

/*----------------------------------------------------------
 * Réception TCP
 *---------------------------------------------------------*/

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
 * Reset buffer RX
 *---------------------------------------------------------*/

static void websocket_server_reset_receive_buffer(void)
{
    s_receive_length = 0;
}

/*==========================================================
 * Traitement TEXT
 *=========================================================*/

static bool websocket_server_handle_text(
    const websocket_frame_t *frame)
{
    char text[
        WEBSOCKET_SERVER_BUFFER_SIZE];

    size_t length;

    (void)socket;

    if (frame == NULL)
    {
        return false;
    }

    if (!frame->fin)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Fragmented TEXT frame not supported");

        return false;
    }

    if (frame->payload_length >
        (uint64_t)(sizeof(text) - 1U))
    {
        LOG_WARN(
            "WEBSOCKET_SERVER",
            "TEXT frame too large");

        return false;
    }

    length =
        (size_t)frame->payload_length;

    if (length > 0 &&
        frame->payload == NULL)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "TEXT frame has NULL payload");

        return false;
    }

    if (length > 0)
    {
        memcpy(
            text,
            frame->payload,
            length);
    }

    text[length] =
        '\0';

    LOG_INFO(
        "WEBSOCKET_SERVER",
        "TEXT received: %s",
        text);

    /*
     * Le transport WebSocket s'arrête ici.
     *
     * Le contenu JSON est transmis à websocket_api.
     */
    if (!websocket_api_handle(text))
    {
        LOG_WARN(
            "WEBSOCKET_SERVER",
            "WebSocket request not handled");
    }

    /*
     * Une requête applicative invalide ne doit pas
     * provoquer automatiquement la fermeture du WebSocket.
     */
    return true;
}

/*==========================================================
 * Traitement d'une trame
 *=========================================================*/

static bool websocket_server_handle_frame(
    websocket_server_socket_t socket,
    websocket_frame_t *frame)
{
    if (frame == NULL)
    {
        return false;
    }

    /*
     * Les trames de données fragmentées ne sont pas encore
     * supportées.
     *
     * Les trames de contrôle doivent toujours avoir FIN=1.
     */
    switch (frame->opcode)
    {
        /*--------------------------------------------------
         * TEXT
         *--------------------------------------------------*/

    case WEBSOCKET_OPCODE_TEXT:
    {
        if (!frame->fin)
        {
            LOG_ERROR(
                "WEBSOCKET_SERVER",
                "Fragmented TEXT frame not supported");

            return false;
        }

        if (!websocket_server_handle_text(frame))
        {
            return false;
        }

        break;
    }

        /*--------------------------------------------------
         * BINARY
         *--------------------------------------------------*/

    case WEBSOCKET_OPCODE_BINARY:
    {
        if (!frame->fin)
        {
            LOG_ERROR(
                "WEBSOCKET_SERVER",
                "Fragmented BINARY frame not supported");

            return false;
        }

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
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Continuation frame not supported");

        return false;
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
            LOG_ERROR(
                "WEBSOCKET_SERVER",
                "Unable to send PONG");

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
         * Réponse CLOSE.
         *
         * On ferme ensuite directement le socket.
         * websocket_server_receive() ne rappellera donc
         * pas websocket_server_close().
         */
        if (!websocket_server_send_close(
                socket,
                1000,
                "Normal closure"))
        {
            LOG_WARN(
                "WEBSOCKET_SERVER",
                "Unable to send CLOSE response");
        }

        websocket_server_socket_close(
            socket);

        if (socket == s_socket)
        {
            websocket_server_reset_socket();
            websocket_server_reset_receive_buffer();
        }

        return false;
    }

        /*--------------------------------------------------
         * Opcode inconnu
         *--------------------------------------------------*/

    default:
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Unknown opcode: 0x%02X",
            frame->opcode);

        return false;
    }
    }

    return true;
}

/*==========================================================
 * Traitement du buffer
 *=========================================================*/

static bool websocket_server_process_buffer(
    websocket_server_socket_t socket)
{
    while (s_receive_length > 0)
    {
        websocket_frame_t frame;

        size_t consumed = 0;

        /*
         * Tentative de décodage.
         *
         * ATTENTION :
         *
         * Avec l'API actuelle de websocket_frame_decode(),
         * false peut signifier :
         *
         * - trame incomplète
         * - trame invalide
         *
         * Nous ne pouvons donc pas distinguer les deux ici.
         *
         * La prochaine amélioration sera de faire retourner
         * un statut explicite au décodeur.
         */
        if (!websocket_frame_decode(
                s_receive_buffer,
                s_receive_length,
                &frame,
                &consumed))
        {
            return true;
        }

        if (consumed == 0 ||
            consumed > s_receive_length)
        {
            LOG_ERROR(
                "WEBSOCKET_SERVER",
                "Invalid consumed length");

            return false;
        }

        if (!websocket_server_handle_frame(
                socket,
                &frame))
        {
            return false;
        }

        /*
         * Si le traitement a fermé le socket, inutile
         * de continuer.
         */
        if (!s_connected)
        {
            return false;
        }

        /*
         * Retirer la trame consommée.
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
 *=========================================================*/

bool websocket_server_init(void)
{
    if (s_initialized)
    {
        return true;
    }

    websocket_server_reset_socket();

    websocket_server_reset_receive_buffer();

    s_initialized = true;

    LOG_INFO(
        "WEBSOCKET_SERVER",
        "WebSocket server initialized");

    return true;
}

/*==========================================================
 * Shutdown
 *=========================================================*/

void websocket_server_shutdown(void)
{
    if (s_connected)
    {
        websocket_server_close(
            s_socket);
    }

    websocket_server_reset_receive_buffer();
    websocket_server_reset_socket();

    s_initialized = false;

    LOG_INFO(
        "WEBSOCKET_SERVER",
        "WebSocket server shutdown");
}

/*==========================================================
 * Acceptation
 *=========================================================*/

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
     * Une seule connexion WebSocket active.
     */
    if (s_connected)
    {
        websocket_server_close(
            s_socket);
    }

    /*
     * Le socket est encore bloquant.
     *
     * Le handshake utilise recv() de manière bloquante.
     */
    if (!websocket_handshake(socket))
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "WebSocket handshake failed");

        websocket_server_socket_close(
            socket);

        return false;
    }

    /*
     * Handshake terminé.
     *
     * Passage en non-bloquant.
     */
    if (!websocket_server_set_nonblocking(
            socket))
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Failed to configure WebSocket socket");

        websocket_server_socket_close(
            socket);

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
 *=========================================================*/

void websocket_server_close(
    websocket_server_socket_t socket)
{
    bool active_socket;

    if (!websocket_server_socket_valid(socket))
    {
        return;
    }

    active_socket =
        s_connected &&
        socket == s_socket;

    if (active_socket)
    {
        /*
         * On tente un CLOSE mais la fermeture locale
         * reste prioritaire.
         */
        if (!websocket_server_send_close(
                socket,
                1000,
                "Normal closure"))
        {
            LOG_DEBUG(
                "WEBSOCKET_SERVER",
                "Unable to send CLOSE frame");
        }
    }

    websocket_server_socket_close(
        socket);

    if (active_socket)
    {
        websocket_server_reset_socket();
        websocket_server_reset_receive_buffer();
    }

    LOG_INFO(
        "WEBSOCKET_SERVER",
        "WebSocket client disconnected");
}

/*==========================================================
 * Etat
 *=========================================================*/

bool websocket_server_is_connected(void)
{
    return s_connected;
}

/*==========================================================
 * Réception
 *=========================================================*/

bool websocket_server_receive(
    websocket_server_socket_t socket)
{
    uint8_t temporary[WEBSOCKET_SERVER_BUFFER_SIZE];

    int received;

    size_t available;

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

    available =
        sizeof(s_receive_buffer) -
        s_receive_length;

    if (available == 0)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "WebSocket receive buffer full");

        websocket_server_close(
            socket);

        return false;
    }

    /*
     * Ne jamais demander à recv() plus que la place
     * restante dans notre buffer.
     */
    received =
        websocket_server_receive_data(
            socket,
            temporary,
            available);

#ifdef _WIN32

    if (received == SOCKET_ERROR)
    {
        int error =
            WSAGetLastError();

        if (error == WSAEWOULDBLOCK)
        {
            return true;
        }

        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Socket receive failed (%d)",
            error);

        websocket_server_close(
            socket);

        return false;
    }

#else

    if (received < 0)
    {
        if (errno == EAGAIN ||
            errno == EWOULDBLOCK)
        {
            return true;
        }

        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "Socket receive failed: %s",
            strerror(errno));

        websocket_server_close(
            socket);

        return false;
    }

#endif

    /*
     * Fermeture TCP.
     */
    if (received == 0)
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
     * Protection supplémentaire.
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

    memcpy(
        &s_receive_buffer[s_receive_length],
        temporary,
        (size_t)received);

    s_receive_length +=
        (size_t)received;

    /*
     * Traiter toutes les trames complètes.
     */
    if (!websocket_server_process_buffer(
            socket))
    {
        /*
         * Le CLOSE a déjà pu fermer le socket.
         */
        if (s_connected)
        {
            websocket_server_close(
                socket);
        }

        return false;
    }

    return s_connected;
}

/*==========================================================
 * Envoi TEXT
 *=========================================================*/

bool websocket_server_send_text(
    websocket_server_socket_t socket,
    const char *json)
{
    uint8_t buffer[WEBSOCKET_SERVER_BUFFER_SIZE];

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
 *=========================================================*/

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
 *=========================================================*/

bool websocket_server_send_pong(
    websocket_server_socket_t socket,
    const uint8_t *payload,
    size_t length)
{
    uint8_t buffer[WEBSOCKET_SERVER_BUFFER_SIZE];

    size_t encoded;

    if (!s_connected ||
        socket != s_socket)
    {
        return false;
    }

    if (length > 0 &&
        payload == NULL)
    {
        return false;
    }

    if (length > 125U)
    {
        LOG_ERROR(
            "WEBSOCKET_SERVER",
            "PONG payload too large");

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
 *=========================================================*/

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

bool websocket_server_send_text_active(
    const char *json)
{
    if (!s_connected)
    {
        return false;
    }

    return websocket_server_send_text(
        s_socket,
        json);
}
