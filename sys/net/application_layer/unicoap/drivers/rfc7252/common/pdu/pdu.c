/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @file
 * @ingroup net_unicoap_drivers_rfc7252_common
 * @brief   Framing and PDU parser implementation of common RFC 7252 driver
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#include <stdint.h>
#include <errno.h>

#include "net/unicoap/message.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

#define PDU_7252_DEBUG(...) _UNICOAP_PREFIX_DEBUG(".pdu.rfc7252", __VA_ARGS__)

/*  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Ver| T |  TKL  |      Code     |          Message ID           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Token (if any, TKL bytes) ...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Options (if any) ...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |1 1 1 1 1 1 1 1|    Payload (if any) ...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * https://datatracker.ietf.org/doc/html/rfc7252#section-3
 */
typedef struct __attribute__((packed)) {
    uint8_t version_type_token_length;
    uint8_t code;
    uint16_t message_id;
} unicoap_header_rfc7252_t;

/**
 * @brief       Gets message CoAP version
 * @param[in]   header   CoAP PDU header
 * @return      CoAP version
 */
static inline unicoap_protocol_version_t _get_version(const unicoap_header_rfc7252_t* header)
{
    return header->version_type_token_length >> 6;
}

/**
 * @brief       Sets message CoAP version to 1
 * @param[in]   header   CoAP PDU header
 */
static inline void _set_version(unicoap_header_rfc7252_t* header)
{
    header->version_type_token_length |= UNICOAP_COAP_VERSION_1 << 6;
}

/**
 * @brief       Gets the message type of an RFC 7252 PDU
 * @param[in]   header   CoAP PDU header
 * @returns     @ref COAP_TYPE_CON
 * @returns             @ref COAP_TYPE_NON
 * @returns             @ref COAP_TYPE_ACK
 * @returns             @ref COAP_TYPE_RST
 */
static inline unsigned _get_type(const unicoap_header_rfc7252_t* header)
{
    return (header->version_type_token_length & 0x30) >> 4;
}

/**
 * @brief       Sets the message type of an RFC 7252 PDU
 * @param[in]   header   CoAP PDU header
 * @param       type   CoAP PDU type
 */
static inline void _set_type(unicoap_header_rfc7252_t* header, unicoap_rfc7252_message_type_t type)
{
    assert((type & ~0x3) == 0);
    header->version_type_token_length |= type << 4;
}

/**
 * @brief       Gets a message's raw code (class + detail)
 * @param[in]   header   CoAP PDU header
 * @returns     Raw message code
 */
static inline uint8_t _get_code(const unicoap_header_rfc7252_t* header)
{
    return header->code;
}

/**
 * @brief       Sets a message's raw code (class + detail)
 * @param[in]   header   CoAP PDU header
 * @param code     Raw message code
 */
static inline void _set_code(unicoap_header_rfc7252_t* header, uint8_t code)
{
    header->code = code;
}

/**
 * @brief       Gets a message's ID
 * @param[in]   header   CoAP PDU header
 * @returns     Raw message ID
 */
static inline uint16_t _get_message_id(const unicoap_header_rfc7252_t* header)
{
    return ntohs(header->message_id);
}

/**
 * @brief       Sets a message's ID
 * @param[in]   header   CoAP PDU header
 * @param       message_id Raw message ID
 */
static inline void _set_message_id(unicoap_header_rfc7252_t* header, uint16_t message_id)
{
    header->message_id = htons(message_id);
}

/**
 * @brief       Gets a message's token length (segment)
 * @param[in]   header   CoAP PDU header
 * @returns     First segment of token length
 */
static inline uint8_t _get_token_length(const unicoap_header_rfc7252_t* header)
{
    return header->version_type_token_length & 0xf;
}

/**
 * @brief       Sets a message's token length (segment)
 * @param[in]   header   CoAP PDU header
 * @param token_length     First segment of token length
 */
static inline void _set_token_length(unicoap_header_rfc7252_t* header, uint8_t token_length)
{
    assert((token_length & ~0xf) == 0);
    header->version_type_token_length |= token_length;
}

ssize_t unicoap_pdu_parse_rfc7252(uint8_t* pdu, size_t size, unicoap_message_t* message,
                                  unicoap_message_properties_t* properties)
{
    if (size < sizeof(unicoap_header_rfc7252_t)) {
        PDU_7252_DEBUG("msg too short\n");
        return -EBADMSG;
    }
    const unicoap_header_rfc7252_t* header = (unicoap_header_rfc7252_t*)pdu;

    if (_get_version(header) != UNICOAP_COAP_VERSION_1) {
        PDU_7252_DEBUG("invalid version %" PRIu8 "\n", _get_version(header));
        return -EBADMSG;
    }

    if ((_get_code(header) == UNICOAP_CODE_EMPTY) && (size > sizeof(unicoap_header_rfc7252_t))) {
        PDU_7252_DEBUG("empty msg is too long\n");
        return -EBADMSG;
    }

    uint8_t* end = pdu + size;
    uint8_t* cursor = pdu + sizeof(unicoap_header_rfc7252_t);

    message->code = _get_code(header);
    properties->rfc7252.type = _get_type(header);
    properties->rfc7252.id = _get_message_id(header);
    properties->token = cursor;
    properties->token_length = _get_token_length(header);

    if (properties->token_length > CONFIG_UNICOAP_EXTERNAL_TOKEN_LENGTH_MAX) {
        /* From RFC 7252, Section 3
         * https://datatracker.ietf.org/doc/html/rfc7252#section-3
         * Lengths 9-15 are
         * reserved, MUST NOT be sent, and MUST be processed as a message
         * format error. */
        PDU_7252_DEBUG("invalid token length %" PRIu8 "\n", properties->token_length);
        return -EBADMSG;
    }

    cursor += properties->token_length;
    if (cursor > end) {
        PDU_7252_DEBUG("invalid token length %" PRIu8 ", overflow\n", properties->token_length);
        return -EBADMSG;
    }

    return unicoap_pdu_parse_options_and_payload(cursor, end, message);
}

ssize_t unicoap_pdu_build_header_rfc7252(uint8_t* header, size_t capacity,
                                         const unicoap_message_t* message,
                                         const unicoap_message_properties_t* properties)
{
    assert(properties->token_length <= 0xf);
    if (capacity < sizeof(unicoap_header_rfc7252_t) + properties->token_length) {
        return -ENOBUFS;
    }

    *header = 0;
    unicoap_header_rfc7252_t* _header = (unicoap_header_rfc7252_t*)header;
    _set_version(_header);
    _set_type(_header, properties->rfc7252.type);
    _set_message_id(_header, properties->rfc7252.id);
    _set_code(_header, message->code);
    _set_token_length(_header, properties->token_length);
    header += sizeof(unicoap_header_rfc7252_t);

    memcpy(header, properties->token, properties->token_length);
    return sizeof(unicoap_header_rfc7252_t) + properties->token_length;
}

const char* unicoap_string_from_rfc7252_type(unicoap_rfc7252_message_type_t type)
{
    switch (type) {
    case UNICOAP_TYPE_NON:
        return "NON";
    case UNICOAP_TYPE_CON:
        return "CON";
    case UNICOAP_TYPE_ACK:
        return "ACK";
    case UNICOAP_TYPE_RST:
        return "RST";
    default:
        return "?";
    }
}
