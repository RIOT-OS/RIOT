/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap_drivers_rfc7252_common
 * @brief   Framing and PDU parser implementation of common RFC 7252 driver
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <errno.h>
#include <stdint.h>

#include "compiler_hints.h"
#include "net/unicoap/constants.h"
#include "net/unicoap/message.h"
#include "private/pdu/rfc7252.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

#define PDU_7252_DEBUG(...) _UNICOAP_PREFIX_DEBUG(".pdu.rfc7252", __VA_ARGS__)

ssize_t unicoap_pdu_parse_rfc7252(uint8_t* pdu, size_t size, unicoap_message_t* message,
                                  unicoap_message_properties_t* properties)
{
    if (size < sizeof(unicoap_rfc7252_header_t)) {
        PDU_7252_DEBUG("msg too short\n");
        return -EBADMSG;
    }
    const unicoap_rfc7252_header_t* header = (unicoap_rfc7252_header_t*)pdu;

    if (unicoap_rfc7252_header_get_version(header) != UNICOAP_COAP_VERSION_1) {
        PDU_7252_DEBUG("invalid version %" PRIu8 "\n", unicoap_rfc7252_header_get_version(header));
        return -EBADMSG;
    }

    if ((unicoap_rfc7252_header_get_code(header) == UNICOAP_CODE_EMPTY) && (size > sizeof(unicoap_rfc7252_header_t))) {
        PDU_7252_DEBUG("empty msg is too long\n");
        return -EBADMSG;
    }

    uint8_t* end = pdu + size;
    uint8_t* cursor = pdu + sizeof(unicoap_rfc7252_header_t);

    message->code = unicoap_rfc7252_header_get_code(header);
    properties->rfc7252.type = unicoap_rfc7252_header_get_type(header);
    properties->rfc7252.id = unicoap_rfc7252_header_get_message_id(header);
    properties->token = cursor;
    properties->token_length = unicoap_rfc7252_header_get_token_length_indicator(header);

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

ssize_t unicoap_pdu_build_header_rfc7252_with_code(uint8_t* header, size_t capacity,
                                                   const unicoap_message_properties_t* properties,
                                                   uint8_t code)
{
    assert(properties->token_length <= 0xf);
    if (capacity < sizeof(unicoap_rfc7252_header_t) + properties->token_length) {
        return -ENOBUFS;
    }

    *header = 0;
    unicoap_rfc7252_header_t* _header = (unicoap_rfc7252_header_t*)header;
    unicoap_rfc7252_header_set_version(_header, UNICOAP_COAP_VERSION_1);
    unicoap_rfc7252_header_set_type(_header, properties->rfc7252.type);
    unicoap_rfc7252_header_set_message_id(_header, properties->rfc7252.id);
    unicoap_rfc7252_header_set_code(_header, code);
    unicoap_rfc7252_header_set_token_length_indicator(_header, properties->token_length);
    header += sizeof(unicoap_rfc7252_header_t);

    if (likely(properties->token_length > 0)) {
        memcpy(header, properties->token, properties->token_length);
    }
    return sizeof(unicoap_rfc7252_header_t) + properties->token_length;
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
