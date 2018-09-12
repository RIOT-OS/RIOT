/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
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
 * @author      Ken Bannister <kb2ma@runbox.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
 * @{
 */
#define COAP_GET                (0x1)
#define COAP_POST               (0x2)
#define COAP_PUT                (0x4)
#define COAP_DELETE             (0x8)
/** @} */

/**
 * @brief   Nanocoap-specific value to indicate no format specified
 */
#define COAP_FORMAT_NONE        (65535)

/**
 * @name    Nanocoap specific maximum values
 * @{
 */
#define NANOCOAP_NOPTS_MAX      (16)
#define NANOCOAP_URI_MAX        (64)
/** @} */

#ifdef MODULE_GCOAP
#define NANOCOAP_URL_MAX        NANOCOAP_URI_MAX
#define NANOCOAP_QS_MAX         (64)
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
    uint8_t data[];             /**< convenience pointer to payload start   */
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
    coap_hdr_t *hdr;                            /**< pointer to raw packet   */
    uint8_t *token;                             /**< pointer to token        */
    uint8_t *payload;                           /**< pointer to payload      */
    uint16_t payload_len;                       /**< length of payload       */
    uint16_t options_len;                       /**< length of options array */
    coap_optpos_t options[NANOCOAP_NOPTS_MAX];  /**< option offset array     */
#ifdef MODULE_GCOAP
    uint8_t url[NANOCOAP_URI_MAX];              /**< parsed request URL      */
    uint8_t qs[NANOCOAP_QS_MAX];                /**< parsed query string     */
    uint16_t content_type;                      /**< content type            */
    uint32_t observe_value;                     /**< observe value           */
#endif
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
size_t coap_opt_put_string(uint8_t *buf, uint16_t lastonum, uint16_t optnum,
                           const char *string, char separator);

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
 * @param[out]  block1  ptr to preallocated coap_block1_t structure
 *
 * @returns     0 if block1 option not present
 * @returns     1 if structure has been filled
 */
int coap_get_block1(coap_pkt_t *pkt, coap_block1_t *block1);

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
size_t coap_put_option_block1(uint8_t *buf, uint16_t lastonum, unsigned blknum, unsigned szx, int more);

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
 * @return        -ENOSPC if no available options
 */
ssize_t coap_opt_add_string(coap_pkt_t *pkt, uint16_t optnum, const char *string, char separator);

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
 * @return        <0 reserved for error but not implemented yet
 */
ssize_t coap_opt_add_uint(coap_pkt_t *pkt, uint16_t optnum, uint32_t value);

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
 */
ssize_t coap_opt_finish(coap_pkt_t *pkt, uint16_t flags);

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
 * @brief   Convenience function for getting the packet's URI_PATH
 *
 * This function decodes the pkt's URI option into a "/"-separated and
 * '\0'-terminated string.
 *
 * Caller must ensure @p target can hold at least NANOCOAP_URI_MAX bytes!
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  target  buffer for target URI
 *
 * @returns     -ENOSPC     if URI option is larger than NANOCOAP_URI_MAX
 * @returns     nr of bytes written to @p target (including '\0')
 */
static inline ssize_t coap_get_uri_path(const coap_pkt_t *pkt, uint8_t *target)
{
    return coap_opt_get_string(pkt, COAP_OPT_URI_PATH, target,
                               NANOCOAP_URI_MAX, '/');
}

/**
 * @brief   Convenience function for getting the packet's URI_QUERY option
 *
 * This function decodes the pkt's URI_QUERY option into a "&"-separated and
 * '\0'-terminated string.
 *
 * Caller must ensure @p target can hold at least NANOCOAP_URI_MAX bytes!
 *
 * @param[in]   pkt     pkt to work on
 * @param[out]  target  buffer for target URI
 *
 * @returns     -ENOSPC     if URI option is larger than NANOCOAP_URI_MAX
 * @returns     nr of bytes written to @p target (including '\0')
 */
static inline ssize_t coap_get_uri_query(const coap_pkt_t *pkt, uint8_t *target)
{
    return coap_opt_get_string(pkt, COAP_OPT_URI_QUERY, target,
                               NANOCOAP_URI_MAX, '&');
}

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

#if defined(MODULE_GCOAP) || defined(DOXYGEN)
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
#endif

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
