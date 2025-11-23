/*
 * Copyright (c) 2015-2017 Ken Bannister. All rights reserved.
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gcoap  GCoAP
 * @ingroup     net
 * @brief       High-level interface to CoAP messaging
 *
 * gcoap provides a high-level interface for writing CoAP messages via RIOT's
 * sock networking API. gcoap internalizes network event processing so an
 * application only needs to focus on request/response handling. For a server,
 * gcoap accepts a list of resource paths with callbacks for writing the
 * response. For a client, gcoap provides a function to send a request, with a
 * callback for reading the server response.
 *
 * gcoap allocates a RIOT message processing thread, so a single instance can
 * serve multiple applications. This approach also means gcoap uses a single UDP
 * port, which supports RFC 6282 compression. Internally, gcoap depends on the
 * nanocoap package for base level structs and functionality. gcoap uses
 * nanocoap's Packet API to write message options.
 *
 * gcoap supports the Observe extension (RFC 7641) for a server. gcoap provides
 * functions to generate and send an observe notification that are similar to
 * the functions to send a client request. gcoap also supports the Block
 * extension (RFC 7959) with block-specific option functions as well as some
 * helpers.
 *
 * *Contents*
 *
 * - Server Operation
 * - Client Operation
 * - Observe Server Operation
 * - Block Operation
 * - Proxy Operation
 * - DTLS for transport security
 * - Implementation Notes
 * - Implementation Status
 *
 * ## Server Operation ##
 *
 * gcoap listens for requests on CONFIG_GCOAP_PORT, 5683 by default. You can redefine
 * this by uncommenting the appropriate lines in gcoap's make file.
 *
 * gcoap allows an application to specify a collection of request resource paths
 * it wants to be notified about. Create an array of resources (coap_resource_t
 * structs) ordered by the resource path, specifically the ASCII encoding of
 * the path characters (digit and capital precede lower case). Use
 * gcoap_register_listener() at application startup to pass in these resources,
 * wrapped in a gcoap_listener_t. Also see _Server path matching_ in the base
 * [nanocoap](group__net__nanocoap.html) documentation.
 *
 * gcoap itself defines a resource for `/.well-known/core` discovery, which
 * lists all of the registered paths. See the _Resource list creation_ section
 * below for more.
 *
 * ### Creating a response ###
 *
 * An application resource includes a callback function, a coap_handler_t. After
 * reading the request, the callback must use functions provided by gcoap to
 * format the response, as described below. The callback *must* read the request
 * thoroughly before calling the functions, because the response buffer likely
 * reuses the request buffer. See `examples/networking/coap/gcoap/client.c` for a simple
 * example of a callback.
 *
 * Here is the expected sequence for a callback function:
 *
 * Read request completely and parse request payload, if any. Use the
 * coap_pkt_t _payload_ and _payload_len_ attributes.
 *
 * If there is a payload, follow the steps below.
 *
 * -# Call gcoap_resp_init() to initialize the response.
 * -# Use the coap_opt_add_xxx() functions to include any Options, for example
 *    coap_opt_add_format() for Content-Format of the payload. Options *must*
 *    be written in order by option number (see "CoAP option numbers" in
 *    [CoAP defines](group__net__coap.html)).
 * -# Call coap_opt_finish() to complete the PDU metadata. Retain the returned
 *    metadata length.
 * -# Write the response payload, starting at the updated _payload_ pointer
 *    in the coap_pkt_t, for up to _payload_len_ bytes.
 * -# Return the sum of the metadata length and payload length. If some error
 *    has occurred, return a negative errno code from the handler, and gcoap
 *    will send a server error (5.00).
 *
 * If no payload, call only gcoap_response() to write the full response. If you
 * need to add Options, follow the first three steps in the list above instead.
 *
 * ### Resource list creation ###
 *
 * gcoap allows customization of the function that provides the list of registered
 * resources for `/.well-known/core` and CoRE Resource Directory registration.
 * By default gcoap provides gcoap_encode_link(), which lists only the target
 * path for each link. However, an application may specify a custom function in
 * the gcoap_listener_t it registers with gcoap. For example, this function may
 * add parameters to provide more information about the resource, as described
 * in RFC 6690. See the gcoap example for use of a custom encoder function.
 *
 * ## Client Operation ##
 *
 * Client operation includes two phases: creating and sending a request, and
 * handling the response asynchronously in a client supplied callback. See
 * `examples/networking/coap/gcoap/client.c` for a simple example of sending a request and
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
 * -# Optionally, mark the request confirmable by calling coap_hdr_set_type()
 *    with COAP_TYPE_CON.
 * -# Use the coap_opt_add_xxx() functions to include any Options beyond
 *    Uri-Path, which was added in the first step. Options *must* be written
 *    in order by option number (see "CoAP option numbers" in
 *    [CoAP defines](group__net__coap.html)).
 * -# Call coap_opt_finish() to complete the PDU metadata. Retain the returned
 *    metadata length.
 * -# Write the request payload, starting at the updated _payload_ pointer
 *    in the coap_pkt_t, for up to _payload_len_ bytes.
 *
 * If no payload, call only gcoap_request() to write the full request. If you
 * need to add Options, follow the first four steps in the list above instead.
 *
 * Finally, call gcoap_req_send() with the sum of the metadata length and
 * payload length, the destination endpoint, and a callback function for the
 * host's response.
 *
 * ### Handling the response ###
 *
 * When gcoap receives the response to a request, it executes the callback from
 * the request. gcoap also executes the callback when a response is not
 * received within GCOAP_RESPONSE_TIMEOUT.
 *
 * Here is the expected sequence for handling a response in the callback.
 *
 * -# Test for a server response or timeout in the `state` field of the `memo`
 *    callback parameter (`memo->state`). See the GCOAP_MEMO... constants.
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
 * It is [suggested](https://tools.ietf.org/html/rfc7641#section-6) that a
 * server adds the 'obs' attribute to resources that are useful for observation
 * (i.e. will produce notifications) as a hint. Keep in mind that this is not
 * mandatory in order to enable the mechanism in RIOT, nor will it prevent a
 * client from observing a resource that does not have this attribute in the
 * link description. See the "Resource list creation" section above for how the
 * gcoap example app publishes the obs attribute.
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
 * -# Use the coap_opt_add_xxx() functions to include any Options, for example
 *    coap_opt_add_format() for Content-Format of the payload. Options *must*
 *    be written in order by option number (see "CoAP option numbers" in
 *    [CoAP defines](group__net__coap.html)).
 * -# Call coap_opt_finish() to complete the PDU metadata. Retain the returned
 *    metadata length.
 * -# Write the notification payload, starting at the updated _payload_ pointer
 *    in the coap_pkt_t, for up to _payload_len_ bytes.
 *
 * Finally, call gcoap_obs_send() for the resource, with the sum of the
 * metadata length and payload length for the representation.
 *
 * ### Other considerations ###
 *
 * By default, the value for the Observe option in a notification is three
 * bytes long. For resources that change slowly, this length can be reduced via
 * CONFIG_GCOAP_OBS_VALUE_WIDTH.
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
 * ## Block Operation ##
 *
 * gcoap provides for both server side and client side blockwise messaging for
 * requests and responses. This section outlines how to write a message for
 * each situation.
 *
 * ### CoAP server GET handling ###
 *
 * The server must slice the full response body into smaller payloads, and
 * identify the slice with a Block2 option. This implementation toggles the
 * actual writing of data as it passes over the code for the full response
 * body. See the _riot_block2_handler() example in
 * [gcoap-block-server](https://github.com/kb2ma/riot-apps/blob/kb2ma-master/gcoap-block-server/gcoap_block.c),
 * which implements the sequence described below.
 *
 * - Use coap_block2_init() to initialize a _slicer_ struct from the Block2
 *   option in the request. The slicer tracks boundaries while writing the
 *   payload. If no option present in the initial request, the init function
 *   defaults to a payload size of 16 bytes.
 * - Use gcoap_resp_init() to begin the response.
 * - Use coap_opt_add_block2() to write the Block2 option from the slicer. Use
 *   1 as a default for the _more_ parameter. At this point, we don't know yet
 *   if this message will be the last in the block exchange. However, we must
 *   add the block option at this location in the message.
 * - Use coap_opt_finish() to add a payload marker.
 * - Add the payload using the `coap_blockwise_put_xxx()` functions. The slicer
 *   knows the current position in the overall body of the response. It writes
 *   only the portion of the body specified by the block number and block size
 *   in the slicer.
 * - Finally, use coap_block2_finish() to finalize the block option with the
 *   proper value for the _more_ parameter.
 *
 * ### CoAP server PUT/POST handling ###
 *
 * The server must ack each blockwise portion of the response body received
 * from the client by writing a Block1 option in the response. See the
 * _sha256_handler() example in
 * [gcoap-block-server](https://github.com/kb2ma/riot-apps/blob/kb2ma-master/gcoap-block-server/gcoap_block.c),
 * which implements the sequence described below.
 *
 * - Use coap_get_block1() to initialize a block1 struct from the request.
 * - Determine the response code. If the block1 _more_ attribute is 1, use
 *   COAP_CODE_CONTINUE to request more responses. Otherwise, use
 *   COAP_CODE_CHANGED to indicate a successful transfer.
 * - Use gcoap_resp_init() to begin the response, including the response code.
 * - Use coap_opt_add_block1_control() to write the Block1 option.
 * - Use coap_opt_finish() to determine the length of the PDU. If appropriate,
 *   use the COAP_OPT_FINISH_PAYLOAD parameter and then write the payload.
 *
 * ### CoAP client GET request ###
 *
 * The client requests a specific blockwise payload from the overall body by
 * writing a Block2 option in the request. See _resp_handler() in the
 * [gcoap](https://github.com/RIOT-OS/RIOT/blob/master/examples/networking/coap/gcoap/client.c)
 * example in the RIOT distribution, which implements the sequence described
 * below.
 *
 * - For the first request, use coap_block_object_init() to initialize a new
 *   block1 struct. For subsequent requests, first use coap_get_block2() to
 *   read the Block2 option in the response to the previous request. If the
 *   _more_ attribute indicates no more blocks, you are done.
 *   - The gcoap example actually does _not_ include a Block2 option in the
 *     original request, but the server response includes a blockwise response
 *     with a Block2 option anyway. On the other hand, this example shows how
 *     blockwise messaging can be supported in a generic way.
 * - If more blocks are available, use gcoap_req_init() to create a new
 *   request.
 * - Increment the _blknum_ attribute in the block1 struct from the previous
 *   response to request the next blockwise payload.
 * - Use coap_opt_put_block2_control() to write the Block2 option to the
 *   request.
 * - Use coap_opt_finish() to determine the length of the PDU.
 *
 * ### CoAP client PUT/POST request ###
 *
 * The client pushes a specific blockwise payload from the overall body to the
 * server by writing a Block1 option in the request. See _do_block_post() in
 * the [gcoap-block-client](https://github.com/kb2ma/riot-apps/blob/kb2ma-master/gcoap-block-client/gcoap_block.c)
 * example, which implements the sequence described below.
 *
 * - For the first request, use coap_block_slicer_init() to initialize a
 *   _slicer_ struct with the desired block number and block size. For
 *   subsequent requests, first read the response from the server to the
 *   previous request. If the response code is COAP_CODE_CONTINUE, then
 *   increment the last block number sent when initializing the slicer struct
 *   for the next request.
 * - Use gcoap_req_init() to initialize the request.
 * - Use coap_opt_add_block1() to add the Block1 option from the slicer. Use 1
 *   as a default for the _more_ parameter. At this point, we don't know yet if
 *   this message will be the last in the block exchange. However, we must add
 *   the block option at this location in the message.
 * - Use coap_opt_finish() with COAP_OPT_FINISH_PAYLOAD to write the payload
 *   marker.
 * - Add the payload using the `coap_blockwise_put_xxx()` functions. The slicer
 *   knows the current position in the overall body of the response. It writes
 *   only the portion of the body specified by the block number and block size
 *   in the slicer.
 * - Finally, use coap_block1_finish() to finalize the block option with the
 *   proper value for the _more_ parameter.
 *
 * ## Proxy Operation ##
 *
 * A [CoAP proxy](https://tools.ietf.org/html/rfc7252#section-5.7.1)
 * forwards incoming requests to an origin server, or again to another
 * proxy server.
 *
 * ### Proxy Client Handling
 *
 * The current implementation only allows the use of `Proxy-Uri` to
 * specify the absolute URI for the origin server and resource. A
 * request that includes a `Proxy-Uri` option must not contain any of
 * the `Uri-*` options. An example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * // endpoint for the proxy server
 *  sock_udp_ep_t *proxy_remote = ...;
 * // absolute URI for the origin server and resource
 * char *uri = "coap://[2001:db8::1]:5683/.well-known/core";
 *
 * gcoap_req_init(&pdu, buf, CONFIG_GCOAP_PDU_BUF_SIZE, COAP_METHOD_GET, NULL);
 * coap_opt_add_proxy_uri(&pdu, uri);
 * unsigned len = coap_opt_finish(&pdu, COAP_OPT_FINISH_NONE);
 *
 * gcoap_req_send((uint8_t *) pdu->hdr, len, proxy_remote, NULL, _resp_handler, NULL,
 *                GCOAP_SOCKET_TYPE_UNDEF);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * See the gcoap example for a sample implementation.
 *
 * ### Proxy Server Handling
 *
 * Not implemented yet.
 *
 * ## DTLS as transport security ##
 *
 * GCoAP allows to use DTLS for transport security by using the @ref net_sock_dtls
 * "DTLS sock API". Using the module gcoap_dtls enables the support. GCoAP
 * listens for requests on CONFIG_GCOAPS_PORT, 5684 by default when DTLS is enabled.
 *
 * Credentials have to been configured before use. See @ref net_credman "Credman"
 * and @ref net_sock_dtls_creds "DTLS sock credentials API" for credential managing.
 * Access to the DTLS socket is provided by gcoap_get_sock_dtls().
 *
 * GCoAP includes a DTLS session management component that stores active sessions.
 * By default, it tries to have CONFIG_GCOAP_DTLS_MINIMUM_AVAILABLE_SESSIONS
 * session slots available to keep the server responsive. If not enough sessions
 * are available the server destroys the session that has not been used for the
 * longest time after CONFIG_GCOAP_DTLS_MINIMUM_AVAILABLE_SESSIONS_TIMEOUT_USEC.
 *
 * ## Implementation Notes ##
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
 * - Observe extension: Provides server-side registration and notifications
 *   and client-side observe.
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

#include <stdint.h>

#include "event/callback.h"
#include "event/timeout.h"
#include "net/sock/udp.h"
#include "net/nanocoap.h"

#if IS_USED(MODULE_GCOAP_DTLS)
#  include "net/sock/dtls.h"
#endif
#if IS_USED(MODULE_NANOCOAP_CACHE)
#  include "net/nanocoap/cache.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_gcoap_conf    GCoAP compile configurations
 * @ingroup  net_gcoap
 * @ingroup  config
 * @{
 */
/**
 * @brief   Server port; use RFC 7252 default if not defined
 */
#ifndef CONFIG_GCOAP_PORT
#define CONFIG_GCOAP_PORT              (5683)
#endif
/**
 * @brief   Secure Server port; use RFC 7252 default if not defined
 */
#ifndef CONFIG_GCOAPS_PORT
#define CONFIG_GCOAPS_PORT             (5684)
#endif

/**
 * @brief   Timeout for the DTLS handshake process. Set to 0 for infinite time
 */
#ifndef CONFIG_GCOAP_DTLS_HANDSHAKE_TIMEOUT_MSEC
#define CONFIG_GCOAP_DTLS_HANDSHAKE_TIMEOUT_MSEC    (3 * MS_PER_SEC)
#endif

/**
 * @brief   Number of minimum available sessions. If the count of available
 *          sessions falls below this threshold, the oldest used session will be
 *          closed after a timeout time. Set to 0 to deactivate this feature.
 */
#ifndef CONFIG_GCOAP_DTLS_MINIMUM_AVAILABLE_SESSIONS
#define CONFIG_GCOAP_DTLS_MINIMUM_AVAILABLE_SESSIONS  (1)
#endif

/**
 * @brief   Timeout for freeing up a session when minimum number of available
 *          sessions is not given.
 */
#ifndef CONFIG_GCOAP_DTLS_MINIMUM_AVAILABLE_SESSIONS_TIMEOUT_MSEC
#define CONFIG_GCOAP_DTLS_MINIMUM_AVAILABLE_SESSIONS_TIMEOUT_MSEC  (15 * MS_PER_SEC)
#endif

/**
 * @brief   Size of the buffer used to build a CoAP request or response
 */
#ifndef CONFIG_GCOAP_PDU_BUF_SIZE
#define CONFIG_GCOAP_PDU_BUF_SIZE      (128)
#endif

/**
 * @brief   Maximum number of requests awaiting a response
 */
#ifndef CONFIG_GCOAP_REQ_WAITING_MAX
#define CONFIG_GCOAP_REQ_WAITING_MAX   (2)
#endif
/** @} */

/**
 * @brief   Maximum length in bytes for a token
 */
#define GCOAP_TOKENLEN_MAX      (8)

/**
 * @brief   Maximum length in bytes for a header, including the token
 */
#define GCOAP_HEADER_MAXLEN     (sizeof(coap_udp_hdr_t) + GCOAP_TOKENLEN_MAX)

/**
 * @ingroup net_gcoap_conf
 * @brief   Length in bytes for a token
 *
 * Value must be in the range 0 to @ref GCOAP_TOKENLEN_MAX.
 */
#ifndef CONFIG_GCOAP_TOKENLEN
#define CONFIG_GCOAP_TOKENLEN          (2)
#endif

/**
 * @brief   Marks the boundary between header and payload
 */
#define GCOAP_PAYLOAD_MARKER    (0xFF)

/**
 * @ingroup net_gcoap_conf
 * @brief   Disables gcoap startup during system auto init
 *
 * If disabled, gcoap_init() must be called by some other means.
 */
#ifndef CONFIG_GCOAP_NO_AUTO_INIT
#define CONFIG_GCOAP_NO_AUTO_INIT      0
#endif

/**
 * @name    States for the memo used to track waiting for a response
 * @{
 */
#define GCOAP_MEMO_UNUSED       (0)     /**< This memo is unused */
#define GCOAP_MEMO_RETRANSMIT   (1)     /**< Request sent, retransmitting until response arrives */
#define GCOAP_MEMO_WAIT         (2)     /**< Request sent; awaiting response */
#define GCOAP_MEMO_RESP         (3)     /**< Got response */
#define GCOAP_MEMO_TIMEOUT      (4)     /**< Timeout waiting for response */
#define GCOAP_MEMO_ERR          (5)     /**< Error processing response packet */
#define GCOAP_MEMO_RESP_TRUNC   (6)     /**< Got response, but it got truncated into the receive
                                             buffer that is now incomplete */
/** @} */

/**
 * @brief   Value for send_limit in request memo when non-confirmable type
 */
#define GCOAP_SEND_LIMIT_NON    (-1)

#ifdef DOXYGEN
/**
 * @ingroup net_gcoap_conf
 * @brief   Turns off retransmission backoff when defined (undefined per default)
 *
 * In normal operations the timeout between retransmissions doubles. When
 * CONFIG_GCOAP_NO_RETRANS_BACKOFF is defined this doubling does not happen.
 *
 * @see CONFIG_COAP_ACK_TIMEOUT_MS
 */
#define CONFIG_GCOAP_NO_RETRANS_BACKOFF
#endif

/**
 * @ingroup net_gcoap_conf
 * @brief   Default time to wait for a non-confirmable response [in msec]
 *
 * Set to 0 to disable timeout.
 */
#ifndef CONFIG_GCOAP_NON_TIMEOUT_MSEC
#define CONFIG_GCOAP_NON_TIMEOUT_MSEC       (5000U)
#endif

/**
 * @ingroup net_gcoap_conf
 * @brief   Maximum number of Observe clients
 *
 * @note As documented in this file, the implementation is limited to one observer per resource.
 *       Therefore, every stored observer is associated with a different resource.
 *       If you have only one observable resource, you could set this value to 1.
 */
#ifndef CONFIG_GCOAP_OBS_CLIENTS_MAX
#define CONFIG_GCOAP_OBS_CLIENTS_MAX   (2)
#endif

/**
 * @ingroup net_gcoap_conf
 * @brief   Maximum number of local notifying endpoint addresses
 *
 * @note As documented in this file, the implementation is limited to one observer per resource.
 *       Therefore, every stored local endpoint alias is associated with an observation context
 *       of a different resource.
 *       If you have only one observable resource, you could set this value to 1.
 */
#ifndef CONFIG_GCOAP_OBS_NOTIFIERS_MAX
#define CONFIG_GCOAP_OBS_NOTIFIERS_MAX  (2)
#endif

/**
 * @ingroup net_gcoap_conf
 * @brief   Maximum number of registrations for Observable resources
 *
 * @note As documented in this file, the implementation is limited to one observer per resource.
 *       Therefore, every stored observation context is associated with a different resource.
 *       If you have only one observable resource, you could set this value to 1.
 */
#ifndef CONFIG_GCOAP_OBS_REGISTRATIONS_MAX
#define CONFIG_GCOAP_OBS_REGISTRATIONS_MAX     (2)
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
 * @ingroup net_gcoap_conf
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
#ifndef CONFIG_GCOAP_OBS_VALUE_WIDTH
#define CONFIG_GCOAP_OBS_VALUE_WIDTH   (3)
#endif

/**
 * @brief   See CONFIG_GCOAP_OBS_VALUE_WIDTH
 */
#if (CONFIG_GCOAP_OBS_VALUE_WIDTH == 3)
#define GCOAP_OBS_TICK_EXPONENT (0)
#elif (CONFIG_GCOAP_OBS_VALUE_WIDTH == 2)
#define GCOAP_OBS_TICK_EXPONENT (6)
#elif (CONFIG_GCOAP_OBS_VALUE_WIDTH == 1)
#define GCOAP_OBS_TICK_EXPONENT (14)
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
 * @{
 */
#ifndef GCOAP_DTLS_EXTRA_STACKSIZE
#if IS_USED(MODULE_GCOAP_DTLS)
#define GCOAP_DTLS_EXTRA_STACKSIZE  (THREAD_STACKSIZE_DEFAULT)
#else
#define GCOAP_DTLS_EXTRA_STACKSIZE  (0)
#endif
#endif

/**
 * @brief Extra stack for VFS operations
 */
#if IS_USED(MODULE_GCOAP_FILESERVER)
#include "vfs.h"
#define GCOAP_VFS_EXTRA_STACKSIZE   (VFS_DIR_BUFFER_SIZE + VFS_FILE_BUFFER_SIZE)
#else
#define GCOAP_VFS_EXTRA_STACKSIZE   (0)
#endif

#ifndef GCOAP_STACK_SIZE
#define GCOAP_STACK_SIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE \
                          + sizeof(coap_pkt_t) + GCOAP_DTLS_EXTRA_STACKSIZE \
                          + GCOAP_VFS_EXTRA_STACKSIZE)
#endif
/** @} */

/**
 * @ingroup net_gcoap_conf
 * @brief   Count of PDU buffers available for resending confirmable messages
 */
#ifndef CONFIG_GCOAP_RESEND_BUFS_MAX
#define CONFIG_GCOAP_RESEND_BUFS_MAX      (1)
#endif

/**
 * @name Bitwise positional flags for encoding resource links
 * @anchor COAP_LINK_FLAG_
 * @{
 */
#define COAP_LINK_FLAG_INIT_RESLIST  (1)  /**< initialize as for first resource
                                           *   in a list */

/** @} */

/**
 * @brief   Context information required to write a resource link
 */
typedef struct {
    unsigned content_format;            /**< link format */
    size_t link_pos;                    /**< position of link within listener */
    uint16_t flags;                     /**< encoder switches; see @ref
                                             COAP_LINK_FLAG_ constants */
} coap_link_encoder_ctx_t;

/**
 * @brief   Handler function to write a resource link
 *
 * @param[in] resource      Resource for link
 * @param[out] buf          Buffer on which to write; may be null
 * @param[in] maxlen        Remaining length for @p buf
 * @param[in] context       Contextual information on what/how to write
 *
 * @return  count of bytes written to @p buf (or writable if @p buf is null)
 * @return  -1 on error
 */
typedef ssize_t (*gcoap_link_encoder_t)(const coap_resource_t *resource, char *buf,
                                        size_t maxlen, coap_link_encoder_ctx_t *context);

/**
 * @name    Return values for resource related operations
 * @{
 */
#define GCOAP_RESOURCE_FOUND        (0)
#define GCOAP_RESOURCE_WRONG_METHOD (1)
#define GCOAP_RESOURCE_NO_PATH      (2)
#define GCOAP_RESOURCE_ERROR        (3)
/** @} */

/**
 * @brief   Forward declaration of the gcoap listener state container
 */
typedef struct gcoap_listener gcoap_listener_t;

/**
 * @brief   Handler function for the request matcher strategy
 *
 * @param[in]  listener     Listener context
 * @param[out] resource     Matching resource
 * @param[in]  pdu          Pointer to the PDU
 *
 * @return  GCOAP_RESOURCE_FOUND      on resource match
 * @return  GCOAP_RESOURCE_NO_PATH    on no path found in @p resource
 *                                    that matches @p pdu
 * @return  GCOAP_RESOURCE_ERROR      on processing failure of the request
 */
typedef int (*gcoap_request_matcher_t)(gcoap_listener_t *listener,
                                       const coap_resource_t **resource,
                                       coap_pkt_t *pdu);

/**
 * @brief   CoAP socket types
 *
 * May be used as flags for @ref gcoap_listener_t, but must be used numerically
 * with @ref gcoap_req_send().
 */
typedef enum {
    GCOAP_SOCKET_TYPE_UNDEF = 0x0,      /**< undefined */
    GCOAP_SOCKET_TYPE_UDP = 0x1,        /**< Unencrypted UDP transport */
    GCOAP_SOCKET_TYPE_DTLS = 0x2,       /**< DTLS-over-UDP transport */
} gcoap_socket_type_t;

/**
 * @brief   CoAP socket to handle multiple transport types
 */
typedef struct {
    gcoap_socket_type_t type;                /**< Type of stored socket */
    union {
        sock_udp_t *udp;
#if IS_USED(MODULE_GCOAP_DTLS) || defined(DOXYGEN)
        sock_dtls_t *dtls;
#endif
    } socket;                               /**< Stored socket */
#if IS_USED(MODULE_GCOAP_DTLS) || defined(DOXYGEN)
    sock_dtls_session_t ctx_dtls_session;   /**< Session object for the stored socket.
                                                 Used for exchanging a session between
                                                 functions. */
#endif
} gcoap_socket_t;

/**
 * @brief   A modular collection of resources for a server
 */
struct gcoap_listener {
    const coap_resource_t *resources;   /**< First element in the array of resources */
    size_t resources_len;               /**< Length of array */
    /**
     * @brief   Transport type for the listener
     *
     * Any transport supported by the implementation can be set as a flag.
     * If @ref GCOAP_SOCKET_TYPE_UNDEF is set, the listener listens on all
     * supported transports. If non of the transports beyond UDP are compiled in
     * (i.e. no usage of modules `gcoap_dtls`, ...) this will be ignored and
     * @ref GCOAP_SOCKET_TYPE_UDP assumed.
     */
    gcoap_socket_type_t tl_type;
    gcoap_link_encoder_t link_encoder;  /**< Writes a link for a resource */
    struct gcoap_listener *next;        /**< Next listener in list */

    /**
     * @brief  Function that picks a suitable request handler from a
     * request.
     *
     * @note Leaving this NULL selects the default strategy that picks
     * handlers by matching their Uri-Path to resource paths (as per
     * the documentation of the @ref resources and @ref resources_len
     * fields). Alternative handlers may cast the @ref resources and
     * @ref resources_len fields to fit their needs.
     */
    gcoap_request_matcher_t request_matcher;
};

/**
 * @brief   Forward declaration of the request memo type
 */
typedef struct gcoap_request_memo gcoap_request_memo_t;

/**
 * @brief   Handler function for a server response, including the state for the
 *          originating request
 *
 * If request timed out, the packet header is for the request.
 */
typedef void (*gcoap_resp_handler_t)(const gcoap_request_memo_t *memo,
                                     coap_pkt_t* pdu,
                                     const sock_udp_ep_t *remote);

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
struct gcoap_request_memo {
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
    void *context;                      /**< ptr to user defined context data */
    event_timeout_t resp_evt_tmout;     /**< Limits wait for response */
    event_callback_t resp_tmout_cb;     /**< Callback for response timeout */
    gcoap_socket_t socket;              /**< Transport type to remote endpoint */
#if IS_USED(MODULE_NANOCOAP_CACHE) || DOXYGEN
    /**
     * @brief   Cache key for the request
     *
     * @note    Only available with module ['nanocoap_cache'](@ref net_nanocoap_cache)
     */
    uint8_t cache_key[CONFIG_NANOCOAP_CACHE_KEY_LENGTH];
#endif
};

/**
 * @brief   Memo for Observe registration and notifications
 */
typedef struct {
    sock_udp_ep_t *observer;            /**< Client endpoint; unused if null */
    sock_udp_ep_t *notifier;            /**< Local endpoint to send notifications */
    const coap_resource_t *resource;    /**< Entity being observed */
    uint8_t token[GCOAP_TOKENLEN_MAX];  /**< Client token for notifications */
    uint16_t last_msgid;                /**< Message ID of last notification */
    unsigned token_len;                 /**< Actual length of token attribute */
    gcoap_socket_t socket;              /**< Transport type to observer */
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
 * @pre @p listener is a valid pointer to a single listener (that is,
 *      `listener->next == NULL`)
 *
 * @note If you are tempted to register a pre-linked chain of listeners,
 *       consider placing all their resources in the resources array of a
 *       single listener instead. In the few cases where this does not work
 *       (that is, when the resources need a different `link_encoder` or other
 *       fields of the listener struct), they can just be registered
 *       individually.
 *
 * @param[in] listener  Listener containing the resources.
 */
void gcoap_register_listener(gcoap_listener_t *listener);

/**
 * @brief   Iterate through all registered listeners and check for a resource, matching by @p uri_path
 *
 *  This functions returns resources matching a subpath @see COAP_MATCH_SUBTREE.
 *  If an exact match is required, check with `strncmp()`.
 *
 * @param[in, out]  last_listener       A pointer to NULL for the first call, otherwise the last returned listener
 * @param[in]       last_resource       NULL for the first call, otherwise the last returned resource
 * @param[in]       uri_path            The URI path to search for
 *
 * @return  The resource that matches the URI path
 */
const coap_resource_t *gcoap_get_resource_by_path_iterator(const gcoap_listener_t **last_listener,
                                                           const coap_resource_t *last_resource,
                                                           const char *uri_path);

/**
 * @brief   Initializes a CoAP request PDU on a buffer.
 *
 * If @p code is COAP_CODE_EMPTY, prepares a complete "CoAP ping" 4 byte empty
 * message request, ready to send.
 *
 * With module module [`nanocoap_cache`](@ref net_nanocoap_cache) an all-zero ETag option of
 * length 8 which is updated with a value or removed in @ref gcoap_req_send() depending on
 * existing cache entries for cache (re-)validation. If you do not use the given send functions
 * or do not want cache entries to revalidated for any reason, remove that empty option using
 * @ref coap_opt_remove().
 *
 * @param[out] pdu      Request metadata
 * @param[out] buf      Buffer containing the PDU
 * @param[in] len       Length of the buffer
 * @param[in] code      Request code, one of COAP_METHOD_XXX or COAP_CODE_EMPTY
 *                      to ping
 * @param[in] path      Resource path, may be NULL. @p path_len will be ignored
 *                      in that case.
 * @param[in] path_len  Length of @p path.
 *
 * @pre @p path must start with `/` if not NULL
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gcoap_req_init_path_buffer(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                               unsigned code, const char *path,
                               size_t path_len);

/**
 * @brief   Initializes a CoAP request PDU on a buffer.
 *
 * If @p code is COAP_CODE_EMPTY, prepares a complete "CoAP ping" 4 byte empty
 * message request, ready to send.
 *
 * With module module [`nanocoap_cache`](@ref net_nanocoap_cache) an all-zero ETag option of
 * length 8 which is updated with a value or removed in @ref gcoap_req_send() depending on
 * existing cache entries for cache (re-)validation. If you do not use the given send functions
 * or do not want cache entries to revalidated for any reason, remove that empty option using
 * @ref coap_opt_remove().
 *
 * @param[out] pdu      Request metadata
 * @param[out] buf      Buffer containing the PDU
 * @param[in] len       Length of the buffer
 * @param[in] code      Request code, one of COAP_METHOD_XXX or COAP_CODE_EMPTY
 *                      to ping
 * @param[in] path      `\0`-terminated resource path, may be NULL
 *
 * @pre @p path must start with `/` if not NULL
 *
 * @return  0 on success
 * @return  < 0 on error
 */
static inline int gcoap_req_init(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                 unsigned code, const char *path)
{
    return gcoap_req_init_path_buffer(pdu, buf, len, code, path,
                                      (path) ? strlen(path) : 0U);
}

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
    if (gcoap_req_init(pdu, buf, len, code, path) == 0) {
        if (IS_USED(MODULE_NANOCOAP_CACHE)) {
            /* remove ETag option slack added for cache validation */
            coap_opt_remove(pdu, COAP_OPT_ETAG);
        }
        return coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
    }
    return -1;
}

/**
 * @brief   Sends a buffer containing a CoAP request to the provided endpoint
 *
 * @param[in] buf           Buffer containing the PDU
 * @param[in] len           Length of the buffer
 * @param[in] remote        Destination for the packet
 * @param[in] local         Local endpoint to send from, may be NULL
 * @param[in] resp_handler  Callback when response received, may be NULL
 * @param[in] context       User defined context passed to the response handler
 * @param[in] tl_type       The transport type to use for send. When
 *                          @ref GCOAP_SOCKET_TYPE_UNDEF is selected, the highest
 *                          available (by value) will be selected. Only single
 *                          types are allowed, not a combination of them.
 *
 * @note The highest supported (by value) gcoap_socket_type_t will be selected
 *       as transport type.
 *
 * @return  length of the packet
 * @return -ENOTCONN, if DTLS was used and session establishment failed
 * @return -EINVAL, if @p tl_type is is not supported
 * @return  0 if cannot send
 */
ssize_t gcoap_req_send(const uint8_t *buf, size_t len,
                       const sock_udp_ep_t *remote, const sock_udp_ep_t *local,
                       gcoap_resp_handler_t resp_handler, void *context,
                       gcoap_socket_type_t tl_type);

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
                ? coap_opt_finish(pdu, COAP_OPT_FINISH_NONE)
                : -1;
}

/**
 * @brief   Initializes a CoAP Observe notification packet on a buffer, for the
 *          observer registered for a resource
 *
 * First verifies that an observer has been registered for the resource.
 *
 * @post    If this function returns @see GCOAP_OBS_INIT_OK you have to call
 *          @ref gcoap_obs_send() afterwards to release a mutex.
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
 * @brief   Forgets (invalidates) an existing observe request.
 *
 * This invalidates the internal (local) observe request state without actually
 * sending a deregistration request to the server. Ths mechanism may be referred
 * to as passive deregistration, as it does not send a deregistration request.
 * This is implemented according to the description in RFC 7641,
 * Section 3.6 (Cancellation): 'A client that is no longer interested in
 * receiving notifications for a resource can simply "forget" the observation.'
 * Successfully invalidating the request by calling this function guarantees
 * that the corresponding observe response handler will not be called anymore.
 *
 * NOTE: There are cases were active deregistration is preferred instead.
 * A server may continue sending notifications if it chooses to ignore the RST
 * which is meant to indicate the client did not recognize the notification.
 * For such server implementations this function must be called *before*
 * sending an explicit deregister request (i.e., a GET request with the token
 * of the registration and the observe option set to COAP_OBS_DEREGISTER).
 * This will instruct the server to stop sending further notifications.
 *
 * @param[in] remote    remote endpoint that hosts the observed resource
 * @param[in] token     token of the original GET request used for registering
 *                      an observe
 * @param[in] tokenlen  the length of the token in bytes
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gcoap_obs_req_forget(const sock_udp_ep_t *remote, const uint8_t *token,
                         size_t tokenlen);

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
 * @param[in]  tl_type  Transport type to get the list for.
 *                      @ref GCOAP_SOCKET_TYPE_UNDEF for all transport types.
 *                      If non of the transports beyond UDP are compiled in
 *                      (i.e. usage of modules no `gcoap_dtls`, ...) this will
 *                      be ignored and @ref GCOAP_SOCKET_TYPE_UDP assumed.
 *
 * @todo    add support for CoRAL once it is done
 *
 * @return  the number of bytes written to @p buf
 * @return  -1 on error
 */
int gcoap_get_resource_list(void *buf, size_t maxlen, uint8_t cf,
                               gcoap_socket_type_t tl_type);

/**
 * @brief   Writes a resource in CoRE Link Format to a provided buffer.
 *
 * This default implementation only writes the resource path.
 *
 * @param[in]  resource  resource to write
 * @param[out] buf       output buffer to write link into, may be null
 * @param[in]  maxlen    length of @p buf, ignored if @p buf is NULL
 * @param[in]  context   other parameters that affect how link is written
 *
 * @return  count of bytes written to @p buf (or writable if @p buf is null)
 * @return  -1 on error
 */
ssize_t gcoap_encode_link(const coap_resource_t *resource, char *buf,
                          size_t maxlen, coap_link_encoder_ctx_t *context);

#if IS_USED(MODULE_GCOAP_DTLS) || defined(DOXYGEN)
/**
 * @brief   Get the underlying DTLS socket of gcoap.
 *
 * Useful for managing credentials of gcoap.
 *
 * @return  pointer to the @ref sock_dtls_t object
 */
sock_dtls_t *gcoap_get_sock_dtls(void);
#endif

/**
 * @brief   Get the buffer from a @ref gcoap_request_memo_t
 *
 * @param[in] memo  A request memo. Must not be NULL.
 *
 * @return  The buffer storing the message
 */
static inline uint8_t *gcoap_request_memo_get_buf(gcoap_request_memo_t *memo)
{
    if (memo->send_limit == GCOAP_SEND_LIMIT_NON) {
        return &memo->msg.hdr_buf[0];
    }
    else {
        return memo->msg.data.pdu_buf;
    }
}

/**
 * @brief   Get the header of a request from a @ref gcoap_request_memo_t
 *
 * @param[in] memo  A request memo. Must not be NULL.
 *
 * @return  The request header for the given request memo.
 *
 * @deprecated  Use @ref gcoap_request_memo_get_buf instead
 */
static inline coap_udp_hdr_t *gcoap_request_memo_get_hdr(const gcoap_request_memo_t *memo)
{
    gcoap_request_memo_t *evil_cast_is_evil = (gcoap_request_memo_t *)memo;
    return (coap_udp_hdr_t *)gcoap_request_memo_get_buf(evil_cast_is_evil);
}

#ifdef __cplusplus
}
#endif

/** @} */
