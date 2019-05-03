/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_nanosock Nanocoap Sock
 * @ingroup     net
 * @brief       Synchronous sock based application messaging with nanocoap
 *
 * nanocoap sock uses the nanocoap CoAP library to provide a synchronous
 * interface to RIOT's sock networking API to read and write CoAP messages.
 * For a server, nanocoap sock accepts a list of resource paths with callbacks
 * for writing the response. For a client, nanocoap sock provides a function
 * to send a request and waits for the server response. nanocoap sock uses
 * nanocoap's Buffer API to write message options.
 *
 * ## Server Operation ##
 *
 * See the nanocoap_server example, which is built on the nanocoap_server()
 * function. A server must define an array of coap_resource_t resources for
 * which it responds. See the declarations of `coap_resources` and
 * `coap_resources_numof`. The array contents must be ordered by the resource
 * path, specifically the ASCII encoding of the path characters (digit and
 * capital precede lower case). Use the COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER
 * macro for the default `/.well-known/core` entry. Also see _Server path
 * matching_ in the base [nanocoap](group__net__nanocoap.html) documentation.
 *
 * ### Handler functions ###
 *
 * For each resource, you must implement a ::coap_handler_t handler function.
 * nanocoap provides functions to help implement the handler. If the handler
 * is called via nanocoap_server(), the response buffer provided to the handler
 * reuses the buffer for the request. So, your handler must read the request
 * thoroughly before writing the response.
 *
 * To read the request, use the functions in the _Header_ and _Options Read_
 * sections of the [nanocoap](group__net__nanocoap.html) documentation. If the
 * pkt _payload_len_ attribute is a positive value, start to read it at the
 * _payload_ pointer attribute.
 *
 * If a response does not require specific CoAP options, use
 * coap_reply_simple(). If there is a payload, it writes a Content-Format
 * option with the provided value.
 *
 * For a response with additional CoAP options, start by calling
 * coap_build_reply(). Then use the Buffer API to write the rest of the
 * response. See the instructions in the section _Write Options and Payload_
 * below.
 *
 * ## Client Operation ##
 *
 * Follow the instructions in the section _Write Options and Payload_ below.
 *
 * To send the message and await the response, see nanocoap_request() as well
 * as nanocoap_get(), which additionally copies the response payload to a user
 * supplied buffer. Finally, read the response as described above in the server
 * _Handler functions_ section for reading a request.
 *
 * ## Write Options and Payload ##
 *
 * For both server responses and client requests, CoAP uses an Option mechanism
 * to encode message metadata that is not required for each message. For
 * example, the resource URI path is required only for a request, and is encoded
 * as the Uri-Path option.
 *
 * nanocoap sock uses the nanocoap Buffer API for options. The caller
 * must provide the last option number written as well as the buffer position.
 * The caller is primarily responsible for tracking and managing the space
 * remaining in the buffer.
 *
 * Before starting, ensure the CoAP header has been initialized with
 * coap_build_hdr(). For a response, coap_build_reply() includes a call to
 * coap_build_hdr(). Use the returned length to track the next position in the
 * buffer to write and remaining length.
 *
 * Next, use the functions in the _Options Write Buffer API_ section of
 * [nanocoap](group__net__nanocoap.html) to write each option. These functions
 * require the position in the buffer to start writing, and return the number
 * of bytes written. Options *must* be written in order by option number (see
 * "CoAP option numbers" in [CoAP defines](group__net__coap.html)).
 *
 * @note You must ensure the buffer has enough space remaining to write each
 * option. The API does not verify the safety of writing an option.
 *
 * If there is a payload, append a payload marker (0xFF). Then write the
 * payload to within the maximum length remaining in the buffer.
 *
 * # Create a Block-wise Response (Block2)
 *
 * Block-wise is a CoAP extension (RFC 7959) to divide a large payload across
 * multiple physical packets. This section describes how to write a block-wise
 * payload for a response, and is known as Block2. (Block1 is for a block-wise
 * payload in a request.) See _riot_board_handler() in the nanocoap_server
 * example for an example handler implementation.
 *
 * Start with coap_block2_init() to read the client request and initialize a
 * coap_slicer_t struct with the size and location for this slice of the
 * overall payload. Then write the block2 option in the response with
 * coap_opt_put_block2(). The option includes an indicator ("more") that a
 * slice completes the overall payload transfer. You may not know the value for
 * _more_ at this point, but you must initialize the space in the packet for
 * the option before writing the payload. The option is rewritten later.
 *
 * Next, use the coap_blockwise_put_xxx() functions to write the payload
 * content. These functions use the coap_block_slicer_t to enable or disable
 * actually writing the content, depending on the current position within the
 * overall payload transfer.
 *
 * Finally, use the convenience function coap_block2_build_reply(), which
 * finalizes the packet and calls coap_block2_finish() internally to update
 * the block2 option.
 *
 * @{
 *
 * @file
 * @brief       nanocoap high-level API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NET_NANOCOAP_SOCK_H
#define NET_NANOCOAP_SOCK_H

#include <stdint.h>
#include <unistd.h>

#include "net/nanocoap.h"
#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Global CoAP resource list
 */
extern const coap_resource_t coap_resources[];

/**
 * @brief   Number of entries in global CoAP resource list
 */
extern const unsigned coap_resources_numof;


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

/**
 * @brief   Resource definition for the default .well-known/core handler
 */
#define COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER \
    { \
        .path = "/.well-known/core", \
        .methods = COAP_GET, \
        .handler = coap_well_known_core_default_handler \
    }

/**
 * @brief   Start a nanocoap server instance
 *
 * This function only returns if there's an error binding to @p local, or if
 * receiving of UDP packets fails.
 *
 * @param[in]   local   local UDP endpoint to bind to
 * @param[in]   buf     input buffer to use
 * @param[in]   bufsize size of @p buf
 *
 * @returns     -1 on error
 */
int nanocoap_server(sock_udp_ep_t *local, uint8_t *buf, size_t bufsize);

/**
 * @brief   Simple synchronous CoAP (confirmable) get
 *
 * @param[in]   remote  remote UDP endpoint
 * @param[in]   path    remote path
 * @param[out]  buf     buffer to write response to
 * @param[in]   len     length of @p buffer
 *
 * @returns     length of response payload on success
 * @returns     <0 on error
 */
ssize_t nanocoap_get(sock_udp_ep_t *remote, const char *path, uint8_t *buf,
                     size_t len);

/**
 * @brief   Simple synchronous CoAP request
 *
 * @param[in,out]   pkt     Packet struct containing the request. Is reused for
 *                          the response
 * @param[in]       local   Local UDP endpoint, may be NULL
 * @param[in]       remote  remote UDP endpoint
 * @param[in]       len     Total length of the buffer associated with the
 *                          request
 *
 * @returns     length of response on success
 * @returns     <0 on error
 */
ssize_t nanocoap_request(coap_pkt_t *pkt, sock_udp_ep_t *local,
                         sock_udp_ep_t *remote, size_t len);

#ifdef __cplusplus
}
#endif
#endif /* NET_NANOCOAP_SOCK_H */
/** @} */
