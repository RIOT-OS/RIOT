/*
 * SPDX-FileCopyrightText: 2024-2025 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2025 TU Dresden
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
#define UNICOAP_ETAG_LENGTH_MAX          (8)

/**
 * @brief Numbers of bits needed to represent token length
 *
 * Corresponds to TKL field width. As per RFC 7252, token lengths greater than 8 bytes must not be
 * used. Hence, no more than 4 bits are needed, which is why TKL fields never occupy more than
 * 4 bits.
 *
 * Use this constant to define bitfields containing a token length member.
 */
#define UNICOAP_TOKEN_LENGTH_FIXED_WIDTH 4
/** @} */

/* MARK: - RFC 7252 message types */
/**
 * @name RFC 7252 message types
 * @{
 */
/**
 * @brief Number of bits needed to represent @ref unicoap_rfc7252_message_type_t
 */
#define UNICOAP_RFC7252_MESSAGE_TYPE_FIXED_WIDTH 3

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
} __attribute__((__packed__)) unicoap_rfc7252_message_type_t;
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

/* This function is needed by this header, yet public use is discouraged. */
#ifndef DOXYGEN
/**
 * @brief Helper macro generating a CoAP message code from a class prefix
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

/* We want to allow casting the code field in the pdu to this enum.
 * This enum is used in a union in unicoap_message_t.
 * The static_asserts in this header guarantee usage of these enums is safe in a union
 * with an uint8_t code field. */
static_assert(sizeof(unicoap_method_t) == sizeof(uint8_t),
              "Method enum is too wide, please file an issue.");

/**
 * @brief CoAP Signal Message Codes (`7.xx` range)
 *
 * Each of these signaling messages has their own option number space.
 * @see [RFC 8323, Section 5.1](https://datatracker.ietf.org/doc/html/rfc8323#section-5.1)
 */
typedef enum {
    /**
     * @brief Capabilities and settings message (CSM)
     * @see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.3)
     */
    UNICOAP_SIGNAL_CAPABILITIES_SETTINGS = 1,

    /**
     * @brief Ping message
     * @see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.4)
     */
    UNICOAP_SIGNAL_PING = 2,

    /**
     * @brief Pong message
     * @see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.4)
     */
    UNICOAP_SIGNAL_PONG = 3,

    /**
     * @brief Release message
     * @see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.5)
     */
    UNICOAP_SIGNAL_RELEASE = 4,

    /**
     * @brief Abort message
     * @see [RFC 8323])(https://datatracker.ietf.org/doc/html/rfc8323#section-5.6)
     */
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
     * @brief 4.08 Request Entity Incomplete
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
     * @brief 4.13 Request Entity Too Large
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
     * @brief `If-None-Match` option
     *
     * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.8.2)
     */
    UNICOAP_OPTION_IF_NONE_MATCH = 5,

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
     * @brief `Block2` option
     *
     * Used to indicate the block size and number in a block-wise transfer. Used only to transfer a request body.
     *
     * @see [RFC 7959](https://datatracker.ietf.org/doc/html/rfc7959)
     */
    UNICOAP_OPTION_BLOCK2 = 23,

    /**
     * @brief `Block1` option
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

    /**
     * @brief Max-Message-Size option
     * Applies to `7.01` @ref UNICOAP_SIGNAL_CAPABILITIES_SETTINGS message
     *
     * @see [RFC8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.3.1)
     */
    UNICOAP_SIGNALING_CSM_OPTION_MAX_MESSAGE_SIZE = 2,

    /**
     * @brief Blockwise-Transfer option
     * Applies to `7.01` @ref UNICOAP_SIGNAL_CAPABILITIES_SETTINGS message
     *
     * @see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.3.2)
     */
    UNICOAP_SIGNALING_CSM_OPTION_BLOCKWISE_TRANSFER = 4,

    /**
     * @brief Custody option
     * Applies to `7.02` @ref UNICOAP_SIGNAL_PING and `7.03` @ref UNICOAP_SIGNAL_PONG message
     *
     * @see [RFC8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.4.1)
     */
    UNICOAP_SIGNALING_PING_PONG_OPTION_CUSTODY = 2,

    /**
     * @brief Alternative-Address option
     * Applies to `7.04` @ref UNICOAP_SIGNAL_RELEASE message
     *
     * @see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.5)
     */
    UNICOAP_SIGNALING_RELEASE_OPTION_ALTERNATIVE_ADDRESS = 2,

    /**
     * @brief Hold-Off option
     * Applies to `7.04` @ref UNICOAP_SIGNAL_RELEASE message
     *
     * @see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.5)
     */
    UNICOAP_SIGNALING_RELEASE_OPTION_HOLD_OFF = 4,

    /**
     * @brief Bad-CSM-Option  option
     * Applies to `7.05` @ref UNICOAP_SIGNAL_ABORT message
     *
     * @see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323#section-5.6)
     */
    UNICOAP_SIGNALING_ABORT_OPTION_BAD_CSM = 2,
} __attribute__((__packed__)) unicoap_option_number_t;

static_assert(sizeof(unicoap_option_number_t) == sizeof(uint16_t),
              "unicoap_option_number_t has unexpected size");
/** @} */
/** @} */

/**
 * @addtogroup net_unicoap_options_predefined
 * @{
 */
/* MARK: - Content-Format values */
/* Please see the generator in iana/contenttype.js */
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
     * @brief Content type `text/plain; charset=utf-8`
     * @see [RFC2046](https://www.iana.org/go/rfc2046)
     * @see [RFC3676](https://www.iana.org/go/rfc3676)
     * @see [RFC5147](https://www.iana.org/go/rfc5147)
     */
    UNICOAP_FORMAT_TEXT = 0,

    /**
     * @brief Content type `application/cose; cose-type=\"cose-encrypt0\"`
     * @see [RFC9052](https://www.iana.org/go/rfc9052)
     */
    UNICOAP_FORMAT_COSE_ENCRYPT0 = 16,

    /**
     * @brief Content type `application/cose; cose-type=\"cose-mac0\"`
     * @see [RFC9052](https://www.iana.org/go/rfc9052)
     */
    UNICOAP_FORMAT_COSE_MAC0 = 17,

    /**
     * @brief Content type `application/cose; cose-type=\"cose-sign1\"`
     * @see [RFC9052](https://www.iana.org/go/rfc9052)
     */
    UNICOAP_FORMAT_COSE_SIGN1 = 18,

    /**
     * @brief Content type `application/ace+cbor`
     * @see [RFC9200](https://www.iana.org/go/rfc9200)
     */
    UNICOAP_FORMAT_ACE_CBOR = 19,

    /**
     * @brief Content type `image/gif`
     * @see [https://www.w3.org/Graphics/GIF/spec-gif89a.txt](https://www.w3.org/Graphics/GIF/spec-gif89a.txt)
     */
    UNICOAP_FORMAT_IMAGE_GIF = 21,

    /**
     * @brief Content type `image/jpeg`
     * @see [ISO/IEC 10918-5](https://www.itu.int/rec/T-REC-T.871-201105-I/en)
     */
    UNICOAP_FORMAT_IMAGE_JPEG = 22,

    /**
     * @brief Content type `image/png`
     * @see [RFC2083](https://www.iana.org/go/rfc2083)
     */
    UNICOAP_FORMAT_IMAGE_PNG = 23,

    /**
     * @brief Content type `application/link-format`
     * @see [RFC6690](https://www.iana.org/go/rfc6690)
     */
    UNICOAP_FORMAT_LINK = 40,

    /**
     * @brief Content type `application/xml`
     * @see [RFC3023](https://www.iana.org/go/rfc3023)
     */
    UNICOAP_FORMAT_XML = 41,

    /**
     * @brief Content type `application/octet-stream`
     * @see [RFC2045](https://www.iana.org/go/rfc2045)
     * @see [RFC2046](https://www.iana.org/go/rfc2046)
     */
    UNICOAP_FORMAT_OCTET_STREAM = 42,

    /**
     * @brief Content type `application/exi`
     * @see [\"Efficient XML Interchange (EXI) Format 1.0 (Second Edition)\", February 2014](http://www.w3.org/TR/2014/REC-exi-20140211/#contenttypeRegistration)
     */
    UNICOAP_FORMAT_EXI = 47,

    /**
     * @brief Content type `application/json`
     * @see [RFC8259](https://www.iana.org/go/rfc8259)
     */
    UNICOAP_FORMAT_JSON = 50,

    /**
     * @brief Content type `application/json-patch+json`
     * @see [RFC6902](https://www.iana.org/go/rfc6902)
     */
    UNICOAP_FORMAT_JSON_PATCH_JSON = 51,

    /**
     * @brief Content type `application/merge-patch+json`
     * @see [RFC7396](https://www.iana.org/go/rfc7396)
     */
    UNICOAP_FORMAT_MERGE_PATCH_JSON = 52,

    /**
     * @brief Content type `application/cbor`
     * @see [RFC8949](https://www.iana.org/go/rfc8949)
     */
    UNICOAP_FORMAT_CBOR = 60,

    /**
     * @brief Content type `application/cwt`
     * @see [RFC8392](https://www.iana.org/go/rfc8392)
     */
    UNICOAP_FORMAT_CWT = 61,

    /**
     * @brief Content type `application/multipart-core`
     * @see [RFC8710](https://www.iana.org/go/rfc8710)
     */
    UNICOAP_FORMAT_MULTIPART_CORE = 62,

    /**
     * @brief Content type `application/cbor-seq`
     * @see [RFC8742](https://www.iana.org/go/rfc8742)
     */
    UNICOAP_FORMAT_CBOR_SEQ = 63,

    /**
     * @brief Content type `application/edhoc+cbor-seq`
     * @see [RFC9528](https://www.iana.org/go/rfc9528)
     */
    UNICOAP_FORMAT_EDHOC_CBOR_SEQ = 64,

    /**
     * @brief Content type `application/cid-edhoc+cbor-seq`
     * @see [RFC9528](https://www.iana.org/go/rfc9528)
     */
    UNICOAP_FORMAT_CID_EDHOC_CBOR_SEQ = 65,

    /**
     * @brief Content type `application/cose; cose-type=\"cose-encrypt\"`
     * @see [RFC9052](https://www.iana.org/go/rfc9052)
     */
    UNICOAP_FORMAT_COSE_ENCRYPT = 96,

    /**
     * @brief Content type `application/cose; cose-type=\"cose-mac\"`
     * @see [RFC9052](https://www.iana.org/go/rfc9052)
     */
    UNICOAP_FORMAT_COSE_MAC = 97,

    /**
     * @brief Content type `application/cose; cose-type=\"cose-sign\"`
     * @see [RFC9052](https://www.iana.org/go/rfc9052)
     */
    UNICOAP_FORMAT_COSE_SIGN = 98,

    /**
     * @brief Content type `application/cose-key`
     * @see [RFC9052](https://www.iana.org/go/rfc9052)
     */
    UNICOAP_FORMAT_COSE_KEY = 101,

    /**
     * @brief Content type `application/cose-key-set`
     * @see [RFC9052](https://www.iana.org/go/rfc9052)
     */
    UNICOAP_FORMAT_COSE_KEY_SET = 102,

    /**
     * @brief Content type `application/senml+json`
     * @see [RFC8428](https://www.iana.org/go/rfc8428)
     */
    UNICOAP_FORMAT_SENML_JSON = 110,

    /**
     * @brief Content type `application/sensml+json`
     * @see [RFC8428](https://www.iana.org/go/rfc8428)
     */
    UNICOAP_FORMAT_SENSML_JSON = 111,

    /**
     * @brief Content type `application/senml+cbor`
     * @see [RFC8428](https://www.iana.org/go/rfc8428)
     */
    UNICOAP_FORMAT_SENML_CBOR = 112,

    /**
     * @brief Content type `application/sensml+cbor`
     * @see [RFC8428](https://www.iana.org/go/rfc8428)
     */
    UNICOAP_FORMAT_SENSML_CBOR = 113,

    /**
     * @brief Content type `application/senml-exi`
     * @see [RFC8428](https://www.iana.org/go/rfc8428)
     */
    UNICOAP_FORMAT_SENML_EXI = 114,

    /**
     * @brief Content type `application/sensml-exi`
     * @see [RFC8428](https://www.iana.org/go/rfc8428)
     */
    UNICOAP_FORMAT_SENSML_EXI = 115,

    /**
     * @brief Content type `application/yang-data+cbor; id=sid`
     * @see [RFC9254](https://www.iana.org/go/rfc9254)
     */
    UNICOAP_FORMAT_YANG_DATA_CBOR_SID = 140,

    /**
     * @brief Content type `application/coap-group+json`
     * @see [RFC7390](https://www.iana.org/go/rfc7390)
     */
    UNICOAP_FORMAT_COAP_GROUP_JSON = 256,

    /**
     * @brief Content type `application/concise-problem-details+cbor`
     * @see [RFC9290](https://www.iana.org/go/rfc9290)
     */
    UNICOAP_FORMAT_CONCISE_PROBLEM_DETAILS_CBOR = 257,

    /**
     * @brief Content type `application/swid+cbor`
     * @see [RFC9393](https://www.iana.org/go/rfc9393)
     */
    UNICOAP_FORMAT_SWID_CBOR = 258,

    /**
     * @brief Content type `application/pkixcmp`
     * @see [RFC9482](https://www.iana.org/go/rfc9482)
     * @see [RFC-ietf-lamps-rfc6712bis-10](https://www.iana.org/go/draft-ietf-lamps-rfc6712bis-10)
     */
    UNICOAP_FORMAT_PKIXCMP = 259,

    /**
     * @brief Content type `application/yang-sid+json`
     * @see [RFC9595](https://www.iana.org/go/rfc9595)
     */
    UNICOAP_FORMAT_YANG_SID_JSON = 260,

    /**
     * @brief Content type `application/ace-groupcomm+cbor`
     * @see [RFC9594](https://www.iana.org/go/rfc9594)
     */
    UNICOAP_FORMAT_ACE_GROUPCOMM_CBOR = 261,

    /**
     * @brief Content type `application/ace-trl+cbor`
     * @see [RFC-ietf-ace-revoked-token-notification-09](https://www.iana.org/go/draft-ietf-ace-revoked-token-notification-09)
     */
    UNICOAP_FORMAT_ACE_TRL_CBOR = 262,

    /**
     * @brief Content type `application/eat+cwt`
     * @see [RFC-ietf-rats-eat-media-type-12](https://www.iana.org/go/draft-ietf-rats-eat-media-type-12)
     */
    UNICOAP_FORMAT_EAT_CWT = 263,

    /**
     * @brief Content type `application/eat+jwt`
     * @see [RFC-ietf-rats-eat-media-type-12](https://www.iana.org/go/draft-ietf-rats-eat-media-type-12)
     */
    UNICOAP_FORMAT_EAT_JWT = 264,

    /**
     * @brief Content type `application/eat-bun+cbor`
     * @see [RFC-ietf-rats-eat-media-type-12](https://www.iana.org/go/draft-ietf-rats-eat-media-type-12)
     */
    UNICOAP_FORMAT_EAT_BUN_CBOR = 265,

    /**
     * @brief Content type `application/eat-bun+json`
     * @see [RFC-ietf-rats-eat-media-type-12](https://www.iana.org/go/draft-ietf-rats-eat-media-type-12)
     */
    UNICOAP_FORMAT_EAT_BUN_JSON = 266,

    /**
     * @brief Content type `application/eat-ucs+cbor`
     * @see [RFC-ietf-rats-eat-media-type-12](https://www.iana.org/go/draft-ietf-rats-eat-media-type-12)
     */
    UNICOAP_FORMAT_EAT_UCS_CBOR = 267,

    /**
     * @brief Content type `application/eat-ucs+json`
     * @see [RFC-ietf-rats-eat-media-type-12](https://www.iana.org/go/draft-ietf-rats-eat-media-type-12)
     */
    UNICOAP_FORMAT_EAT_UCS_JSON = 268,

    /**
     * @brief Content type `application/coap-eap`
     * @see [RFC-ietf-ace-wg-coap-eap-14](https://www.iana.org/go/draft-ietf-ace-wg-coap-eap-14)
     */
    UNICOAP_FORMAT_COAP_EAP = 269,

    /**
     * @brief Content type `application/dots+cbor`
     * @see [RFC9132](https://www.iana.org/go/rfc9132)
     */
    UNICOAP_FORMAT_DOTS_CBOR = 271,

    /**
     * @brief Content type `application/missing-blocks+cbor-seq`
     * @see [RFC9177](https://www.iana.org/go/rfc9177)
     */
    UNICOAP_FORMAT_MISSING_BLOCKS_CBOR_SEQ = 272,

    /**
     * @brief Content type `application/pkcs7-mime; smime-type=server-generated-key`
     * @see [RFC7030](https://www.iana.org/go/rfc7030)
     * @see [RFC8551](https://www.iana.org/go/rfc8551)
     * @see [RFC9148](https://www.iana.org/go/rfc9148)
     */
    UNICOAP_FORMAT_PKCS7_MIME_SERVER_GENERATED_KEY = 280,

    /**
     * @brief Content type `application/pkcs7-mime; smime-type=certs-only`
     * @see [RFC8551](https://www.iana.org/go/rfc8551)
     * @see [RFC9148](https://www.iana.org/go/rfc9148)
     */
    UNICOAP_FORMAT_PKCS7_MIME_CERTS_ONLY = 281,

    /**
     * @brief Content type `application/pkcs8`
     * @see [RFC5958](https://www.iana.org/go/rfc5958)
     * @see [RFC8551](https://www.iana.org/go/rfc8551)
     * @see [RFC9148](https://www.iana.org/go/rfc9148)
     */
    UNICOAP_FORMAT_PKCS8 = 284,

    /**
     * @brief Content type `application/csrattrs`
     * @see [RFC7030](https://www.iana.org/go/rfc7030)
     * @see [RFC9148](https://www.iana.org/go/rfc9148)
     */
    UNICOAP_FORMAT_CSRATTRS = 285,

    /**
     * @brief Content type `application/pkcs10`
     * @see [RFC5967](https://www.iana.org/go/rfc5967)
     * @see [RFC8551](https://www.iana.org/go/rfc8551)
     * @see [RFC9148](https://www.iana.org/go/rfc9148)
     */
    UNICOAP_FORMAT_PKCS10 = 286,

    /**
     * @brief Content type `application/pkix-cert`
     * @see [RFC2585](https://www.iana.org/go/rfc2585)
     * @see [RFC9148](https://www.iana.org/go/rfc9148)
     */
    UNICOAP_FORMAT_PKIX_CERT = 287,

    /**
     * @brief Content type `application/aif+cbor`
     * @see [RFC9237](https://www.iana.org/go/rfc9237)
     */
    UNICOAP_FORMAT_AIF_CBOR = 290,

    /**
     * @brief Content type `application/aif+json`
     * @see [RFC9237](https://www.iana.org/go/rfc9237)
     */
    UNICOAP_FORMAT_AIF_JSON = 291,

    /**
     * @brief Content type `application/senml+xml`
     * @see [RFC8428](https://www.iana.org/go/rfc8428)
     */
    UNICOAP_FORMAT_SENML_XML = 310,

    /**
     * @brief Content type `application/sensml+xml`
     * @see [RFC8428](https://www.iana.org/go/rfc8428)
     */
    UNICOAP_FORMAT_SENSML_XML = 311,

    /**
     * @brief Content type `application/senml-etch+json`
     * @see [RFC8790](https://www.iana.org/go/rfc8790)
     */
    UNICOAP_FORMAT_SENML_ETCH_JSON = 320,

    /**
     * @brief Content type `application/senml-etch+cbor`
     * @see [RFC8790](https://www.iana.org/go/rfc8790)
     */
    UNICOAP_FORMAT_SENML_ETCH_CBOR = 322,

    /**
     * @brief Content type `application/yang-data+cbor`
     * @see [RFC9254](https://www.iana.org/go/rfc9254)
     */
    UNICOAP_FORMAT_YANG_DATA_CBOR = 340,

    /**
     * @brief Content type `application/yang-data+cbor; id=name`
     * @see [RFC9254](https://www.iana.org/go/rfc9254)
     */
    UNICOAP_FORMAT_YANG_DATA_CBOR_NAME = 341,

    /**
     * @brief Content type `application/td+json`
     * @see [\"Web of Things (WoT) Thing Description 1.1\", April 2022](https://www.w3.org/TR/wot-thing-description11/)
     */
    UNICOAP_FORMAT_TD_JSON = 432,

    /**
     * @brief Content type `application/tm+json`
     * @see [\"Web of Things (WoT) Thing Description 1.1\", April 2022](https://www.w3.org/TR/wot-thing-description11/)
     */
    UNICOAP_FORMAT_TM_JSON = 433,

    /**
     * @brief Content type `application/sdf+json`
     * @see [RFC-ietf-asdf-sdf-23](https://www.iana.org/go/draft-ietf-asdf-sdf-23)
     */
    UNICOAP_FORMAT_SDF_JSON = 434,

    /**
     * @brief Content type `application/uccs+cbor`
     * @see [RFC-ietf-rats-uccs-12, Section 6.4](https://www.iana.org/go/draft-ietf-rats-uccs-12)
     */
    UNICOAP_FORMAT_UCCS_CBOR = 601,

    /**
     * @brief Content type `application/vnd.ocf+cbor`
     * @see [Michael_Koster](https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#Michael_Koster)
     */
    UNICOAP_FORMAT_OCF_CBOR = 10000,

    /**
     * @brief Content type `application/oscore`
     * @see [RFC8613](https://www.iana.org/go/rfc8613)
     */
    UNICOAP_FORMAT_OSCORE = 10001,

    /**
     * @brief Content type `application/javascript`
     * @see [RFC4329](https://www.iana.org/go/rfc4329)
     */
    UNICOAP_FORMAT_JAVASCRIPT = 10002,

    /**
     * @brief Content type `application/eat+cwt; eat_profile=\"tag:psacertified.org,2023:psa#tfm\"`
     * @see [RFC-tschofenig-rats-psa-token-24](https://www.iana.org/go/draft-tschofenig-rats-psa-token-24)
     */
    UNICOAP_FORMAT_EAT_CWT_PSA_TFM = 10003,

    /**
     * @brief Content type `application/eat+cwt; eat_profile=\"tag:psacertified.org,2019:psa#legacy\"`
     * @see [RFC-tschofenig-rats-psa-token-24](https://www.iana.org/go/draft-tschofenig-rats-psa-token-24)
     */
    UNICOAP_FORMAT_EAT_CWT_PSA_LEGACY = 10004,

    /**
     * @brief Content type `application/eat+cwt; eat_profile=2.16.840.1.113741.1.16.1`
     * @see [RFC-ietf-rats-eat-media-type-12](https://www.iana.org/go/draft-ietf-rats-eat-media-type-12)
     * @see [draft-cds-rats-intel-corim-profile-02](https://www.iana.org/go/draft-cds-rats-intel-corim-profile-02)
     */
    UNICOAP_FORMAT_EAT_CWT_INTEL = 10005,

    /**
     * @brief Content type `application/toc+cbor`
     * @see [CE-Binding, Section 6.3.1](https://trustedcomputinggroup.org/wp-content/uploads/TCG-DICE-Concise-Evidence-Binding-for-SPDM-Version-1.0-Revision-54_pub.pdf)
     */
    UNICOAP_FORMAT_TOC_CBOR = 10570,

    /**
     * @brief Content type `application/ce+cbor`
     * @see [CE-Binding, Section 6.3.2](https://trustedcomputinggroup.org/wp-content/uploads/TCG-DICE-Concise-Evidence-Binding-for-SPDM-Version-1.0-Revision-54_pub.pdf)
     */
    UNICOAP_FORMAT_CE_CBOR = 10571,

    /**
     * @brief Content type `application/toc+cbor;profile=2.16.840.1.113741.1.16.1`
     * @see [TCG DICE Concise Evidence Binding for SPDM](https://trustedcomputinggroup.org/wp-content/uploads/TCG-DICE-Concise-Evidence-Binding-for-SPDM-Version-1.1-RC1_10April25.pdf)
     * @see [draft-cds-rats-intel-corim-profile](https://www.iana.org/go/draft-cds-rats-intel-corim-profile)
     */
    UNICOAP_FORMAT_TOC_CBOR_INTEL = 10572,

    /**
     * @brief Content type `application/ce+cbor;profile=2.16.840.1.113741.1.16.1`
     * @see [TCG DICE Concise Evidence Binding for SPDM](https://trustedcomputinggroup.org/wp-content/uploads/TCG-DICE-Concise-Evidence-Binding-for-SPDM-Version-1.1-RC1_10April25.pdf)
     * @see [draft-cds-rats-intel-corim-profile](https://www.iana.org/go/draft-cds-rats-intel-corim-profile)
     */
    UNICOAP_FORMAT_CE_CBOR_INTEL = 10573,

    /**
     * @brief Content type `application/json` in `deflate` coding
     * @see [RFC8259](https://www.iana.org/go/rfc8259)
     * @see [RFC9110, Section 8.4.1.2](https://www.iana.org/go/rfc9110)
     */
    UNICOAP_FORMAT_JSON_DEFLATE = 11050,

    /**
     * @brief Content type `application/cbor` in `deflate` coding
     * @see [RFC8949](https://www.iana.org/go/rfc8949)
     * @see [RFC9110, Section 8.4.1.2](https://www.iana.org/go/rfc9110)
     */
    UNICOAP_FORMAT_CBOR_DEFLATE = 11060,

    /**
     * @brief Content type `application/vnd.oma.lwm2m+tlv`
     * @see [OMA-TS-LightweightM2M-V1_0](http://www.openmobilealliance.org/tech/extref/OMA-TS-LightweightM2M-V1_0.zip)
     */
    UNICOAP_FORMAT_OMA_LWM2M_TLV = 11542,

    /**
     * @brief Content type `application/vnd.oma.lwm2m+json`
     * @see [OMA-TS-LightweightM2M-V1_0](http://www.openmobilealliance.org/tech/extref/OMA-TS-LightweightM2M-V1_0.zip)
     */
    UNICOAP_FORMAT_OMA_LWM2M_JSON = 11543,

    /**
     * @brief Content type `application/vnd.oma.lwm2m+cbor`
     * @see [OMA-TS-LightweightM2M-V1_2](http://www.openmobilealliance.org/wp/Overviews/lightweightm2m_overview.html)
     */
    UNICOAP_FORMAT_OMA_LWM2M_CBOR = 11544,

    /**
     * @brief Content type `text/plain;charset=utf-8` in `zstd` coding
     * @see [Benjamin_Valentin](https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#Benjamin_Valentin)
     */
    UNICOAP_FORMAT_TEXT_ZSTD = 12000,

    /**
     * @brief Content type `application/xml` in `zstd` coding
     * @see [Benjamin_Valentin](https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#Benjamin_Valentin)
     */
    UNICOAP_FORMAT_XML_ZSTD = 12041,

    /**
     * @brief Content type `application/json` in `zstd` coding
     * @see [Benjamin_Valentin](https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#Benjamin_Valentin)
     */
    UNICOAP_FORMAT_JSON_ZSTD = 12050,

    /**
     * @brief Content type `text/css`
     * @see [RFC2318](https://www.iana.org/go/rfc2318)
     */
    UNICOAP_FORMAT_TEXT_CSS = 20000,

    /**
     * @brief Content type `image/svg+xml`
     * @see [https://www.w3.org/TR/SVG/mimereg.html](https://www.w3.org/TR/SVG/mimereg.html)
     */
    UNICOAP_FORMAT_IMAGE_SVG_XML = 30000,
} __attribute__((__packed__)) unicoap_content_format_t;
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif
