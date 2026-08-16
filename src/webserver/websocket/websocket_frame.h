#ifndef WEBSOCKET_FRAME_H
#define WEBSOCKET_FRAME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*==========================================================
 * Opcodes WebSocket
 *==========================================================*/

#define WEBSOCKET_OPCODE_CONTINUATION 0x0
#define WEBSOCKET_OPCODE_TEXT         0x1
#define WEBSOCKET_OPCODE_BINARY       0x2
#define WEBSOCKET_OPCODE_CLOSE        0x8
#define WEBSOCKET_OPCODE_PING         0x9
#define WEBSOCKET_OPCODE_PONG         0xA

/*==========================================================
 * Frame décodée
 *==========================================================*/

typedef struct
{
    bool     fin;
    uint8_t  opcode;
    bool     masked;

    uint64_t payload_length;

    const uint8_t *payload;

} websocket_frame_t;

/*==========================================================
 * Encodage
 *==========================================================*/

/**
 * Encode une trame TEXT WebSocket.
 *
 * Une trame envoyée par un client WebSocket doit
 * obligatoirement être masquée.
 *
 * @param text         Texte à envoyer.
 * @param length       Taille du texte.
 * @param buffer       Buffer de sortie.
 * @param buffer_size  Taille du buffer.
 * @param encoded      Nombre d'octets produits.
 *
 * @return true si l'encodage réussit.
 */
bool websocket_frame_encode_text(
    const char *text,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded);

/**
 * Encode une trame PING.
 */
bool websocket_frame_encode_ping(
    const uint8_t *payload,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded);

/**
 * Encode une trame PONG.
 */
bool websocket_frame_encode_pong(
    const uint8_t *payload,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded);

/**
 * Encode une trame CLOSE.
 */
bool websocket_frame_encode_close(
    uint16_t status_code,
    const char *reason,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded);

/*==========================================================
 * Décodage
 *==========================================================*/

/**
 * Décode une trame WebSocket reçue.
 *
 * Le payload retourné pointe directement dans le buffer
 * fourni par l'appelant.
 *
 * @param buffer       Buffer contenant la trame.
 * @param buffer_size  Nombre d'octets disponibles.
 * @param frame        Structure de sortie.
 * @param consumed     Nombre d'octets consommés.
 *
 * @return true si une trame complète est disponible.
 */
bool websocket_frame_decode(
    uint8_t *buffer,
    size_t buffer_size,
    websocket_frame_t *frame,
    size_t *consumed);

#endif /* WEBSOCKET_FRAME_H */
