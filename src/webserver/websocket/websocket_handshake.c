#include "websocket_handshake.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#ifdef _WIN32

#include <winsock2.h>

#define strcasecmp  _stricmp
#define strncasecmp _strnicmp

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#endif

#include "logger.h"

/*==========================================================
 * Constantes WebSocket
 *=========================================================*/

#define WEBSOCKET_GUID \
    "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

#define WEBSOCKET_VERSION "13"

#define WS_ACCEPT_SIZE 29U

/*
 * Une clé Sec-WebSocket-Key valide est un Base64
 * représentant 16 octets.
 *
 * Taille Base64 :
 *
 *     16 octets -> 24 caractères
 */
#define WEBSOCKET_KEY_LENGTH 24U

/*==========================================================
 * SHA-1
 *==========================================================*/

typedef struct
{
    uint32_t state[5];

    uint64_t bit_count;

    uint8_t buffer[64];

} sha1_context_t;

/*----------------------------------------------------------
 * Rotation gauche
 *---------------------------------------------------------*/

static uint32_t sha1_rotate_left(
    uint32_t value,
    uint32_t bits)
{
    return
        (value << bits) |
        (value >> (32U - bits));
}

/*----------------------------------------------------------
 * Transformation SHA-1
 *---------------------------------------------------------*/

static void sha1_transform(
    sha1_context_t *ctx,
    const uint8_t data[64])
{
    uint32_t w[80];

    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;

    uint32_t f;
    uint32_t k;
    uint32_t temp;

    int i;

    for (i = 0; i < 16; ++i)
    {
        w[i] =
            ((uint32_t)data[i * 4] << 24) |
            ((uint32_t)data[i * 4 + 1] << 16) |
            ((uint32_t)data[i * 4 + 2] << 8) |
            ((uint32_t)data[i * 4 + 3]);
    }

    for (i = 16; i < 80; ++i)
    {
        w[i] =
            sha1_rotate_left(
                w[i - 3] ^
                w[i - 8] ^
                w[i - 14] ^
                w[i - 16],
                1U);
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];

    for (i = 0; i < 80; ++i)
    {
        if (i < 20)
        {
            f =
                (b & c) |
                ((~b) & d);

            k = 0x5A827999U;
        }
        else if (i < 40)
        {
            f =
                b ^ c ^ d;

            k = 0x6ED9EBA1U;
        }
        else if (i < 60)
        {
            f =
                (b & c) |
                (b & d) |
                (c & d);

            k = 0x8F1BBCDCU;
        }
        else
        {
            f =
                b ^ c ^ d;

            k = 0xCA62C1D6U;
        }

        temp =
            sha1_rotate_left(a, 5U) +
            f +
            e +
            k +
            w[i];

        e = d;
        d = c;
        c = sha1_rotate_left(b, 30U);
        b = a;
        a = temp;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
}

/*----------------------------------------------------------
 * SHA-1 init
 *---------------------------------------------------------*/

static void sha1_init(
    sha1_context_t *ctx)
{
    memset(
        ctx,
        0,
        sizeof(*ctx));

    ctx->state[0] = 0x67452301U;
    ctx->state[1] = 0xEFCDAB89U;
    ctx->state[2] = 0x98BADCFEU;
    ctx->state[3] = 0x10325476U;
    ctx->state[4] = 0xC3D2E1F0U;
}

/*----------------------------------------------------------
 * SHA-1 update
 *---------------------------------------------------------*/

static void sha1_update(
    sha1_context_t *ctx,
    const uint8_t *data,
    size_t length)
{
    size_t index;
    size_t part_length;

    if (length == 0U)
    {
        return;
    }

    index =
        (size_t)((ctx->bit_count / 8U) % 64U);

    ctx->bit_count +=
        (uint64_t)length * 8U;

    part_length =
        64U - index;

    if (length >= part_length)
    {
        memcpy(
            &ctx->buffer[index],
            data,
            part_length);

        sha1_transform(
            ctx,
            ctx->buffer);

        for (
            size_t i = part_length;
            i + 63U < length;
            i += 64U)
        {
            sha1_transform(
                ctx,
                &data[i]);
        }

        index = 0U;

        data +=
            part_length;

        length -=
            part_length;
    }

    if (length > 0U)
    {
        memcpy(
            &ctx->buffer[index],
            data,
            length);
    }
}

/*----------------------------------------------------------
 * SHA-1 final
 *---------------------------------------------------------*/

static void sha1_final(
    sha1_context_t *ctx,
    uint8_t digest[20])
{
    uint8_t padding[64];

    uint8_t length_bytes[8];

    size_t index;
    size_t padding_length;

    uint64_t bit_count;

    memset(
        padding,
        0,
        sizeof(padding));

    padding[0] =
        0x80U;

    index =
        (size_t)((ctx->bit_count / 8U) % 64U);

    if (index < 56U)
    {
        padding_length =
            56U - index;
    }
    else
    {
        padding_length =
            120U - index;
    }

    bit_count =
        ctx->bit_count;

    sha1_update(
        ctx,
        padding,
        padding_length);

    length_bytes[0] =
        (uint8_t)(bit_count >> 56);

    length_bytes[1] =
        (uint8_t)(bit_count >> 48);

    length_bytes[2] =
        (uint8_t)(bit_count >> 40);

    length_bytes[3] =
        (uint8_t)(bit_count >> 32);

    length_bytes[4] =
        (uint8_t)(bit_count >> 24);

    length_bytes[5] =
        (uint8_t)(bit_count >> 16);

    length_bytes[6] =
        (uint8_t)(bit_count >> 8);

    length_bytes[7] =
        (uint8_t)bit_count;

    sha1_update(
        ctx,
        length_bytes,
        sizeof(length_bytes));

    for (int i = 0; i < 5; ++i)
    {
        digest[i * 4] =
            (uint8_t)(ctx->state[i] >> 24);

        digest[i * 4 + 1] =
            (uint8_t)(ctx->state[i] >> 16);

        digest[i * 4 + 2] =
            (uint8_t)(ctx->state[i] >> 8);

        digest[i * 4 + 3] =
            (uint8_t)ctx->state[i];
    }
}

/*==========================================================
 * Base64
 *=========================================================*/

static const char s_base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

/*----------------------------------------------------------
 * Base64 encode
 *---------------------------------------------------------*/

static bool base64_encode(
    const uint8_t *input,
    size_t input_length,
    char *output,
    size_t output_size)
{
    size_t required;

    size_t i = 0U;
    size_t o = 0U;

    if (input == NULL ||
        output == NULL)
    {
        return false;
    }

    /*
     * Protection contre overflow.
     */
    if (input_length >
        (SIZE_MAX - 3U) / 4U * 3U)
    {
        return false;
    }

    required =
        ((input_length + 2U) / 3U) * 4U + 1U;

    if (output_size < required)
    {
        return false;
    }

    while (i < input_length)
    {
        uint32_t value = 0U;

        size_t remaining =
            input_length - i;

        value |=
            (uint32_t)input[i] << 16;

        if (remaining > 1U)
        {
            value |=
                (uint32_t)input[i + 1U] << 8;
        }

        if (remaining > 2U)
        {
            value |=
                input[i + 2U];
        }

        output[o++] =
            s_base64_table[
                (value >> 18U) & 0x3FU];

        output[o++] =
            s_base64_table[
                (value >> 12U) & 0x3FU];

        if (remaining > 1U)
        {
            output[o++] =
                s_base64_table[
                    (value >> 6U) & 0x3FU];
        }
        else
        {
            output[o++] =
                '=';
        }

        if (remaining > 2U)
        {
            output[o++] =
                s_base64_table[
                    value & 0x3FU];
        }
        else
        {
            output[o++] =
                '=';
        }

        i += 3U;
    }

    output[o] =
        '\0';

    return true;
}

/*==========================================================
 * Réception HTTP
 *=========================================================*/

/**
 * Reçoit la requête HTTP du handshake.
 *
 * Important :
 *
 * On s'arrête dès que \r\n\r\n est détecté.
 *
 * Le handshake actuel ne conserve volontairement
 * pas d'octets WebSocket supplémentaires.
 *
 * Le navigateur envoie normalement la requête HTTP
 * séparément du premier frame WebSocket.
 */
static bool websocket_receive_http_request(
    websocket_socket_t socket,
    char *buffer,
    size_t buffer_size)
{
    size_t total = 0U;

    if (buffer == NULL ||
        buffer_size < 4U)
    {
        return false;
    }

    while (total < buffer_size - 1U)
    {
#ifdef _WIN32

        int received =
            recv(
                socket,
                buffer + total,
                (int)(buffer_size - total - 1U),
                0);

#else

        ssize_t received =
            recv(
                socket,
                buffer + total,
                buffer_size - total - 1U,
                0);

#endif

#ifdef _WIN32

        if (received == SOCKET_ERROR)
        {
            int error =
                WSAGetLastError();

            LOG_ERROR(
                "WEBSOCKET_HANDSHAKE",
                "Failed to receive HTTP request (%d)",
                error);

            return false;
        }

#else

        if (received < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            LOG_ERROR(
                "WEBSOCKET_HANDSHAKE",
                "Failed to receive HTTP request: %s",
                strerror(errno));

            return false;
        }

#endif

        if (received == 0)
        {
            LOG_ERROR(
                "WEBSOCKET_HANDSHAKE",
                "Client closed connection during handshake");

            return false;
        }

        total +=
            (size_t)received;

        buffer[total] =
            '\0';

        /*
         * Fin des headers HTTP.
         */
        if (strstr(
                buffer,
                "\r\n\r\n") != NULL)
        {
            return true;
        }
    }

    LOG_ERROR(
        "WEBSOCKET_HANDSHAKE",
        "HTTP request too large");

    return false;
}

/*==========================================================
 * Recherche d'un header HTTP
 *=========================================================*/

static bool websocket_get_header(
    const char *request,
    const char *name,
    char *value,
    size_t value_size)
{
    const char *line =
        request;

    size_t name_length;

    if (request == NULL ||
        name == NULL ||
        value == NULL ||
        value_size == 0U)
    {
        return false;
    }

    name_length =
        strlen(name);

    while (line != NULL &&
           *line != '\0')
    {
        const char *line_end =
            strstr(
                line,
                "\r\n");

        if (line_end == NULL)
        {
            break;
        }

        /*
         * Ne pas considérer la ligne GET
         * comme un header.
         */
        if (line != request)
        {
            const char *colon =
                strchr(
                    line,
                    ':');

            if (colon != NULL &&
                colon < line_end)
            {
                size_t header_name_length =
                    (size_t)(colon - line);

                if (header_name_length ==
                        name_length &&
                    strncasecmp(
                        line,
                        name,
                        name_length) == 0)
                {
                    const char *start =
                        colon + 1;

                    while (
                        start < line_end &&
                        isspace(
                            (unsigned char)*start))
                    {
                        ++start;
                    }

                    size_t length =
                        (size_t)(
                            line_end - start);

                    while (
                        length > 0U &&
                        isspace(
                            (unsigned char)
                                start[length - 1U]))
                    {
                        --length;
                    }

                    if (length >= value_size)
                    {
                        return false;
                    }

                    memcpy(
                        value,
                        start,
                        length);

                    value[length] =
                        '\0';

                    return true;
                }
            }
        }

        line =
            line_end + 2;
    }

    return false;
}

/*==========================================================
 * Recherche token HTTP
 *=========================================================*/

static bool websocket_header_contains_token(
    const char *header,
    const char *token)
{
    const char *p;

    size_t token_length;

    if (header == NULL ||
        token == NULL)
    {
        return false;
    }

    p =
        header;

    token_length =
        strlen(token);

    while (*p != '\0')
    {
        while (
            *p == ' ' ||
            *p == '\t' ||
            *p == ',')
        {
            ++p;
        }

        if (*p == '\0')
        {
            break;
        }

        const char *end =
            strchr(
                p,
                ',');

        size_t length;

        if (end != NULL)
        {
            length =
                (size_t)(end - p);
        }
        else
        {
            length =
                strlen(p);
        }

        while (
            length > 0U &&
            isspace(
                (unsigned char)
                    p[length - 1U]))
        {
            --length;
        }

        while (
            length > 0U &&
            isspace(
                (unsigned char)*p))
        {
            ++p;
            --length;
        }

        if (length == token_length &&
            strncasecmp(
                p,
                token,
                token_length) == 0)
        {
            return true;
        }

        if (end == NULL)
        {
            break;
        }

        p =
            end + 1;
    }

    return false;
}

/*==========================================================
 * Validation Sec-WebSocket-Key
 *=========================================================*/

static bool websocket_validate_client_key(
    const char *key)
{
    size_t length;

    if (key == NULL)
    {
        return false;
    }

    length =
        strlen(key);

    /*
     * Une clé WebSocket v13 doit faire
     * exactement 24 caractères Base64.
     */
    if (length != WEBSOCKET_KEY_LENGTH)
    {
        return false;
    }

    /*
     * Validation Base64 stricte.
     *
     * Format :
     *
     * 22 caractères Base64
     * puis ==
     */
    for (size_t i = 0; i < 22U; ++i)
    {
        unsigned char c =
            (unsigned char)key[i];

        bool valid =
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '+' ||
            c == '/';

        if (!valid)
        {
            return false;
        }
    }

    if (key[22] != '=' ||
        key[23] != '=')
    {
        return false;
    }

    return true;
}

/*==========================================================
 * Calcul Sec-WebSocket-Accept
 *=========================================================*/

static bool websocket_compute_accept(
    const char *client_key,
    char *accept,
    size_t accept_size)
{
    char input[256];

    uint8_t digest[20];

    sha1_context_t sha1;

    int length;

    if (!websocket_validate_client_key(
            client_key))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Invalid Sec-WebSocket-Key");

        return false;
    }

    length =
        snprintf(
            input,
            sizeof(input),
            "%s%s",
            client_key,
            WEBSOCKET_GUID);

    if (length < 0 ||
        (size_t)length >= sizeof(input))
    {
        return false;
    }

    sha1_init(
        &sha1);

    sha1_update(
        &sha1,
        (const uint8_t *)input,
        (size_t)length);

    sha1_final(
        &sha1,
        digest);

    return base64_encode(
        digest,
        sizeof(digest),
        accept,
        accept_size);
}

/*==========================================================
 * Envoi complet
 *=========================================================*/

static bool websocket_send_all(
    websocket_socket_t socket,
    const char *data,
    size_t length)
{
    size_t total = 0U;

    if (data == NULL ||
        length == 0U)
    {
        return false;
    }

    while (total < length)
    {
#ifdef _WIN32

        int sent =
            send(
                socket,
                data + total,
                (int)(length - total),
                0);

        if (sent == SOCKET_ERROR)
        {
            int error =
                WSAGetLastError();

            LOG_ERROR(
                "WEBSOCKET_HANDSHAKE",
                "Failed to send handshake (%d)",
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
                data + total,
                length - total,
                flags);

        if (sent < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            LOG_ERROR(
                "WEBSOCKET_HANDSHAKE",
                "Failed to send handshake: %s",
                strerror(errno));

            return false;
        }

#endif

        if (sent == 0)
        {
            LOG_ERROR(
                "WEBSOCKET_HANDSHAKE",
                "Handshake send returned zero");

            return false;
        }

        total +=
            (size_t)sent;
    }

    return true;
}

/*==========================================================
 * Validation requête
 *=========================================================*/

static bool websocket_validate_request(
    const char *request,
    char *client_key,
    size_t client_key_size)
{
    char upgrade[64];

    char connection[128];

    char version[32];

    if (request == NULL ||
        client_key == NULL ||
        client_key_size == 0U)
    {
        return false;
    }

    /*
     * Méthode GET.
     */
    if (strncmp(
            request,
            "GET ",
            4U) != 0)
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Request is not GET");

        return false;
    }

    /*
     * Upgrade.
     */
    if (!websocket_get_header(
            request,
            "Upgrade",
            upgrade,
            sizeof(upgrade)))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Missing Upgrade header");

        return false;
    }

    if (strcasecmp(
            upgrade,
            "websocket") != 0)
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Invalid Upgrade header");

        return false;
    }

    /*
     * Connection.
     *
     * Peut contenir plusieurs tokens :
     *
     * Connection: keep-alive, Upgrade
     */
    if (!websocket_get_header(
            request,
            "Connection",
            connection,
            sizeof(connection)))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Missing Connection header");

        return false;
    }

    if (!websocket_header_contains_token(
            connection,
            "Upgrade"))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Invalid Connection header");

        return false;
    }

    /*
     * Version.
     */
    if (!websocket_get_header(
            request,
            "Sec-WebSocket-Version",
            version,
            sizeof(version)))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Missing WebSocket version");

        return false;
    }

    if (strcmp(
            version,
            WEBSOCKET_VERSION) != 0)
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Unsupported WebSocket version");

        return false;
    }

    /*
     * Sec-WebSocket-Key.
     */
    if (!websocket_get_header(
            request,
            "Sec-WebSocket-Key",
            client_key,
            client_key_size))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Missing Sec-WebSocket-Key");

        return false;
    }

    if (!websocket_validate_client_key(
            client_key))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Invalid Sec-WebSocket-Key");

        return false;
    }

    return true;
}

/*==========================================================
 * API publique
 *=========================================================*/

bool websocket_handshake(
    websocket_socket_t socket)
{
    char request[
        WEBSOCKET_HANDSHAKE_BUFFER_SIZE];

    char client_key[128];

    char accept[WS_ACCEPT_SIZE];

    char response[1024];

    int response_length;

    /*
     * Réception de la requête HTTP.
     */
    memset(
        request,
        0,
        sizeof(request));

    if (!websocket_receive_http_request(
            socket,
            request,
            sizeof(request)))
    {
        return false;
    }

    LOG_INFO(
        "WEBSOCKET_HANDSHAKE",
        "HTTP WebSocket request received");

    /*
     * Extraction et validation.
     */
    memset(
        client_key,
        0,
        sizeof(client_key));

    if (!websocket_validate_request(
            request,
            client_key,
            sizeof(client_key)))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Invalid WebSocket request");

        return false;
    }

    LOG_DEBUG(
        "WEBSOCKET_HANDSHAKE",
        "Client key: %s",
        client_key);

    /*
     * Calcul Sec-WebSocket-Accept.
     */
    if (!websocket_compute_accept(
            client_key,
            accept,
            sizeof(accept)))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Failed to calculate accept key");

        return false;
    }

    /*
     * Réponse HTTP 101.
     */
    response_length =
        snprintf(
            response,
            sizeof(response),

            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n"
            "\r\n",

            accept);

    if (response_length < 0 ||
        (size_t)response_length >=
            sizeof(response))
    {
        LOG_ERROR(
            "WEBSOCKET_HANDSHAKE",
            "Handshake response too large");

        return false;
    }

    if (!websocket_send_all(
            socket,
            response,
            (size_t)response_length))
    {
        return false;
    }

    LOG_INFO(
        "WEBSOCKET_HANDSHAKE",
        "WebSocket handshake successful");

    return true;
}
