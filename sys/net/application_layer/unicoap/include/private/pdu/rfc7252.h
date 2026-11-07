/*
 * SPDX-FileCopyrightText: 2026 Carl Seifert
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <assert.h>
#include <stdint.h>
#include "byteorder.h"
#include "net/unicoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup net_unicoap_private
 * @{
 */

/**
 * @file
 * @brief  RFC 7252 PDU
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

/**
 * @name RFC 7252 PDU
 * @{
 */

/** @cond */
#if BYTE_ORDER == BIG_ENDIAN
#  define _UNICOAP_RFC7252_HEADER_BIT_FIELDS(a, b, c) a; b; c;
#elif BYTE_ORDER == LITTLE_ENDIAN
#  define _UNICOAP_RFC7252_HEADER_BIT_FIELDS(a, b, c) c; b; a;
#else
#  error Unknown byte order
#endif
/** @endcond */

/** @brief RFC 7252 header */
typedef struct __attribute__((packed)) {
    /** @cond */
_UNICOAP_RFC7252_HEADER_BIT_FIELDS(
    unicoap_protocol_version_t version : 2,
    unicoap_rfc7252_message_type_t type : 2,
    uint8_t token_length : 4
)
    uint8_t code;
    network_uint16_t message_id;
    /** @endcond */
} unicoap_rfc7252_header_t;

/**
 * @brief Reads the CoAP version field from the header
 *
 * @param[in] header Parsed and validated header
 *
 * @pre The header must have been parsed and validated
 *
 * @returns CoAP version
 */
static inline unicoap_protocol_version_t unicoap_rfc7252_header_get_version(const unicoap_rfc7252_header_t* header)
{
    return header->version;
}

/**
 * @brief Populates the version field in the header
 *
 * @param[in,out] header Header
 * @param version CoAP version field value
 */
static inline void unicoap_rfc7252_header_set_version(unicoap_rfc7252_header_t* header,
                                                      unicoap_protocol_version_t version)
{
    header->version = version;
}

/**
 * @brief  Reads the message type from the header
 *
 * @param[in] header Parsed and validated header
 *
 * @pre The header must have been parsed and validated
 *
 * @returns @ref unicoap_rfc7252_message_type_t
 */
static inline unicoap_rfc7252_message_type_t unicoap_rfc7252_header_get_type(
    const unicoap_rfc7252_header_t* header)
{
    return header->type;
}

/**
 * @brief Populates the message type field in the header
 *
 * @param[in,out] header Header
 * @param type Message type
 */
static inline void unicoap_rfc7252_header_set_type(unicoap_rfc7252_header_t* header,
                                                    unicoap_rfc7252_message_type_t type)
{
    assert((type & ~0x3) == 0);
    header->type = type;
}

/**
 * @brief  Reads the code from the header
 *
 * @param[in] header Parsed and validated header
 *
 * @pre The header must have been parsed and validated
 *
 * @returns Message code
 */
static inline uint8_t unicoap_rfc7252_header_get_code(const unicoap_rfc7252_header_t* header)
{
    return header->code;
}

/**
 * @brief Populates the code field in the header
 *
 * @param[in,out] header Header
 * @param code Message code
 */
static inline void unicoap_rfc7252_header_set_code(unicoap_rfc7252_header_t* header, uint8_t code)
{
    header->code = code;
}

/**
 * @brief  Reads the message ID from the header
 *
 * @param[in] header Parsed and validated header
 *
 * @pre The header must have been parsed and validated
 *
 * @returns 16-bit message ID in host byte order
 */
static inline uint16_t unicoap_rfc7252_header_get_message_id(const unicoap_rfc7252_header_t* header)
{
    return byteorder_ntohs(header->message_id);
}

/**
 * @brief Populates the message ID field in the header
 *
 * @param[in,out] header Header
 * @param id Message ID value in host byte order
 */
static inline void unicoap_rfc7252_header_set_message_id(unicoap_rfc7252_header_t* header,
                                                         uint16_t id)
{
    header->message_id = byteorder_htons(id);
}

/**
 * @brief Reads the `TKL` field from the header indicating the token length
 *
 * @param[in] header Parsed and validated header
 *
 * @pre The header must have been parsed and validated
 *
 * When a `TKL` value of 13 or 14 is read from the header, an 8-bit or 16-bit token length
 * field follows the header and precedes the token, respectively. The `TKL` value 15 is reserved.
 * Smaller values directly indicate the length of the token. Refer to
 * [RFC 8974](https://www.rfc-editor.org/info/rfc8974/#name-extended-tokens) for details on extended
 * tokens and the token length encoding.
 *
 * @returns 4-bit token length field
 */
static inline uint8_t unicoap_rfc7252_header_get_token_length_indicator(
    const unicoap_rfc7252_header_t* header)
{
    return header->token_length;
}

/**
 * @brief Populates the `TKL` field in the header indicating the token length
 *
 * @param[in,out] header Header
 * @param tkl Token length indicator
 *
 * When a `TKL` value of 13 or 14 is written from the header, an 8-bit or 16-bit token length
 * field must follow the header and precedes the token, respectively.
 * The `TKL` value 15 is reserved.
 * Smaller values directly indicate the length of the token. Refer to
 * [RFC 8974](https://www.rfc-editor.org/info/rfc8974/#name-extended-tokens) for details on extended
 * tokens and the token length encoding.
 */
static inline void unicoap_rfc7252_header_set_token_length_indicator(
    unicoap_rfc7252_header_t* header, uint8_t tkl)
{
    assert((tkl & ~0xf) == 0);
    header->token_length = tkl;
}
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
