/*
 * Copyright (c) 2015-2016 Ken Bannister. All rights reserved.
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
 * ## Server Operation ##
 *
 * gcoap listens for requests on GCOAP_PORT, 5683 by default. You can redefine
 * this by uncommenting the appropriate lines in gcoap's make file.
 *
 * gcoap allows an application to specify a collection of request resource paths
 * it wants to be notified about. Create an array of resources, coap_resource_t
 * structs. Use gcoap_register_listener() at application startup to pass in
 * these resources, wrapped in a gcoap_listener_t.
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
 * -# Write the request payload, starting at the updated _payload_ pointer
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
 * Here is the expected sequence for preparing and sending a request:
 *
 * Allocate a buffer and a coap_pkt_t for the request.
 *
 * If there is a payload, follow the three steps below.
 *
 * -# Call gcoap_req_init() to initialize the request.
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
 * @{
 *
 * @file
 * @brief       gcoap definition
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 */

#ifndef GCOAP_H
#define GCOAP_H

#include "net/sock/udp.h"
#include "nanocoap.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Size for module message queue */
#define GCOAP_MSG_QUEUE_SIZE (4)

/** @brief Server port; use RFC 7252 default if not defined */
#ifndef GCOAP_PORT
#define GCOAP_PORT  (5683)
#endif

/** @brief Size of the buffer used to build a CoAP request or response. */
#define GCOAP_PDU_BUF_SIZE  (128)

/**
 * @brief Size of the buffer used to write options, other than Uri-Path, in a
 *        request.
 *
 * Accommodates Content-Format.
 */
#define GCOAP_REQ_OPTIONS_BUF  (8)

/**
 * @brief Size of the buffer used to write options in a response.
 *
 * Accommodates Content-Format.
 */
#define GCOAP_RESP_OPTIONS_BUF  (8)

/** @brief Maximum number of requests awaiting a response */
#define GCOAP_REQ_WAITING_MAX   (2)

/** @brief Maximum length in bytes for a token */
#define GCOAP_TOKENLEN_MAX      (8)

/** @brief Maximum length in bytes for a header, including the token */
#define GCOAP_HEADER_MAXLEN     (sizeof(coap_hdr_t) + GCOAP_TOKENLEN_MAX)

/** @brief Length in bytes for a token; use 2 if not defined */
#ifndef GCOAP_TOKENLEN
#define GCOAP_TOKENLEN      (2)
#endif

/** @brief  Marks the boundary between header and payload */
#define GCOAP_PAYLOAD_MARKER (0xFF)

/**
 * @name States for the memo used to track waiting for a response
 * @{
 */
#define GCOAP_MEMO_UNUSED   (0)  /**< This memo is unused */
#define GCOAP_MEMO_WAIT     (1)  /**< Request sent; awaiting response */
#define GCOAP_MEMO_RESP     (2)  /**< Got response */
#define GCOAP_MEMO_TIMEOUT  (3)  /**< Timeout waiting for response */
#define GCOAP_MEMO_ERR      (4)  /**< Error processing response packet */
/** @} */

/** @brief Time in usec that the event loop waits for an incoming CoAP message */
#define GCOAP_RECV_TIMEOUT   (1 * US_PER_SEC)

/**
 *
 * @brief Default time to wait for a non-confirmable response, in usec
 *
 * Set to 0 to disable timeout.
 */
#define GCOAP_NON_TIMEOUT    (5000000U)

/** @brief Identifies waiting timed out for a response to a sent message. */
#define GCOAP_MSG_TYPE_TIMEOUT    (0x1501)

/**
 * @brief Identifies a request to interrupt listening for an incoming message
 *        on a sock.
 *
 * Allows the event loop to process IPC messages.
 */
#define GCOAP_MSG_TYPE_INTR    (0x1502)

/**
 * @brief  A modular collection of resources for a server
 */
typedef struct gcoap_listener {
    coap_resource_t *resources;   /**< First element in the array of resources;
                                       must order alphabetically */
    size_t resources_len;         /**< Length of array */
    struct gcoap_listener *next;  /**< Next listener in list */
} gcoap_listener_t;

/**
 * @brief  Handler function for a server response, including the state for the
 *         originating request.
 *
 * If request timed out, the packet header is for the request.
 */
typedef void (*gcoap_resp_handler_t)(unsigned req_state, coap_pkt_t* pdu);

/**
 * @brief  Memo to handle a response for a request
 */
typedef struct {
    unsigned state;                     /**< State of this memo, a GCOAP_MEMO... */
    uint8_t hdr_buf[GCOAP_HEADER_MAXLEN];
                                        /**< Stores a copy of the request header */
    gcoap_resp_handler_t resp_handler;  /**< Callback for the response */
    xtimer_t response_timer;            /**< Limits wait for response */
    msg_t timeout_msg;                  /**< For response timer */
} gcoap_request_memo_t;

/**
 * @brief  Container for the state of gcoap itself
 */
typedef struct {
    gcoap_listener_t *listeners;       /**< List of registered listeners */
    gcoap_request_memo_t open_reqs[GCOAP_REQ_WAITING_MAX];
                                       /**< Storage for open requests; if first
                                            byte of an entry is zero, the entry
                                            is available */
    uint16_t last_message_id;          /**< Last message ID used */
} gcoap_state_t;

/**
 * @brief   Initializes the gcoap thread and device.
 *
 * Must call once before first use.
 *
 * @return  PID of the gcoap thread on success.
 * @return  -EEXIST, if thread already has been created.
 * @return  -EINVAL, if the IP port already is in use.
 */
kernel_pid_t gcoap_init(void);

/**
 * @brief   Starts listening for resource paths.
 *
 * @param listener Listener containing the resources.
 */
void gcoap_register_listener(gcoap_listener_t *listener);

/**
 * @brief  Initializes a CoAP request PDU on a buffer.
 *
 * @param[in] pdu Request metadata
 * @param[in] buf Buffer containing the PDU
 * @param[in] len Length of the buffer
 * @param[in] code Request code
 * @param[in] path Resource path
 *
 * @return 0 on success
 * @return < 0 on error
 */
int gcoap_req_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code,
                                                              char *path);

/**
 * @brief  Finishes formatting a CoAP PDU after the payload has been written.
 *
 * Assumes the PDU has been initialized with gcoap_req_init() or
 * gcoap_resp_init().
 *
 * @param[in] pdu Request metadata
 * @param[in] payload_len Length of the payload, or 0 if none
 * @param[in] format Format code for the payload; use COAP_FORMAT_NONE if not
 *                   specified
 *
 * @return size of the PDU
 * @return < 0 on error
 */
ssize_t gcoap_finish(coap_pkt_t *pdu, size_t payload_len, unsigned format);

/**
 *  @brief Writes a complete CoAP request PDU when there is not a payload.
 *
 * @param[in] pdu Request metadata
 * @param[in] buf Buffer containing the PDU
 * @param[in] len Length of the buffer
 * @param[in] code Request code
 * @param[in] path Resource path
 *
 * @return size of the PDU within the buffer
 * @return < 0 on error
 */
static inline ssize_t gcoap_request(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                                   unsigned code,
                                                                   char *path)
{
    return (gcoap_req_init(pdu, buf, len, code, path) == 0)
                ? gcoap_finish(pdu, 0, COAP_FORMAT_NONE)
                : -1;
}

/**
 * @brief  Sends a buffer containing a CoAP request to the provided endpoint.
 *
 * @param[in] buf Buffer containing the PDU
 * @param[in] len Length of the buffer
 * @param[in] remote Destination for the packet
 * @param[in] resp_handler Callback when response received
 *
 * @return length of the packet
 * @return 0 if cannot send
 */
size_t gcoap_req_send2(uint8_t *buf, size_t len, sock_udp_ep_t *remote,
                                                 gcoap_resp_handler_t resp_handler);

/**
 * @brief  Sends a buffer containing a CoAP request to the provided host/port.
 *
 * @deprecated  Please use @ref gcoap_req_send2() instead
 *
 * @param[in] buf Buffer containing the PDU
 * @param[in] len Length of the buffer
 * @param[in] addr Destination for the packet
 * @param[in] port Port at the destination
 * @param[in] resp_handler Callback when response received
 *
 * @return length of the packet
 * @return 0 if cannot send
 */
size_t gcoap_req_send(uint8_t *buf, size_t len, ipv6_addr_t *addr, uint16_t port,
                                                gcoap_resp_handler_t resp_handler);

/**
 * @brief  Initializes a CoAP response packet on a buffer.
 *
 * Initializes payload location within the buffer based on packet setup.
 *
 * @param[in] pdu Response metadata
 * @param[in] buf Buffer containing the PDU
 * @param[in] len Length of the buffer
 * @param[in] code Response code
 *
 * @return 0 on success
 * @return < 0 on error
 */
int gcoap_resp_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code);

/**
 * @brief  Writes a complete CoAP response PDU when there is no payload.
 *
 * @param[in] pdu Response metadata
 * @param[in] buf Buffer containing the PDU
 * @param[in] len Length of the buffer
 * @param[in] code Response code
 *
 * @return size of the PDU within the buffer
 * @return < 0 on error
 */
static inline ssize_t gcoap_response(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                                    unsigned code)
{
    return (gcoap_resp_init(pdu, buf, len, code) == 0)
                ? gcoap_finish(pdu, 0, COAP_FORMAT_NONE)
                : -1;
}

/**
 * @brief Provides important operational statistics.
 *
 * Useful for monitoring.
 *
 * @param[out] open_reqs Count of unanswered requests
 */
void gcoap_op_state(uint8_t *open_reqs);

#ifdef __cplusplus
}
#endif

#endif /* GCOAP_H */
/** @} */
