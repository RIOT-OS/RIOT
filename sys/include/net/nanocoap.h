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

/**
 * @defgroup    net_nanocoap Nanocoap small CoAP library
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

#ifndef NET_NANOCOAP_H
#define NET_NANOCOAP_H

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#ifdef RIOT_VERSION
#include "byteorder.h"
#include "net/coap.h"
#else
#include "coap.h"
#include <arpa/inet.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Nanocoap specific CoAP method flags used in coap_handlers array
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
#define COAP_MATCH_SUBTREE      (0x8000) /**< Path is considered as a prefix
                                              when matching */
/** @} */

/**
 * @brief   Nanocoap-specific value to indicate no format specified
 */
#define COAP_FORMAT_NONE        (UINT16_MAX)

/**
 * @defgroup net_nanocoap_conf    Nanocoap compile configurations
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

/** @brief   Maximum length of a query string written to a message */
#ifndef CONFIG_NANOCOAP_QS_MAX
#define CONFIG_NANOCOAP_QS_MAX             (64)
#endif
/** @} */

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
 */
typedef struct {
    coap_hdr_t *hdr;                                  /**< pointer to raw packet   */
    uint8_t *token;                                   /**< pointer to token        */
    uint8_t *payload;                                 /**< pointer to payload      */
    uint16_t payload_len;                             /**< length of payload       */
    uint16_t options_len;                             /**< length of options array */
    coap_optpos_t options[CONFIG_NANOCOAP_NOPTS_MAX]; /**< option offset array     */
#ifdef MODULE_GCOAP
    uint32_t observe_value;                           /**< observe value           */
#endif
} coap_pkt_t;

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
 */
typedef ssize_t (*coap_handler_t)(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context);

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
 * @brief   Block1 helper struct
 */
typedef struct {
    size_t offset;                  /**< offset of received data            */
    uint32_t blknum;                /**< block number                       */
    unsigned szx;                   /**< szx value                          */
    int more;                       /**< -1 for no option, 0 for last block,
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

/**
 * @brief   Global CoAP resource list
 */
extern const coap_resource_t coap_resources[];

/**
 * @brief   Number of entries in global CoAP resource list
 */
extern const unsigned coap_resources_numof;


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
static inline unsigned coap_get_code(coap_pkt_t *pkt)
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
static inline unsigned coap_get_code_raw(coap_pkt_t *pkt)
{
    return (unsigned)pkt->hdr->code;
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
 * @brief   Get a message's token length [in byte]
 *
 * @param[in]   pkt   CoAP packet
 *
 * @returns     length of token in the given message (0-8 byte)
 */
static inline unsigned coap_get_token_len(const coap_pkt_t *pkt)
{
    return (pkt->hdr->ver_t_tkl & 0xf);
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
    return sizeof(coap_hdr_t) + coap_get_token_len(pkt);
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
 * @brief   Get the start of data after the header
 *
 * @param[in]   hdr   Header of CoAP packet in contiguous memory
 *
 * @returns     pointer to first byte after the header
 */
static inline uint8_t *coap_hdr_data_ptr(coap_hdr_t *hdr)
{
    return ((uint8_t *)hdr) + sizeof(coap_hdr_t);
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
/**@}*/


/**
 * @name    Functions -- Options Read
 *
 * Read options from a parsed packet.
 */
/**@{*/
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
int coap_opt_get_uint(const coap_pkt_t *pkt, uint16_t optnum, uint32_t *value);

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
ssize_t coap_opt_get_string(const coap_pkt_t *pkt, uint16_t optnum,
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
static inline ssize_t coap_get_location_path(const coap_pkt_t *pkt,
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
static inline ssize_t coap_get_location_query(const coap_pkt_t *pkt,
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
static inline ssize_t coap_get_uri_path(const coap_pkt_t *pkt, uint8_t *target)
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
 * Caller must ensure @p target can hold at least CONFIG_NANOCOAP_URI_MAX bytes!
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  target  buffer for target URI
 *
 * @returns     -ENOSPC     if URI option is larger than CONFIG_NANOCOAP_URI_MAX
 * @returns     nr of bytes written to @p target (including '\0')
 */
static inline ssize_t coap_get_uri_query(const coap_pkt_t *pkt, uint8_t *target)
{
    return coap_opt_get_string(pkt, COAP_OPT_URI_QUERY, target,
                               CONFIG_NANOCOAP_URI_MAX, '&');
}

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
ssize_t coap_opt_get_opaque(const coap_pkt_t *pkt, unsigned opt_num, uint8_t **value);
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
static inline ssize_t coap_get_proxy_uri(const coap_pkt_t *pkt, char **target)
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
 */
void coap_block_finish(coap_block_slicer_t *slicer, uint16_t option);

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
 */
static inline void coap_block1_finish(coap_block_slicer_t *slicer)
{
    coap_block_finish(slicer, COAP_OPT_BLOCK1);
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
 */
static inline void coap_block2_finish(coap_block_slicer_t *slicer)
{
    coap_block_finish(slicer, COAP_OPT_BLOCK2);
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
                                const uint8_t *c, size_t len);

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
int coap_get_blockopt(coap_pkt_t *pkt, uint16_t option, uint32_t *blknum, unsigned *szx);

/**
 * @brief   Helper to decode SZX value to size in bytes
 *
 * @param[in]   szx     SZX value to decode
 *
 * @returns     SZX value decoded to bytes
 */
static inline unsigned coap_szx2size(unsigned szx)
{
    return (1 << (szx + 4));
}
/**@}*/


/**
 * @name    Functions -- Options Write Packet API
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
ssize_t coap_opt_add_opaque(coap_pkt_t *pkt, uint16_t optnum, const uint8_t *val, size_t val_len);

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
 * @param[in]     path        Resource (sub)path
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
size_t coap_put_option(uint8_t *buf, uint16_t lastonum, uint16_t onum, const uint8_t *odata, size_t olen);

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
 * @returns      length of resulting header
 */
ssize_t coap_build_hdr(coap_hdr_t *hdr, unsigned type, uint8_t *token,
                       size_t token_len, unsigned code, uint16_t id);

/**
 * @brief   Build reply to CoAP request
 *
 * This function can be used to create a reply to any CoAP request packet.  It
 * will create the reply packet header based on parameters from the request
 * (e.g., id, token).
 *
 * Passing a non-zero @p payload_len will ensure the payload fits into the
 * buffer along with the header. For this validation, payload_len must include
 * any options, the payload marker, as well as the payload proper.
 *
 * @param[in]   pkt         packet to reply to
 * @param[in]   code        reply code (e.g., COAP_CODE_204)
 * @param[out]  rbuf        buffer to write reply to
 * @param[in]   rlen        size of @p rbuf
 * @param[in]   payload_len length of payload
 *
 * @returns     size of reply packet on success
 * @returns     <0 on error
 * @returns     -ENOSPC if @p rbuf too small
 */
ssize_t coap_build_reply(coap_pkt_t *pkt, unsigned code,
                         uint8_t *rbuf, unsigned rlen, unsigned payload_len);

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
 * @brief   Pass a coap request to a matching handler
 *
 * This function will try to find a matching handler in @p resources and call
 * the handler.
 *
 * @param[in]   pkt             pointer to (parsed) CoAP packet
 * @param[out]  resp_buf        buffer for response
 * @param[in]   resp_buf_len    size of response buffer
 * @param[in]   resources       Array of coap endpoint resources
 * @param[in]   resources_numof length of the coap endpoint resources
 *
 * @returns     size of the reply packet on success
 * @returns     <0 on error
 */
ssize_t coap_tree_handler(coap_pkt_t *pkt, uint8_t *resp_buf,
                          unsigned resp_buf_len,
                          const coap_resource_t *resources,
                          size_t resources_numof);

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
 * @param[in]   ct          content type of payload
 * @param[in]   payload     ptr to payload
 * @param[in]   payload_len length of payload
 *
 * @returns     size of reply packet on success
 * @returns     <0 on error
 * @returns     -ENOSPC if @p buf too small
 */
ssize_t coap_reply_simple(coap_pkt_t *pkt,
                          unsigned code,
                          uint8_t *buf, size_t len,
                          unsigned ct,
                          const uint8_t *payload, uint8_t payload_len);

/**
 * @brief   Reference to the default .well-known/core handler defined by the
 *          application
 */
extern ssize_t coap_well_known_core_default_handler(coap_pkt_t *pkt, \
                                                    uint8_t *buf, size_t len,
                                                    void *context);
/**@}*/


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
int coap_match_path(const coap_resource_t *resource, uint8_t *uri);

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
#endif /* NET_NANOCOAP_H */
/** @} */
