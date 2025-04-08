/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

#include <stdint.h>

#include "macros/utils.h"

#include "net/unicoap/config.h"

/**
 * @file
 * @brief   Constants used in CoAP such as option numbers and message codes
 * @ingroup net_unicoap_message
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup net_unicoap_message
 * @{
 */
/* MARK: - CoAP PDUs */
/**
 * @name CoAP PDUs
 * @{
 */
/** @brief CoAP version number */
typedef enum {
    /**
     * @brief Identifier for CoAP version 1 (RFC 7252)
     */
    UNICOAP_COAP_VERSION_1 = 1
} unicoap_protocol_version_t;

/**
 * @brief Marks the boundary between header and payload
 */
#define UNICOAP_PAYLOAD_MARKER           (0xFF)

/**
 * @brief Maximum length of the ETag option in bytes
 */
#define UNICOAP_ETAG_LENGTH_MAX          (8) /**<  */

/**
 * @brief Numbers of bits needed to represent token length
 *
 * Corresponds to TKL field width. As per RFC 7252, token lengths greater than 8 bytes must not be
 * used. Hence, no more than 4 bits are needed, which is why TKL fields never occupy more than
 * 4 bits.
 *
 * Use this constant to define bitfields containg a token length member.
 */
#define UNICOAP_TOKEN_LENGTH_FIXED_WIDTH 4
/** @} */

/* MARK: - RFC 7252 message types */
/**
 * @name RFC 7252 message types
 * @{
 */
/**
 * @brief Number of bits needed to represent @ref unicoap_message_type_t
 */
#define UNICOAP_MESSAGE_TYPE_FIXED_WIDTH 3

/**
 * @brief RFC 7252 message type
 */
typedef enum {
    /** @brief A confirmable message */
    UNICOAP_TYPE_CON = 0,

    /** @brief A non-confirmable message */
    UNICOAP_TYPE_NON = 1,

    /** @brief An acknowledgement message */
    UNICOAP_TYPE_ACK = 2,

    /** @brief An reset message */
    UNICOAP_TYPE_RST = 3
} __attribute__((__packed__)) unicoap_message_type_t;
/** @} */

/* MARK: - Message codes */
/**
 * @name Message codes
 * @{
 */
/**
 * @brief Code for an empty CoAP message
 */
#define UNICOAP_CODE_EMPTY (0)

#ifndef DOXYGEN
/**
 * @brief Helper macro generating a CoAP message code from a class prefix and two digits from 00 to 31.
 */
#  define UNICOAP_CODE(suffix, detail) ((CONCAT(UNICOAP_CODE_CLASS_, suffix) << 5) | detail)
#endif

/**
 * @brief Message code class
 *
 * Corresponds to the upper three bits of a message code
 */
typedef enum {
    /** @brief Message class for requests */
    UNICOAP_CODE_CLASS_REQUEST = 0,

    /** @brief Message class for success responses */
    UNICOAP_CODE_CLASS_RESPONSE_SUCCESS = 2,

    /** @brief Message class for responses indicating a client error */
    UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE = 4,

    /** @brief Message class for responses indicating a server error */
    UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE = 5,

    /** @brief Signaling message */
    UNICOAP_CODE_CLASS_SIGNAL = 7,
} __attribute__((__packed__)) unicoap_code_class_t;

/**
 * @brief CoAP request method codes (`0.xx` range)
 */
typedef enum {
    /** @brief `GET` request (no paylod) */
    UNICOAP_METHOD_GET = 1,

    /** @brief `POST` request (resource processes payload) */
    UNICOAP_METHOD_POST = 2,

    /** @brief `PUT` request (update resource with payload) */
    UNICOAP_METHOD_PUT = 3,

    /** @brief `DELETE` request (no payload, remove resource)*/
    UNICOAP_METHOD_DELETE = 4,

    /** @brief `FETCH` request (RFC 8132) */
    UNICOAP_METHOD_FETCH = 5,

    /** @brief `PATCH` request (RFC 8132) */
    UNICOAP_METHOD_PATCH = 6,

    /** @brief `iPATCH` request (RFC 8132) */
    UNICOAP_METHOD_IPATCH = 7,
} __attribute__((__packed__)) unicoap_method_t;

static_assert(sizeof(unicoap_method_t) == sizeof(uint8_t),
              "Method enum is too wide, please file an issue.");

/**
 * @brief CoAP Signal Message Codes (`7.xx` range)
 */
typedef enum {
    /** @brief Capabilities and settings message (CSM) */
    UNICOAP_SIGNAL_CAPABILITIES_SETTINGS = 1,

    /** @brief Ping message */
    UNICOAP_SIGNAL_PING = 2,

    /** @brief Pong message */
    UNICOAP_SIGNAL_PONG = 3,

    /** @brief Release message */
    UNICOAP_SIGNAL_RELEASE = 4,

    /** @brief Abort message */
    UNICOAP_SIGNAL_ABORT = 5,
} __attribute__((__packed__)) unicoap_signal_t;

static_assert(sizeof(unicoap_signal_t) == sizeof(uint8_t),
              "Signal enum is too wide, please file an issue.");

/**
 * @brief CoAP response status codes
 */
typedef enum {
    /**
     * @brief 2.01 Create
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.1.1)
     */
    UNICOAP_STATUS_CREATED = UNICOAP_CODE(RESPONSE_SUCCESS, 1),

    /**
     * @brief 2.02 Deleted
     *
     * @see [RFC7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.1.2)
     */
    UNICOAP_STATUS_DELETED = UNICOAP_CODE(RESPONSE_SUCCESS, 2),

    /**
     * @brief 2.03 Valid
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.1.3)
     */
    UNICOAP_STATUS_VALID = UNICOAP_CODE(RESPONSE_SUCCESS, 3),

    /**
     * @brief 2.04 Changed
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.1.4)
     */
    UNICOAP_STATUS_CHANGED = UNICOAP_CODE(RESPONSE_SUCCESS, 4),

    /**
     * @brief 2.05 Content
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.1.5)
     */
    UNICOAP_STATUS_CONTENT = UNICOAP_CODE(RESPONSE_SUCCESS, 5),

    /**
     * @brief 2.31 Continue
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7959#section-2.9.1)
     */
    UNICOAP_STATUS_CONTINUE = UNICOAP_CODE(RESPONSE_SUCCESS, 31),

    /**
     * @brief 4.00 Bad Request
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.1)
     */
    UNICOAP_STATUS_BAD_REQUEST = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 0),

    /**
     * @brief 4.01 Unauthorized
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.2)
     */
    UNICOAP_STATUS_UNAUTHORIZED = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 1),

    /**
     * @brief 4.02 Bad Option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.3)
     */
    UNICOAP_STATUS_BAD_OPTION = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 2),

    /**
     * @brief 4.03 Forbidden
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.4)
     */
    UNICOAP_STATUS_FORBIDDEN = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 3),

    /**
     * @brief 4.04 Not Found
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.5)
     */
    UNICOAP_STATUS_PATH_NOT_FOUND = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 4),

    /**
     * @brief 4.05 Method Not Allowed
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.6)
     */
    UNICOAP_STATUS_METHOD_NOT_ALLOWED = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 5),

    /**
     * @brief 4.06 Not Acceptable
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.7
     */
    UNICOAP_STATUS_NOT_ACCEPTABLE = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 6),

    /**
     * @brief 4.08 Request Entitiy Incomplete
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7959#section-2.9.2)
     */
    UNICOAP_STATUS_REQUEST_ENTITY_INCOMPLETE = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 8),

    /**
     * @brief 4.09 Conflict
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc8132#section-3.4)
     */
    UNICOAP_STATUS_CONFLICT = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 9),

    /**
     * @brief 4.12 Precondition Failed
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.8)
     */
    UNICOAP_STATUS_PRECONDITION_FAILED = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 12),

    /**
     * @brief 4.13 Request Entitiy Too Large
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.9)
     * @see [RFC 7959](https://datatracker.ietf.org/doc/html/rfc7959#section-2.9.3)
     */
    UNICOAP_STATUS_REQUEST_ENTITY_TOO_LARGE = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 13),

    /**
     * @brief 4.15 Unsupported Content Format
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.2.10)
     */
    UNICOAP_STATUS_UNSUPPORTED_CONTENT_FORMAT = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 15),

    /**
     * @brief Unprocessable Entity
     *
     * @see [RFC 8132](https://datatracker.ietf.org/doc/html/rfc8132#section-3.4)
     */
    UNICOAP_STATUS_UNPROCESSABLE_ENTITY = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 22),

    /**
     * @brief 4.29 Too Many Requests
     *
     * @see [RFC 8516](https://datatracker.ietf.org/doc/html/rfc8516#section-1)
     */
    UNICOAP_STATUS_TOO_MANY_REQUESTS = UNICOAP_CODE(RESPONSE_CLIENT_FAILURE, 29),

    /**
     * @brief 5.00 Internal Server Error
     *
     * @see [RFC7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.3.1)
     */
    UNICOAP_STATUS_INTERNAL_SERVER_ERROR = UNICOAP_CODE(RESPONSE_SERVER_FAILURE, 0),

    /**
     * @brief 5.01 Not Implemented
     *
     * @see [RFC7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.3.2)
     */
    UNICOAP_STATUS_NOT_IMPLEMENTED = UNICOAP_CODE(RESPONSE_SERVER_FAILURE, 1),

    /**
     * @brief 5.02 Bad Gateway
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.3.3)
     */
    UNICOAP_STATUS_BAD_GATEWAY = UNICOAP_CODE(RESPONSE_SERVER_FAILURE, 2),

    /**
     * @brief 5.03 Service Unavailable
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.3.4)
     */
    UNICOAP_STATUS_SERVICE_UNAVAILABLE = UNICOAP_CODE(RESPONSE_SERVER_FAILURE, 3),

    /**
     * @brief 5.04 Gateway Timeout
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.3.5)
     */
    UNICOAP_STATUS_GATEWAY_TIMEOUT = UNICOAP_CODE(RESPONSE_SERVER_FAILURE, 4),

    /**
     * @brief 5.05 Proxying Not Supported
     *
     * @see [RFC7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.9.3.6)
     */
    UNICOAP_STATUS_PROXYING_NOT_SUPPORTED = UNICOAP_CODE(RESPONSE_SERVER_FAILURE, 5),
} __attribute__((__packed__)) unicoap_status_t;

static_assert(sizeof(unicoap_status_t) == sizeof(uint8_t),
              "Status enum is too wide, please file an issue.");
/** @} */
/** @} */

/**
 * @addtogroup net_unicoap_options
 * @{
 */
/* MARK: - Option numbers */
/**
 * @name Option numbers
 * @{
 */
/**
 * @brief CoAP option number
 *
 * These option numbers are not valid in signaling messages.
 */
typedef enum {
    /**
     * @brief `If-Match` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.8.1)
     */
    UNICOAP_OPTION_IF_MATCH = 1,

    /**
     * @brief `If-None-Match` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.8.2)
     */
    UNICOAP_OPTION_IF_NONE_MATCH = 5,

    /**
     * @brief `Uri-Host` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.8.2)
     */
    UNICOAP_OPTION_URI_HOST = 3,

    /**
     * @brief `ETag` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.6)
     */
    UNICOAP_OPTION_ETAG = 4,

    /**
     * @brief `Observe` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7641#section-2)
     */
    UNICOAP_OPTION_OBSERVE = 6,

    /**
     * @brief `Uri-Port`
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.1)
     */
    UNICOAP_OPTION_URI_PORT = 7,

    /**
     * @brief `Location-Path` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.7)
     */
    UNICOAP_OPTION_LOCATION_PATH = 8,

    /**
     * @brief OSCORE option
     *
     * Indicates that the CoAP message is an OSCORE message and that it contains a
     * compressed COSE object.
     *
     * @see [RFC 8613](https://datatracker.ietf.org/doc/html/rfc8613)
     */
    UNICOAP_OPTION_OSCORE = 9,

    /**
     * @brief `Uri-Path` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.1)
     */
    UNICOAP_OPTION_URI_PATH = 11,

    /**
     * @brief `Content-Format` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.3)
     */
    UNICOAP_OPTION_CONTENT_FORMAT = 12,

    /**
     * @brief `Max-Age` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.5)
     */
    UNICOAP_OPTION_MAX_AGE = 14,

    /**
     * @brief `Uri-Query` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.1)
     *
     * **Examples**: `greet=yes`, `polite` in URI `coap://example.org/hello-world?greet=yes&polite`
     */
    UNICOAP_OPTION_URI_QUERY = 15,

    /**
     * @brief `Hop-Limit` option
     *
     * Used to prevent infinite loops when communicating over multiple proxies.
     *
     * @see [RFC 8768](https://datatracker.ietf.org/doc/html/rfc8768)
     */
    UNICOAP_OPTION_HOP_LIMIT = 16,

    /**
     * @brief `Accept` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.4)
     */
    UNICOAP_OPTION_ACCEPT = 17,

    /**
     * @brief `Q-Block1` option
     *
     * Used for block-wise transfer supporting robust transmission in requests.
     *
     * @see [RFC 9177](https://datatracker.ietf.org/doc/html/rfc9177)
     */
    UNICOAP_OPTION_Q_BLOCK1 = 19,

    /**
     * @brief `Location-Query` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.7)
     */
    UNICOAP_OPTION_LOCATION_QUERY = 20,

    /**
     * @brief EDHOC option
     *
     * Used in a CoAP request to signal that the request payload conveys both an
     * EDHOC message_3 and OSCORE protected data, combined together.
     *
     * @see [draft-ietf-core-oscore-edhoc-02](https://datatracker.ietf.org/doc/draft-ietf-core-oscore-edhoc/02/)
     */
    UNICOAP_OPTION_EDHOC = 21,

    /**
     * @brief `Block1` option
     *
     * Used to indicate the block size and number in a block-wise transfer. Used only to transfer a request body.
     *
     * @see [RFC 7959](https://datatracker.ietf.org/doc/html/rfc7959)
     */
    UNICOAP_OPTION_BLOCK2 = 23,

    /**
     * @brief `Block2` option
     *
     * Used to indicate the block size and number in a block-wise transfer. Used only to transfer a response body.
     *
     * @see [RFC 7959](https://datatracker.ietf.org/doc/html/rfc7959)
     */
    UNICOAP_OPTION_BLOCK1 = 27,

    /**
     * @brief Size2 option
     *
     * Used by clients to request an estimate of a resource's
     * total size from a server during block-wise transfer and by
     * servers to inform clients about the size.
     *
     * @see [RFC 7959](https://datatracker.ietf.org/doc/html/rfc7959)
     * @see [RFC 8613](https://datatracker.ietf.org/doc/html/rfc8613)
     */
    UNICOAP_OPTION_SIZE2 = 28,

    /**
     * @brief Q-Block2 option
     *
     * Used for block-wise transfer supporting robust transmission in responses.
     *
     * @see [RFC 9177](https://datatracker.ietf.org/doc/html/rfc9177)
     */
    UNICOAP_OPTION_Q_BLOCK2 = 31,

    /**
     * @brief `Proxy-Uri` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.2)
     */
    UNICOAP_OPTION_PROXY_URI = 35,

    /**
     * @brief `Proxy-Scheme` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.2)
     */
    UNICOAP_OPTION_PROXY_SCHEME = 39,

    /**
     * @brief Size1 option
     *
     * Used by clients to give servers an estimate of the total request
     * payload size during block-wise server and by servers to indicate
     * the maximum acceptable payload size in a 4.13 = "Request Entity
     * Too Large") response.
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252)
     * @see [RFC 7959](https://datatracker.ietf.org/doc/html/rfc7959)
     * @see [RFC 8613](https://datatracker.ietf.org/doc/html/rfc8613)
     */
    UNICOAP_OPTION_SIZE1 = 60,

    /**
     * @brief Echo option
     *
     * Enables a CoAP server to verify the freshness of a request or to force a
     * client to demonstrate reachability at its claimed network address.
     *
     * @see [RFC 9175](https://datatracker.ietf.org/doc/html/rfc9175)
     */
    UNICOAP_OPTION_ECHO = 252,

    /**
     * @brief suppress CoAP response
     * @see [RFC 7968](https://datatracker.ietf.org/doc/html/rfc7967)
     */
    UNICOAP_OPTION_NO_RESPONSE = 258,

    /**
     * @brief Request-Tag option
     *
     * Allows a CoAP server to match block-wise message fragments belonging to the
     * same request.
     *
     * @see [RFC 9175](https://datatracker.ietf.org/doc/html/rfc9175)
     */
    UNICOAP_OPTION_REQUEST_TAG = 292,
} __attribute__((__packed__)) unicoap_option_number_t;

/**
 * @brief CoAP signaling option number
 *
 * These option numbers are only valid in signaling messages.
 */
typedef enum {
    /**
     * @brief Max-Message-Size option
     * Applies to `7.01` @ref UNICOAP_SIGNAL_CAPABILITIES_SETTINGS message
     */
    UNICOAP_SIGNALING_OTPION_MAX_MESSAGE_SIZE = 2,

    /**
     * @brief Blockwise-Transfer option
     * Applies to `7.01` @ref UNICOAP_SIGNAL_CAPABILITIES_SETTINGS message
     */
    UNICOAP_SIGNALING_OTPION_BLOCKWISE_TRANSFER = 4,

    /**
     * @brief Custody option
     * Applies to `7.02` @ref UNICOAP_SIGNAL_PING and `7.03` @ref UNICOAP_SIGNAL_PONG message
     */
    UNICOAP_SIGNALING_OPTION_CUSTODY = 2,

    /**
     * @brief Alternative-Address option
     * Applies to `7.04` @ref UNICOAP_SIGNAL_RELEASE message
     */
    UNICOAP_SIGNALING_OPTION_ALTERNATIVE_ADDRESS = 2,

    /**
     * @brief Hold-Off option
     * Applies to `7.04` @ref UNICOAP_SIGNAL_RELEASE message
     */
    UNICOAP_SIGNALING_OPTION_HOLD_OFF = 4,

    /**
     * @brief Bad-CSM-Option  option
     * Applies to `7.05` @ref UNICOAP_SIGNAL_ABORT message
     */
    UNICOAP_SIGNALING_OPTION_BAD_CSM = 2,
} __attribute__((__packed__)) unicoap_signaling_option_number_t;
/** @} */
/** @} */

/**
 * @addtogroup net_unicoap_options_predefined
 * @{
 */
/* MARK: - Content-Format values */
/**
 * @name Content-Format values
 * @{
 */
/**
 * @brief `Content-Format` option values
 *
 * @see [Content-Format values assigned by IANA](https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats)
 */
typedef enum {
    /**
     * @brief   Content-Type `plain/text`
     */
    UNICOAP_FORMAT_TEXT = 0,

    /**
     * @brief   Content-Type `application/cose; cose-type="cose-encrypt0"`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
     */
    UNICOAP_FORMAT_COSE_ENCRYPT0 = 16,

    /**
     * @brief   Content-Type `application/cose; cose-type="cose-mac0"`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
     */
    UNICOAP_FORMAT_COSE_MAC0 = 17,

    /**
     * @brief   Content-Type `application/cose; cose-type="cose-sign1"`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
     */
    UNICOAP_FORMAT_COSE_SIGN1 = 18,

    /**
     * @brief   Content-Type `application/ace+cbor`
     * @see     [RFC 9200](https://datatracker.ietf.org/doc/html/rfc9200)
     */
    UNICOAP_FORMAT_ACE_CBOR = 19,

    /**
     * @brief   Content-Type `image/gif`
     * @see     https://www.w3.org/Graphics/GIF/spec-gif89a.txt
     */
    UNICOAP_FORMAT_IMAGE_GIF = 21,

    /**
     * @brief   Content-Type `image/jpeg`
     * @see     [ISO/IEC 10918-5](https://www.itu.int/rec/T-REC-T.871-201105-I/en)
     */
    UNICOAP_FORMAT_IMAGE_JPEG = 22,

    /**
     * @brief   Content-Type `image/png`
     * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc2083)
     */
    UNICOAP_FORMAT_IMAGE_PNG = 23,

    /**
     * @brief   Content-Type `application/link`
     * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc6690)
     */
    UNICOAP_FORMAT_LINK = 40,

    /**
     * @brief   Content-Type `application/xml`
     * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc3023)
     */
    UNICOAP_FORMAT_XML = 41,

    /**
     * @brief   Content-Type `application/octet-stream`
     * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc2045)
     * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc2046)
     */
    UNICOAP_FORMAT_OCTET = 42,

    /**
     * @brief   Content-Type `application/exi`
     * @see     [Efficient XML Interchange (EXI) Format](http://www.w3.org/TR/2014/REC-exi-20140211/#contenttypeRegistration)
     */
    UNICOAP_FORMAT_EXI = 47,

    /**
     * @brief   Content-Type `application/json`
     * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc8259)
     */
    UNICOAP_FORMAT_JSON = 50,

    /**
     * @brief   Content-Type `application/json-patch+json`
     * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc6902)
     */
    UNICOAP_FORMAT_JSON_PATCH_JSON = 51,

    /**
     * @brief   Content-Type `application/merge-patch+json`
     * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc7396)
     */
    UNICOAP_FORMAT_MERGE_PATCH_JSON = 52,

    /**
     * @brief   Content-Type `application/cbor`
     * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc8949)
     */
    UNICOAP_FORMAT_CBOR = 60,

    /**
     * @brief   Content-Type `application/cwt`
     * @see     [RFC 8392](https://datatracker.ietf.org/doc/html/rfc8392)
     */
    UNICOAP_FORMAT_CWT = 61,

    /**
     * @brief   Content-Type `application/multipart-core`
     * @see     [RFC 8710](https://datatracker.ietf.org/doc/html/rfc8710)
     */
    UNICOAP_FORMAT_MULTIPART_CORE = 62,

    /**
     * @brief   Content-Type `application/cbor-seq`
     * @see     [RFC 8742](https://datatracker.ietf.org/doc/html/rfc8742)
     */
    UNICOAP_FORMAT_CBOR_SEQ = 63,

    /**
     * @brief   Content-Type `application/cose; cose-type="cose-encrypt"`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
     */
    UNICOAP_FORMAT_COSE_ENCRYPT = 96,

    /**
     * @brief   Content-Type `application/cose; cose-type="cose-mac"`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
     */
    UNICOAP_FORMAT_COSE_MAC = 97,

    /**
     * @brief   Content-Type `application/cose; cose-type="cose-sign"`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
     */
    UNICOAP_FORMAT_COSE_SIGN = 98,

    /**
     * @brief   Content-Type `application/cose-key`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
     */
    UNICOAP_FORMAT_COSE_KEY = 101,

    /**
     * @brief   Content-Type `application/cose-key-set`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
     */
    UNICOAP_FORMAT_COSE_KEY_SET = 102,

    /**
     * @brief   Content-Type `application/senml+json`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc8428)
     */
    UNICOAP_FORMAT_SENML_JSON = 110,

    /**
     * @brief   Content-Type `application/sensml+json`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc8428)
     */
    UNICOAP_FORMAT_SENSML_JSON = 111,

    /**
     * @brief   Content-Type `application/senml+cbor`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc8428)
     */
    UNICOAP_FORMAT_SENML_CBOR = 112,

    /**
     * @brief   Content-Type `application/sensml+json`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc8428)
     */
    UNICOAP_FORMAT_SENSML_CBOR = 113,

    /**
     * @brief   Content-Type `application/senml+exi`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc8428)
     */
    UNICOAP_FORMAT_SENML_EXI = 114,

    /**
     * @brief   Content-Type `application/sensml+exi`
     * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc8428)
     */
    UNICOAP_FORMAT_SENSML_EXI = 115,

    /**
     * @brief   Content-Type `application/yang-data+cbor; id=sid`
     * @see     [RFC 9254](https://datatracker.ietf.org/doc/html/rfc9254)
     */
    UNICOAP_FORMAT_YANG_DATA_CBOR_SID = 140,

    /**
     * @brief   Content-Type `application/coap-group+json`
     * @see     [RFC 7390](https://datatracker.ietf.org/doc/html/rfc7390)
     */
    UNICOAP_FORMAT_COAP_GROUP_JSON = 256,

    /**
     * @brief   Content-Type `application/concise-problem-details+cbor`
     * @see     [RFC 9290](https://datatracker.ietf.org/doc/html/rfc9290)
     */
    UNICOAP_FORMAT_PROBLEM_DETAILS_CBOR = 257,

    /**
     * @brief   Content-Type `application/swid+cbor`
     * @see     [RFC 9393](https://datatracker.ietf.org/doc/html/rfc9393)
     */
    UNICOAP_FORMAT_SWID_CBOR = 258,

    /**
     * @brief   Content-Type `application/pkixcmp`
     * @see     [draft-ietf-ace-cmpv2-coap-transport](https://datatracker.ietf.org/doc/draft-ietf-ace-cmpv2-coap-transport/)
     * @see     [RFC 4210](https://datatracker.ietf.org/doc/html/rfc4210)
     */
    UNICOAP_FORMAT_PKIXCMP = 259,

    /**
     * @brief   Content-Type `application/dots+cbor`
     * @see     [RFC 9132](https://datatracker.ietf.org/doc/html/rfc9132)
     */
    UNICOAP_FORMAT_DOTS_CBOR = 271,

    /**
     * @brief   Content-Type `application/missing-blocks+cbor-seq`
     * @see     [RFC 9177](https://datatracker.ietf.org/doc/html/rfc9177)
     */
    UNICOAP_FORMAT_MISSING_BLOCKS_CBOR_SEQ = 272,

    /**
     * @brief   Content-Type `application/pkcs7-mime; smime-type=server-generated-key`
     * @see     [RFC 7030](https://datatracker.ietf.org/doc/html/rfc7030)
     * @see     [RFC 8551](https://datatracker.ietf.org/doc/html/rfc8551)
     * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
     */
    UNICOAP_FORMAT_PKCS7_MIME_SERVER_GEN = 280,

    /**
     * @brief   Content-Type `application/pkcs7-mime; smime-type=certs-only`
     * @see     [RFC 8551](https://datatracker.ietf.org/doc/html/rfc8551)
     * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
     */
    UNICOAP_FORMAT_PKCS7_MIME_CERTS_ONLY = 281,

    /**
     * @brief   Content-Type `application/pkcs8`
     * @see     [RFC 5958](https://datatracker.ietf.org/doc/html/rfc5958)
     * @see     [RFC 8551](https://datatracker.ietf.org/doc/html/rfc8551)
     * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
     */
    UNICOAP_FORMAT_PKCS8 = 284,

    /**
     * @brief   Content-Type `application/csrattrs`
     * @see     [RFC 7030](https://datatracker.ietf.org/doc/html/rfc7030)
     * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
     */
    UNICOAP_FORMAT_CSRATTRS = 285,

    /**
     * @brief   Content-Type `application/pkcs10`
     * @see     [RFC 5967](https://datatracker.ietf.org/doc/html/rfc5967)
     * @see     [RFC 8551](https://datatracker.ietf.org/doc/html/rfc8551)
     * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
     */
    UNICOAP_FORMAT_PKCS10 = 286,

    /**
     * @brief   Content-Type `application/pkix-cert`
     * @see     [RFC 2585](https://datatracker.ietf.org/doc/html/rfc2585)
     * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
     */
    UNICOAP_FORMAT_PKIX_CERT = 287,

    /**
     * @brief   Content-Type `application/aif+cbor`
     * @see     [RFC 9237](https://datatracker.ietf.org/doc/html/rfc9237)
     */
    UNICOAP_FORMAT_AIF_CBOR = 290,

    /**
     * @brief   Content-Type `application/aif+json`
     * @see     [RFC 9237](https://datatracker.ietf.org/doc/html/rfc9237)
     */
    UNICOAP_FORMAT_AIF_JSON = 291,

    /**
     * @brief   Content-Type `application/senml+xml`
     * @see     [RFC 9237](https://datatracker.ietf.org/doc/html/rfc8428)
     */
    UNICOAP_FORMAT_SENML_XML = 310,

    /**
     * @brief   Content-Type `application/sensml+xml`
     * @see     [RFC 9237](https://datatracker.ietf.org/doc/html/rfc8428)
     */
    UNICOAP_FORMAT_SENSML_XML = 311,

    /**
     * @brief   Content-Type `application/senml-etch+json`
     * @see     [RFC 8790](https://datatracker.ietf.org/doc/html/rfc8790)
     */
    UNICOAP_FORMAT_SNML_ETCH_JSON = 320,

    /**
     * @brief   Content-Type `application/senml-etch+cbor`
     * @see     [RFC 8790](https://datatracker.ietf.org/doc/html/rfc8790)
     */
    UNICOAP_FORMAT_SNML_ETCH_CBOR = 322,

    /**
     * @brief   Content-Type `application/yang-data+cbor`
     * @see     [RFC 9254](https://datatracker.ietf.org/doc/html/rfc9254)
     */
    UNICOAP_FORMAT_YAML_DATA_CBOR = 340,

    /**
     * @brief   Content-Type `application/yang-data+cbor; id=name`
     * @see     [RFC 9254](https://datatracker.ietf.org/doc/html/rfc9254)
     */
    UNICOAP_FORMAT_YAML_DATA_CBOR_ID_NAME = 341,

    /**
     * @brief   Content-Type `application/td+json`
     * @see     [Web of Things = WoT) Thing Description 1.1](https://www.w3.org/TR/wot-thing-description11/)
     */
    UNICOAP_FORMAT_TD_JSON = 432,

    /**
     * @brief   Content-Type `application/tm+json`
     * @see     [Web of Things = WoT) Thing Description 1.1](https://www.w3.org/TR/wot-thing-description11/)
     */
    UNICOAP_FORMAT_TM_JSON = 433,

    /**
     * @brief   Content-Type `application/dns-message`
     * @see     [draft-ietf-core-dns-over-coap](https://datatracker.ietf.org/doc/draft-ietf-core-dns-over-coap/)
     */
    UNICOAP_FORMAT_DNS_MESSAGE = 553,

    /**
     * @brief   Content-Type `application/voucher-cose+cbor`
     * @see     [draft-ietf-anima-constrained-voucher](https://datatracker.ietf.org/doc/draft-ietf-anima-constrained-voucher/)
     * @note    Temporary registration until April 12, 2024.
     */
    UNICOAP_FORMAT_VOUCER_COSE_CBOR = 836,

    /**
     * @brief   Content-Type `application/vnd.ocf+cbor`
     */
    UNICOAP_FORMAT_VND_OCF_CBOR = 10000,

    /**
     * @brief   Content-Type `application/oscore`
     * @see     [RFC 8613](https://datatracker.ietf.org/doc/html/rfc8613)
     */
    UNICOAP_FORMAT_OSCORE = 10001,

    /**
     * @brief   Content-Type `application/javascript`
     * @see     [RFC 4329](https://datatracker.ietf.org/doc/html/rfc4329)
     */
    UNICOAP_FORMAT_JAVASCRIPT = 10002,

    /**
     * @brief   Content-Type `application/json` with Content Coding `deflate`
     * @see     [RFC 8259](https://datatracker.ietf.org/doc/html/rfc8259)
     * @see     [RFC 9110,  Section 8.4.1.2](https://datatracker.ietf.org/doc/html/rfc9110)
     */
    UNICOAP_FORMAT_JSON_DEFLATE = 11050,

    /**
     * @brief   Content-Type `application/cbor` with Content Coding `deflate`
     * @see     [RFC 8949](https://datatracker.ietf.org/doc/html/rfc8949)
     * @see     [RFC 9110,  Section 8.4.1.2](https://datatracker.ietf.org/doc/html/rfc9110)
     */
    UNICOAP_FORMAT_CBOR_DEFLATE = 11060,

    /**
     * @brief   Content-Type `application/vnd.oma.lwm2m+tlv`
     * @see     [OMA-TS-LightweightM2M-V1_0](https://www.openmobilealliance.org/release/LightweightM2M/V1_0-20170208-A/OMA-TS-LightweightM2M-V1_0-20170208-A.pdf)
     */
    UNICOAP_FORMAT_VND_OMA_LWM2M_TLV = 11542,

    /**
     * @brief   Content-Type `application/vnd.oma.lwm2m+json`
     * @see     [OMA-TS-LightweightM2M-V1_0](https://www.openmobilealliance.org/release/LightweightM2M/V1_0-20170208-A/OMA-TS-LightweightM2M-V1_0-20170208-A.pdf)
     */
    UNICOAP_FORMAT_VND_OMA_LWM2M_JSON = 11543,

    /**
     * @brief   Content-Type `application/vnd.oma.lwm2m+cbor`
     * @see     [OMA-TS-LightweightM2M-V1_2](https://www.openmobilealliance.org/release/LightweightM2M/V1_2-20201110-A/HTML-Version/OMA-TS-LightweightM2M_Core-V1_2-20201110-A.html)
     */
    UNICOAP_FORMAT_VND_OMA_LWM2M_CBOR = 11544,

    /**
     * @brief   Content-Type `text/css`
     * @see     https://datatracker.ietf.org/doc/html/rfc2318
     */
    UNICOAP_FORMAT_TEXT_CSS = 20000,

    /**
     * @brief   Content-Type `image/svg+xml`
     * @see     [RFC 2318](https://www.w3.org/TR/SVG/mimereg.html)
     */
    UNICOAP_FORMAT_IMAGE_SVG_XML = 30000,
} __attribute__((__packed__)) unicoap_content_format_t;
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif
