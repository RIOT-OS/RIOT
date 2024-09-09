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
 * @defgroup    net_nanosock nanoCoAP Sock
 * @ingroup     net
 * @brief       Synchronous sock based messaging with nanocoap
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
 * See the nanocoap_server example, which is built on the `nanocoap_server()`
 * function. A server must define CoAP resources for which it responds.
 *
 * Each @ref coap_resource_t is added to the XFA with NANOCOAP_RESOURCE(name)
 * followed by the declaration of the CoAP resource, e.g.:
 *
 * ```C
 * NANOCOAP_RESOURCE(board) {
 *   .path = "/board", .methods = COAP_GET, .handler = _board_handler,
 * };
 * ```
 *
 * nanocoap itself provides the COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER entry for
 * `/.well-known/core`.
 *
 * To use the CoAP resource XFA, enable the `nanocoap_resources` module.
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
 * To send the message and await the response, see nanocoap_sock_request() as well
 * as nanocoap_sock_get(), which additionally copies the response payload to a user
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

#include "random.h"
#include "net/nanocoap.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#if IS_USED(MODULE_NANOCOAP_DTLS)
#include "net/credman.h"
#include "net/sock/dtls.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Credman tag used for NanoCoAP
 *          Tag together with the credential type (PSK) needs to be unique
 */
#ifndef CONFIG_NANOCOAP_SOCK_DTLS_TAG
#define CONFIG_NANOCOAP_SOCK_DTLS_TAG           (0xc0ab)
#endif

/**
 * @brief   CoAP server work buf size
 *          Used both for RX and TX, needs to hold payload block + header
 */
#ifndef CONFIG_NANOCOAP_SERVER_BUF_SIZE
#define CONFIG_NANOCOAP_SERVER_BUF_SIZE         ((1 << (CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT + 3)) \
                                                 + CONFIG_NANOCOAP_URI_MAX + 16)
#endif

/**
 * @brief   CoAP server thread stack size
 */
#ifndef CONFIG_NANOCOAP_SERVER_STACK_SIZE
#define CONFIG_NANOCOAP_SERVER_STACK_SIZE       THREAD_STACKSIZE_DEFAULT
#endif

/**
 * @brief   Include a random token with block-wise transfers.
 *
 * This is a workaround for buggy CoPA implementations (e.g. go-coap) that expect
 * to identify block-wise transfers based on the token.
 *
 * See https://github.com/plgd-dev/go-coap/issues/512
 */
#ifndef CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN
#define CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN        (0)
#endif

/**
 * @brief   NanoCoAP socket types
 */
typedef enum {
    COAP_SOCKET_TYPE_UDP,                   /**< transport is plain UDP */
    COAP_SOCKET_TYPE_DTLS,                  /**< transport is DTLS      */
} nanocoap_socket_type_t;

/**
 * @brief   NanoCoAP socket struct
 */
typedef struct {
    sock_udp_t udp;                         /**< UDP socket     */
#if IS_USED(MODULE_NANOCOAP_DTLS) || defined(DOXYGEN)
    sock_dtls_t dtls;                       /**< DTLS socket    */
    sock_dtls_session_t dtls_session;       /**< Session object for the stored socket.
                                                 Used for exchanging a session between
                                                 functions. */
    nanocoap_socket_type_t type;            /**< Socket type (UDP, DTLS) */
#endif
    uint16_t msg_id;                        /**< next CoAP message ID */
} nanocoap_sock_t;

/**
 * @brief Blockwise request helper struct
 */
typedef struct {
    nanocoap_sock_t *sock;          /**< socket used for the request        */
    const char *path;               /**< path on the server                 */
    uint32_t blknum;                /**< current block number               */
    coap_method_t method;           /**< request method (GET, POST, PUT)    */
    uint8_t blksize;                /**< CoAP blocksize exponent            */
} coap_block_request_t;

/**
 * @brief   Context from CoAP request for separate response
 */
typedef struct {
    sock_udp_ep_t remote;           /**< remote to send response to         */
#if defined(MODULE_SOCK_AUX_LOCAL) || DOXYGEN
    sock_udp_ep_t local;            /**< local from which to send response  */
#endif
    uint8_t token[COAP_TOKEN_LENGTH_MAX];   /**< request token              */
    uint8_t tkl;                    /**< request token length               */
    uint8_t no_response;            /**< no-response bitmap                 */
} nanocoap_server_response_ctx_t;

/**
 * @brief   Prepare the context for a separate response
 *
 * This function serializes the CoAP request information so that
 * a separate response can be generated outside the CoAP handler.
 *
 * The CoAP handler should then respond with an empty ACK by calling
 * @ref coap_build_empty_ack
 *
 * @param[out]  ctx     Context information for separate response
 * @param[in]   pkt     CoAP packet to which the response will be generated
 * @param[in]   req     Context of the CoAP request
 */
void nanocoap_server_prepare_separate(nanocoap_server_response_ctx_t *ctx,
                                      coap_pkt_t *pkt, const coap_request_ctx_t *req);

/**
 * @brief   Send a separate response to a CoAP request
 *
 *  This sends a response to a CoAP request outside the CoAP handler
 *
 * @pre     @ref nanocoap_server_prepare_separate has been called on @p ctx
 *          inside the CoAP handler
 *
 * @param[in]   ctx     Context information for the CoAP response
 * @param[in]   code    CoAP response code
 * @param[in]   type    Response type, may be `COAP_TYPE_NON`
 * @param[in]   payload Response payload
 * @param[in]   len     Payload length
 *
 * @returns     0 on success
 *              negative error (see @ref sock_udp_sendv_aux)
 */
int nanocoap_server_send_separate(const nanocoap_server_response_ctx_t *ctx,
                                  unsigned code, unsigned type,
                                  const void *payload, size_t len);
/**
 * @brief   Get next consecutive message ID for use when building a new
 *          CoAP request.
 *
 * @param[in]   sock    CoAP socket on which the ID is used
 *
 * @return  A new message ID that can be used for a request or response.
 */
static inline uint16_t nanocoap_sock_next_msg_id(nanocoap_sock_t *sock)
{
    return sock->msg_id++;
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
 * @brief   Create and start the nanoCoAP server thread
 *
 * To automatically start the nanoCoAP server on startup, select the
 * `nanocoap_server_auto_init` module.
 *
 * @param[in] local UDP endpoint to bind to
 *
 * @return pid of the server thread
 */
kernel_pid_t nanocoap_server_start(const sock_udp_ep_t *local);

/**
 * @brief   Create a CoAP client socket
 *
 * @param[out]  sock    CoAP UDP socket
 * @param[in]   local   Local UDP endpoint, may be NULL
 * @param[in]   remote  remote UDP endpoint
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
static inline int nanocoap_sock_connect(nanocoap_sock_t *sock,
                                        const sock_udp_ep_t *local,
                                        const sock_udp_ep_t *remote)
{
#if IS_USED(MODULE_NANOCOAP_DTLS)
    sock->type = COAP_SOCKET_TYPE_UDP;
#endif
    sock->msg_id = random_uint32();

    return sock_udp_create(&sock->udp, local, remote, 0);
}

#if IS_USED(MODULE_NANOCOAP_DTLS) || DOXYGEN
/**
 * @brief   Create a DTLS secured CoAP client socket
 *
 * @param[out]  sock    CoAP UDP socket
 * @param[in]   local   Local UDP endpoint, may be NULL
 * @param[in]   remote  remote UDP endpoint
 * @param[in]   tag     Tag of the PSK credential to use
 *                      Has to be added with @ref credman_add
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int nanocoap_sock_dtls_connect(nanocoap_sock_t *sock, sock_udp_ep_t *local,
                               const sock_udp_ep_t *remote, credman_tag_t tag);
#endif

/**
 * @brief   Create a CoAP client socket by URL
 *
 * @param[in]   url     URL with server information to connect to
 * @param[out]  sock    CoAP UDP socket
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int nanocoap_sock_url_connect(const char *url, nanocoap_sock_t *sock);

/**
 * @brief   Close a CoAP client socket
 *
 * @param[in]  sock     CoAP UDP socket
 */
static inline void nanocoap_sock_close(nanocoap_sock_t *sock)
{
#if IS_USED(MODULE_NANOCOAP_DTLS)
    if (sock->type == COAP_SOCKET_TYPE_DTLS) {
        sock_dtls_session_destroy(&sock->dtls, &sock->dtls_session);
        sock_dtls_close(&sock->dtls);
    }
#endif
    sock_udp_close(&sock->udp);
}

/**
 * @brief   Simple synchronous CoAP (confirmable) GET
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[out]  buf     buffer to write response to
 * @param[in]   len     length of @p buffer
 *
 * @returns     length of response payload on success
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_sock_get(nanocoap_sock_t *sock, const char *path, void *buf,
                          size_t len);

/**
 * @brief   Simple non-confirmable GET
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_sock_get_non(nanocoap_sock_t *sock, const char *path,
                              void *response, size_t len_max);

/**
 * @brief   Simple synchronous CoAP (confirmable) PUT
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[in]   request buffer containing the payload
 * @param[in]   len     length of the payload to send
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_sock_put(nanocoap_sock_t *sock, const char *path,
                          const void *request, size_t len,
                          void *response, size_t len_max);

/**
 * @brief   Simple non-confirmable PUT
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[in]   request buffer containing the payload
 * @param[in]   len     length of the payload to send
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     0 if the request was sent and no response buffer was provided,
 *              independently of success (because no response is requested in that case)
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_sock_put_non(nanocoap_sock_t *sock, const char *path,
                              const void *request, size_t len,
                              void *response, size_t len_max);

/**
 * @brief   Simple synchronous CoAP (confirmable) PUT to URL
 *
 * @param[in]   url     Absolute URL pointer to source path
 * @param[in]   request buffer containing the payload
 * @param[in]   len     length of the payload to send
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     <0 on error
 */
ssize_t nanocoap_sock_put_url(const char *url,
                              const void *request, size_t len,
                              void *response, size_t len_max);

/**
 * @brief   Simple synchronous CoAP (confirmable) POST
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[in]   request buffer containing the payload
 * @param[in]   len     length of the payload to send
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_sock_post(nanocoap_sock_t *sock, const char *path,
                           const void *request, size_t len,
                           void *response, size_t len_max);

/**
 * @brief   Simple non-confirmable POST
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[in]   request buffer containing the payload
 * @param[in]   len     length of the payload to send
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     0 if the request was sent and no response buffer was provided,
 *              independently of success (because no response is requested in that case)
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_sock_post_non(nanocoap_sock_t *sock, const char *path,
                               const void *request, size_t len,
                               void *response, size_t len_max);

/**
 * @brief   Simple synchronous CoAP (confirmable) POST to URL
 *
 * @param[in]   url     Absolute URL pointer to source path
 * @param[in]   request buffer containing the payload
 * @param[in]   len     length of the payload to send
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     <0 on error
 */
ssize_t nanocoap_sock_post_url(const char *url,
                               const void *request, size_t len,
                               void *response, size_t len_max);

/**
 * @brief   Simple synchronous CoAP (confirmable) FETCH
 *          ([RFC 8132](https://datatracker.ietf.org/doc/html/rfc8132))
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[in]   request buffer containing the payload
 * @param[in]   len     length of the payload to send
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_sock_fetch(nanocoap_sock_t *sock, const char *path,
                            const void *request, size_t len,
                            void *response, size_t len_max);

/**
 * @brief   Simple non-confirmable FETCH
 *          ([RFC 8132](https://datatracker.ietf.org/doc/html/rfc8132))
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[in]   request buffer containing the payload
 * @param[in]   len     length of the payload to send
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     0 if the request was sent and no response buffer was provided,
 *              independently of success (because no response is requested in that case)
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_sock_fetch_non(nanocoap_sock_t *sock, const char *path,
                                const void *request, size_t len,
                                void *response, size_t len_max);

/**
 * @brief   Simple synchronous CoAP (confirmable) FETCH to URL
 *          ([RFC 8132](https://datatracker.ietf.org/doc/html/rfc8132))
 *
 * @param[in]   url     Absolute URL pointer to source path
 * @param[in]   request buffer containing the payload
 * @param[in]   len     length of the payload to send
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @returns     length of response payload on success
 * @returns     <0 on error
 */
ssize_t nanocoap_sock_fetch_url(const char *url,
                                const void *request, size_t len,
                                void *response, size_t len_max);

/**
 * @brief   Simple synchronous CoAP (confirmable) DELETE
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path (with query) to delete
 *
 * @returns     0 on success
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_sock_delete(nanocoap_sock_t *sock, const char *path);

/**
 * @brief   Simple synchronous CoAP (confirmable) DELETE for URL
 *
 * @param[in]   url     URL of the resource that should be deleted
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
ssize_t nanocoap_sock_delete_url(const char *url);

/**
 * @brief    Performs a blockwise coap get request on a socket.
 *
 * This function will fetch the content of the specified resource path via
 * block-wise-transfer. A coap_blockwise_cb_t will be called on each received
 * block.
 *
 * @param[in]   sock       socket to use for the request
 * @param[in]   path       pointer to source path
 * @param[in]   blksize    sender suggested SZX for the COAP block request
 * @param[in]   callback   callback to be executed on each received block
 * @param[in]   arg        optional function arguments
 *
 * @returns     -1         if failed to fetch the url content
 * @returns      0         on success
 */
int nanocoap_sock_get_blockwise(nanocoap_sock_t *sock, const char *path,
                                coap_blksize_t blksize,
                                coap_blockwise_cb_t callback, void *arg);

/**
 * @brief    Performs a blockwise coap get request to the specified url, store
 *           the response in a buffer.
 *
 * @param[in]   sock       socket to use for the request
 * @param[in]   path       Absolute URL pointer to source path
 * @param[in]   blksize    sender suggested SZX for the COAP block request
 * @param[in]   offset     Offset in bytes from the start of the resource
 * @param[in]   dst        Target buffer
 * @param[in]   len        Target buffer length
 *
 * @returns     <0 on error
 * @returns     -EINVAL    if an invalid url is provided
 * @returns     size of the response payload on success
 */
int nanocoap_sock_get_slice(nanocoap_sock_t *sock, const char *path,
                            coap_blksize_t blksize, size_t offset,
                            void *dst, size_t len);

/**
 * @brief    Performs a blockwise coap get request to the specified url.
 *
 * This function will fetch the content of the specified resource path via
 * block-wise-transfer. A coap_blockwise_cb_t will be called on each received
 * block.
 *
 * @param[in]   url        Absolute URL pointer to source path (i.e. not containing
 *                         a fragment identifier)
 * @param[in]   blksize    sender suggested SZX for the COAP block request
 * @param[in]   callback   callback to be executed on each received block
 * @param[in]   arg        optional function arguments
 *
 * @returns     -EINVAL    if an invalid url is provided
 * @returns     -1         if failed to fetch the url content
 * @returns      0         on success
 */
int nanocoap_get_blockwise_url(const char *url,
                               coap_blksize_t blksize,
                               coap_blockwise_cb_t callback, void *arg);

/**
 * @brief    Performs a blockwise coap get request to the specified url, store
 *           the response in a buffer.
 *
 * This function will fetch the content of the specified resource path via
 * block-wise-transfer.
 * The blocks will be re-assembled into @p buf
 *
 * @param[in]   url        Absolute URL pointer to source path (i.e. not containing
 *                         a fragment identifier)
 * @param[in]   blksize    sender suggested SZX for the COAP block request
 * @param[in]   buf        Target buffer
 * @param[in]   len        Target buffer length
 *
 * @returns     <0 on error
 * @returns     -EINVAL    if an invalid url is provided
 * @returns     -ENOBUFS   if the provided buffer was too small
 * @returns     size of the response payload on success
 */
ssize_t nanocoap_get_blockwise_url_to_buf(const char *url,
                                          coap_blksize_t blksize,
                                          void *buf, size_t len);

/**
 * @brief    Performs a blockwise CoAP GET request, store the response
 *           in a buffer.
 *
 * This function will fetch the content of the specified resource path via
 * block-wise-transfer.
 * The blocks will be re-assembled into @p buf
 *
 * @param[in]   sock       socket to use for the request
 * @param[in]   path       pointer to source path
 * @param[in]   blksize    sender suggested SZX for the COAP block request
 * @param[in]   buf        Target buffer
 * @param[in]   len        Target buffer length
 *
 * @returns     <0 on error
 * @returns     -EINVAL    if an invalid url is provided
 * @returns     -ENOBUFS   if the provided buffer was too small
 * @returns     size of the response payload on success
 */
ssize_t nanocoap_get_blockwise_to_buf(nanocoap_sock_t *sock, const char *path,
                                      coap_blksize_t blksize,
                                      void *buf, size_t len);

/**
 * @brief   Simple synchronous CoAP request
 *
 * @param[in]       sock    socket to use for the request
 * @param[in,out]   pkt     Packet struct containing the request. Is reused for
 *                          the response
 * @param[in]       len     Total length of the buffer associated with the
 *                          request
 *
 * @returns     length of response on success
 * @returns     <0 on error
 */
ssize_t nanocoap_sock_request(nanocoap_sock_t *sock, coap_pkt_t *pkt, size_t len);

/**
 * @brief   Simple synchronous CoAP request with callback
 *
 *          The response will be handled by a callback, which avoids copying the
 *          response packet out of the network stack internal buffer.
 *
 * @param[in]       sock    socket to use for the request
 * @param[in,out]   pkt     Packet struct containing the request. Is reused for
 *                          the response
 * @param[in]       cb      Callback executed for response packet
 * @param[in]       arg     Optional callback argumnent
 *
 * @returns     length of response on success
 * @returns     0 for a request for which no response is expected, indicated by @p cb == NULL,
 *              or for a 2.xx response
 * @returns     -ETIMEDOUT, if no matching ACK or no response was received
 * @returns     -EBADMSG, if a matching RST was received
 * @returns     -ENXIO, if @p cb == NULL and the response indicates a 4.xx client error
 * @returns     -ENETRESET, if @p cb == NULL and the response indicates a 5.xx server error
 * @returns     any error on @see sock_udp_sendv or @see sock_dtls_sendv
 * @returns     any error on @see sock_udp_recv_buf or @see sock_dtls_recv_buf
 * @returns     any return value of @p cb for a matching response
 */
ssize_t nanocoap_sock_request_cb(nanocoap_sock_t *sock, coap_pkt_t *pkt,
                                 coap_request_cb_t cb, void *arg);

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
 * @returns     @see nanocoap_sock_request_cb on error
 */
ssize_t nanocoap_request(coap_pkt_t *pkt, const sock_udp_ep_t *local,
                         const sock_udp_ep_t *remote, size_t len);

/**
 * @brief   Initialize block request context by URL and connect a socket
 *
 * @param[out]  ctx     The block request context to initialize
 * @param[out]  sock    Socket to initialize and use for the request
 * @param[in]   url     The request URL
 * @param[in]   method  Request method (`COAP_METHOD_{GET|PUT|POST|FETCH}`)
 * @param[in]   blksize Request blocksize exponent
 *
 * @retval      0       Success
 * @retval      <0      Error (see @ref nanocoap_sock_url_connect for details)
 */
static inline int nanocoap_block_request_connect_url(coap_block_request_t *ctx,
                                                     nanocoap_sock_t *sock,
                                                     const char *url,
                                                     coap_method_t method,
                                                     coap_blksize_t blksize)
{
    ctx->sock = sock;
    ctx->path = sock_urlpath(url);
    ctx->blknum = 0;
    ctx->method = method;
    ctx->blksize = blksize;
    return nanocoap_sock_url_connect(url, ctx->sock);
}

/**
 * @brief   Do a block-wise request, send a single block.
 *
 *          This method is expected to be called in a loop until all
 *          payload blocks have been transferred.
 *
 * @pre     @p ctx was initialized with @ref nanocoap_block_request_connect_url
 *          or manually.
 *
 * @param[in]   ctx     blockwise request context
 * @param[in]   data    payload to send
 * @param[in]   len     payload length
 * @param[in]   more    more blocks after this one
 *                      (will be set automatically if @p len > block size)
 * @param[in]   cb      callback for response
 * @param[in]   arg     callback context
 *
 * @return      Number of payload bytes written on success
 *              Negative error on failure
 */
int nanocoap_sock_block_request(coap_block_request_t *ctx,
                                const void *data, size_t len, bool more,
                                coap_request_cb_t cb, void *arg);
#ifdef __cplusplus
}
#endif
#endif /* NET_NANOCOAP_SOCK_H */
/** @} */
