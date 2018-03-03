/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_nanocoap nanocoap small CoAP library
 * @ingroup     net
 * @brief       Provides CoAP functionality optimized for minimal resource usage
 *
 * @{
 *
 * @file
 * @brief       nanocoap API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NET_NANOCOAP_H
#define NET_NANOCOAP_H

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#ifdef RIOT_VERSION
#include "byteorder.h"
#else
#include <arpa/inet.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CoAP port to use
 */
#define COAP_PORT               (5683)

/**
 * @name    Nanocoap specific maximum values
 * @{
 */
#define NANOCOAP_URL_MAX        (64)
#define NANOCOAP_QS_MAX         (64)
/** @} */

/**
 * @name    CoAP option numbers
 * @{
 */
#define COAP_OPT_URI_HOST       (3)
#define COAP_OPT_OBSERVE        (6)
#define COAP_OPT_URI_PATH       (11)
#define COAP_OPT_CONTENT_FORMAT (12)
#define COAP_OPT_URI_QUERY      (15)
/** @} */

/**
 * @name    CoAP packet types
 * @{
 */
#define COAP_REQ                (0)
#define COAP_RESP               (2)
#define COAP_RST                (3)

/**
 * @name    Message types -- confirmable, non-confirmable, etc.
 * @{
 */
#define COAP_TYPE_CON           (0)
#define COAP_TYPE_NON           (1)
#define COAP_TYPE_ACK           (2)
#define COAP_TYPE_RST           (3)
/** @} */

/**
 * @name    CoAP method codes used in header
 * @{
 */
#define COAP_CLASS_REQ          (0)
#define COAP_METHOD_GET         (1)
#define COAP_METHOD_POST        (2)
#define COAP_METHOD_PUT         (3)
#define COAP_METHOD_DELETE      (4)
/** @} */

/**
 * @name    CoAP method flags used in coap_handlers array
 * @{
 */
#define COAP_GET                (0x1)
#define COAP_POST               (0x2)
#define COAP_PUT                (0x4)
#define COAP_DELETE             (0x8)
/** @} */

/**
 * @name    Empty CoAP message code
 * @{
 */
#define COAP_CODE_EMPTY         (0)
/** @} */

/**
 * @name    Response message codes: success
 * @{
 */
#define COAP_CLASS_SUCCESS      (2)
#define COAP_CODE_CREATED      ((2 << 5) | 1)
#define COAP_CODE_DELETED      ((2 << 5) | 2)
#define COAP_CODE_VALID        ((2 << 5) | 3)
#define COAP_CODE_CHANGED      ((2 << 5) | 4)
#define COAP_CODE_204          ((2 << 5) | 4)
#define COAP_CODE_CONTENT      ((2 << 5) | 5)
#define COAP_CODE_205          ((2 << 5) | 5)
#define COAP_CODE_231          ((2 << 5) | 31)
/** @} */

/**
 * @name    Response message codes: client error
 * @{
 */
#define COAP_CLASS_CLIENT_FAILURE             (4)
#define COAP_CODE_BAD_REQUEST                ((4 << 5) | 0)
#define COAP_CODE_UNAUTHORIZED               ((4 << 5) | 1)
#define COAP_CODE_BAD_OPTION                 ((4 << 5) | 2)
#define COAP_CODE_FORBIDDEN                  ((4 << 5) | 3)
#define COAP_CODE_PATH_NOT_FOUND             ((4 << 5) | 4)
#define COAP_CODE_404                        ((4 << 5) | 4)
#define COAP_CODE_METHOD_NOT_ALLOWED         ((4 << 5) | 5)
#define COAP_CODE_NOT_ACCEPTABLE             ((4 << 5) | 6)
#define COAP_CODE_PRECONDITION_FAILED        ((4 << 5) | 0xC)
#define COAP_CODE_REQUEST_ENTITY_TOO_LARGE   ((4 << 5) | 0xD)
#define COAP_CODE_UNSUPPORTED_CONTENT_FORMAT ((4 << 5) | 0xF)
/** @} */

/**
 * @name    Response message codes: server error
 * @{
 */
#define COAP_CLASS_SERVER_FAILURE             (5)
#define COAP_CODE_INTERNAL_SERVER_ERROR      ((5 << 5) | 0)
#define COAP_CODE_NOT_IMPLEMENTED            ((5 << 5) | 1)
#define COAP_CODE_BAD_GATEWAY                ((5 << 5) | 2)
#define COAP_CODE_SERVICE_UNAVAILABLE        ((5 << 5) | 3)
#define COAP_CODE_GATEWAY_TIMEOUT            ((5 << 5) | 4)
#define COAP_CODE_PROXYING_NOT_SUPPORTED     ((5 << 5) | 5)
/** @} */

/**
 * @name    Content types
 * @{
 */
#define COAP_CT_LINK_FORMAT     (40)
#define COAP_CT_XML             (41)
#define COAP_CT_OCTET_STREAM    (42)
#define COAP_CT_EXI             (47)
#define COAP_CT_JSON            (50)
/** @} */

/**
 * @name    Content-Format option codes
 * @{
 */
#define COAP_FORMAT_TEXT         (0)
#define COAP_FORMAT_LINK        (40)
#define COAP_FORMAT_OCTET       (42)
#define COAP_FORMAT_JSON        (50)
#define COAP_FORMAT_CBOR        (60)
/** @brief   nanocoap-specific value to indicate no format specified. */
#define COAP_FORMAT_NONE     (65535)
/** @} */

/**
 * @name    Observe (RFC 7641) constants
 * @{
 */
#define COAP_OBS_REGISTER        (0)
#define COAP_OBS_DEREGISTER      (1)
/** @} */

/**
 * @name    Timing parameters
 * @{
 */
#define COAP_ACK_TIMEOUT        (2U)
#define COAP_RANDOM_FACTOR      (1.5)

/**
 * @brief Maximum variation for confirmable timeout.
 *
 * Must be an integer, defined as:
 *
 *     (COAP_ACK_TIMEOUT * COAP_RANDOM_FACTOR) - COAP_ACK_TIMEOUT
 */
#define COAP_ACK_VARIANCE       (1U)
#define COAP_MAX_RETRANSMIT     (4)
#define COAP_NSTART             (1)
#define COAP_DEFAULT_LEISURE    (5)
/** @} */

/**
 * @brief   Raw CoAP PDU header structure
 */
typedef struct __attribute__((packed)) {
    uint8_t ver_t_tkl;          /**< version, token, token length           */
    uint8_t code;               /**< CoAP code (e.g.m 205)                  */
    uint16_t id;                /**< Req/resp ID                            */
    uint8_t data[];             /**< convenience pointer to payload start   */
} coap_hdr_t;

/**
 * @brief   CoAP option array entry
 */
typedef struct {
    coap_hdr_t *hdr;                /**< pointer to raw packet              */
    uint8_t url[NANOCOAP_URL_MAX];  /**< parsed request URL                 */
    uint8_t qs[NANOCOAP_QS_MAX];    /**< parsed query string                */
    uint8_t *token;                 /**< pointer to token                   */
    uint8_t *payload;               /**< pointer to payload                 */
    unsigned payload_len;           /**< length of payload                  */
    uint16_t content_type;          /**< content type                       */
    uint32_t observe_value;         /**< observe value                      */
} coap_pkt_t;

/**
 * @brief   Resource handler type
 */
typedef ssize_t (*coap_handler_t)(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context);

/**
 * @brief   Type for CoAP resource entry
 */
typedef struct {
    const char *path;               /**< URI path of resource               */
    unsigned methods;               /**< OR'ed methods this resource allows */
    coap_handler_t handler;         /**< ptr to resource handler            */
    void *context;                  /**< ptr to user defined context data   */
} coap_resource_t;

/**
 * @brief   Global CoAP resource list
 */
extern const coap_resource_t coap_resources[];

/**
 * @brief   Number of entries in global CoAP resource list
 */
extern const unsigned coap_resources_numof;

/**
 * @brief   Parse a CoAP PDU
 *
 * This function parses a raw CoAP PDU from @p buf with size @p len and fills
 * the structure pointed to by @p pkt.
 * @p pkt must point to a preallocated coap_pkt_t structure.
 *
 * @param[out]  pkt     structure to parse into
 * @param[in]   buf     pointer to raw packet data
 * @param[in]   len     length of packet at @p buf
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int coap_parse(coap_pkt_t *pkt, uint8_t *buf, size_t len);

/**
 * @brief   Build reply to CoAP request
 *
 * This function can be used to create a reply to any CoAP request packet.  It
 * will create the reply packet header based on parameters from the request
 * (e.g., id, token).  Passing a non-zero @p payload_len will ensure the payload
 * fits into the buffer along with the header.
 *
 * @param[in]   pkt         packet to reply to
 * @param[in]   code        reply code (e.g., COAP_CODE_204)
 * @param[out]  rbuf        buffer to write reply to
 * @param[in]   rlen        size of @p rbuf
 * @param[in]   payload_len length of payload
 *
 * @returns     size of reply packet on success
 * @returns     <0 on error
 */
ssize_t coap_build_reply(coap_pkt_t *pkt, unsigned code,
                         uint8_t *rbuf, unsigned rlen, unsigned payload_len);

/**
 * @brief   Create CoAP reply (convenience function)
 *
 * This is a simple wrapper that allows for building CoAP replies for simple
 * use-cases.
 *
 * The reply will be written to @p buf. Is @p payload and @p payload_len
 * non-zero, the payload will be copied into the resulting reply packet.
 *
 * @param[in]   pkt         packet to reply to
 * @param[in]   code        reply code (e.g., COAP_CODE_204)
 * @param[out]  buf         buffer to write reply to
 * @param[in]   len         size of @p buf
 * @param[in]   ct          content type of payload
 * @param[in]   payload     ptr to payload
 * @param[in]   payload_len length of payload
 *
 * @returns     size of reply packet on success
 * @returns     <0 on error
 */
ssize_t coap_reply_simple(coap_pkt_t *pkt,
                          unsigned code,
                          uint8_t *buf, size_t len,
                          unsigned ct,
                          const uint8_t *payload, uint8_t payload_len);

/**
 * @brief   Handle incoming CoAP request
 *
 * This function will find the correct handler, call it and write the reply
 * into @p resp_buf.
 *
 * @param[in]   pkt             pointer to (parsed) CoAP packet
 * @param[out]  resp_buf        buffer for response
 * @param[in]   resp_buf_len    size of response buffer
 *
 * @returns     size of reply packet on success
 * @returns     <0 on error
 */
ssize_t coap_handle_req(coap_pkt_t *pkt, uint8_t *resp_buf, unsigned resp_buf_len);

/**
 * @brief   Builds a CoAP header
 *
 * Caller *must* ensure @p hdr can hold the header and the full token!
 *
 * @param[out]   hdr        hdr to fill
 * @param[in]    type       CoAP packet type (e.g., COAP_TYPE_CON, ...)
 * @param[in]    token      token
 * @param[in]    token_len  length of @p token
 * @param[in]    code       CoAP code (e.g., COAP_CODE_204, ...)
 * @param[in]    id         CoAP request id
 *
 * @returns      length of resulting header
 */
ssize_t coap_build_hdr(coap_hdr_t *hdr, unsigned type, uint8_t *token,
                       size_t token_len, unsigned code, uint16_t id);

/**
 * @brief   Insert a CoAP option into buffer
 *
 * This function writes a CoAP option with nr. @p onum to @p buf.
 * It handles calculating the option delta (from @p lastonum), encoding the
 * length from @p olen and copying the option data from @p odata.
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 for first option
 * @param[in]   onum        number of option
 * @param[in]   odata       ptr to raw option data (or NULL)
 * @param[in]   olen        length of @p odata (if any)
 *
 * @returns     amount of bytes written to @p buf
 */
size_t coap_put_option(uint8_t *buf, uint16_t lastonum, uint16_t onum, uint8_t *odata, size_t olen);

/**
 * @brief   Insert content type option into buffer
 *
 * @param[out]  buf             buffer to write to
 * @param[in]   lastonum        number of previous option (for delta
 *                              calculation), or 0 if first option
 * @param[in]   content_type    content type to set
 *
 * @returns     amount of bytes written to @p buf
 */
size_t coap_put_option_ct(uint8_t *buf, uint16_t lastonum, uint16_t content_type);

/**
 * @brief   Insert URI encoded option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 if first option
 * @param[in]   uri         ptr to source URI
 * @param[in]   optnum      option number to use (e.g., COAP_OPT_URI_PATH)
 *
 * @returns     amount of bytes written to @p buf
 */
size_t coap_put_option_uri(uint8_t *buf, uint16_t lastonum, const char *uri, uint16_t optnum);

/**
 * @brief   Get the CoAP version number
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     CoAP version number
 */
static inline unsigned coap_get_ver(coap_pkt_t *pkt)
{
    return (pkt->hdr->ver_t_tkl & 0x60) >> 6;
}

/**
 * @brief   Get the message type
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     COAP_TYPE_CON
 * @returns     COAP_TYPE_NON
 * @returns     COAP_TYPE_ACK
 * @returns     COAP_TYPE_RST
 */
static inline unsigned coap_get_type(coap_pkt_t *pkt)
{
    return (pkt->hdr->ver_t_tkl & 0x30) >> 4;
}

/**
 * @brief   Get a message's token length [in byte]
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     length of token in the given message (0-8 byte)
 */
static inline unsigned coap_get_token_len(coap_pkt_t *pkt)
{
    return (pkt->hdr->ver_t_tkl & 0xf);
}

/**
 * @brief   Get a message's code class (3 most significant bits of code)
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     message code class
 */
static inline unsigned coap_get_code_class(coap_pkt_t *pkt)
{
    return pkt->hdr->code >> 5;
}

/**
 * @brief   Get a message's code detail (5 least significant bits of code)
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     message code detail
 */
static inline unsigned coap_get_code_detail(coap_pkt_t *pkt)
{
    return pkt->hdr->code & 0x1f;
}

/**
 * @brief   Get a message's raw code (class + detail)
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     raw message code
 */
static inline unsigned coap_get_code_raw(coap_pkt_t *pkt)
{
    return (unsigned)pkt->hdr->code;
}

/**
 * @brief   Get a message's code in decimal format ((class * 100) + detail)
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     message code in decimal format
 */
static inline unsigned coap_get_code(coap_pkt_t *pkt)
{
    return (coap_get_code_class(pkt) * 100) + coap_get_code_detail(pkt);
}

/**
 * @brief   Get the message ID of the given CoAP packet
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     message ID
 */
static inline unsigned coap_get_id(coap_pkt_t *pkt)
{
    return ntohs(pkt->hdr->id);
}

/**
 * @brief   Get the total header length (4-byte header + token length)
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     total header length
 */
static inline unsigned coap_get_total_hdr_len(coap_pkt_t *pkt)
{
    return sizeof(coap_hdr_t) + coap_get_token_len(pkt);
}

/**
 * @brief   Encode given code class and code detail to raw code
 *
 * @param[in]   class   message code class
 * @param[in]   detail  message code detail
 *
 * @returns     raw message code
 */
static inline uint8_t coap_code(unsigned class, unsigned detail)
{
    return (class << 5) | detail;
}

/**
 * @brief   Write the given raw message code to given CoAP header
 *
 * @param[out]  hdr     CoAP header to write to
 * @param[in]   code    raw message code
 */
static inline void coap_hdr_set_code(coap_hdr_t *hdr, uint8_t code)
{
    hdr->code = code;
}

/**
 * @brief   Set the message type for the given CoAP header
 *
 * @pre     (type := [0-3])
 *
 * @param[out]  hdr     CoAP header to write
 * @param[in]   type    message type as integer value [0-3]
 */
static inline void coap_hdr_set_type(coap_hdr_t *hdr, unsigned type)
{
    /* assert correct range of type */
    assert(!(type & ~0x3));

    hdr->ver_t_tkl &= ~0x30;
    hdr->ver_t_tkl |= type << 4;
}

/**
 * @brief   Convert message code (request method) into a corresponding bit field
 *
 * @param[in]   code    request code denoting the request method
 *
 * @returns     bit field corresponding to the given request method
 */
static inline unsigned coap_method2flag(unsigned code)
{
    return (1 << (code - 1));
}

/**
 * @brief   Identifies a packet containing an observe option
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     true if observe value is set
 * @returns     false if not
 */
static inline bool coap_has_observe(coap_pkt_t *pkt)
{
    return pkt->observe_value != UINT32_MAX;
}

/**
 * @brief   Clears the observe option value from a packet
 *
 * @param[in]   pkt   CoAP packet
 */
static inline void coap_clear_observe(coap_pkt_t *pkt)
{
    pkt->observe_value = UINT32_MAX;
}

/**
 * @brief   Get the value of the observe option from the given packet
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     value of the observe option
 */
static inline uint32_t coap_get_observe(coap_pkt_t *pkt)
{
    return pkt->observe_value;
}

/**
 * @brief   Reference to the default .well-known/core handler defined by the
 *          application
 */
extern ssize_t coap_well_known_core_default_handler(coap_pkt_t *pkt, \
                                                    uint8_t *buf, size_t len,
                                                    void *context);

/**
 * @brief   Resource definition for the default .well-known/core handler
 */
#define COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER \
    { \
        .path = "/.well-known/core", \
        .methods = COAP_GET, \
        .handler = coap_well_known_core_default_handler \
    }

#ifdef __cplusplus
}
#endif
#endif /* NET_NANOCOAP_H */
/** @} */
