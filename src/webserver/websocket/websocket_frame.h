#ifndef WEBSOCKET_FRAME_H
#define WEBSOCKET_FRAME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*==========================================================
 * Opcodes
 *=========================================================*/

#define WEBSOCKET_OPCODE_CONTINUATION 0x00U
#define WEBSOCKET_OPCODE_TEXT         0x01U
#define WEBSOCKET_OPCODE_BINARY      0x02U

#define WEBSOCKET_OPCODE_CLOSE       0x08U
#define WEBSOCKET_OPCODE_PING        0x09U
#define WEBSOCKET_OPCODE_PONG        0x0AU

/*==========================================================
 * Frame
 *=========================================================*/

typedef struct
{
    bool fin;
    uint8_t opcode;
    bool masked;

    uint64_t payload_length;

    uint8_t *payload;

} websocket_frame_t;

/*==========================================================
 * Decode status
 *=========================================================*/

typedef enum
{
    WEBSOCKET_FRAME_INCOMPLETE = 0,
    WEBSOCKET_FRAME_COMPLETE,
    WEBSOCKET_FRAME_INVALID

} websocket_frame_decode_status_t;

/*==========================================================
 * Encoding
 *=========================================================*/

bool websocket_frame_encode_text(
    const char *text,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded);

bool websocket_frame_encode_ping(
    const uint8_t *payload,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded);

bool websocket_frame_encode_pong(
    const uint8_t *payload,
    size_t length,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded);

bool websocket_frame_encode_close(
    uint16_t status_code,
    const char *reason,
    uint8_t *buffer,
    size_t buffer_size,
    size_t *encoded);

/*==========================================================
 * Decoding
 *=========================================================*/

websocket_frame_decode_status_t
websocket_frame_decode(
    uint8_t *buffer,
    size_t buffer_size,
    websocket_frame_t *frame,
    size_t *consumed);

#endif