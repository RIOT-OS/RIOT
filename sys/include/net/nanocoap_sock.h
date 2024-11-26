/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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
 * coap_opt_put_block2(). Use @ref coap_get_response_hdr_len to get the length
 * of the response header: This will be the offset in the buffer where you
 * should start adding options. The Block2 option includes an indicator
 * ("more") that a slice completes the overall payload transfer. You may not
 * know the value for _more_ at this point, but you must initialize the space
 * in the packet for the option before writing the payload. The option is
 * rewritten later.
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

#include <stdint.h>
#include <unistd.h>

#include "mutex.h"
#include "net/nanocoap.h"
#include "net/sock/util.h"
#include "xfa.h"

#if MODULE_NANOCOAP_UDP
#  include "net/sock/udp.h"
#  include "random.h"
#endif
#if MODULE_NANOCOAP_TCP
#  include "net/sock/tcp.h"
#endif
#if MODULE_NANOCOAP_DTLS
#  include "net/credman.h"
#  include "net/sock/dtls.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Credman tag used for NanoCoAP
 *          Tag together with the credential type (PSK) needs to be unique
 */
#ifndef CONFIG_NANOCOAP_SOCK_DTLS_TAG
#  define CONFIG_NANOCOAP_SOCK_DTLS_TAG (0xc0ab)
#endif

/**
 * @brief   CoAP server work buf size
 *          Used both for RX and TX, needs to hold payload block + header
 */
#ifndef CONFIG_NANOCOAP_SERVER_BUF_SIZE
#  define CONFIG_NANOCOAP_SERVER_BUF_SIZE \
      ((1 << (CONFIG_NANOCOAP_BLOCKSIZE_DEFAULT + 3)) + CONFIG_NANOCOAP_URI_MAX + 16)
#endif

/**
 * @brief   CoAP server thread stack size
 */
#ifndef CONFIG_NANOCOAP_SERVER_STACK_SIZE
#  define CONFIG_NANOCOAP_SERVER_STACK_SIZE THREAD_STACKSIZE_DEFAULT
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
#  define CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN (0)
#endif

#ifndef CONFIG_NANOCOAP_SERVER_TCP_MAX_CLIENTS
/**
 * @brief   Maximum number of clients the TCP server can handle concurrently
 */
#  define CONFIG_NANOCOAP_SERVER_TCP_MAX_CLIENTS 4
#endif

/**
 * @brief   Event priority for nanoCoAP sock events (e.g. used by `nanocoap_sock_observe`)
 */
#ifndef CONFIG_NANOCOAP_SOCK_EVENT_PRIO
#  define CONFIG_NANOCOAP_SOCK_EVENT_PRIO       EVENT_PRIO_MEDIUM
#endif

/**
 * @brief   NanoCoAP socket types
 *
 * @details This can be casted to @ref coap_transport_t
 */
typedef enum {
    COAP_SOCKET_TYPE_UDP = COAP_TRANSPORT_UDP,   /**< transport is plain UDP */
    COAP_SOCKET_TYPE_DTLS = COAP_TRANSPORT_DTLS, /**< transport is DTLS      */
    COAP_SOCKET_TYPE_TCP = COAP_TRANSPORT_TCP,   /**< transport is TCP       */
} nanocoap_socket_type_t;

/**
 * @brief   NanoCoAP socket struct
 */
typedef struct {
    union {
#if MODULE_NANOCOAP_UDP || MODULE_NANOCOAP_DTLS || DOXYGEN
        struct {
            sock_udp_t udp; /**< UDP socket     */
            uint16_t msg_id; /**< next CoAP message ID */
#  if MODULE_NANOCOAP_DTLS || DOXYGEN
            sock_dtls_t dtls;                 /**< DTLS socket    */
            /**
             * @brief   Session object for the stored socket.
             *          Used for exchanging a session between
             *          functions.
             */
            sock_dtls_session_t dtls_session;
#  endif
        };
#endif
#if MODULE_NANOCOAP_TCP || DOXYGEN
        struct {
            sock_tcp_t tcp;             /**< TCP socket */
            void *tcp_buf;              /**< recv buffer for TCP socket
                                         *   (output of malloc()) */
            uint16_t tcp_buf_fill;      /**< number of bytes in
                                         *   @ref nanocoap_sock_t::tcp_buf */
        };
#endif
    };
#if NANOCOAP_ENABLED_TRANSPORTS > 1
    nanocoap_socket_type_t type; /**< Socket type (UDP, DTLS, TCP) */
#endif
    uint8_t hdr_buf[CONFIG_NANOCOAP_BLOCK_HEADER_MAX]; /**< buffer for CoAP header with options,
                                                            token and payload marker */
} nanocoap_sock_t;

/**
 * @brief Blockwise request helper struct
 */
typedef struct {
    nanocoap_sock_t *sock; /**< socket used for the request        */
    const char *path;      /**< path on the server                 */
    uint32_t blknum;       /**< current block number               */
    coap_method_t method;  /**< request method (GET, POST, PUT)    */
    uint8_t blksize;       /**< CoAP blocksize exponent            */
} coap_block_request_t;

/**
 * @brief   Observe Client helper struct
 */
typedef struct {
    coap_request_cb_t cb;           /**< user callback function             */
    void *arg;                      /**< callback function argument         */
    nanocoap_sock_t sock;           /**< socket used for the request        */
} coap_observe_client_t;

/**
 * @brief   Context from CoAP request for separate response
 */
typedef struct {
    union{
#if MODULE_NANOCOAP_UDP || MODULE_NANOCOAP_DTLS || DOXYGEN
        struct {
            sock_udp_ep_t remote_udp;       /**< UDP endpoint to send response to */
            sock_udp_ep_t local_udp;        /**< UDP endpoint from which to send response */
        };
#endif
#if MODULE_NANOCOAP_SERVER_TCP || DOXYGEN
        sock_tcp_t *sock_tcp;               /**< TCP socket to send the response over */
#endif
    };
    uint8_t token[COAP_TOKEN_LENGTH_MAX];   /**< request token            */
    uint8_t tkl;                            /**< request token length     */
    uint8_t no_response;                    /**< no-response bitmap       */
#if NANOCOAP_ENABLED_TRANSPORTS > 1
    coap_transport_t transport;             /**< transport to use */
#endif
} nanocoap_server_response_ctx_t;

#if MODULE_NANOCOAP_SERVER_TCP || DOXYGEN
/**
 * @brief   Context to pass to a NanoCoAP TCP server
 *
 * @warning All members are for internal use, do not access them.
 */
typedef struct {
    sock_udp_ep_t local;    /**< Local endpoint to listen at */
    sock_tcp_queue_t queue; /**< TCP queue to use */
    /**
     * @brief   TCP sockets to use
     *
     * This limits the number of concurrent connections (in other words the
     * number of clients to serve).
     */
    sock_tcp_t socks[CONFIG_NANOCOAP_SERVER_TCP_MAX_CLIENTS];
    /**
     * @brief   Number of bytes in the per client RX buf
     */
    uint16_t rx_fill[CONFIG_NANOCOAP_SERVER_TCP_MAX_CLIENTS];
    /**
     * @brief   Receive buffer, one per client
     */
    uint8_t rx_bufs[CONFIG_NANOCOAP_SERVER_TCP_MAX_CLIENTS * CONFIG_NANOCOAP_SERVER_BUF_SIZE];
    /**
     * @brief   Send buffer, shared between all connections
     */
    uint8_t tx_buf[CONFIG_NANOCOAP_SERVER_BUF_SIZE];
    event_queue_t *evq; /**< event queue to handle TCP connections in */
} nanocoap_tcp_server_ctx_t;
#endif

/**
 * @brief   Get the type of a given nanocoap socket
 * @param[in]   sock    Socket to get the type of
 *
 * @retval COAP_SOCKET_TYPE_UDP     CoAP over UDP socket
 * @retval COAP_SOCKET_TYPE_DTLS    CoAP over DTLS socket
 * @retval COAP_SOCKET_TYPE_TCP     CoAP over TCP socket
 */
static inline nanocoap_socket_type_t nanocoap_sock_get_type(const nanocoap_sock_t *sock)
{
    (void)sock;
#if NANOCOAP_ENABLED_TRANSPORTS > 1
    return sock->type;
#elif MODULE_NANOCOAP_UDP
    return COAP_SOCKET_TYPE_UDP;
#elif MODULE_NANOCOAP_DTLS
    return COAP_SOCKET_TYPE_DTLS;
#elif MODULE_NANOCOAP_TCP
    return COAP_SOCKET_TYPE_TCP;
#else
#  error "nanocoap: no transport enabled"
#endif
}

/**
 * @brief   Set the nanocoap socket type
 * @param[in]   sock        Socket to set the type of
 * @param[in]   type        The socket type to set
 *
 * @warning This function is internal. Calls outside of nanocoap's sock
 *          implementation *will* break things.
 *
 */
static inline void nanocoap_sock_set_type(nanocoap_sock_t *sock,
                                          nanocoap_socket_type_t type)
{
    (void)sock;
    (void)type;
#if NANOCOAP_ENABLED_TRANSPORTS > 1
    sock->type = type;
#endif
}

/**
 * @brief   Prepare the context for a separate response
 *
 * This function serializes the CoAP request information so that
 * a separate response can be generated outside the CoAP handler.
 *
 * The CoAP handler should then respond with an empty ACK by calling
 * @ref coap_build_empty_ack
 *
 * @warning This function is only available when using the module
 *          `nanocoap_server_separate`
 *
 * @param[out]  ctx     Context information for separate response
 * @param[in]   pkt     CoAP packet to which the response will be generated
 * @param[in]   req     Context of the CoAP request
 *
 * @retval  0           Success
 * @retval  -EOVERFLOW  Storing context would have overflown buffers in @p ctx
 *                      (e.g. RFC 8974 (module `nanocoap_token_ext`) is in
 *                      use and token too long)
 * @retval  <0          Other error
 */
int nanocoap_server_prepare_separate(nanocoap_server_response_ctx_t *ctx,
                                     coap_pkt_t *pkt, const coap_request_ctx_t *req);

/**
 * @brief   Check if a given separate response context was prepared for the
 *          remote endpoint of a given request
 *
 * @param[in]   ctx     Separate response context to check
 * @param[in]   req     Request from the remote to check for
 *
 * @retval  true        The remote endpoint given by @p req is in @p ctx
 * @retval  false       @p ctx was prepared for a different remote endpoint
 */
bool nanocoap_server_is_remote_in_response_ctx(const nanocoap_server_response_ctx_t *ctx,
                                               const coap_request_ctx_t *req);

/**
 * @brief   Get the transport of the given separate response context
 * @param[in]   ctx     Context to get the transport of
 * @return  The transport the response will be send on
 */
static inline coap_transport_t nanocoap_server_response_ctx_transport(const nanocoap_server_response_ctx_t *ctx)
{
    (void)ctx;
#if NANOCOAP_ENABLED_TRANSPORTS > 1
    return ctx->transport;
#elif MODULE_NANOCOAP_UDP
    return COAP_TRANSPORT_UDP;
#elif MODULE_NANOCOAP_DTLS
    return COAP_TRANSPORT_DTLS;
#elif MODULE_NANOCOAP_TCP
    return COAP_TRANSPORT_TCP;
#endif
}

/**
 * @brief   Build and send a separate response to a CoAP request
 *
 *  This sends a response to a CoAP request outside the CoAP handler
 *
 * @pre     @ref nanocoap_server_prepare_separate has been called on @p ctx
 *          inside the CoAP handler
 * @pre     Synchronization between calls of this function and calls of
 *          @ref nanocoap_server_prepare_separate is ensured
 *
 * @warning This function is only available when using the module
 *          `nanocoap_server_separate`
 *
 * @pre     The transport used is UDP/DTLS. (There are not separate responses
 *          in CoAP over TCP / WebSocket.)
 *
 * @param[in]   ctx     Context information for the CoAP response
 * @param[in]   code    CoAP response code
 * @param[in]   type    Response type, may be `COAP_TYPE_NON`
 * @param[in]   payload Response payload
 * @param[in]   len     Payload length
 *
 * @retval      0               Success
 * @retval      -ECANCELED      Request contained no-response option that did match the given @p code
 * @retval      <0              Negative errno code indicating the error
 */
int nanocoap_server_send_separate(const nanocoap_server_response_ctx_t *ctx,
                                  unsigned code, unsigned type,
                                  const void *payload, size_t len);

/**
 * @brief   Build a separate response header to a CoAP request
 *
 * This builds the response packet header. You may add CoAP Options, a payload
 * marker and a payload as needed after the header.
 *
 * @pre     @ref nanocoap_server_prepare_separate has been called on @p ctx
 *          inside the CoAP handler
 * @pre     Synchronization between calls of this function and calls of
 *          @ref nanocoap_server_prepare_separate is ensured
 *
 * @warning This function is only available when using the module
 *          `nanocoap_server_separate`
 *
 * @param[in]   ctx     Context information for the CoAP response
 * @param[out]  buf     Buffer to write the header to
 * @param[in]   buf_len Length of @p buf in bytes
 * @param[in]   code    CoAP response code
 * @param[in]   type    Response type, may be `COAP_TYPE_NON`
 * @param[in]   msg_id  Message ID to send
 *
 * @return      Length of the header build in bytes
 * @retval      -ECANCELED      Request contained no-response option that did match the given @p code
 * @retval      <0              Negative errno code indicating the error
 */
ssize_t nanocoap_server_build_separate(const nanocoap_server_response_ctx_t *ctx,
                                       void *buf, size_t buf_len,
                                       unsigned code, unsigned type,
                                       uint16_t msg_id);

/**
 * @brief   Send an already build separate response
 *
 * @pre     @ref nanocoap_server_prepare_separate has been called on @p ctx
 *          inside the CoAP handler
 * @pre     Synchronization between calls of this function and calls of
 *          @ref nanocoap_server_prepare_separate is ensured
 * @pre     @ref nanocoap_server_build_separate has been used to build the
 *          header in @p msg
 *
 * @warning This function is only available when using the module
 *          `nanocoap_server_separate`
 *
 * @param[in]   ctx     Context information for the CoAP response
 * @param[in]   reply   I/O list containing the reply to send
 *
 * @retval      0   Success
 * @retval      <0  negative errno code indicating the error
 */
int nanocoap_server_sendv_separate(const nanocoap_server_response_ctx_t *ctx,
                                   const iolist_t *reply);

/**
 * @brief           Register an observer
 * @param[in]       req_ctx         Request context belonging to @p req_pkt
 * @param[in,out]   req_pkt         Request that contained the observe registration request
 *
 * @warning This depends on module `nanocoap_server_observe`
 *
 * @note    If the same endpoint already was registered on the same resource,
 *          it will just update the token and keep the existing entry. This
 *          way duplicate detection is not needed and we eagerly can reclaim
 *          resources when a client lost state.
 *
 * @warning Preventing the same endpoint to registers more than once (using
 *          different tokens) to the same resource deviates from RFC 7641.
 *
 * The deviation here is intentional. A server can receive a second registration
 * from the same endpoint for the same resource for one of the following
 * reasons:
 *
 * 1. Reaffirming the registration by using the same token again.
 * 2. Losing state on the client side.
 * 3. A malicious client trying to exhaust resources.
 * 4. The same resource has different representations depending on the
 *    request. (E.g. `/.well-known/core` can yield a wildly different response
 *    depending on filters provided via URI-Query Options.)
 *
 * For case 1 updating the registration is matching what the spec mandates.
 * For two the old registration will not be of value for the client, and
 * overwriting it makes more efficient use of network bandwidth and RAM.
 * For 3 the deviation forces the adversary to send observe requests from
 * different ports to exhaust resources, which is a very minor improvement.
 * For 4 the deviation is a problem. However, the observe API does not allow to
 * send out different notification messages for the same resource anyway, so
 * case 4 cannot occur here.
 *
 * @retval  0           Success
 * @retval  -ENOMEM     Not enough resources to register another observer
 * @retval  <0          Negative errno code indicating error
 */
int nanocoap_register_observer(const coap_request_ctx_t *req_ctx, coap_pkt_t *req_pkt);

/**
 * @brief   Unregister an observer
 * @param   req_ctx         Request context belonging to @p req_pkt
 * @param   req_pkt         Received request for unregistration
 *
 * @warning This depends on module `nanocoap_server_observe`
 *
 * @note    It is safe to call this multiple times, e.g. duplicate detection
 *          is not needed for this.
 */
void nanocoap_unregister_observer(const coap_request_ctx_t *req_ctx,
                                  const coap_pkt_t *req_pkt);

#if MODULE_NANOCOAP_UDP || MODULE_NANOCOAP_DTLS || DOXYGEN
/**
 * @brief   Unregister a stale observation due to a reset message received
 * @param[in]   ep      Endpoint to wipe from the observer list
 * @param[in]   msg_id  Message ID of the notification send.
 */
void nanocoap_unregister_observer_due_to_reset(const sock_udp_ep_t *ep,
                                               uint16_t msg_id);
#endif

/**
 * @brief   Notify all currently registered observers of the given resource
 *
 * @param[in]   res     Resource to send updates for
 * @param[in]   iol     I/O list containing the CoAP Options, payload marker,
 *                      and payload of the update to send up
 *
 * @pre     @p iol contains everything but the CoAP header needed to send out.
 *          This will at least be a CoAP observe option, a payload marker,
 *          and a payload
 *
 * @post    For each registered observer a CoAP packet header is generated and
 *          the concatenation of that header and the provided list is sent
 */
void nanocoap_notify_observers(const coap_resource_t *res, const iolist_t *iol);

/**
 * @brief   Build and send notification to observers registered to a specific
 *          resource.
 *
 * @note    Use @ref nanocoap_notify_observers for more control (such
 *          as adding custom options) over the notification(s) to send.
 *
 * @param[in]   res         Resource to send updates for
 * @param[in]   obs         24-bit number to add as observe option
 * @param[in]   payload     Payload to send out
 * @param[in]   payload_len Length of @p payload in bytes
 */
void nanocoap_notify_observers_simple(const coap_resource_t *res, uint32_t obs,
                                      const void *payload, size_t payload_len);

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

#if MODULE_NANOCOAP_SERVER_TCP || DOXYGEN
/**
 * @brief   Start a nanocoap server instance listening on TCP
 *
 * This function only returns if there's an error binding to @p local, or if
 * listening for TCP connection fails.
 *
 * @param[in,out]   ctx     Context to operate on
 * @param[in]       evq     Event queue to operate on
 * @param[in]       local   Local endpoint to listen on or `NULL`.
 *
 * @retval  0   Success, server set up and connections will be accepted and
 *              handled from the event thread
 * @retval  <0  Error setting up server
 */
int nanocoap_server_tcp(nanocoap_tcp_server_ctx_t *ctx,
                        event_queue_t *evq,
                        const sock_tcp_ep_t *local);
#endif

#if MODULE_NANOCOAP_UDP || DOXYGEN
/**
 * @brief   Start a nanoCoAP server instance listening on UDP
 *
 * This function only returns if there's an error binding to @p local.
 *
 * @param[in]   local   local UDP endpoint to bind to
 * @param[in]   buf     response buffer to use
 * @param[in]   bufsize size of @p buf
 *
 * @returns     return code of @see sock_udp_create on error
 */
int nanocoap_server_udp(sock_udp_ep_t *local, void *buf, size_t bufsize);

/**
 * @brief   Alias of @ref nanocoap_server_udp
 *
 * @deprecated  Call @ref nanocoap_server_udp directly.
 */
static inline int nanocoap_server(sock_udp_ep_t *local, void *buf, size_t bufsize)
{
    return nanocoap_server_udp(local, buf, bufsize);
}

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
kernel_pid_t nanocoap_server_start_udp(const sock_udp_ep_t *local);

/**
 * @brief   Alias for @ref nanocoap_server_start_udp
 *
 * @deprecated  Use @ref nanocoap_server_start_udp instead
 */
static inline kernel_pid_t nanocoap_server_start(const sock_udp_ep_t *local)
{
    return nanocoap_server_start_udp(local);
}

/**
 * @brief   Create a CoAP over UDP client socket
 *
 * @param[out]  sock    CoAP UDP socket
 * @param[in]   local   Local UDP endpoint, may be NULL
 * @param[in]   remote  remote UDP endpoint
 *
 * @note    Requires module `nanocoap_udp`
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
static inline int nanocoap_sock_udp_connect(nanocoap_sock_t *sock, const sock_udp_ep_t *local,
                                            const sock_udp_ep_t *remote)
{
    nanocoap_sock_set_type(sock, COAP_SOCKET_TYPE_UDP);
    sock->msg_id = random_uint32();

    return sock_udp_create(&sock->udp, local, remote, 0);
}

/**
 * @brief   Create a CoAP over UDP client socket
 *
 * @param[out]  sock    CoAP UDP socket
 * @param[in]   local   Local UDP endpoint, may be NULL
 * @param[in]   remote  remote UDP endpoint
 *
 * @deprecated  Use @ref nanocoap_sock_connect_udp instead
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
static inline int nanocoap_sock_connect(nanocoap_sock_t *sock, const sock_udp_ep_t *local,
                                        const sock_udp_ep_t *remote)
{
    return nanocoap_sock_udp_connect(sock, local, remote);
}
#endif

#if MODULE_NANOCOAP_DTLS || DOXYGEN
/**
 * @brief   Create a CoAP over DTLS client socket
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

#if MODULE_NANOCOAP_TCP
/**
 * @brief   Create a CoAP over TCP client socket
 *
 * @param[out]  sock        CoAP TCP socket
 * @param[in]   local_port  local TCP port, may be `0`
 * @param[in]   remote      remote TCP endpoint
 *
 * @note    Requires module `nanocoap_tcp`
 *
 * @returns     0 on success
 * @returns     <0 on error
 */
int nanocoap_sock_tcp_connect(nanocoap_sock_t *sock,
                              uint16_t local_port, const sock_tcp_ep_t *remote);
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
    switch (nanocoap_sock_get_type(sock)) {
#if MODULE_NANOCOAP_DTLS
    case COAP_SOCKET_TYPE_DTLS:
        sock_dtls_session_destroy(&sock->dtls, &sock->dtls_session);
        sock_dtls_close(&sock->dtls);
        /* always close the UDP connection (see: nanocoap_sock_t) */
        sock_udp_close(&sock->udp);
        break;
#endif
#if MODULE_NANOCOAP_UDP
    case COAP_SOCKET_TYPE_UDP:
        sock_udp_close(&sock->udp);
        break;
#endif
#if MODULE_NANOCOAP_TCP
    case COAP_SOCKET_TYPE_TCP:
        if (sock->tcp_buf) {
            sock_tcp_disconnect(&sock->tcp);
            free(sock->tcp_buf);
            sock->tcp_buf = NULL;
            sock->tcp_buf_fill = 0;
        }
        break;
#endif
    default:
        assert(0);
    }
}

/**
 * @brief   Observe a CoAP resource behind a URL (via GET)
 *
 * @note This requires the `nanocoap_sock_observe` module.
 *
 * @param[in]   url     URL to subscribe to
 * @param[out]  ctx     nanoCoAP observe context
 * @param[in]   cb      callback function called for every resource update
 * @param[in]   arg     callback function argument
 *
 * @returns     Number of bytes send on success
 * @retval      -EPROTONOSUPPORT    registration failed
 * @retval      <0                  other error

 */
ssize_t nanocoap_sock_observe_url(const char *url, coap_observe_client_t *ctx,
                                  coap_request_cb_t cb, void *arg);
/**
 * @brief   Stop observing a CoAP resource
 *
 * @note This requires the `nanocoap_sock_observe` module.
 *
 * @param[in]   url     URL to unsubscribe subscribe from
 * @param[out]  ctx     nanoCoAP observe context that was previously used with
 *                      @see nanocoap_sock_observe_url
 *
 * @pre         @p nanocoap_sock_observe_url has been called on the same arguments
 *              before and the Observation has not yet been cancelled yet.
 *
 * @returns     >=0 on success
 * @returns     <0 on error
 */
ssize_t nanocoap_sock_unobserve_url(const char *url, coap_observe_client_t *ctx);

/**
 * @brief   Build a CoAP packet for sending it over the given socket
 *
 * @note    This will pick the CoAP format suitable for the transport
 *          given in @p sock via @ref nanocoap_sock_request_cb
 *
 * @param[in]   sock        Socket the packet is intended for
 * @param[out]  pkt         Write the packet metadata here
 *                          (may be `NULL` to only write the content)
 * @param[out]  buf         Write the packet content here
 * @param[in]   buf_len     Size of @p buf in bytes
 * @param[in]   type        CoAP packet type (ignored, unless transport is UDP/DTLS)
 * @param[in]   token       CoAP Token to use
 * @param[in]   token_len   Length of @p token in bytes
 * @param[in]   code        Code (e.g. @ref COAP_METHOD_GET etc.)
 *
 * @return  The number of bytes written into @p buf on success
 * @retval  -EOVERFLOW      Required more than @p buf_len bytes in the buffer
 * @retval  -EINVAL         Called with invalid arguments
 */
ssize_t nanocoap_sock_build_pkt(nanocoap_sock_t *sock, coap_pkt_t *pkt,
                                void *buf, size_t buf_len,
                                uint8_t type,
                                const void *token, size_t token_len,
                                uint8_t code);

/**
 * @brief   Simple synchronous CoAP (confirmable) GET
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[out]  response buffer to write response to
 * @param[in]   len_max length of @p buffer
 *
 * @return          length of response payload or error code
 * @retval  >=0     Success, return value is the length of the response payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
ssize_t nanocoap_sock_get(nanocoap_sock_t *sock, const char *path,
                          void *response, size_t len_max);

/**
 * @brief   Simple non-confirmable GET
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path and query
 * @param[out]  response buffer for the response, may be NULL
 * @param[in]   len_max length of @p response
 *
 * @return          length of response payload or error code
 * @retval  >=0     Success, return value is the length of the response payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
ssize_t nanocoap_sock_get_non(nanocoap_sock_t *sock, const char *path, void *response,
                              size_t len_max);

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
 * @return          length of response payload or error code
 * @retval  >=0     Success, return value is the length of the response payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
ssize_t nanocoap_sock_put(nanocoap_sock_t *sock, const char *path, const void *request, size_t len,
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
 * @return          length of response payload or error code
 * @retval  >=0     Success, return value is the length of the response payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
ssize_t nanocoap_sock_put_non(nanocoap_sock_t *sock, const char *path, const void *request,
                              size_t len, void *response, size_t len_max);

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
ssize_t nanocoap_sock_put_url(const char *url, const void *request, size_t len, void *response,
                              size_t len_max);

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
 * @return          length of response payload or error code
 * @retval  >0      Success, return value is the length of the response payload
 * @retval  0       Success, response contained no payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
ssize_t nanocoap_sock_post(nanocoap_sock_t *sock, const char *path, const void *request, size_t len,
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
 * @return          length of response payload or error code
 * @retval  >0      Success, return value is the length of the response payload
 * @retval  0       Success, response contained no payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
ssize_t nanocoap_sock_post_non(nanocoap_sock_t *sock, const char *path, const void *request,
                               size_t len, void *response, size_t len_max);

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
ssize_t nanocoap_sock_post_url(const char *url, const void *request, size_t len, void *response,
                               size_t len_max);

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
 * @return          length of response payload or error code
 * @retval  >=0     Success, return value is the length of the response payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
ssize_t nanocoap_sock_fetch(nanocoap_sock_t *sock, const char *path, const void *request,
                            size_t len, void *response, size_t len_max);

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
 * @return          length of response payload or error code
 * @retval  >=0     Success, return value is the length of the response payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
ssize_t nanocoap_sock_fetch_non(nanocoap_sock_t *sock, const char *path, const void *request,
                                size_t len, void *response, size_t len_max);

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
ssize_t nanocoap_sock_fetch_url(const char *url, const void *request, size_t len, void *response,
                                size_t len_max);

/**
 * @brief   Simple synchronous CoAP (confirmable) DELETE
 *
 * @param[in]   sock    socket to use for the request
 * @param[in]   path    remote path (with query) to delete
 *
 * @retval  0       Success, return value is the length of the response payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
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
int nanocoap_sock_get_blockwise(nanocoap_sock_t *sock, const char *path, coap_blksize_t blksize,
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
int nanocoap_sock_get_slice(nanocoap_sock_t *sock, const char *path, coap_blksize_t blksize,
                            size_t offset, void *dst, size_t len);

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
int nanocoap_get_blockwise_url(const char *url, coap_blksize_t blksize,
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
ssize_t nanocoap_get_blockwise_url_to_buf(const char *url, coap_blksize_t blksize, void *buf,
                                          size_t len);

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
                                      coap_blksize_t blksize, void *buf, size_t len);

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
ssize_t nanocoap_sock_request_cb(nanocoap_sock_t *sock, coap_pkt_t *pkt, coap_request_cb_t cb,
                                 void *arg);

#if MODULE_NANOCOAP_UDP || DOXYGEN
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
 * @return          length of response payload or error code
 * @retval  >=0     Success, return value is the length of the response payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
ssize_t nanocoap_request_udp(coap_pkt_t *pkt,
                             const sock_udp_ep_t *local, const sock_udp_ep_t *remote,
                             size_t len);

/**
 * @brief   Alias for @ref nanocoap_request_udp
 *
 * @param[in,out]   pkt     Packet struct containing the request. Is reused for
 *                          the response
 * @param[in]       local   Local UDP endpoint, may be NULL
 * @param[in]       remote  remote UDP endpoint
 * @param[in]       len     Total length of the buffer associated with the
 *                          request
 *
 * @deprecated  Use @ref nanocoap_request_udp instead
 *
 * @return          length of response payload or error code
 * @retval  >=0     Success, return value is the length of the response payload
 * @retval  <0      Failure, return value is the error code. See
                    @ref nanocoap_sock_request_cb on error for the list of
                    error codes
 */
static inline ssize_t nanocoap_request(coap_pkt_t *pkt,
                                      const sock_udp_ep_t *local,
                                      const sock_udp_ep_t *remote,
                                      size_t len)
{
    return nanocoap_request_udp(pkt, local, remote, len);
}
#endif

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
                                                     nanocoap_sock_t *sock, const char *url,
                                                     coap_method_t method, coap_blksize_t blksize)
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
int nanocoap_sock_block_request(coap_block_request_t *ctx, const void *data, size_t len, bool more,
                                coap_request_cb_t cb, void *arg);

#if MODULE_NANOCOAP_TCP || DOXYGEN
/**
 * @brief   Send a CoAP-over-TCP CSM message
 *
 * @warning This is an internal function. It's API may change as needed without
 *          deprecation.
 *
 * @param[in]   sock        TCP socket to send the CSM message over
 * @param[out]  buf         Message buffer to assemble the CSM message in before
 *                          sending it out
 * @param[in]   buf_size    Size of @p buf in bytes. This will also be reported
 *                          to the other side as maximum size for messages
 *                          to receive.
 *
 * This will send a CSM message that indicates messages up at most @p buf_size
 * bytes will be processed (assuming @p buf is also used as receive buffer).
 * It will also indicate support for block-wise transfer. If and only if
 * module `nanocoap_token_ext` is used, support for RFC 8974 Extended Tokens
 * will be indicated.
 */
ssize_t nanocoap_send_csm_message(sock_tcp_t *sock, void *buf, size_t buf_size);
#endif

/**
 * @brief   Send a CoAP-over-TCP/CoAP-over-WebSocket Abort Signaling message
 *
 * @warning This is an internal function. It's API may change as needed without
 *          deprecation.
 *
 * @param[in]   sock        CoAP TCP/WS socket to send the CSM message over
 *
 * @retval 0    Success
 * @retval <0   Negative errno code indicating the rror
 */
int nanocoap_send_abort_signal(nanocoap_sock_t *sock);
#ifdef __cplusplus
}
#endif
/** @} */
