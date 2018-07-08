/*
 * Copyright (c) 2015-2017 Ken Bannister. All rights reserved.
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gcoap  CoAP
 * @ingroup     net
 * @brief       High-level interface to CoAP messaging
 *
 * gcoap provides a high-level interface for writing CoAP messages via RIOT's
 * sock networking API. gcoap internalizes network event processing so an
 * application only needs to focus on request/response handling. For a server,
 * gcoap accepts a list of resource paths with callbacks for writing the
 * response. For a client, gcoap provides a function to send a request, with a
 * callback for reading the server response. Generation of the request or
 * response requires from one to three well-defined steps, depending on
 * inclusion of a payload.
 *
 * gcoap allocates a RIOT message processing thread, so a single instance can
 * serve multiple applications. This approach also means gcoap uses a single UDP
 * port, which supports RFC 6282 compression. Internally, gcoap depends on the
 * nanocoap package for base level structs and functionality.
 *
 * gcoap also supports the Observe extension (RFC 7641) for a server. gcoap
 * provides functions to generate and send an observe notification that are
 * similar to the functions to send a client request.
 *
 * *Contents*
 *
 * - Server Operation
 * - Client Operation
 * - Observe Server Operation
 * - Implementation Notes
 * - Implementation Status
 *
 * ## Server Operation ##
 *
 * gcoap listens for requests on GCOAP_PORT, 5683 by default. You can redefine
 * this by uncommenting the appropriate lines in gcoap's make file.
 *
 * gcoap allows an application to specify a collection of request resource paths
 * it wants to be notified about. Create an array of resources (coap_resource_t
 * structs). Note that the elements must be ordered alphabetically with respect
 * to the resource path. Use gcoap_register_listener() at application startup
 * to pass in these resources, wrapped in a gcoap_listener_t.
 *
 * gcoap itself defines a resource for `/.well-known/core` discovery, which
 * lists all of the registered paths.
 *
 * ### Creating a response ###
 *
 * An application resource includes a callback function, a coap_handler_t. After
 * reading the request, the callback must use one or two functions provided by
 * gcoap to format the response, as described below. The callback *must* read
 * the request thoroughly before calling the functions, because the response
 * buffer likely reuses the request buffer. See `examples/gcoap/gcoap_cli.c`
 * for a simple example of a callback.
 *
 * Here is the expected sequence for a callback function:
 *
 * Read request completely and parse request payload, if any. Use the
 * coap_pkt_t _payload_ and _payload_len_ attributes.
 *
 * If there is a payload, follow the three steps below.
 *
 * -# Call gcoap_resp_init() to initialize the response.
 * -# Write the response payload, starting at the updated _payload_ pointer
 *    in the coap_pkt_t. If some error occurs, return a negative errno
 *    code from the handler, and gcoap will send a server error (5.00).
 * -# Call gcoap_finish() to complete the PDU after writing the payload,
 *    and return the result. gcoap will send the message.
 *
 * If no payload, call only gcoap_response() to write the full response.
 * Alternatively, you still can use gcoap_resp_init() and gcoap_finish(), as
 * described above. In fact, the gcoap_response() function is inline, and uses
 * those two functions.
 *
 * ## Client Operation ##
 *
 * Client operation includes two phases:  creating and sending a request, and
 * handling the response aynchronously in a client supplied callback.  See
 * `examples/gcoap/gcoap_cli.c` for a simple example of sending a request and
 * reading the response.
 *
 * ### Creating a request ###
 *
 * Here is the expected sequence to prepare and send a request:
 *
 * Allocate a buffer and a coap_pkt_t for the request.
 *
 * If there is a payload, follow the steps below.
 *
 * -# Call gcoap_req_init() to initialize the request.
 *    -# Optionally, mark the request confirmable by calling
 *       coap_hdr_set_type() with COAP_TYPE_CON.
 * -# Write the request payload, starting at the updated _payload_ pointer
 *    in the coap_pkt_t.
 * -# Call gcoap_finish(), which updates the packet for the payload.
 *
 * If no payload, call only gcoap_request() to write the full request.
 * Alternatively, you still can use gcoap_req_init() and gcoap_finish(),
 * as described above. The gcoap_request() function is inline, and uses those
 * two functions.
 *
 * Finally, call gcoap_req_send2() for the destination endpoint, as well as a
 * callback function for the host's response.
 *
 * ### Handling the response ###
 *
 * When gcoap receives the response to a request, it executes the callback from
 * the request. gcoap also executes the callback when a response is not
 * received within GCOAP_RESPONSE_TIMEOUT.
 *
 * Here is the expected sequence for handling a response in the callback.
 *
 * -# Test for a server response or timeout in the _req_state_ callback
 *    parameter. See the GCOAP_MEMO... constants.
 * -# Test the response with coap_get_code_class() and coap_get_code_detail().
 * -# Test the response payload with the coap_pkt_t _payload_len_ and
 *    _content_type_ attributes.
 * -# Read the payload, if any.
 *
 * ## Observe Server Operation
 *
 * A CoAP client may register for Observe notifications for any resource that
 * an application has registered with gcoap. An application does not need to
 * take any action to support Observe client registration. However, gcoap
 * limits registration for a given resource to a _single_ observer.
 *
 * An Observe notification is considered a response to the original client
 * registration request. So, the Observe server only needs to create and send
 * the notification -- no further communication or callbacks are required.
 *
 * ### Creating a notification ###
 *
 * Here is the expected sequence to prepare and send a notification:
 *
 * Allocate a buffer and a coap_pkt_t for the notification, then follow the
 * steps below.
 *
 * -# Call gcoap_obs_init() to initialize the notification for a resource.
 *    Test the return value, which may indicate there is not an observer for
 *    the resource. If so, you are done.
 * -# Write the notification payload, starting at the updated _payload_ pointer
 *    in the coap_pkt_t.
 * -# Call gcoap_finish(), which updates the packet for the payload.
 *
 * Finally, call gcoap_obs_send() for the resource.
 *
 * ### Other considerations ###
 *
 * By default, the value for the Observe option in a notification is three
 * bytes long. For resources that change slowly, this length can be reduced via
 * GCOAP_OBS_VALUE_WIDTH.
 *
 * A client always may re-register for a resource with the same token or with
 * a new token to indicate continued interest in receiving notifications about
 * it. Of course the client must not already be using any new token in the
 * registration for a different resource. Successful registration always is
 * indicated by the presence of the Observe option in the response.
 *
 * To cancel registration, the server expects to receive a GET request with
 * the Observe option value set to 1. The server does not support cancellation
 * via a reset (RST) response to a non-confirmable notification.
 *
 * ## Implementation Notes ##
 *
 * ### Building a packet ###
 *
 * The sequence and functions described above to build a request or response
 * is designed to provide a relatively simple API for the user.
 *
 * The structure of a CoAP PDU requires that options are placed between the
 * header and the payload. So, gcoap provides space in the buffer for them in
 * the request/response ...init() function, and then writes them during
 * gcoap_finish(). We trade some inefficiency/work in the buffer for
 * simplicity in the API.
 *
 * ### Waiting for a response ###
 *
 * We take advantage of RIOT's asynchronous messaging by using an xtimer to wait
 * for a response, so the gcoap thread does not block while waiting. The user is
 * notified via the same callback, whether the message is received or the wait
 * times out. We track the response with an entry in the
 * `_coap_state.open_reqs` array.
 *
 * ## Implementation Status ##
 * gcoap includes server and client capability. Available features include:
 *
 * - Message Type: Supports non-confirmable (NON) messaging. Additionally
 *   provides a callback on timeout. Provides piggybacked ACK response to a
 *   confirmable (CON) request.
 * - Observe extension: Provides server-side registration and notifications.
 * - Server and Client provide helper functions for writing the
 *   response/request. See the CoAP topic in the source documentation for
 *   details. See the gcoap example for sample implementations.
 * - Server allows an application to register a 'listener', which includes an
 *   array of endpoint paths and function callbacks used to write a response.
 * - Server listens on a port at startup; defaults to 5683.
 * - Client operates asynchronously; sends request and then handles response
 *   in a user provided callback.
 * - Client generates token; length defined at compile time.
 * - Options: Supports Content-Format for payload.
 *
 * @{
 *
 * @file
 * @brief       gcoap definition
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_GCOAP_H
#define NET_GCOAP_H

#include <stdint.h>

#include "net/ipv6/addr.h"
#include "net/sock/udp.h"
#include "net/nanocoap.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Size for module message queue
 */
#ifndef GCOAP_MSG_QUEUE_SIZE
#define GCOAP_MSG_QUEUE_SIZE    (4)
#endif

/**
 * @brief   Server port; use RFC 7252 default if not defined
 */
#ifndef GCOAP_PORT
#define GCOAP_PORT              (5683)
#endif

/**
 * @brief   Size of the buffer used to build a CoAP request or response
 */
#ifndef GCOAP_PDU_BUF_SIZE
#define GCOAP_PDU_BUF_SIZE      (128)
#endif

/**
 * @brief   Size of the buffer used to write options, other than Uri-Path, in a
 *          request
 *
 * Accommodates Content-Format and Uri-Queries
 */
#define GCOAP_REQ_OPTIONS_BUF   (40)

/**
 * @brief   Size of the buffer used to write options in a response
 *
 * Accommodates Content-Format.
 */
#define GCOAP_RESP_OPTIONS_BUF  (8)

/**
 * @brief   Size of the buffer used to write options in an Observe notification
 *
 * Accommodates Content-Format and Observe.
 */
#define GCOAP_OBS_OPTIONS_BUF   (8)

/**
 * @brief   Maximum number of requests awaiting a response
 */
#ifndef GCOAP_REQ_WAITING_MAX
#define GCOAP_REQ_WAITING_MAX   (2)
#endif

/**
 * @brief   Maximum length in bytes for a token
 */
#define GCOAP_TOKENLEN_MAX      (8)

/**
 * @brief   Maximum length in bytes for a header, including the token
 */
#define GCOAP_HEADER_MAXLEN     (sizeof(coap_hdr_t) + GCOAP_TOKENLEN_MAX)

/**
 * @brief   Length in bytes for a token; use 2 if not defined
 */
#ifndef GCOAP_TOKENLEN
#define GCOAP_TOKENLEN          (2)
#endif

/**
 * @brief   Marks the boundary between header and payload
 */
#define GCOAP_PAYLOAD_MARKER    (0xFF)

/**
 * @name    States for the memo used to track waiting for a response
 * @{
 */
#define GCOAP_MEMO_UNUSED       (0)     /**< This memo is unused */
#define GCOAP_MEMO_WAIT         (1)     /**< Request sent; awaiting response */
#define GCOAP_MEMO_RESP         (2)     /**< Got response */
#define GCOAP_MEMO_TIMEOUT      (3)     /**< Timeout waiting for response */
#define GCOAP_MEMO_ERR          (4)     /**< Error processing response packet */
/** @} */

/**
 * @brief   Value for send_limit in request memo when non-confirmable type
 */
#define GCOAP_SEND_LIMIT_NON    (-1)

/**
 * @brief   Time in usec that the event loop waits for an incoming CoAP message
 */
#ifndef GCOAP_RECV_TIMEOUT
#define GCOAP_RECV_TIMEOUT      (1 * US_PER_SEC)
#endif

/**
 * @brief   Default time to wait for a non-confirmable response [in usec]
 *
 * Set to 0 to disable timeout.
 */
#ifndef GCOAP_NON_TIMEOUT
#define GCOAP_NON_TIMEOUT       (5000000U)
#endif

/**
 * @brief   Identifies waiting timed out for a response to a sent message
 */
#define GCOAP_MSG_TYPE_TIMEOUT  (0x1501)

/**
 * @brief   Identifies a request to interrupt listening for an incoming message
 *          on a sock
 *
 * Allows the event loop to process IPC messages.
 */
#define GCOAP_MSG_TYPE_INTR     (0x1502)

/**
 * @brief   Maximum number of Observe clients; use 2 if not defined
 */
#ifndef GCOAP_OBS_CLIENTS_MAX
#define GCOAP_OBS_CLIENTS_MAX   (2)
#endif

/**
 * @brief   Maximum number of registrations for Observable resources; use 2 if
 *          not defined
 */
#ifndef GCOAP_OBS_REGISTRATIONS_MAX
#define GCOAP_OBS_REGISTRATIONS_MAX     (2)
#endif

/**
 * @name    States for the memo used to track Observe registrations
 * @{
 */
#define GCOAP_OBS_MEMO_UNUSED   (0) /**< This memo is unused */
#define GCOAP_OBS_MEMO_IDLE     (1) /**< Registration OK; no current activity */
#define GCOAP_OBS_MEMO_PENDING  (2) /**< Resource changed; notification pending */
/** @} */

/**
 * @brief   Width in bytes of the Observe option value for a notification
 *
 * This width is used to determine the length of the 'tick' used to measure
 * the time between observable changes to a resource. A tick is expressed
 * internally as GCOAP_OBS_TICK_EXPONENT, which is the base-2 log value of the
 * tick length in microseconds.
 *
 * The canonical setting for the value width is 3 (exponent 5), which results
 * in a tick length of 32 usec, per sec. 3.4, 4.4 of the RFC. Width 2
 * (exponent 16) results in a tick length of ~65 msec, and width 1 (exponent
 * 24) results in a tick length of ~17 sec.
 *
 * The tick length must be short enough so that the Observe value strictly
 * increases for each new notification. The purpose of the value is to allow a
 * client to detect message reordering within the network latency period (128
 * sec). For resources that change only slowly, the reduced message length is
 * useful when packet size is limited.
 */
#ifndef GCOAP_OBS_VALUE_WIDTH
#define GCOAP_OBS_VALUE_WIDTH   (3)
#endif

/**
 * @brief   See GCOAP_OBS_VALUE_WIDTH
 */
#if (GCOAP_OBS_VALUE_WIDTH == 3)
#define GCOAP_OBS_TICK_EXPONENT (5)
#elif (GCOAP_OBS_VALUE_WIDTH == 2)
#define GCOAP_OBS_TICK_EXPONENT (16)
#elif (GCOAP_OBS_VALUE_WIDTH == 1)
#define GCOAP_OBS_TICK_EXPONENT (24)
#endif

/**
 * @name    Return values for gcoap_obs_init()
 * @{
 */
#define GCOAP_OBS_INIT_OK       (0)
#define GCOAP_OBS_INIT_ERR      (-1)
#define GCOAP_OBS_INIT_UNUSED   (-2)
/** @} */

/**
 * @brief Stack size for module thread
 */
#ifndef GCOAP_STACK_SIZE
#define GCOAP_STACK_SIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE \
                          + sizeof(coap_pkt_t))
#endif

/**
 * @brief   Count of PDU buffers available for resending confirmable messages
 */
#ifndef GCOAP_RESEND_BUFS_MAX
#define GCOAP_RESEND_BUFS_MAX      (1)
#endif

/**
 * @brief   A modular collection of resources for a server
 */
typedef struct gcoap_listener {
    const coap_resource_t *resources;   /**< First element in the array of
                                         *   resources; must order alphabetically */
    size_t resources_len;               /**< Length of array */
    struct gcoap_listener *next;        /**< Next listener in list */
} gcoap_listener_t;

/**
 * @brief   Handler function for a server response, including the state for the
 *          originating request
 *
 * If request timed out, the packet header is for the request.
 */
typedef void (*gcoap_resp_handler_t)(unsigned req_state, coap_pkt_t* pdu,
                                     sock_udp_ep_t *remote);

/**
 * @brief  Extends request memo for resending a confirmable request.
 */
typedef struct {
    uint8_t *pdu_buf;                   /**< Buffer containing the PDU */
    size_t pdu_len;                     /**< Length of pdu_buf */
} gcoap_resend_t;

/**
 * @brief   Memo to handle a response for a request
 */
typedef struct {
    unsigned state;                     /**< State of this memo, a GCOAP_MEMO... */
    int send_limit;                     /**< Remaining resends, 0 if none;
                                             GCOAP_SEND_LIMIT_NON if non-confirmable */
    union {
        uint8_t hdr_buf[GCOAP_HEADER_MAXLEN];
                                        /**< Copy of PDU header, if no resends */
        gcoap_resend_t data;            /**< Endpoint and PDU buffer, for resend */
    } msg;                              /**< Request message data; if confirmable,
                                             supports resending message */
    sock_udp_ep_t remote_ep;            /**< Remote endpoint */
    gcoap_resp_handler_t resp_handler;  /**< Callback for the response */
    xtimer_t response_timer;            /**< Limits wait for response */
    msg_t timeout_msg;                  /**< For response timer */
} gcoap_request_memo_t;

/**
 * @brief   Memo for Observe registration and notifications
 */
typedef struct {
    sock_udp_ep_t *observer;            /**< Client endpoint; unused if null */
    const coap_resource_t *resource;    /**< Entity being observed */
    uint8_t token[GCOAP_TOKENLEN_MAX];  /**< Client token for notifications */
    unsigned token_len;                 /**< Actual length of token attribute */
} gcoap_observe_memo_t;

/**
 * @brief   Initializes the gcoap thread and device
 *
 * Must call once before first use.
 *
 * @return  PID of the gcoap thread on success.
 * @return  -EEXIST, if thread already has been created.
 * @return  -EINVAL, if the IP port already is in use.
 */
kernel_pid_t gcoap_init(void);

/**
 * @brief   Starts listening for resource paths
 *
 * @param[in] listener  Listener containing the resources.
 */
void gcoap_register_listener(gcoap_listener_t *listener);

/**
 * @brief   Initializes a CoAP request PDU on a buffer.
 *
 * @param[out] pdu      Request metadata
 * @param[out] buf      Buffer containing the PDU
 * @param[in] len       Length of the buffer
 * @param[in] code      Request code: GCOAP_[GET|POST|PUT|DELETE]
 * @param[in] path      Resource path, *must* start with '/'
 *
 * @pre @p path not `NULL`
 * @pre @p path must start with `/`
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gcoap_req_init(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                   unsigned code, const char *path);

/**
 * @brief   Finishes formatting a CoAP PDU after the payload has been written
 *
 * Assumes the PDU has been initialized with gcoap_req_init() or
 * gcoap_resp_init().
 *
 * @param[in,out] pdu       Request metadata
 * @param[in] payload_len   Length of the payload, or 0 if none
 * @param[in] format        Format code for the payload; use COAP_FORMAT_NONE if
 *                          not specified
 *
 * @return  size of the PDU
 * @return  < 0 on error
 */
ssize_t gcoap_finish(coap_pkt_t *pdu, size_t payload_len, unsigned format);

/**
 * @brief   Writes a complete CoAP request PDU when there is not a payload
 *
 * @param[in,out] pdu   Request metadata
 * @param[in,out] buf   Buffer containing the PDU
 * @param[in] len       Length of the buffer
 * @param[in] code      Request code: GCOAP_[GET|POST|PUT|DELETE]
 * @param[in] path      Resource path, *must* start with '/'
 *
 * @return  size of the PDU within the buffer
 * @return  < 0 on error
 */
static inline ssize_t gcoap_request(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                    unsigned code, char *path)
{
    return (gcoap_req_init(pdu, buf, len, code, path) == 0)
                ? gcoap_finish(pdu, 0, COAP_FORMAT_NONE)
                : -1;
}

/**
 * @brief   Sends a buffer containing a CoAP request to the provided endpoint
 *
 * @param[in] buf           Buffer containing the PDU
 * @param[in] len           Length of the buffer
 * @param[in] remote        Destination for the packet
 * @param[in] resp_handler  Callback when response received, may be NULL
 *
 * @return  length of the packet
 * @return  0 if cannot send
 */
size_t gcoap_req_send2(const uint8_t *buf, size_t len,
                       const sock_udp_ep_t *remote,
                       gcoap_resp_handler_t resp_handler);

/**
 * @brief  Sends a buffer containing a CoAP request to the provided host/port
 *
 * @deprecated  Please use @ref gcoap_req_send2() instead
 *
 * @param[in] buf           Buffer containing the PDU
 * @param[in] len           Length of the buffer
 * @param[in] addr          Destination for the packet
 * @param[in] port          Port at the destination
 * @param[in] resp_handler  Callback when response received, may be NULL
 *
 * @return  length of the packet
 * @return  0 if cannot send
 */
size_t gcoap_req_send(const uint8_t *buf, size_t len, const ipv6_addr_t *addr,
                      uint16_t port, gcoap_resp_handler_t resp_handler);

/**
 * @brief   Initializes a CoAP response packet on a buffer
 *
 * Initializes payload location within the buffer based on packet setup.
 *
 * @param[out] pdu      Response metadata
 * @param[in] buf       Buffer containing the PDU
 * @param[in] len       Length of the buffer
 * @param[in] code      Response code
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gcoap_resp_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code);

/**
 * @brief   Writes a complete CoAP response PDU when there is no payload
 *
 * @param[out] pdu      Response metadata
 * @param[out] buf      Buffer containing the PDU
 * @param[in] len       Length of the buffer
 * @param[in] code      Response code
 *
 * @return  size of the PDU within the buffer
 * @return  < 0 on error
 */
static inline ssize_t gcoap_response(coap_pkt_t *pdu, uint8_t *buf,
                                     size_t len, unsigned code)
{
    return (gcoap_resp_init(pdu, buf, len, code) == 0)
                ? gcoap_finish(pdu, 0, COAP_FORMAT_NONE)
                : -1;
}

/**
 * @brief   Initializes a CoAP Observe notification packet on a buffer, for the
 *          observer registered for a resource
 *
 * First verifies that an observer has been registered for the resource.
 *
 * @param[out] pdu      Notification metadata
 * @param[out] buf      Buffer containing the PDU
 * @param[in] len       Length of the buffer
 * @param[in] resource  Resource for the notification
 *
 * @return  GCOAP_OBS_INIT_OK     on success
 * @return  GCOAP_OBS_INIT_ERR    on error
 * @return  GCOAP_OBS_INIT_UNUSED if no observer for resource
 */
int gcoap_obs_init(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                   const coap_resource_t *resource);

/**
 * @brief   Sends a buffer containing a CoAP Observe notification to the
 *          observer registered for a resource
 *
 * Assumes a single observer for a resource.
 *
 * @param[in] buf Buffer containing the PDU
 * @param[in] len Length of the buffer
 * @param[in] resource Resource to send
 *
 * @return  length of the packet
 * @return  0 if cannot send
 */
size_t gcoap_obs_send(const uint8_t *buf, size_t len,
                      const coap_resource_t *resource);

/**
 * @brief   Provides important operational statistics
 *
 * Useful for monitoring.
 *
 * @return  count of unanswered requests
 */
uint8_t gcoap_op_state(void);

/**
 * @brief   Get the resource list, currently only `CoRE Link Format`
 *          (COAP_FORMAT_LINK) supported
 *
 * If @p buf := NULL, nothing will be written but the size of the resulting
 * resource list is computed and returned.
 *
 * @param[out] buf      output buffer to write resource list into, my be NULL
 * @param[in]  maxlen   length of @p buf, ignored if @p buf is NULL
 * @param[in]  cf       content format to use for the resource list, currently
 *                      only COAP_FORMAT_LINK supported
 *
 * @todo    add support for `JSON CoRE Link Format`
 * @todo    add support for 'CBOR CoRE Link Format`
 *
 * @return  the number of bytes written to @p buf
 * @return  -1 on error
 */
int gcoap_get_resource_list(void *buf, size_t maxlen, uint8_t cf);

/**
 * @brief   Adds a single Uri-Query option to a CoAP request
 *
 * To add multiple Uri-Query options, simply call this function multiple times.
 * The Uri-Query options will be added in the order those calls.
 *
 * @param[out] pdu      The package that is being build
 * @param[in]  key      Key to add to the query string
 * @param[in]  val      Value to assign to @p key (may be NULL)
 *
 * @pre     ((pdu != NULL) && (key != NULL))
 *
 * @return  overall length of new query string
 * @return  -1 on error
 */
int gcoap_add_qstring(coap_pkt_t *pdu, const char *key, const char *val);

#ifdef __cplusplus
}
#endif

#endif /* NET_GCOAP_H */
/** @} */
