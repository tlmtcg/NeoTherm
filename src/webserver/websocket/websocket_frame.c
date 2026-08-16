#include "websocket_frame.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "logger.h"

/*==========================================================
 * Constantes
 *==========================================================*/

#define WEBSOCKET_MAX_CONTROL_PAYLOAD 125U

/*==========================================================
 * Utilitaires internes
 *==========================================================*/

/**
 * Génère une clé de masquage.
 *
 * Pour un client WebSocket, chaque trame doit utiliser
 * une nouvelle clé de masquage.
 */
static void websocket_generate_mask(
    uint8_t mask[4])
{
    static bool initialized = false;

    if (!initialized)
    {
        srand(
            (unsigned int)time(NULL));

        initialized = true;
    }

    for (int i = 0; i < 4; ++i)
    {
        mask[i] =
            (uint8_t)(rand() & 0xFF);
    }
}

/*----------------------------------------------------------
 * Masquage / démasquage
 *----------------------------------------------------------*/

static void websocket_apply_mask(
    uint8_t *data,
    size_t length,
    const uint8_t mask[4])
{
    for (size_t i = 0; i < length; ++i)
    {
        data[i] ^=
            mask[i % 4];
    }
}

/*----------------------------------------------------------
 * Taille de l'en-tête
 *----------------------------------------------------------*/

static size_t websocket_header_size(
    size_t payload_length)
{
    if (payload_length <= 125U)
    {
        return 2U + 4U;
    }

    if (payload_length <= 65535U)
    {
        return 2U + 2U + 4U;
    }

    return 2U + 8U + 4U;
}

/*==========================================================
 * Encodage générique
 *==========================================================*/

static bool websocket_frame_encode(
    uint8_t opcode,
    const uint8_t *payload,
    size_t payload_length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded)
{
    size_t header_size;
    size_t total_size;

    uint8_t mask[4];

    size_t offset = 0;

    if (encoded != NULL)
    {
        *encoded = 0;
    }

    if (buffer == NULL ||
        encoded == NULL)
    {
        return false;
    }

    if (payload_length > 0 &&
        payload == NULL)
    {
        return false;
    }

    /*
     * Les trames de contrôle doivent être FIN=1
     * et leur payload <= 125 octets.
     */
    if (opcode == WEBSOCKET_OPCODE_CLOSE ||
        opcode == WEBSOCKET_OPCODE_PING ||
        opcode == WEBSOCKET_OPCODE_PONG)
    {
        if (payload_length >
            WEBSOCKET_MAX_CONTROL_PAYLOAD)
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Control frame payload too large");

            return false;
        }
    }

    header_size =
        websocket_header_size(
            payload_length);

    total_size =
        header_size +
        payload_length;

    if (buffer_size < total_size)
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Output buffer too small");

        return false;
    }

    /*
     * FIN = 1
     * RSV1/2/3 = 0
     * OPCODE
     */
    buffer[offset++] =
        0x80U |
        (opcode & 0x0FU);

    /*
     * Client -> serveur :
     *
     * MASK = 1
     */
    if (payload_length <= 125U)
    {
        buffer[offset++] =
            0x80U |
            (uint8_t)payload_length;
    }
    else if (payload_length <= 65535U)
    {
        buffer[offset++] =
            0x80U | 126U;

        buffer[offset++] =
            (uint8_t)(
                (payload_length >> 8) &
                0xFFU);

        buffer[offset++] =
            (uint8_t)(
                payload_length &
                0xFFU);
    }
    else
    {
        buffer[offset++] =
            0x80U | 127U;

        uint64_t length =
            (uint64_t)payload_length;

        /*
         * RFC 6455 :
         * longueur 64 bits big endian.
         */
        for (int i = 7; i >= 0; --i)
        {
            buffer[offset++] =
                (uint8_t)(
                    (length >>
                     (i * 8)) &
                    0xFFU);
        }
    }

    /*
     * Génération du masque.
     */
    websocket_generate_mask(mask);

    memcpy(
        &buffer[offset],
        mask,
        sizeof(mask));

    offset +=
        sizeof(mask);

    /*
     * Copie du payload.
     */
    if (payload_length > 0)
    {
        memcpy(
            &buffer[offset],
            payload,
            payload_length);

        /*
         * Application du masque.
         */
        websocket_apply_mask(
            &buffer[offset],
            payload_length,
            mask);
    }

    offset +=
        payload_length;

    *encoded =
        offset;

    return true;
}

/*==========================================================
 * TEXT
 *==========================================================*/

bool websocket_frame_encode_text(
    const char *text,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded)
{
    if (text == NULL &&
        length > 0)
    {
        return false;
    }

    return websocket_frame_encode(
        WEBSOCKET_OPCODE_TEXT,
        (const uint8_t *)text,
        length,
        buffer,
        buffer_size,
        encoded);
}

/*==========================================================
 * PING
 *==========================================================*/

bool websocket_frame_encode_ping(
    const uint8_t *payload,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded)
{
    return websocket_frame_encode(
        WEBSOCKET_OPCODE_PING,
        payload,
        length,
        buffer,
        buffer_size,
        encoded);
}

/*==========================================================
 * PONG
 *==========================================================*/

bool websocket_frame_encode_pong(
    const uint8_t *payload,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded)
{
    return websocket_frame_encode(
        WEBSOCKET_OPCODE_PONG,
        payload,
        length,
        buffer,
        buffer_size,
        encoded);
}

/*==========================================================
 * CLOSE
 *==========================================================*/

bool websocket_frame_encode_close(
    uint16_t status_code,
    const char *reason,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded)
{
    uint8_t payload[
        WEBSOCKET_MAX_CONTROL_PAYLOAD];

    size_t reason_length = 0;

    if (reason != NULL)
    {
        reason_length =
            strlen(reason);
    }

    /*
     * 2 octets pour le status code.
     */
    if (reason_length >
        WEBSOCKET_MAX_CONTROL_PAYLOAD - 2U)
    {
        reason_length =
            WEBSOCKET_MAX_CONTROL_PAYLOAD - 2U;
    }

    payload[0] =
        (uint8_t)(status_code >> 8);

    payload[1] =
        (uint8_t)(status_code & 0xFFU);

    if (reason_length > 0)
    {
        memcpy(
            &payload[2],
            reason,
            reason_length);
    }

    return websocket_frame_encode(
        WEBSOCKET_OPCODE_CLOSE,
        payload,
        2U + reason_length,
        buffer,
        buffer_size,
        encoded);
}

/*==========================================================
 * Décodage
 *==========================================================*/

bool websocket_frame_decode(
    uint8_t *buffer,
    size_t buffer_size,
    websocket_frame_t *frame,
    size_t *consumed)
{
    uint8_t byte0;
    uint8_t byte1;

    uint8_t opcode;
    bool fin;
    bool masked;

    uint64_t payload_length;

    size_t offset;

    if (consumed != NULL)
    {
        *consumed = 0;
    }

    if (buffer == NULL ||
        frame == NULL ||
        consumed == NULL)
    {
        return false;
    }

    /*
     * Il faut au minimum les deux premiers octets.
     */
    if (buffer_size < 2U)
    {
        return false;
    }

    byte0 =
        buffer[0];

    byte1 =
        buffer[1];

    fin =
        (byte0 & 0x80U) != 0;

    /*
     * RSV1/RSV2/RSV3 doivent être à zéro
     * puisque nous ne gérons pas d'extensions.
     */
    if ((byte0 & 0x70U) != 0)
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Unsupported RSV bits");

        return false;
    }

    opcode =
        byte0 & 0x0FU;

    masked =
        (byte1 & 0x80U) != 0;

    payload_length =
        byte1 & 0x7FU;

    offset = 2U;

    /*
     * Longueur 126.
     */
    if (payload_length == 126U)
    {
        if (buffer_size < offset + 2U)
        {
            return false;
        }

        payload_length =
            ((uint64_t)buffer[offset] << 8) |
            ((uint64_t)buffer[offset + 1]);

        offset += 2U;
    }

    /*
     * Longueur 127.
     */
    else if (payload_length == 127U)
    {
        if (buffer_size < offset + 8U)
        {
            return false;
        }

        payload_length = 0;

        for (int i = 0; i < 8; ++i)
        {
            payload_length =
                (payload_length << 8) |
                buffer[offset + i];
        }

        offset += 8U;

        /*
         * Le bit 63 doit être à zéro selon RFC 6455.
         */
        if (payload_length &
            (1ULL << 63))
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Invalid 64-bit payload length");

            return false;
        }
    }

    /*
     * Les trames de contrôle doivent être :
     *
     * FIN = 1
     * longueur <= 125
     */
    if (opcode >= 0x8U)
    {
        if (!fin)
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Fragmented control frame");

            return false;
        }

        if (payload_length > 125U)
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Control frame too large");

            return false;
        }
    }

    /*
     * Vérification de la présence du masque.
     *
     * Pour une trame reçue par le serveur :
     * MASK doit être 1.
     *
     * Notre fonction de décodage est utilisée côté
     * serveur, donc on exige ici MASK=1.
     */
    if (!masked)
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Received unmasked client frame");

        return false;
    }

    /*
     * Vérification de la taille du masque.
     */
    if (buffer_size <
        offset + 4U)
    {
        return false;
    }

    uint8_t mask[4];

    memcpy(
        mask,
        &buffer[offset],
        sizeof(mask));

    offset +=
        sizeof(mask);

    /*
     * Vérification que le payload complet est disponible.
     */
    if (payload_length >
        (uint64_t)(buffer_size - offset))
    {
        /*
         * La trame est incomplète.
         */
        return false;
    }

    /*
     * Attention :
     *
     * buffer contient la trame reçue du client.
     * Le payload est donc masqué.
     *
     * On le démasque directement dans le buffer.
     */
    if (payload_length > 0)
    {
        websocket_apply_mask(
            &buffer[offset],
            (size_t)payload_length,
            mask);
    }

    /*
     * Remplissage de la structure.
     */
    frame->fin =
        fin;

    frame->opcode =
        opcode;

    frame->masked =
        masked;

    frame->payload_length =
        payload_length;

    frame->payload =
        &buffer[offset];

    offset +=
        (size_t)payload_length;

    *consumed =
        offset;

    return true;
}
