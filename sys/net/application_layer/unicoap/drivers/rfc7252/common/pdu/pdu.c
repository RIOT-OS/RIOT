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
#include <sys/types.h>

#include "byteorder.h"
#include "compiler_hints.h"
#include "modules.h"
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
    properties->token_length = unicoap_rfc7252_header_get_token_length_indicator(header);

    if (unlikely(properties->token_length > UNICOAP_RFC7252_TOKEN_LENGTH_VERBATIM_UPPER_BOUND)) {
        if (properties->token_length == UNICOAP_RFC7252_TOKEN_LENGTH_INDICATOR_RFU) {
            PDU_7252_DEBUG("TKL=15 is reserved\n");
            return -EBADMSG;
        }
        if (IS_ACTIVE(CONFIG_UNICOAP_ALLOW_EXTENDED_TOKENS)) {
            uint8_t token_length_size = properties->token_length -
                                        UNICOAP_RFC7252_TOKEN_LENGTH_VERBATIM_UPPER_BOUND;
            uint8_t* token_length = cursor;
            cursor += token_length_size;
            if (cursor > end) {
                PDU_7252_DEBUG("no space to read ext TKL field of %" PRIu8 " bytes\n",
                               token_length_size);
                return -EBADMSG;
            }

            properties->token_length += token_length_size == 1 ?
                                            *token_length :
                                            byteorder_bebuftohs(token_length) + 0xff;
        } else {
            PDU_7252_DEBUG("extended tokens not supported\n");
            unicoap_assist(API_ERROR("Extended token present")
                           FIXIT("Enable CONFIG_UNICOAP_ALLOW_EXTENDED_TOKENS"));
            /* RFC 7252 requires TKL 9–15 to be treated as a message format error,
             * so -EBADMSG and not -ENOTSUP. */
            return -EBADMSG;
        }
    }

    if (properties->token_length > CONFIG_UNICOAP_EXTERNAL_TOKEN_CAPACITY) {
        /* From RFC 7252, Section 3
         * https://datatracker.ietf.org/doc/html/rfc7252#section-3
         * Lengths 9-15 are
         * reserved, MUST NOT be sent, and MUST be processed as a message
         * format error. */
        PDU_7252_DEBUG("token too long " _UNICOAP_NEED_HAVE "\n",
                       (size_t)properties->token_length,
                       (size_t)CONFIG_UNICOAP_EXTERNAL_TOKEN_CAPACITY);
        return -EBADMSG;
    }

    properties->token = cursor;
    cursor += properties->token_length;
    if (cursor > end) {
        PDU_7252_DEBUG("invalid token length %" PRIuSIZE ", PDU too short\n",
                       (size_t)properties->token_length);
        return -EBADMSG;
    }

    return unicoap_pdu_parse_options_and_payload(cursor, end, message);
}

ssize_t unicoap_pdu_build_header_rfc7252_with_code(uint8_t* header, size_t capacity,
                                                   const unicoap_message_properties_t* properties,
                                                   uint8_t code)
{
    uint8_t token_length_size = unicoap_rfc7252_header_extra_token_length_bytes(
        (size_t)properties->token_length);
    if (capacity < 
        sizeof(unicoap_rfc7252_header_t) + properties->token_length + token_length_size
    ) {
        return -ENOBUFS;
    }

    *header = 0;
    unicoap_rfc7252_header_t* _header = (unicoap_rfc7252_header_t*)header;
    unicoap_rfc7252_header_set_version(_header, UNICOAP_COAP_VERSION_1);
    unicoap_rfc7252_header_set_type(_header, properties->rfc7252.type);
    unicoap_rfc7252_header_set_message_id(_header, properties->rfc7252.id);
    unicoap_rfc7252_header_set_code(_header, code);
    header += sizeof(unicoap_rfc7252_header_t);

    if (unlikely(token_length_size > 0)) {
        if (IS_ACTIVE(CONFIG_UNICOAP_ALLOW_EXTENDED_TOKENS)) {
            unicoap_rfc7252_header_set_token_length_indicator(
                _header, UNICOAP_RFC7252_TOKEN_LENGTH_VERBATIM_UPPER_BOUND + token_length_size);

            if (token_length_size == 1) {
                *header = (uint8_t)(properties->token_length -
                                    UNICOAP_RFC7252_TOKEN_LENGTH_ONE_BYTE_LOWER_BOUND);
            } else {
                assume(token_length_size == 2);
                byteorder_htobebufs(header, properties->token_length -
                                                UNICOAP_RFC7252_TOKEN_LENGTH_TWO_BYTE_LOWER_BOUND);
            }
            header += (size_t)token_length_size;
        } else {
            PDU_7252_DEBUG("extended tokens not supported\n");
            unicoap_assist(API_ERROR("Extended token present")
                           FIXIT("Enable CONFIG_UNICOAP_ALLOW_EXTENDED_TOKENS"));
            return -ENOTSUP;
        }
    } else {
        unicoap_rfc7252_header_set_token_length_indicator(_header,
                                                          (uint8_t)properties->token_length);
    }
    
    if (likely(properties->token_length > 0)) {
        memcpy(header, properties->token, properties->token_length);
    }
    return sizeof(unicoap_rfc7252_header_t) + properties->token_length + token_length_size;
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
