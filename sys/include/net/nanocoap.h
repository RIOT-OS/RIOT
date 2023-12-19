/*
 * Copyright (C) 2016-18 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *               2018 Inria
 *               2018 Ken Bannister <kb2ma@runbox.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_nanocoap nanoCoAP small CoAP library
 * @ingroup     net
 * @brief       CoAP library optimized for minimal resource usage
 *
 * nanocoap is a toolbox for reading and writing CoAP messages. It provides
 * functions for core header attributes like message type and code. It also
 * provides high and low level interfaces to CoAP options, including Block.
 *
 * nanocoap includes the core structs to store message information. It also
 * provides helper functions for use before sending and after receiving a
 * message, such as coap_parse() to read an incoming message.
 *
 * ## Application APIs
 *
 * This page provides reference documentation for the contents of nanocoap. To
 * use nanocoap in an application, see the functional APIs that are built with
 * it. [nanocoap sock](group__net__nanosock.html) is for a targeted client or
 * server with lesser resource requirements, and [gcoap](group__net__gcoap.html)
 * provides a more featureful messaging hub.
 *
 * ### Option APIs
 *
 * For both server responses and client requests, CoAP uses an Option mechanism
 * to encode message metadata that is not required for each message. For
 * example, the resource URI path is required only for a request, and is encoded
 * as the Uri-Path option.
 *
 * nanocoap sock uses the simpler Buffer API, described in the section
 * _Options Write Buffer API_. gcoap uses the more convenient Packet API,
 * described in the section _Options Write Packet API_.
 *
 * For either API, the caller *must* write options in order by option number
 * (see "CoAP option numbers" in [CoAP defines](group__net__coap.html)).
 *
 * ## Server path matching
 *
 * By default the URI-path of an incoming request should match exactly one of
 * the registered resources. But also, a resource can be configured to
 * match just a prefix of the URI-path of the request by adding the
 * @ref COAP_MATCH_SUBTREE option to coap_resource_t::methods.
 *
 * For example, if a resource is configured with a
 * @ref coap_resource_t::path "path" `/resource01` and the
 * @ref COAP_MATCH_SUBTREE option is used it would match any of `/resource01/`,
 * `/resource01/sub/path`, `/resource01alt`.
 *
 * If the behavior of matching `/resource01alt` is not wanted and only subtrees
 * are wanted to match, the path should be `/resource01/`.
 *
 * If in addition just `/resource01` is wanted to match, together with any
 * subtrees of `/resource01/`, then a first resource with the path `/resource01`
 * and exact matching should be register, and then a second one with the path
 * `/resource01/` and subtree matching.
 *
 * @{
 *
 * @file
 * @brief       nanocoap API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ken Bannister <kb2ma@runbox.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include "bitarithm.h"
#include "bitfield.h"
#include "byteorder.h"
#include "iolist.h"
#include "macros/utils.h"
#include "modules.h"
#include "net/coap.h"
#include "net/sock/udp.h"

#if defined(MODULE_NANOCOAP_RESOURCES)
#include "xfa.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    nanoCoAP specific CoAP method flags used in coap_handlers array
 * @anchor  nanocoap_method_flags
 * @{
 */
#define COAP_GET                (0x01)
#define COAP_POST               (0x02)
#define COAP_PUT                (0x04)
#define COAP_DELETE             (0x08)
#define COAP_FETCH              (0x10)
#define COAP_PATCH              (0x20)
#define COAP_IPATCH             (0x40)
#define COAP_IGNORE             (0xFF)   /**< For situations where the method
                                              is not important */
#define COAP_MATCH_SUBTREE      (0x8000) /**< Path is considered as a prefix
                                              when matching */
/** @} */

/**
 * @brief   nanoCoAP-specific value to indicate no format specified
 */
#define COAP_FORMAT_NONE        (UINT16_MAX)

/**
 * @defgroup net_nanocoap_conf    nanoCoAP compile configurations
 * @ingroup  net_nanocoap
 * @ingroup  config
 * @{
 */
/** @brief   Maximum number of Options in a message */
#ifndef CONFIG_NANOCOAP_NOPTS_MAX
#define CONFIG_NANOCOAP_NOPTS_MAX          (16)
#endif

/**
 * @brief    Maximum length of a resource path string read from or written to
 *           a message
 */
#ifndef CONFIG_NANOCOAP_URI_MAX
#define CONFIG_NANOCOAP_URI_MAX            (64)
#endif

/**
 * @brief    Maximum size for a blockwise transfer as a power of 2
 */
#ifndef CONFIG_NANOCOAP_BLOCK_SIZE_EXP_MAX
#define CONFIG_NANOCOAP_BLOCK_SIZE_EXP_MAX  (6)
#endif

/**
 * @brief CoAP block-wise-transfer size that should be used by default
 */
#ifndef CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT
#define CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT  COAP_BLOCKSIZE_64
#endif

/** @brief   Maximum length of a query string written to a message */
#ifndef CONFIG_NANOCOAP_QS_MAX
#define CONFIG_NANOCOAP_QS_MAX             (64)
#endif
/** @} */

/**
 * @brief    Maximum length of a CoAP header for a blockwise message
 *
 *           Value obtained experimentally when using SUIT
 */
#ifndef CONFIG_NANOCOAP_BLOCK_HEADER_MAX
#define CONFIG_NANOCOAP_BLOCK_HEADER_MAX   (80)
#endif

/**
 * @name coap_opt_finish() flag parameter values
 *
 * Directs packet/buffer updates when user finishes adding options
 * @{
 */
/** @brief    no special handling required */
#define COAP_OPT_FINISH_NONE     (0x0000)
/** @brief    expect a payload to follow */
#define COAP_OPT_FINISH_PAYLOAD  (0x0001)
/** @} */

/**
 * @brief   Raw CoAP PDU header structure
 */
typedef struct __attribute__((packed)) {
    uint8_t ver_t_tkl;          /**< version, token, token length           */
    uint8_t code;               /**< CoAP code (e.g.m 205)                  */
    uint16_t id;                /**< Req/resp ID                            */
} coap_hdr_t;

/**
 * @brief   CoAP option array entry
 */
typedef struct {
    uint16_t opt_num;           /**< full CoAP option number    */
    uint16_t offset;            /**< offset in packet           */
} coap_optpos_t;

/**
 * @brief   CoAP PDU parsing context structure
 *
 * When this struct is used to assemble the header, @p payload is used as the
 * write pointer and @p payload_len contains the number of free bytes left in
 * then packet buffer pointed to by @ref coap_pkt_t::hdr
 *
 * When the header was written, @p payload must not be changed, it must remain
 * pointing to the end of the header.
 * @p payload_len must then be set to the size of the payload that was further
 * copied into the packet buffer, after the header.
 *
 * @ref coap_pkt_t::snips can be used to attach further payload buffers without copying them
 * into the CoAP packet buffer.
 * If there are any, they will be attached in order after the last payload byte
 * (or header byte) in the original CoAP packet buffer.
 */
typedef struct {
    coap_hdr_t *hdr;                                  /**< pointer to raw packet   */
    uint8_t *payload;                                 /**< pointer to end of the header */
    iolist_t *snips;                                  /**< payload snips (optional)*/
    uint16_t payload_len;                             /**< length of payload       */
    uint16_t options_len;                             /**< length of options array */
    coap_optpos_t options[CONFIG_NANOCOAP_NOPTS_MAX]; /**< option offset array     */
    BITFIELD(opt_crit, CONFIG_NANOCOAP_NOPTS_MAX);    /**< unhandled critical option */
#ifdef MODULE_GCOAP
    uint32_t observe_value;                           /**< observe value           */
#endif
} coap_pkt_t;

/**
 * @brief   Forward declaration of internal CoAP resource request handler context
 */
typedef struct _coap_request_ctx coap_request_ctx_t;

/**
 * @brief   Resource handler type
 *
 * Functions that implement this must be prepared to be called multiple times
 * for the same request, as the server implementations do not perform message
 * deduplication. That optimization is [described in the CoAP
 * specification](https://tools.ietf.org/html/rfc7252#section-4.5).
 *
 * This should be trivial for requests of the GET, PUT, DELETE, FETCH and
 * iPATCH methods, as they are defined as idempotent methods in CoAP.
 *
 * For POST, PATCH and other non-idempotent methods, this is an additional
 * requirement introduced by the contract of this type.
 *
 * @param[in]  pkt      The request packet
 * @param[out] buf      Buffer for the response
 * @param[in]  len      Size of the response buffer
 * @param[in]  context  Request context
 *
 * @return     Number of response bytes written on success
 *             Negative error on failure
 */
typedef ssize_t (*coap_handler_t)(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                  coap_request_ctx_t *context);

/**
 * @brief   Coap blockwise request callback descriptor
 *
 * @param[in] arg      Pointer to be passed as arguments to the callback
 * @param[in] offset   Offset of received data
 * @param[in] buf      Pointer to the received data
 * @param[in] len      Length of the received data
 * @param[in] more     -1 for no option, 0 for last block, 1 for more blocks
 *
 * @returns  >=0       on success
 * @returns   <0       on error
 */
typedef int (*coap_blockwise_cb_t)(void *arg, size_t offset, uint8_t *buf, size_t len, int more);

/**
 * @brief   Coap request callback descriptor
 *
 * @param[in] arg      Pointer to be passed as arguments to the callback
 * @param[in] pkt      The received CoAP response.
 *                     Buffers point to network stack internal memory.
 *
 * @returns   >=0       on success
 * @returns    <0       on error
 */
typedef int (*coap_request_cb_t)(void *arg, coap_pkt_t *pkt);

/**
 * @brief   Method flag type
 *
 * Must be large enough to contain all @ref nanocoap_method_flags "CoAP method flags"
 */
typedef uint16_t coap_method_flags_t;

/**
 * @brief   Type for CoAP resource entry
 */
typedef struct {
    const char *path;               /**< URI path of resource               */
    coap_method_flags_t methods;    /**< OR'ed methods this resource allows */
    coap_handler_t handler;         /**< ptr to resource handler            */
    void *context;                  /**< ptr to user defined context data   */
} coap_resource_t;

/**
 * @brief   Type for CoAP resource subtrees
 */
typedef const struct {
    const coap_resource_t *resources;   /**< ptr to resource array  */
    const size_t resources_numof;       /**< number of entries in array */
} coap_resource_subtree_t;

/**
 * @brief   Initialize CoAP request context
 *
 * @param[in] ctx    Pointer to the request context to initialize
 * @param[in] remote Remote endpoint that made the request
 */
void coap_request_ctx_init(coap_request_ctx_t *ctx, sock_udp_ep_t *remote);

/**
 * @brief   CoAP resource request handler context
 * @internal
 */
struct _coap_request_ctx {
    const coap_resource_t *resource;    /**< resource of the request */
    sock_udp_ep_t *remote;              /**< remote endpoint of the request */
#if defined(MODULE_SOCK_AUX_LOCAL) || DOXYGEN
    sock_udp_ep_t *local;               /**< local endpoint of the request */
#endif
#if defined(MODULE_GCOAP) || DOXYGEN
    /**
     * @brief   transport the packet was received over
     * @see     @ref gcoap_socket_type_t for values.
     * @note    @ref gcoap_socket_type_t can not be used, as this would
     *          cyclically include the @ref net_gcoap header.
     */
    uint32_t tl_type;
#endif
};

/* forward declarations */
static inline uint8_t *coap_hdr_data_ptr(const coap_hdr_t *hdr);
static inline size_t coap_hdr_get_token_len(const coap_hdr_t *hdr);
static inline const void * coap_hdr_get_token(const coap_hdr_t *hdr);

/**
 * @brief   Get resource path associated with a CoAP request
 *
 * @param[in]   ctx The request context
 *
 * @return  Resource path of the request
 */
const char *coap_request_ctx_get_path(const coap_request_ctx_t *ctx);

/**
 * @brief   Get resource context associated with a CoAP request
 *
 * @param[in]   ctx The request context
 *
 * @return  Resource context of the request
 */
void *coap_request_ctx_get_context(const coap_request_ctx_t *ctx);

/**
 * @brief   Get transport the packet was received over
 * @see     @ref gcoap_socket_type_t for values.
 *
 * @param[in]   ctx The request context
 *
 * @return  Transport Layer type of the request
 */
uint32_t coap_request_ctx_get_tl_type(const coap_request_ctx_t *ctx);

/**
 * @brief   Get the remote endpoint from which the request was received
 *
 * @param[in]   ctx The request context
 *
 * @return  Remote endpoint from which the request originated
 * @return  NULL    The request was not received via UDP
 */
const sock_udp_ep_t *coap_request_ctx_get_remote_udp(const coap_request_ctx_t *ctx);

/**
 * @brief   Get the local endpoint on which the request has been received
 *
 * @param[in]   ctx The request context
 *
 * @return  Local   endpoint to which the request has been received
 * @return  NULL    The request was not received via UDP
 */
const sock_udp_ep_t *coap_request_ctx_get_local_udp(const coap_request_ctx_t *ctx);

/**
 * @brief   Block1 helper struct
 */
typedef struct {
    size_t offset;                  /**< offset of received data            */
    uint32_t blknum;                /**< block number                       */
    uint8_t szx;                    /**< szx value                          */
    int8_t more;                    /**< -1 for no option, 0 for last block,
                                          1 for more blocks coming          */
} coap_block1_t;

/**
 * @brief Blockwise transfer helper struct
 */
typedef struct {
    size_t start;                   /**< Start offset of the current block  */
    size_t end;                     /**< End offset of the current block    */
    size_t cur;                     /**< Offset of the generated content    */
    uint8_t *opt;                   /**< Pointer to the placed option       */
} coap_block_slicer_t;

#if defined(MODULE_NANOCOAP_RESOURCES) || DOXYGEN
/**
 * @brief   CoAP XFA resource entry
 *
 * @param name  internal name of the resource entry, must be unique
 */
#define NANOCOAP_RESOURCE(name) \
    XFA_CONST(coap_resource_t, coap_resources_xfa, 0) CONCAT(coap_resource_, name) =
#else
/**
 * @brief   Global CoAP resource list
 * @deprecated Use @ref NANOCOAP_RESOURCE instead.
 *             The function will be removed after the 2024.01 release.
 */
extern const coap_resource_t coap_resources[];

/**
 * @brief   Number of entries in global CoAP resource list
 * @deprecated Use @ref NANOCOAP_RESOURCE instead.
 *             The function will be removed after the 2024.01 release.
 */
extern const unsigned coap_resources_numof;
#endif

/**
 * @name    Functions -- Header Read/Write
 *
 * Includes message ID, code, type, token, CoAP version
 */
/**@{*/
/**
 * @brief   Encode given code class and code detail to raw code
 *
 * @param[in]   cls     message code class
 * @param[in]   detail  message code detail
 *
 * @returns     raw message code
 */
static inline uint8_t coap_code(unsigned cls, unsigned detail)
{
    return (cls << 5) | detail;
}

/**
 * @brief   Get a message's code class (3 most significant bits of code)
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     message code class
 */
static inline unsigned coap_get_code_class(const coap_pkt_t *pkt)
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
static inline unsigned coap_get_code_detail(const coap_pkt_t *pkt)
{
    return pkt->hdr->code & 0x1f;
}

/**
 * @brief   Get a message's code in decimal format ((class * 100) + detail)
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     message code in decimal format
 */
static inline unsigned coap_get_code_decimal(const coap_pkt_t *pkt)
{
    return (coap_get_code_class(pkt) * 100) + coap_get_code_detail(pkt);
}

/**
 * @brief   Get a message's raw code (class + detail)
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     raw message code
 */
static inline unsigned coap_get_code_raw(const coap_pkt_t *pkt)
{
    return (unsigned)pkt->hdr->code;
}

/**
 * @brief   Get a request's method type
 *
 * @param[in]   pkt   CoAP request packet
 *
 * @returns     request method type
 */
static inline coap_method_t coap_get_method(const coap_pkt_t *pkt)
{
    return pkt->hdr->code;
}

/**
 * @brief   Get the message ID of the given CoAP packet
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     message ID
 */
static inline unsigned coap_get_id(const coap_pkt_t *pkt)
{
    return ntohs(pkt->hdr->id);
}

/**
 * @brief   Get a message's token length [in byte]
 *
 * If the `nanocoap_token_ext` module is enabled, this will include
 * the extended token length.
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     length of token in the given message (0-8 byte)
 */
static inline unsigned coap_get_token_len(const coap_pkt_t *pkt)
{
    return coap_hdr_get_token_len(pkt->hdr);
}

/**
 * @brief   Get pointer to a message's token
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     pointer to the token position
 */
static inline void *coap_get_token(const coap_pkt_t *pkt)
{
    return coap_hdr_data_ptr(pkt->hdr);
}

/**
 * @brief   Get the total length of a CoAP packet in the packet buffer
 *
 * @note This does not include possible payload snips.
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     total CoAP length
 */
static inline unsigned coap_get_total_len(const coap_pkt_t *pkt)
{
    return (uintptr_t)pkt->payload - (uintptr_t)pkt->hdr + pkt->payload_len;
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
static inline unsigned coap_get_type(const coap_pkt_t *pkt)
{
    return (pkt->hdr->ver_t_tkl & 0x30) >> 4;
}

/**
 * @brief   Get the CoAP version number
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     CoAP version number
 */
static inline unsigned coap_get_ver(const coap_pkt_t *pkt)
{
    return (pkt->hdr->ver_t_tkl & 0x60) >> 6;
}

/**
 * @brief   Get the size of the extended Token length field
 *          (RFC 8974)
 *
 * @note    This requires the `nanocoap_token_ext` module to be enabled
 *
 * @param[in]   hdr   CoAP header
 *
 * @returns     number of bytes used for extended token length
 */
static inline uint8_t coap_hdr_tkl_ext_len(const coap_hdr_t *hdr)
{
    if (!IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        return 0;
    }

    switch (hdr->ver_t_tkl & 0xf) {
    case 13:
        return 1;
    case 14:
        return 2;
    case 15:
        assert(0);
        /* fall-through */
    default:
        return 0;
    }
}

/**
 * @brief   Get the start of data after the header
 *
 * @param[in]   hdr   Header of CoAP packet in contiguous memory
 *
 * @returns     pointer to first byte after the header
 */
static inline uint8_t *coap_hdr_data_ptr(const coap_hdr_t *hdr)
{
    return ((uint8_t *)hdr) + sizeof(coap_hdr_t) + coap_hdr_tkl_ext_len(hdr);
}

/**
 * @brief   Get the total header length (4-byte header + token length)
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     total header length
 */
static inline unsigned coap_get_total_hdr_len(const coap_pkt_t *pkt)
{
    return sizeof(coap_hdr_t) + coap_hdr_tkl_ext_len(pkt->hdr) +
           coap_get_token_len(pkt);
}

/**
 * @brief   Get the header length a response to the given packet will have
 *
 * @param[in]   pkt     CoAP packet to reply to
 * @return      Length of the response header including token excluding
 *              CoAP options and any payload marker
 *
 * @note    The main use case is the use of @ref coap_block2_build_reply, which
 *          is building the CoAP header of the response after options and
 *          payload have been added.
 */
static inline unsigned coap_get_response_hdr_len(const coap_pkt_t *pkt)
{
    return coap_get_total_hdr_len(pkt);
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
 * @brief   Write the given raw message code to given CoAP pkt
 *
 * @param[out]  pkt     CoAP packet to write to
 * @param[in]   code    raw message code
 */
static inline void coap_pkt_set_code(coap_pkt_t *pkt, uint8_t code)
{
    coap_hdr_set_code(pkt->hdr, code);
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
 * @brief       Get the token length of a CoAP over UDP (DTLS) packet
 * @param[in]   hdr     CoAP over UDP header
 * @return      The size of the token in bytes
 *
 * @warning     This API is super goofy. It assumes that the packet is valid
 *              and will read more than `sizeof(*hdr)` into the data `hdr`
 *              points to while crossing fingers hard.
 *
 * @deprecated  This function was introduced to keep legacy code alive.
 *              Introducing new callers should be avoided. In the RX path an
 *              @ref coap_pkt_t will be available, so that you can call
 *              @ref coap_get_token instead. In the TX path the token was
 *              added by us, so we really should know.
 */
static inline size_t coap_hdr_get_token_len(const coap_hdr_t *hdr)
{
    const uint8_t *buf = (const void *)hdr;
    /* Regarding use unnamed magic numbers 13 and 269:
     * - If token length is < 13 it fits into TKL field (4 bit)
     * - If token length is < 269 it fits into 8-bit extended TKL field
     * - Otherwise token length goes into 16-bit extended TKL field.
     *
     * (Not using named constants here, as RFC 8974 also has no names for those
     * magic numbers.)
     *
     * See: https://www.rfc-editor.org/rfc/rfc8974#name-extended-token-length-tkl-f
     */
    switch (coap_hdr_tkl_ext_len(hdr)) {
    case 0:
        return hdr->ver_t_tkl & 0xf;
    case 1:
        return buf[sizeof(coap_hdr_t)] + 13;
    case 2:
        return byteorder_bebuftohs(buf + sizeof(coap_hdr_t)) + 269;
    }

    return 0;
}

/**
 * @brief       Get the Token of a CoAP over UDP (DTLS) packet
 * @param[in]   hdr     CoAP over UDP header
 * @return      The CoAP Token inside the packet that @p hdr belongs to
 *
 * @warning     This API is super goofy. It assumes that the packet is valid
 *              and will read more than `sizeof(*hdr)` into the data `hdr`
 *              points to while crossing fingers hard.
 *
 * @deprecated  This function was introduced to keep legacy code alive.
 *              Introducing new callers should be avoided. In the RX path an
 *              @ref coap_pkt_t will be available, so that you can call
 *              @ref coap_get_token instead. In the TX path the token was
 *              added by us, so we really should know.
 */
static inline const void * coap_hdr_get_token(const coap_hdr_t *hdr)
{
    uint8_t *token = (void *)hdr;
    token += sizeof(*hdr) + coap_hdr_tkl_ext_len(hdr);
    return token;
}

/**
 * @brief       Get the header length of a CoAP packet.
 *
 * @warning     This API is super goofy. It assumes that the packet is valid
 *              and will read more than `sizeof(*hdr)` into the data `hdr`
 *              points to while crossing fingers hard.
 *
 * @deprecated  This function was introduced to keep legacy code alive.
 *              Introducing new callers should be avoided. In the RX path an
 *              @ref coap_pkt_t will be available, so that you can call
 *              @ref coap_get_total_hdr_len instead. In the TX path the header
 *              was created by us (e.g. using @ref coap_build_hdr which returns
 *              the header size), so we really should know already.
 */
static inline size_t coap_hdr_len(const coap_hdr_t *hdr)
{
    return sizeof(*hdr) + coap_hdr_tkl_ext_len(hdr) + coap_hdr_get_token_len(hdr);
}
/**@}*/

/**
 * @name    Functions -- Options Read
 *
 * Read options from a parsed packet.
 *
 * Packets accessed through @ref coap_find_option or any of the `coap_opt_get_*` functions
 * track their access in bit field created at parsing time to enable checking for critical
 * options in @ref coap_has_unprocessed_critical_options.
 * These functions thus have a side effect, and code that calls them on critical options
 * needs to ensure that failure to process the accessed option is propagated into failure
 * to process the message.
 * For example, a server helper that tries to read the If-None-Match option (which is critical)
 * and finds it to be longer than it can process must not return as if no If-None-Match option
 * was present, as it has already triggered the side effect of marking the option as processed.
 */
/**@{*/

/**
 * @brief   Get pointer to an option field by type
 *
 * @param[in]   pkt     packet to work on
 * @param[in]   opt_num the option number to search for
 *
 * @returns     pointer to the option data
 *              NULL if option number was not found
 */
uint8_t *coap_find_option(coap_pkt_t *pkt, unsigned opt_num);

/**
 * @brief   Get pointer to an option field, can be called in a loop
 *          if there are multiple options with the same number
 *
 * @param[in]   pkt     packet to work on
 * @param[in]   opt_num the option number to search for
 * @param[out]  opt_pos opaque, must be set to `NULL` on first call
 * @param[out]  opt_len size of the current option data
 *
 * @returns     pointer to the option data
 *              NULL if option number was not found
 */
uint8_t *coap_iterate_option(coap_pkt_t *pkt, unsigned opt_num,
                             uint8_t **opt_pos, int *opt_len);

/**
 * @brief   Get content type from packet
 *
 * @param[in]   pkt     packet to work on
 *
 * @returns     the packet's content type value if included,
 *              COAP_FORMAT_NONE otherwise
 */
unsigned coap_get_content_type(coap_pkt_t *pkt);

/**
 * @brief   Get the Accept option value from a packet if present
 *
 * @param[in]   pkt     packet to work on
 *
 * @returns     the packet's Accept option value if included,
 *              COAP_FORMAT_NONE otherwise
 */
unsigned coap_get_accept(coap_pkt_t *pkt);

/**
 * @brief   Get a uint32 option value
 *
 * @param[in]   pkt         packet to read from
 * @param[in]   optnum      absolute option number
 * @param[out]  value       the parsed option value
 *
 * @return      0 if the option was found and the value was parsed correctly
 * @return      -ENOENT if the option was not found in @p pkt
 * @return      -ENOSPC if option length is greater than 4 octets
 * @return      -EBADMSG if option value is invalid
 */
int coap_opt_get_uint(coap_pkt_t *pkt, uint16_t optnum, uint32_t *value);

/**
 * @brief   Read a full option as null terminated string into the target buffer
 *
 * This function is for reading and concatenating string based, multi-part CoAP
 * options like COAP_OPT_URI_PATH or COAP_OPT_LOCATION_PATH. It will write all
 * parts of the given option into the target buffer, separating the parts using
 * the given @p separator. The resulting string is `\0` terminated.
 *
 * @param[in]   pkt         packet to read from
 * @param[in]   optnum      absolute option number
 * @param[out]  target      target buffer
 * @param[in]   max_len     size of @p target
 * @param[in]   separator   character used for separating the option parts
 *
 * @return      -ENOSPC if the complete option does not fit into @p target
 * @return      nr of bytes written to @p target (including '\0')
 */
ssize_t coap_opt_get_string(coap_pkt_t *pkt, uint16_t optnum,
                            uint8_t *target, size_t max_len, char separator);

/**
 * @brief   Convenience function for getting the packet's LOCATION_PATH option
 *
 * This function decodes the pkt's LOCATION_PATH option into a '/'-separated and
 * '\0'-terminated string.
 *
 * Caller must ensure @p target can hold at least 2 bytes!
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  target  buffer for location path
 * @param[in]   max_len size of @p target in bytes
 *
 * @returns     -ENOSPC     if URI option is larger than @p max_len
 * @returns     nr of bytes written to @p target (including '\0')
 */
static inline ssize_t coap_get_location_path(coap_pkt_t *pkt,
                                             uint8_t *target, size_t max_len)
{
    return coap_opt_get_string(pkt, COAP_OPT_LOCATION_PATH,
                               target, max_len, '/');
}

/**
 * @brief   Convenience function for getting the packet's LOCATION_QUERY option
 *
 * This function decodes the pkt's LOCATION_PATH option into a '&'-separated and
 * '\0'-terminated string.
 *
 * Caller must ensure @p target can hold at least 2 bytes!
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  target  buffer for location path
 * @param[in]   max_len size of @p target in bytes
 *
 * @returns     -ENOSPC     if URI option is larger than @p max_len
 * @returns     nr of bytes written to @p target (including '\0')
 */
static inline ssize_t coap_get_location_query(coap_pkt_t *pkt,
                                              uint8_t *target, size_t max_len)
{
    return coap_opt_get_string(pkt, COAP_OPT_LOCATION_QUERY,
                               target, max_len, '&');
}

/**
 * @brief   Convenience function for getting the packet's URI_PATH
 *
 * This function decodes the pkt's URI option into a "/"-separated and
 * '\0'-terminated string.
 *
 * Caller must ensure @p target can hold at least CONFIG_NANOCOAP_URI_MAX bytes!
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  target  buffer for target URI
 *
 * @returns     -ENOSPC     if URI option is larger than CONFIG_NANOCOAP_URI_MAX
 * @returns     nr of bytes written to @p target (including '\0')
 */
static inline ssize_t coap_get_uri_path(coap_pkt_t *pkt, uint8_t *target)
{
    return coap_opt_get_string(pkt, COAP_OPT_URI_PATH, target,
                               CONFIG_NANOCOAP_URI_MAX, '/');
}

/**
 * @brief   Convenience function for getting the packet's URI_QUERY option
 *
 * This function decodes the pkt's URI_QUERY option into a "&"-separated and
 * '\0'-terminated string.
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  target  buffer for target URI
 * @param[in]   max_len size of @p target in bytes
 *
 * @returns     -ENOSPC     if URI option is larger than @p max_len
 * @returns     nr of bytes written to @p target (including '\0')
 */
static inline ssize_t coap_get_uri_query_string(coap_pkt_t *pkt, char *target,
                                                size_t max_len)
{
    return coap_opt_get_string(pkt, COAP_OPT_URI_QUERY,
                               (uint8_t *)target, max_len, '&');
}

/**
 * @brief   Find a URI query option of the packet
 *
 * This function searches for a query option of the form "?key=value"
 * and would, when present, return the pointer to "value" when searching
 * for "key".
 *
 * @param[in]   pkt     pkt to work on
 * @param[in]   key     key string to look for
 * @param[out]  value   found value if present, may be NULL
 * @param[out]  len     length of value if present, may be NULL if value is NULL
 *
 * @returns     true if the key was found, false if not
 */
bool coap_find_uri_query(coap_pkt_t *pkt, const char *key,
                         const char **value, size_t *len);

/**
 * @brief   Iterate over a packet's URI Query options
 *
 * This expects that the Uri-Query options follow the widespread format `key=value`
 * or just `key`
 *
 * Key and Value will be copied into the supplied buffers as a NULL-terminated
 * string.
 *
 * @param[in]   pkt           packet to read from
 * @param[out]  ctx           opaque, must be set to `NULL` on first call
 * @param[out]  key           Output buffer for the key
 * @param[in]   key_len_max   Size of the key output buffer
 * @param[out]  value         Output buffer for the value
 * @param[in]   value_len_max Size of the value output buffer
 *
 * @return      2           if key and value were found
 * @return      1           if key was found
 * @return      0           if no key was found
 * @return -E2BIG           if key or value does not fit the supplied space
 */
int coap_iterate_uri_query(coap_pkt_t *pkt, void **ctx,
                           char *key, size_t key_len_max,
                           char *value, size_t value_len_max);

/**
 * @brief   Iterate over a packet's options
 *
 * To start iteration from the first option, set @p init_opt to true. To start
 * iteration from a specific option, set @p init_opt to false, set
 * @p opt->offset to the offset of the desired option from pkt->hdr, and
 * @p opt->opt_num as required. See below for how @p opt->opt_num is modified.
 *
 * With each invocation, this function returns the length of the option value
 * and sets @p value to point to the start of the value. The value for
 * @p opt->opt_num is increased by the delta in the option number value over
 * the preceding option in the packet. So, @p opt->opt_num is accurate if
 * iteration started with the first option. Otherwise, it is useful for
 * identification of repeated options. Finally, @p opt->offset is set to the
 * offset for any following option, to prepare for the next iteration.
 *
 * The end of the options is indicated by a -ENOENT return value. In this case
 * @p value and @p opt are unchanged from their input values.
 *
 * @param[in]     pkt         packet to read from
 * @param[in,out] opt         option attributes; read on input if @p init_opt
 *                            is false
 * @param[out]    value       start of the option value
 * @param[in]     init_opt    true to retrieve first option; false to retrieve
 *                            option at opt->offset
 *
 * @return        length of option value
 * @return        -ENOENT if option not found
 */
ssize_t coap_opt_get_next(const coap_pkt_t *pkt, coap_optpos_t *opt,
                          uint8_t **value, bool init_opt);

/**
 * @brief   Retrieve the value for an option as an opaque array of bytes
 *
 * Retrieves the location and length of the option value of any type. Useful for
 * an opaque option, which essentially is an array of bytes. If more than one
 * option for a given option number, retrieves the first option. To retrieve
 * subsequent options, see coap_opt_get_next().
 *
 * @param[in]     pkt         packet to read from
 * @param[in]     opt_num     option number to retrieve
 * @param[out]    value       start of the option value
 *
 * @return        length of option; 0 if the option exists but is empty
 * @return        -ENOENT if option not found
 * @return        -EINVAL if option cannot be parsed
 */
ssize_t coap_opt_get_opaque(coap_pkt_t *pkt, unsigned opt_num, uint8_t **value);
/**@}*/

/**
 * @brief   Convenience function for getting the packet's Proxy-Uri option
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  target  pointer to the PROXY_URI in @p pkt
 *
 * @pre     ((pkt != NULL) && (target != NULL))
 *
 * @return      length of the Proxy-Uri option
 * @return      -ENOENT if Proxy-Uri option not found
 * @return      -EINVAL if Proxy-Uri option cannot be parsed
 */
static inline ssize_t coap_get_proxy_uri(coap_pkt_t *pkt, char **target)
{
    return coap_opt_get_opaque(pkt, COAP_OPT_PROXY_URI, (uint8_t **)target);
}

/**
 * @name    Functions -- Options for Block
 *
 * Read Block1 (POST/PUT request) or Block2 (GET response) options, and
 * generally useful functions to write block options.
 */
/**@{*/
/**
 * @brief Initialize a block struct from content information
 *
 * @param[out]   block      block struct to initialize
 * @param[in]    blknum     offset from the beginning of content, in terms of
                            @p blksize byte blocks
 * @param[in]    blksize    size of each block; must be a power of 2 between 16
 *                          and 2 raised to #CONFIG_NANOCOAP_BLOCK_SIZE_EXP_MAX
 * @param[in]    more       more blocks? use 1 if yes; 0 if no or unknown
 */
void coap_block_object_init(coap_block1_t *block, size_t blknum, size_t blksize,
                            int more);

/**
 * @brief Finish a block request (block1 or block2)
 *
 * This function finalizes the block response header
 *
 * Checks whether the `more` bit should be set in the block option and
 * sets/clears it if required.  Doesn't return the number of bytes, as this
 * function overwrites bytes in the packet rather than adding new.
 *
 * @param[in]     slicer      Preallocated slicer struct to use
 * @param[in]     option      option number (block1 or block2)
 *
 * @return      true if the `more` bit is set in the block option
 * @return      false if the `more` bit is not set the block option
 */
bool coap_block_finish(coap_block_slicer_t *slicer, uint16_t option);

/**
 * @brief Finish a block1 request
 *
 * This function finalizes the block1 response header
 *
 * Checks whether the `more` bit should be set in the block1 option and
 * sets/clears it if required.  Doesn't return the number of bytes, as this
 * function overwrites bytes in the packet rather than adding new.
 *
 * @param[in]     slicer      Preallocated slicer struct to use
 *
 * @return      true if the `more` bit is set in the block option
 * @return      false if the `more` bit is not set the block option
 */
static inline bool coap_block1_finish(coap_block_slicer_t *slicer)
{
    return coap_block_finish(slicer, COAP_OPT_BLOCK1);
}

/**
 * @brief Finish a block2 response
 *
 * This function finalizes the block2 response header
 *
 * Checks whether the `more` bit should be set in the block2 option and
 * sets/clears it if required.  Doesn't return the number of bytes, as this
 * function overwrites bytes in the packet rather than adding new.
 *
 * @param[in]     slicer      Preallocated slicer struct to use
 *
 * @return      true if the `more` bit is set in the block option
 * @return      false if the `more` bit is not set the block option
 */
static inline bool coap_block2_finish(coap_block_slicer_t *slicer)
{
    return coap_block_finish(slicer, COAP_OPT_BLOCK2);
}

/**
 * @brief Initialize a block2 slicer struct for writing the payload
 *
 * This function determines the size of the response payload based on the
 * size requested by the client in @p pkt.
 *
 * @param[in]   pkt         packet to work on
 * @param[out]  slicer      Preallocated slicer struct to fill
 */
void coap_block2_init(coap_pkt_t *pkt, coap_block_slicer_t *slicer);

/**
 * @brief Initialize a block slicer struct from content information
 *
 * @param[out]   slicer     slicer struct to initialize
 * @param[in]    blknum     offset from the beginning of content, in terms of
                            @p blksize byte blocks
 * @param[in]    blksize    size of each block; must be a power of 2 between 16
 *                          and 2 raised to #CONFIG_NANOCOAP_BLOCK_SIZE_EXP_MAX
 */
void coap_block_slicer_init(coap_block_slicer_t *slicer, size_t blknum,
                            size_t blksize);

/**
 * @brief Add a byte array to a block2 reply.
 *
 * This function is used to add an array of bytes to a CoAP block2 reply. it
 * checks which parts of the string should be added to the reply and ignores
 * parts that are outside the current block2 request.
 *
 * @param[in]   slicer      slicer to use
 * @param[in]   bufpos      pointer to the current payload buffer position
 * @param[in]   c           byte array to copy
 * @param[in]   len         length of the byte array
 *
 * @returns     Number of bytes written to @p bufpos
 */
size_t coap_blockwise_put_bytes(coap_block_slicer_t *slicer, uint8_t *bufpos,
                                const void *c, size_t len);

/**
 * @brief Add a single character to a block2 reply.
 *
 * This function is used to add single characters to a CoAP block2 reply. It
 * checks whether the character should be added to the buffer and ignores it
 * when the character is outside the current block2 request.
 *
 * @param[in]   slicer      slicer to use
 * @param[in]   bufpos      pointer to the current payload buffer position
 * @param[in]   c           character to write
 *
 * @returns     Number of bytes written to @p bufpos
 */
size_t coap_blockwise_put_char(coap_block_slicer_t *slicer, uint8_t *bufpos, char c);

/**
 * @brief    Block option getter
 *
 * This function gets a CoAP packet's block option and parses it into a helper
 * structure.
 *
 * If no block option is present in @p pkt, the values in @p block will be
 * initialized with zero. That implies both block->offset and block->more are
 * also valid in that case, as packet with offset==0 and more==0 means it contains
 * all the payload for the corresponding request.
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  block   ptr to preallocated coap_block1_t structure
 * @param[in]   option  block1 or block2
 *
 * @returns     0 if block option not present
 * @returns     1 if structure has been filled
 */
int coap_get_block(coap_pkt_t *pkt, coap_block1_t *block, uint16_t option);

/**
 * @brief    Block1 option getter
 *
 * This function gets a CoAP packet's block1 option and parses it into a helper
 * structure.
 *
 * If no block1 option is present in @p pkt, the values in @p block1 will be
 * initialized with zero. That implies both block1->offset and block1->more are
 * also valid in that case, as packet with offset==0 and more==0 means it contains
 * all the payload for the corresponding request.
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  block   ptr to preallocated coap_block1_t structure
 *
 * @returns     0 if block1 option not present
 * @returns     1 if structure has been filled
 */
static inline int coap_get_block1(coap_pkt_t *pkt, coap_block1_t *block)
{
    return coap_get_block(pkt, block, COAP_OPT_BLOCK1);
}

/**
 * @brief    Block2 option getter
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  block   ptr to preallocated coap_block1_t structure
 *
 * @returns     0 if block2 option not present
 * @returns     1 if structure has been filled
 */
static inline int coap_get_block2(coap_pkt_t *pkt, coap_block1_t *block)
{
    return coap_get_block(pkt, block, COAP_OPT_BLOCK2);
}

/**
 * @brief    Generic block option getter
 *
 * @param[in]   pkt     pkt to work on
 * @param[in]   option  actual block option number to get
 * @param[out]  blknum  block number
 * @param[out]  szx     SZX value
 *
 * @returns     -1 if option not found
 * @returns     0 if more flag is not set
 * @returns     1 if more flag is set
 */
int coap_get_blockopt(coap_pkt_t *pkt, uint16_t option, uint32_t *blknum, uint8_t *szx);

/**
 * @brief    Check whether any of the packet's options that are critical
 *
 * (i.e must be understood by the receiver, indicated by a 1 in the option number's least
 * significant bit) were not accessed since the packet was parsed.
 *
 * Call this in a server on requests after all their option processing has happened,
 * and stop processing the request if it returns true, returning a 4.02 Bad Option response.
 *
 * Call this in a client when receiving a response before acting on it;
 * consider the response unprocessable if it returns true.
 *
 * @param[in]   pkt     pkt to work on
 *
 * @returns true if any of the options marked as critical at parse time have not been accessed.
 * @returns false if there are no critical options, or all have been accessed.
 */
bool coap_has_unprocessed_critical_options(const coap_pkt_t *pkt);

/**
 * @brief   Helper to decode SZX value to size in bytes
 *
 * @param[in]   szx     SZX value to decode
 *
 * @returns     SZX value decoded to bytes
 */
#define coap_szx2size(szx) (1U << ((szx) + 4))

/**
 * @brief   Helper to encode byte size into next equal or smaller SZX value
 *
 * @param[in]   len     Size in bytes
 *
 * @returns     closest SZX value that fits into a buffer of @p len
 */
static inline unsigned coap_size2szx(unsigned len)
{
    assert(len >= 16);
    return bitarithm_msb(len >> 4);
}
/**@}*/

/**
 * @name    Functions -- Options Write Packet API
 * @anchor  net_nanocoap_opt_add
 *
 * Use a coap_pkt_t struct to manage writing Options to the PDU.
 *
 * The caller must monitor space remaining in the buffer; however, the API
 * *will not* write past the end of the buffer, and returns -ENOSPC when it is
 * full.
 */
/**@{*/
/**
 * @brief   Add block option in descriptive use from a slicer object
 *
 * When calling this function to initialize a packet with a block option, the
 * more flag must be set to prevent the creation of an option with a length too
 * small to contain the size bit.

 * @post pkt.payload advanced to first byte after option
 * @post pkt.payload_len reduced by option length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     slicer      coap blockwise slicer helper struct
 * @param[in]     more        more flag (1 or 0)
 * @param[in]     option      option number (block1 or block2)
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
ssize_t coap_opt_add_block(coap_pkt_t *pkt, coap_block_slicer_t *slicer,
                           bool more, uint16_t option);

/**
 * @brief   Add block1 option in descriptive use from a slicer object
 *
 * When calling this function to initialize a packet with a block option, the
 * more flag must be set to prevent the creation of an option with a length too
 * small to contain the size bit.

 * @post pkt.payload advanced to first byte after option
 * @post pkt.payload_len reduced by option length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     slicer      coap blockwise slicer helper struct
 * @param[in]     more        more flag (1 or 0)
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
static inline ssize_t coap_opt_add_block1(coap_pkt_t *pkt,
                                          coap_block_slicer_t *slicer, bool more)
{
    return coap_opt_add_block(pkt, slicer, more, COAP_OPT_BLOCK1);
}

/**
 * @brief   Add block2 option in descriptive use from a slicer object
 *
 * When calling this function to initialize a packet with a block option, the
 * more flag must be set to prevent the creation of an option with a length too
 * small to contain the size bit.

 * @post pkt.payload advanced to first byte after option
 * @post pkt.payload_len reduced by option length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     slicer      coap blockwise slicer helper struct
 * @param[in]     more        more flag (1 or 0)
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
static inline ssize_t coap_opt_add_block2(coap_pkt_t *pkt,
                                          coap_block_slicer_t *slicer, bool more)
{
    return coap_opt_add_block(pkt, slicer, more, COAP_OPT_BLOCK2);
}
/**
 * @brief   Encode the given uint option into pkt
 *
 * @post pkt.payload advanced to first byte after option
 * @post pkt.payload_len reduced by option length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     optnum      option number to use
 * @param[in]     value       uint to encode
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
ssize_t coap_opt_add_uint(coap_pkt_t *pkt, uint16_t optnum, uint32_t value);

/**
 * @brief   Encode the given block1 option in control use
 *
 * @post pkt.payload advanced to first byte after option
 * @post pkt.payload_len reduced by option length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     block       block to encode
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
static inline ssize_t coap_opt_add_block1_control(coap_pkt_t *pkt, coap_block1_t *block) {
    return coap_opt_add_uint(pkt, COAP_OPT_BLOCK1,
                             (block->blknum << 4) | block->szx | (block->more ? 0x8 : 0));
}

/**
 * @brief   Encode the given block2 option in control use
 *
 * @post pkt.payload advanced to first byte after option
 * @post pkt.payload_len reduced by option length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     block       block to encode
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
static inline ssize_t coap_opt_add_block2_control(coap_pkt_t *pkt, coap_block1_t *block) {
    /* block.more must be zero, so no need to 'or' it in */
    return coap_opt_add_uint(pkt, COAP_OPT_BLOCK2,
                             (block->blknum << 4) | block->szx);
}

/**
 * @brief   Append an Accept option to the pkt buffer
 *
 * @post pkt.payload advanced to first byte after option
 * @post pkt.payload_len reduced by option length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     format      COAP_FORMAT_xxx to accept
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
static inline ssize_t coap_opt_add_accept(coap_pkt_t *pkt, uint16_t format)
{
    return coap_opt_add_uint(pkt, COAP_OPT_ACCEPT, format);
}

/**
 * @brief   Append a Content-Format option to the pkt buffer
 *
 * @post pkt.payload advanced to first byte after option
 * @post pkt.payload_len reduced by option length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     format      COAP_FORMAT_xxx to use
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
static inline ssize_t coap_opt_add_format(coap_pkt_t *pkt, uint16_t format)
{
    return coap_opt_add_uint(pkt, COAP_OPT_CONTENT_FORMAT, format);
}

/**
 * @brief   Encode the given buffer as an opaque data option into pkt
 *
 * @post pkt.payload advanced to first byte after option(s)
 * @post pkt.payload_len reduced by option(s) length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     optnum      option number to use
 * @param[in]     val         pointer to the value to be set
 * @param[in]     val_len     length of val
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options
 */
ssize_t coap_opt_add_opaque(coap_pkt_t *pkt, uint16_t optnum, const void *val, size_t val_len);

/**
 * @brief   Adds a single Uri-Query option in the form 'key=value' into pkt
 *
 *
 * @param[in,out] pkt         Packet being built
 * @param[in]     key         Key to add to the query string
 * @param[in]     key_len     Length of @p key
 * @param[in]     val         Value to assign to @p key (may be NULL)
 * @param[in]     val_len     Length of @p val. 0 if @p val is NULL
 *
 * @pre     ((pkt != NULL) && (key != NULL) && (key_len > 0)
 *              && ((val_len == 0) || ((val != NULL) && (val_len > 0))))
 *
 * @return        number of bytes written to pkt buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or pkt full
 */
ssize_t coap_opt_add_uri_query2(coap_pkt_t *pkt, const char *key, size_t key_len,
                                const char *val, size_t val_len);

/**
 * @brief   Adds a single Uri-Query option in the form 'key=value' into pkt
 *
 * @note Use this only for null-terminated string. See @ref coap_opt_add_uri_query2()
 *       for non null-terminated string.
 *
 * @param[in,out] pkt         Packet being built
 * @param[in]     key         Key to add to the query string
 * @param[in]     val         Value to assign to @p key (may be NULL)
 *
 * @pre     ((pkt != NULL) && (key != NULL))
 *
 * @return        number of bytes written to pkt buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or pkt full
 */
static inline ssize_t coap_opt_add_uri_query(coap_pkt_t *pkt, const char *key,
                                             const char *val)
{
    return coap_opt_add_uri_query2(pkt, key, strlen(key), val, val ? strlen(val) : 0);
}

/**
 * @brief   Adds a single Proxy-URI option into @p pkt
 *
 * @note @p uri must be a NULL-terminated absolute URI
 *
 * @param[in,out] pkt         Packet being built
 * @param[in]     uri         absolute proxy URI
 *
 * @pre     ((pkt != NULL) && (uri != NULL))
 *
 * @return        number of bytes written to pkt buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or pkt full
 */
ssize_t coap_opt_add_proxy_uri(coap_pkt_t *pkt, const char *uri);

/**
 * @brief   Encode the given array of characters as option(s) into pkt
 *
 * Use separator to split array of characters into multiple options.
 *
 * @post pkt.payload advanced to first byte after option(s)
 * @post pkt.payload_len reduced by option(s) length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     optnum      option number to use
 * @param[in]     chars       array of characters to encode as option
 * @param[in]     chars_len   length of @p chars
 * @param[in]     separator   character used in @p string to separate parts
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
ssize_t coap_opt_add_chars(coap_pkt_t *pkt, uint16_t optnum, const char *chars,
                           size_t chars_len, char separator);

/**
 * @brief   Encode the given string as option(s) into pkt
 *
 * Use separator to split string into multiple options.
 *
 * @post pkt.payload advanced to first byte after option(s)
 * @post pkt.payload_len reduced by option(s) length
 *
 * @param[in,out] pkt         pkt referencing target buffer
 * @param[in]     optnum      option number to use
 * @param[in]     string      string to encode as option
 * @param[in]     separator   character used in @p string to separate parts
 *
 * @return        number of bytes written to buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or insufficient buffer space
 */
static inline ssize_t coap_opt_add_string(coap_pkt_t *pkt, uint16_t optnum,
                                          const char *string, char separator)
{
    return coap_opt_add_chars(pkt, optnum, string, strlen(string), separator);
}

/**
 * @brief   Adds one or multiple Uri-Path options in the form '/path' into pkt
 *
 * @note Use this only for null-terminated strings.
 *
 * @param[in,out] pkt         Packet being built
 * @param[in]     path        `\0`-terminated resource (sub)path
 *
 * @pre     ((pkt != NULL) && (path != NULL))
 *
 * @return        number of bytes written to pkt buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or pkt full
 */
static inline ssize_t coap_opt_add_uri_path(coap_pkt_t *pkt, const char *path)
{
    return coap_opt_add_string(pkt, COAP_OPT_URI_PATH, path, '/');
}

/**
 * @brief   Adds one or multiple Uri-Path options in the form '/path' into pkt
 *
 * @note Use this only for null-terminated strings.
 *
 * @param[in,out] pkt         Packet being built
 * @param[in]     path        Resource (sub)path
 * @param[in]     path_len    Length of @p path
 *
 * @pre     ((pkt != NULL) && (path != NULL))
 *
 * @return        number of bytes written to pkt buffer
 * @return        <0 on error
 * @return        -ENOSPC if no available options or pkt full
 */
static inline ssize_t coap_opt_add_uri_path_buffer(coap_pkt_t *pkt,
                                                   const char *path,
                                                   size_t path_len)
{
    return coap_opt_add_chars(pkt, COAP_OPT_URI_PATH, path, path_len, '/');
}

/**
 * @brief   Finalizes options as required and prepares for payload
 *
 * @post pkt.payload advanced to first available byte after options
 * @post pkt.payload_len is maximum bytes available for payload
 *
 * @param[in,out] pkt         pkt to update
 * @param[in]     flags       see COAP_OPT_FINISH... macros
 *
 * @return        total number of bytes written to buffer
 * @return        -ENOSPC if no buffer space for payload marker
 */
ssize_t coap_opt_finish(coap_pkt_t *pkt, uint16_t flags);

/**
 * @brief   Removes an option previously added with function in
 *          [the coap_opt_add_...() group](@ref net_nanocoap_opt_add)
 *
 * @param[in] pkt       Packet to remove option from.
 * @param[in] optnum    Option number of the option to remove. If the option appears more than once,
 *                      the first occurrence will be removed
 *
 * @pre     pkt != NULL
 * @pre     The packet was finished using @ref coap_opt_finish() and any payload is in its final
 *          configuration.
 *
 * @return  The new size of the packet with option of number @p optnum removed. In turn this means
 *          if there is no such option in the packet, the original size of the packet will be
 *          removed.
 * @return  -EINVAL if there was a parsing error for the existing options
 */
ssize_t coap_opt_remove(coap_pkt_t *pkt, uint16_t optnum);
/**@}*/

/**
 * @name    Functions -- Options Write Buffer API
 *
 * Write PDU Options directly to the array of bytes for a message.
 *
 * The caller must provide the last option number written as well as the buffer
 * position. The caller is primarily responsible for tracking and managing the
 * space remaining in the buffer.
 */
/**@{*/
/**
 * @brief   Insert block option into buffer
 *
 * When calling this function to initialize a packet with a block option, the
 * more flag must be set to prevent the creation of an option with a length too
 * small to contain the size bit.
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option, must be < @p option
 * @param[in]   slicer      coap blockwise slicer helper struct
 * @param[in]   more        more flag (1 or 0)
 * @param[in]   option      option number (block1 or block2)
 *
 * @returns     amount of bytes written to @p buf
 */
size_t coap_opt_put_block(uint8_t *buf, uint16_t lastonum, coap_block_slicer_t *slicer,
                          bool more, uint16_t option);

/**
 * @brief   Insert block1 option into buffer
 *
 * When calling this function to initialize a packet with a block1 option, the
 * more flag must be set to prevent the creation of an option with a length too
 * small to contain the size bit.
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          must be < 27
 * @param[in]   slicer      coap blockwise slicer helper struct
 * @param[in]   more        more flag (1 or 0)
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_block1(uint8_t *buf, uint16_t lastonum,
                                         coap_block_slicer_t *slicer, bool more)
{
    return coap_opt_put_block(buf, lastonum, slicer, more, COAP_OPT_BLOCK1);
}

/**
 * @brief   Insert block2 option into buffer
 *
 * When calling this function to initialize a packet with a block2 option, the
 * more flag must be set to prevent the creation of an option with a length too
 * small to contain the size bit.
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          must be < 23
 * @param[in]   slicer      coap blockwise slicer helper struct
 * @param[in]   more        more flag (1 or 0)
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_block2(uint8_t *buf, uint16_t lastonum,
                                         coap_block_slicer_t *slicer, bool more)
{
    return coap_opt_put_block(buf, lastonum, slicer, more, COAP_OPT_BLOCK2);
}

/**
 * @brief   Encode the given uint option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 for first option
 * @param[in]   onum        number of option
 * @param[in]   value       value to encode
 *
 * @returns     amount of bytes written to @p buf
 */
size_t coap_opt_put_uint(uint8_t *buf, uint16_t lastonum, uint16_t onum,
                         uint32_t value);

/**
 * @brief   Insert block1 option into buffer in control usage
 *
 * @param[in]   buf         buffer to write to
 * @param[in]   lastonum    last option number (must be < 27)
 * @param[in]   block       block option attribute struct
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_block1_control(uint8_t *buf, uint16_t lastonum,
                                                 coap_block1_t *block)
{
    return coap_opt_put_uint(buf, lastonum, COAP_OPT_BLOCK1,
                             (block->blknum << 4) | block->szx | (block->more ? 0x8 : 0));
}

/**
 * @brief   Insert block2 option into buffer in control usage
 *
 * Forces value of block 'more' attribute to zero, per spec.
 *
 * @param[in]   buf         buffer to write to
 * @param[in]   lastonum    last option number (must be < 27)
 * @param[in]   block       block option attribute struct
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_block2_control(uint8_t *buf, uint16_t lastonum,
                                                 coap_block1_t *block)
{
    /* block.more must be zero, so no need to 'or' it in */
    return coap_opt_put_uint(buf, lastonum, COAP_OPT_BLOCK2,
                             (block->blknum << 4) | block->szx);
}

/**
 * @brief   Insert an CoAP Observe Option into the buffer
 *
 * @param[out]  buf         Buffer to write to
 * @param[in]   lastonum    last option number (must be < 6)
 * @param[in]   obs         observe number to write
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_observe(uint8_t *buf, uint16_t lastonum,
                                          uint32_t obs)
{
    obs &= COAP_OBS_MAX_VALUE_MASK; /* trim obs down to 24 bit */
    return coap_opt_put_uint(buf, lastonum, COAP_OPT_OBSERVE, obs);
}

/**
 * @brief   Encode the given string as multi-part option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 if first option
 * @param[in]   optnum      option number to use
 * @param[in]   string      string to encode as option
 * @param[in]   len         length of the string
 * @param[in]   separator   character used in @p string to separate parts
 *
 * @return      number of bytes written to @p buf
 */
size_t coap_opt_put_string_with_len(uint8_t *buf, uint16_t lastonum, uint16_t optnum,
                                    const char *string, size_t len, char separator);
/**
 * @brief   Encode the given string as multi-part option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 if first option
 * @param[in]   optnum      option number to use
 * @param[in]   string      string to encode as option
 * @param[in]   separator   character used in @p string to separate parts
 *
 * @return      number of bytes written to @p buf
 */
static inline size_t coap_opt_put_string(uint8_t *buf, uint16_t lastonum,
                                         uint16_t optnum,
                                         const char *string, char separator)
{
    return coap_opt_put_string_with_len(buf, lastonum, optnum,
                                        string, strlen(string), separator);
}

/**
 * @brief   Convenience function for inserting LOCATION_PATH option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 if first option
 * @param[in]   location    ptr to string holding the location
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_location_path(uint8_t *buf,
                                                uint16_t lastonum,
                                                const char *location)
{
    return coap_opt_put_string(buf, lastonum, COAP_OPT_LOCATION_PATH,
                               location, '/');
}

/**
 * @brief   Convenience function for inserting LOCATION_QUERY option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 if first option
 * @param[in]   location    ptr to string holding the location
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_location_query(uint8_t *buf,
                                                 uint16_t lastonum,
                                                 const char *location)
{
    return coap_opt_put_string(buf, lastonum, COAP_OPT_LOCATION_QUERY,
                               location, '&');
}

/**
 * @brief   Convenience function for inserting URI_PATH option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 if first option
 * @param[in]   uri         ptr to source URI
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_uri_path(uint8_t *buf, uint16_t lastonum,
                                           const char *uri)
{
    return coap_opt_put_string(buf, lastonum, COAP_OPT_URI_PATH, uri, '/');
}

/**
 * @brief   Convenience function for inserting URI_QUERY option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 if first option
 * @param[in]   uri         ptr to source URI
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_uri_query(uint8_t *buf, uint16_t lastonum,
                                            const char *uri)
{
    return coap_opt_put_string(buf, lastonum, COAP_OPT_URI_QUERY, uri, '&');
}

/**
 * @brief   Convenience function for inserting URI_PATH and URI_QUERY into buffer
 *          This function will automatically split path and query parameters.
 *
 * @param[out]  buf         buffer to write to
 * @param[in,out] lastonum  number of previous option (for delta calculation),
 *                          or 0 if first option
 *                          May be NULL, then previous option is assumed to be 0.
 * @param[in]   uri         ptr into a source URI, to the first character after
 *                          the authority component
 *
 * @returns     amount of bytes written to @p buf
 *
 * This function may produce two different options (Uri-Path and Uri-Query).
 * Users that need to insert Content-Format, Max-Age or the currently
 * unassigned option 13 need to split their URI themselves and call the
 * respective helper functions.
 */
size_t coap_opt_put_uri_pathquery(uint8_t *buf, uint16_t *lastonum, const char *uri);

/**
 * @brief   Convenience function for inserting PROXY_URI option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          or 0 if first option
 * @param[in]   uri         ptr to source URI
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_opt_put_proxy_uri(uint8_t *buf, uint16_t lastonum,
                                            const char *uri)
{
    return coap_opt_put_string(buf, lastonum, COAP_OPT_PROXY_URI, uri, '\0');
}

/**
 * @brief   Insert block1 option into buffer (from coap_block1_t)
 *
 * This function is wrapper around @ref coap_put_option_block1(),
 * taking its arguments from a coap_block1_t struct.
 *
 * It will write option Nr. 27 (COAP_OPT_BLOCK1).
 *
 * It is safe to be called when @p block1 was generated for a non-blockwise
 * request.
 *
 * @param[in]   pkt_pos     buffer to write to
 * @param[in]   block1      ptr to block1 struct (created by coap_get_block1())
 * @param[in]   lastonum    last option number (must be < 27)
 *
 * @returns     amount of bytes written to @p pkt_pos
 */
size_t coap_put_block1_ok(uint8_t *pkt_pos, coap_block1_t *block1, uint16_t lastonum);

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
size_t coap_put_option(uint8_t *buf, uint16_t lastonum, uint16_t onum, const void *odata, size_t olen);

/**
 * @brief   Insert block1 option into buffer
 *
 * @param[out]  buf         buffer to write to
 * @param[in]   lastonum    number of previous option (for delta calculation),
 *                          must be < 27
 * @param[in]   blknum      block number
 * @param[in]   szx         SXZ value
 * @param[in]   more        more flag (1 or 0)
 *
 * @returns     amount of bytes written to @p buf
 */
static inline size_t coap_put_option_block1(uint8_t *buf, uint16_t lastonum,
                                            unsigned blknum, unsigned szx, int more)
{
    return coap_opt_put_uint(buf, lastonum, COAP_OPT_BLOCK1,
                             (blknum << 4) | szx | (more ? 0x8 : 0));
}

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
static inline size_t coap_put_option_ct(uint8_t *buf, uint16_t lastonum,
                                        uint16_t content_type)
{
    return coap_opt_put_uint(buf, lastonum, COAP_OPT_CONTENT_FORMAT, content_type);
}
/**@}*/

/**
 * @name    Functions -- Messaging
 *
 * Functions to support sending and receiving messages.
 */
/**@{*/
/**
 * @brief   Build reply to CoAP block2 request
 *
 * This function can be used to create a reply to a CoAP block2 request
 * packet. In addition to @ref coap_build_reply, this function checks the
 * block2 option and returns an error message to the client if necessary.
 *
 * @param[in]   pkt         packet to reply to
 * @param[in]   code        reply code (e.g., COAP_CODE_204)
 * @param[out]  rbuf        buffer to write reply to
 * @param[in]   rlen        size of @p rbuf
 * @param[in]   payload_len length of payload
 * @param[in]   slicer      slicer to use
 *
 * @warning Use @ref coap_get_response_hdr_len to determine the size of the
 *          header this will write.
 *
 * @returns     size of reply packet on success
 * @returns     <0 on error
 */
ssize_t coap_block2_build_reply(coap_pkt_t *pkt, unsigned code,
                                uint8_t *rbuf, unsigned rlen, unsigned payload_len,
                                coap_block_slicer_t *slicer);

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
 * @pre     @p token is either not overlapping with the memory buffer
 *          @p hdr points to, or is already at the right offset (e.g.
 *          when building the response inside the buffer the contained
 *          the request).
 *
 * @returns      length of resulting header
 */
ssize_t coap_build_hdr(coap_hdr_t *hdr, unsigned type, const void *token,
                       size_t token_len, unsigned code, uint16_t id);

/**
 * @brief   Build reply to CoAP request
 *
 * This function can be used to create a reply to any CoAP request packet.  It
 * will create the reply packet header based on parameters from the request
 * (e.g., id, token).
 *
 * Passing a non-zero @p max_data_len will ensure the remaining data fits into
 * the buffer along with the header. For this validation, @p max_data_len must
 * include any CoAP Options, the payload marker, as well as the payload proper.
 *
 * @param[in]   pkt             packet to reply to
 * @param[in]   code            reply code (e.g., COAP_CODE_204)
 * @param[out]  rbuf            buffer to write reply to
 * @param[in]   rlen            size of @p rbuf
 * @param[in]   max_data_len    Length of additional CoAP options, the payload marker and payload
 *
 * @warning     CoAP request handlers *must* check the return value for being
 *              negative. If it is, they must stop further processing of the
 *              request and pass on the return value unmodified.
 *
 * @return      @p max_data_len + size of the header written in bytes
 *
 * @retval      -ECANCELED          No-Response Option present and matching
 * @retval      -ENOSPC             @p rbuf too small
 * @retval      <0                  other error
 *
 * Usage:
 *
 * ```C
 * static ssize_t _foo_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
 *                             coap_request_ctx_t *context)
 * {
 *     static const char *payload = "Hello World";
 *     const payload_len = strlen(payload);
 *     // Worst case estimation of the data to add:
 *     size_t max_data_len = COAP_OPT_FOO_MAX_LEN + COAP_OPT_BAR_MAX_LEN
 *                         + COAP_PAYLOAD_MARKER_SIZE + payload_len;
 *     ssize_t hdr_len = coap_build_reply(pkt, COAP_CODE_CONTENT, buf, len, max_data_len);
 *
 *     if (hdr_len < 0) {
 *         return hdr_len; // pass through error
 *     }
 *
 *     // This step is needed due to an API design flaw
 *     hdr_len -= max_data_len;
 *
 *     uint8_t *pos = buf + hdr_len;
 *     uint16_t lastonum = 0;
 *     pos += coap_opt_put_uint(buf, lastonum, COAP_OPT_FOO, 42);
 *     lastonum = COAP_OPT_FOO;
 *     pos += coap_opt_put_uint(buf, lastonum, COAP_OPT_BAR, 1337);
 *     *pos++ = COAP_PAYLOAD_MARKER;
 *     memcpy(pos, payload, payload_len);
 *     pos += payload_len;
 *     return (uintptr_t)pos - (uintptr_t)buf;
 * }
 * ```
 */
ssize_t coap_build_reply(coap_pkt_t *pkt, unsigned code,
                         uint8_t *rbuf, unsigned rlen, unsigned max_data_len);

/**
 * @brief   Build empty reply to CoAP request
 *
 * This function can be used to create an empty ACK so that a later, separate
 * response can be sent independently.
 *
 * If the request was non-confirmable, this will generate nothing.
 *
 * @param[in]   pkt         packet to reply to
 * @param[out]  ack         buffer to write reply to
 *
 * @returns     size of reply packet on success
 * @returns     -ENOSPC if @p rbuf too small
 */
ssize_t coap_build_empty_ack(coap_pkt_t *pkt, coap_hdr_t *ack);

/**
 * @brief   Handle incoming CoAP request
 *
 * This function will find the correct handler, call it and write the reply
 * into @p resp_buf.
 *
 * @param[in]   pkt             pointer to (parsed) CoAP packet
 * @param[out]  resp_buf        buffer for response
 * @param[in]   resp_buf_len    size of response buffer
 * @param[in]   ctx             CoAP request context information
 *
 * @returns     size of reply packet on success
 * @returns     <0 on error
 */
ssize_t coap_handle_req(coap_pkt_t *pkt, uint8_t *resp_buf, unsigned resp_buf_len,
                        coap_request_ctx_t *ctx);

/**
 * @brief   Pass a coap request to a matching handler
 *
 * This function will try to find a matching handler in @p resources and call
 * the handler.
 *
 * @param[in]   pkt             pointer to (parsed) CoAP packet
 * @param[out]  resp_buf        buffer for response
 * @param[in]   resp_buf_len    size of response buffer
 * @param[in]   ctx             CoAP request context information
 * @param[in]   resources       Array of coap endpoint resources
 * @param[in]   resources_numof length of the coap endpoint resources
 *
 * @returns     size of the reply packet on success
 * @returns     <0 on error
 */
ssize_t coap_tree_handler(coap_pkt_t *pkt, uint8_t *resp_buf,
                          unsigned resp_buf_len, coap_request_ctx_t *ctx,
                          const coap_resource_t *resources,
                          size_t resources_numof);

/**
 * @brief   Generic coap subtree handler
 *
 * This function can be used as a generic handler for resources with the
 * @ref COAP_MATCH_SUBTREE where a new @ref coap_resource_t is to be parsed.
 *
 * @note The @p context must be of type @ref coap_resource_subtree_t.
 *
 * @param[in]   pkt             pointer to (parsed) CoAP packet
 * @param[out]  resp_buf        buffer for response
 * @param[in]   resp_buf_len    size of response buffer
 * @param[in]   context         pointer to request context, must contain context
 *                              to @ref coap_resource_subtree_t instance
 *
 * @returns     size of the reply packet on success
 * @returns     <0 on error
 */
ssize_t coap_subtree_handler(coap_pkt_t *pkt, uint8_t *resp_buf,
                             size_t resp_buf_len, coap_request_ctx_t *context);

/**
 * @brief   Convert message code (request method) into a corresponding bit field
 *
 * @param[in]   code    request code denoting the request method
 *
 * @returns     bit field corresponding to the given request method
 */
static inline coap_method_flags_t coap_method2flag(unsigned code)
{
    return (1 << (code - 1));
}

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
 * @brief   Initialize a packet struct, to build a message buffer
 *
 * @pre  buf              CoAP header already initialized
 * @post pkt.flags        all zeroed
 * @post pkt.payload      points to first byte after header
 * @post pkt.payload_len  set to maximum space available for options + payload
 *
 * @param[out]   pkt        pkt to initialize
 * @param[in]    buf        buffer to write for pkt, with CoAP header already
 *                          initialized
 * @param[in]    len        length of buf
 * @param[in]    header_len length of header in buf, including token
 */
void coap_pkt_init(coap_pkt_t *pkt, uint8_t *buf, size_t len, size_t header_len);

/**
 * @brief   Advance the payload pointer.
 *
 * @pre     You added @p len bytes of data to `pkt->payload`.
 *
 * You can add payload to a CoAP request by writing data directly to
 * `pkt->payload`.
 * This convenience function takes care of advancing the payload pointer
 * afterwards.
 *
 * @param[out]   pkt        pkt to which payload was added
 * @param[in]    len        length of payload
 */
static inline void coap_payload_advance_bytes(coap_pkt_t *pkt, size_t len)
{
    pkt->payload += len;
    pkt->payload_len -= len;
}

/**
 * @brief   Add payload data to the CoAP request.
 *
 * @pre     @ref coap_opt_finish must have been called before with
 *               the @ref COAP_OPT_FINISH_PAYLOAD option.
 *
 * The function copies @p data into the payload buffer of @p pkt and
 * advances the payload pointer.
 *
 * This is just a convenience function, you can also directly write
 * to `pkt->payload` if you have a function that outputs payload to
 * a buffer.
 * In this case you should instead call @ref coap_payload_advance_bytes.
 *
 * @param[out]   pkt        pkt to add payload to
 * @param[in]    data       payload data
 * @param[in]    len        length of payload
 *
 * @returns      number of payload bytes added on success
 * @returns      < 0 on error
 */
ssize_t coap_payload_put_bytes(coap_pkt_t *pkt, const void *data, size_t len);

/**
 * @brief Add a single character to the payload data of the CoAP request
 *
 * This function is used to add single characters to a CoAP payload data. It
 * checks whether the character can be added to the buffer and ignores if the
 * payload area is already exhausted.
 *
 * @param[out]   pkt        pkt to add payload to
 * @param[in]    c          character to write
 *
 * @returns      number of payload bytes added on success (always one)
 * @returns      < 0 on error
 */
ssize_t coap_payload_put_char(coap_pkt_t *pkt, char c);

/**
 * @brief   Create CoAP reply header (convenience function)
 *
 * This function generates the reply CoAP header and sets
 * the payload pointer inside the response buffer to point to
 * the start of the payload, so that it can be written directly
 * after the header.
 *
 * @param[in]   pkt         packet to reply to
 * @param[in]   code        reply code (e.g., COAP_CODE_204)
 * @param[out]  buf         buffer to write reply to
 * @param[in]   len         size of @p buf
 * @param[in]   ct          content type of payload or @ref COAP_FORMAT_NONE
 * @param[out]  payload     Will be set to the start of the payload inside
 *                          @p buf.
 *                          May be set to NULL if no payload response is
 *                          wanted (no-reply option)
 * @param[out]  payload_len_max max length of payload left in @p buf
 *
 * @returns     size of reply header on success
 * @retval      -ECANCELED  reply should be sent due to no-reply option
 *                          (pass this error through, server will handle this)
 * @retval      -ENOSPC     @p buf too small
 * @retval      <0          other error
 */
ssize_t coap_build_reply_header(coap_pkt_t *pkt, unsigned code,
                                void *buf, size_t len, uint16_t ct,
                                void **payload, size_t *payload_len_max);

/**
 * @brief   Create CoAP reply (convenience function)
 *
 * This is a simple wrapper that allows for building CoAP replies for simple
 * use-cases.
 *
 * The reply will be written to @p buf. If @p payload and @p payload_len are
 * non-zero, the payload will be copied into the resulting reply packet.
 *
 * @param[in]   pkt         packet to reply to
 * @param[in]   code        reply code (e.g., COAP_CODE_204)
 * @param[out]  buf         buffer to write reply to
 * @param[in]   len         size of @p buf
 * @param[in]   ct          content type of payload or @ref COAP_FORMAT_NONE
 * @param[in]   payload     ptr to payload
 * @param[in]   payload_len length of payload
 *
 * @returns     size of reply packet on success
 * @retval      -ECANCELED  reply should be sent due to no-reply option
 *                          (pass this error through, server will handle this)
 * @retval      -ENOSPC     @p buf too small
 * @retval      <0          other error
 */
ssize_t coap_reply_simple(coap_pkt_t *pkt,
                          unsigned code,
                          uint8_t *buf, size_t len,
                          uint16_t ct,
                          const void *payload, size_t payload_len);

/**
 * @brief   Reference to the default .well-known/core handler defined by the
 *          application
 */
extern ssize_t coap_well_known_core_default_handler(coap_pkt_t *pkt, \
                                                    uint8_t *buf, size_t len,
                                                    coap_request_ctx_t *context);
/**@}*/

/**
 * @brief   Respond to `/.well-known/core` to list all resources on the server
 */
#ifndef CONFIG_NANOCOAP_SERVER_WELL_KNOWN_CORE
#define CONFIG_NANOCOAP_SERVER_WELL_KNOWN_CORE !IS_USED(MODULE_GCOAP)
#endif

/**
 * @brief   Checks if a CoAP resource path matches a given URI
 *
 * Builds on strcmp() with rules specific to URI path matching
 *
 * @note This function is not intended for application use.
 * @internal
 *
 * @param[in] resource CoAP resource to check
 * @param[in] uri Null-terminated string URI to compare
 *
 * @return 0  if the resource path matches the URI
 * @return <0 if the resource path sorts before the URI
 * @return >0 if the resource path sorts after the URI
 */
int coap_match_path(const coap_resource_t *resource, const uint8_t *uri);

#if defined(MODULE_GCOAP) || defined(DOXYGEN)
/**
 * @name    Functions -- gcoap specific
 */
/**@{*/
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
/**@}*/
#endif

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
/** @} */
