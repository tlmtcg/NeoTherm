#include "websocket_frame.h"

#include <string.h>

#include "logger.h"

/*==========================================================
 * Constantes
 *=========================================================*/

#define WEBSOCKET_MAX_CONTROL_PAYLOAD 125U

/*==========================================================
 * Utilitaires internes
 *=========================================================*/

/**
 * Vérifie si un opcode est un opcode de contrôle.
 */
static bool websocket_opcode_is_control(
    uint8_t opcode)
{
    return opcode >= 0x08U;
}

/*----------------------------------------------------------
 * Vérification opcode
 *---------------------------------------------------------*/

static bool websocket_opcode_is_valid(
    uint8_t opcode)
{
    switch (opcode)
    {
        case WEBSOCKET_OPCODE_CONTINUATION:
        case WEBSOCKET_OPCODE_TEXT:
        case WEBSOCKET_OPCODE_BINARY:
        case WEBSOCKET_OPCODE_CLOSE:
        case WEBSOCKET_OPCODE_PING:
        case WEBSOCKET_OPCODE_PONG:
            return true;

        default:
            return false;
    }
}

/*----------------------------------------------------------
 * Vérification code CLOSE
 *---------------------------------------------------------*/

static bool websocket_close_status_valid(
    uint16_t status_code)
{
    /*
     * Codes réservés / invalides RFC 6455.
     */
    if (status_code < 1000U)
    {
        return false;
    }

    if (status_code >= 1004U &&
        status_code <= 1006U)
    {
        return false;
    }

    if (status_code >= 1012U &&
        status_code <= 1016U)
    {
        return false;
    }

    if (status_code >= 1100U)
    {
        return false;
    }

    return true;
}

/*----------------------------------------------------------
 * Masque WebSocket
 *---------------------------------------------------------*/

/**
 * Applique un masque WebSocket.
 *
 * Utilisé uniquement pour les trames reçues du client.
 */
static void websocket_apply_mask(
    uint8_t *data,
    size_t length,
    const uint8_t mask[4])
{
    for (size_t i = 0; i < length; ++i)
    {
        data[i] ^=
            mask[i % 4U];
    }
}

/*----------------------------------------------------------
 * Taille en-tête serveur
 *---------------------------------------------------------*/

static size_t websocket_header_size(
    size_t payload_length)
{
    if (payload_length <= 125U)
    {
        return 2U;
    }

    if (payload_length <= 65535U)
    {
        return 4U;
    }

    return 10U;
}

/*==========================================================
 * Encodage serveur
 *=========================================================*/

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

    if (payload_length > 0U &&
        payload == NULL)
    {
        return false;
    }

    if (!websocket_opcode_is_valid(opcode))
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Invalid opcode: 0x%02X",
            opcode);

        return false;
    }

    /*
     * Les trames de contrôle :
     *
     * FIN = 1
     * payload <= 125
     */
    if (websocket_opcode_is_control(opcode) &&
        payload_length > WEBSOCKET_MAX_CONTROL_PAYLOAD)
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Control frame payload too large");

        return false;
    }

    header_size =
        websocket_header_size(
            payload_length);

    if (payload_length >
        SIZE_MAX - header_size)
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Frame size overflow");

        return false;
    }

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
     * MASK = 0
     */
    buffer[offset++] =
        0x80U |
        (opcode & 0x0FU);

    /*
     * Payload <= 125
     */
    if (payload_length <= 125U)
    {
        buffer[offset++] =
            (uint8_t)payload_length;
    }

    /*
     * Payload 126 .. 65535
     */
    else if (payload_length <= 65535U)
    {
        buffer[offset++] =
            126U;

        buffer[offset++] =
            (uint8_t)(
                (payload_length >> 8U) &
                0xFFU);

        buffer[offset++] =
            (uint8_t)(
                payload_length &
                0xFFU);
    }

    /*
     * Payload > 65535
     */
    else
    {
        uint64_t length =
            (uint64_t)payload_length;

        buffer[offset++] =
            127U;

        for (int i = 7; i >= 0; --i)
        {
            buffer[offset++] =
                (uint8_t)(
                    (length >>
                     (i * 8)) &
                    0xFFU);
        }
    }

    if (payload_length > 0U)
    {
        memcpy(
            &buffer[offset],
            payload,
            payload_length);

        offset +=
            payload_length;
    }

    *encoded =
        offset;

    return true;
}

/*==========================================================
 * TEXT
 *=========================================================*/

bool websocket_frame_encode_text(
    const char *text,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded)
{
    if (text == NULL &&
        length > 0U)
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
 *=========================================================*/

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
 *=========================================================*/

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
 *=========================================================*/

bool websocket_frame_encode_close(
    uint16_t status_code,
    const char *reason,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded)
{
    uint8_t payload[
        WEBSOCKET_MAX_CONTROL_PAYLOAD];

    size_t reason_length = 0U;

    if (!websocket_close_status_valid(
            status_code))
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Invalid CLOSE status code: %u",
            status_code);

        return false;
    }

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
        (uint8_t)(status_code >> 8U);

    payload[1] =
        (uint8_t)(status_code & 0xFFU);

    if (reason_length > 0U)
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
 * Décodage client -> serveur
 *=========================================================*/

websocket_frame_decode_status_t
websocket_frame_decode(
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

    uint8_t mask[4];

    if (consumed != NULL)
    {
        *consumed = 0U;
    }

    if (buffer == NULL ||
        frame == NULL ||
        consumed == NULL)
    {
        return WEBSOCKET_FRAME_INVALID;
    }

    /*
     * Minimum : 2 octets.
     */
    if (buffer_size < 2U)
    {
        return WEBSOCKET_FRAME_INCOMPLETE;
    }

    byte0 =
        buffer[0];

    byte1 =
        buffer[1];

    /*------------------------------------------------------
     * FIN
     *------------------------------------------------------*/

    fin =
        (byte0 & 0x80U) != 0U;

    /*------------------------------------------------------
     * RSV
     *------------------------------------------------------*/

    if ((byte0 & 0x70U) != 0U)
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Unsupported RSV bits");

        return WEBSOCKET_FRAME_INVALID;
    }

    /*------------------------------------------------------
     * OPCODE
     *------------------------------------------------------*/

    opcode =
        byte0 & 0x0FU;

    if (!websocket_opcode_is_valid(opcode))
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Unsupported opcode: 0x%02X",
            opcode);

        return WEBSOCKET_FRAME_INVALID;
    }

    /*------------------------------------------------------
     * Fragmentation
     *
     * Notre couche actuelle ne supporte pas la
     * fragmentation des messages.
     *
     * On refuse donc toute trame de données
     * dont FIN = 0.
     *------------------------------------------------------*/

    if (!fin &&
        !websocket_opcode_is_control(opcode))
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Fragmented data frame not supported");

        return WEBSOCKET_FRAME_INVALID;
    }

    /*
     * Une continuation n'est pas supportée.
     */
    if (opcode == WEBSOCKET_OPCODE_CONTINUATION)
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Continuation frame not supported");

        return WEBSOCKET_FRAME_INVALID;
    }

    /*------------------------------------------------------
     * MASK
     *------------------------------------------------------*/

    masked =
        (byte1 & 0x80U) != 0U;

    /*
     * Client -> serveur :
     * MASK doit obligatoirement être 1.
     */
    if (!masked)
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Received unmasked client frame");

        return WEBSOCKET_FRAME_INVALID;
    }

    payload_length =
        byte1 & 0x7FU;

    offset =
        2U;

    /*------------------------------------------------------
     * Longueur 126
     *------------------------------------------------------*/

    if (payload_length == 126U)
    {
        if (buffer_size <
            offset + 2U)
        {
            return WEBSOCKET_FRAME_INCOMPLETE;
        }

        payload_length =
            ((uint64_t)buffer[offset] << 8U) |
            (uint64_t)buffer[offset + 1U];

        offset +=
            2U;

        /*
         * Encodage minimal obligatoire.
         *
         * Une longueur <= 125 aurait dû utiliser
         * le format court.
         */
        if (payload_length < 126U)
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Non-minimal payload length");

            return WEBSOCKET_FRAME_INVALID;
        }
    }

    /*------------------------------------------------------
     * Longueur 127
     *------------------------------------------------------*/

    else if (payload_length == 127U)
    {
        if (buffer_size <
            offset + 8U)
        {
            return WEBSOCKET_FRAME_INCOMPLETE;
        }

        payload_length =
            0U;

        for (int i = 0; i < 8; ++i)
        {
            payload_length =
                (payload_length << 8U) |
                (uint64_t)buffer[offset + i];
        }

        offset +=
            8U;

        /*
         * Bit 63 doit être à zéro.
         */
        if (payload_length &
            (1ULL << 63))
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Invalid 64-bit payload length");

            return WEBSOCKET_FRAME_INVALID;
        }

        /*
         * Encodage minimal :
         * 65536 minimum pour utiliser 127.
         */
        if (payload_length < 65536ULL)
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Non-minimal 64-bit payload length");

            return WEBSOCKET_FRAME_INVALID;
        }
    }

    /*------------------------------------------------------
     * Trames de contrôle
     *------------------------------------------------------*/

    if (websocket_opcode_is_control(opcode))
    {
        if (!fin)
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Fragmented control frame");

            return WEBSOCKET_FRAME_INVALID;
        }

        if (payload_length >
            WEBSOCKET_MAX_CONTROL_PAYLOAD)
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Control frame too large");

            return WEBSOCKET_FRAME_INVALID;
        }
    }

    /*------------------------------------------------------
     * CLOSE
     *------------------------------------------------------*/

    if (opcode == WEBSOCKET_OPCODE_CLOSE)
    {
        /*
         * Payload CLOSE :
         *
         * 0 octet          -> autorisé
         * 2..125 octets    -> autorisé
         * 1 octet          -> interdit
         */
        if (payload_length == 1U)
        {
            LOG_ERROR(
                "WEBSOCKET_FRAME",
                "Invalid CLOSE payload length");

            return WEBSOCKET_FRAME_INVALID;
        }
    }

    /*------------------------------------------------------
     * Masque
     *------------------------------------------------------*/

    if (buffer_size <
        offset + 4U)
    {
        return WEBSOCKET_FRAME_INCOMPLETE;
    }

    memcpy(
        mask,
        &buffer[offset],
        sizeof(mask));

    offset +=
        sizeof(mask);

    /*------------------------------------------------------
     * Vérification taille payload
     *------------------------------------------------------*/

    if (payload_length >
        (uint64_t)(buffer_size - offset))
    {
        /*
         * La trame est simplement incomplète.
         */
        return WEBSOCKET_FRAME_INCOMPLETE;
    }

    /*
     * Protection uint64_t -> size_t.
     */
    if (payload_length >
        (uint64_t)SIZE_MAX)
    {
        LOG_ERROR(
            "WEBSOCKET_FRAME",
            "Payload length exceeds size_t");

        return WEBSOCKET_FRAME_INVALID;
    }

    /*------------------------------------------------------
     * Démasquage
     *------------------------------------------------------*/

    if (payload_length > 0U)
    {
        websocket_apply_mask(
            &buffer[offset],
            (size_t)payload_length,
            mask);
    }

    /*------------------------------------------------------
     * Structure résultat
     *------------------------------------------------------*/

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

    return WEBSOCKET_FRAME_COMPLETE;
}
