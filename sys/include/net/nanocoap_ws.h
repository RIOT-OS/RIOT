/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef NET_NANOCOAP_WS_H
#define NET_NANOCOAP_WS_H

/**
 * @defgroup    net_nanocoap_ws nanoCoAP WebSocket Interface
 * @ingroup     net_nanocoap
 * @brief       An interface for using nanoCoAP over
 *              WebSocket(-compatible) transports
 *
 * This is a generic interface to provide a reliable, order preserving,
 * duplication free, connection oriented, message based transport. This can
 * very well be a WebSocket implementation, but is intentionally open for
 * "abuse".
 *
 * @note    Using CoAP over WebSocket serialization over transports other than
 *          WebSockets is not backed by any standard. There will be absolutely
 *          no interoperability when doing so.
 *
 * @{
 *
 * @file
 * @brief       WebSocket interface for nanocoap
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include <stdint.h>
#include <unistd.h>

#include "iolist.h"

#if defined(MODULE_SOCK_UDP) || defined(DOXYGEN)
#  include "net/sock/udp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_NANOCOAP_WS_TX_BUF_SIZE
#  define CONFIG_NANOCOAP_WS_TX_BUF_SIZE    128
#endif

/**
 * @brief   Canonical name of @ref struct coap_ws_conn
 */
typedef struct coap_ws_conn coap_ws_conn_t;

/**
 * @brief   Canonical name of @ref struct coap_ws_transport
 */
typedef struct coap_ws_transport coap_ws_transport_t;

/**
 * @brief   Canonical name of @ref struct coap_ws_handle
 */
typedef struct coap_ws_handle coap_ws_handle_t;

typedef struct {
    /**
     * @brief   Function to call when a request to create a connection request
     *          is available
     * @param[in,out]   handle  Handle a new connection can be accepted on
     * @param[in,out]   conn    New connection
     *
     * @note    The called function may write to @ref coap_ws_conn_t::arg of
     *          @p conn
     *
     * @retval  true    Accept new connection
     * @retval  false   Reject new connection
     */
    bool (*conn_accept_cb)(coap_ws_handle_t *handle, coap_ws_conn_t *conn);
    /**
     * @brief   Function to call when a message was received
     * @param[in,out]   conn    Connection the message was received over
     * @param[in,out]   msg     Message that was received
     * @param[in]       msg_len Length of @p msg in bytes
     */
    void (*conn_recv_cb)(coap_ws_conn_t *conn, void *msg, size_t msg_len);
    /**
     * @brief   Function to call to reclaim a connection
     *
     * @retval  true    Also clean up the handle this callback returns
     * @retval  false   Keep the handle
     *
     * The user of the connection may still use @p conn during the callback,
     * but must drop any reference to @p conn before returning.
     */
    bool (*conn_reclaim_cb)(coap_ws_conn_t *conn);
} coap_ws_cbs_t;

/**
 * @brief   A CoAP over WebSocket connection
 *
 * The canonical name is @ref coap_ws_conn_t
 */
struct coap_ws_conn {
    /**
     * @brief   The handle this connection belongs to
     */
    coap_ws_handle_t * handle;
    /**
     * @brief   Pointer used internally by NanoCoAP
     */
    void *arg;
};

/**
 * @brief   A CoAP over WebSocket handle
 *
 * The canonical name is @ref coap_ws_handle_t
 *
 * A WebSocket handle is used to create and accept connections on.
 */
struct coap_ws_handle {
    const coap_ws_transport_t *transport;           /**< The transport this belongs to */
    uint8_t tx_buf[CONFIG_NANOCOAP_WS_TX_BUF_SIZE]; /**< Buffer to use for assembling outgoing messages in */
};

/**
 * @brief   A CoAP over WebSocket transport
 *
 * The canonical name is @ref coap_ws_transport_t
 */
struct coap_ws_transport {
    /**
     * @brief   Open a WebSocket handle
     * @param[in]       cbs     Callbacks to call on events
     * @param[in,out]   arg     Argument (content is opaque to NanoCoAP)
     * @param[in]       ep      Endpoint to listen on
     * @param[in]       el_len  Length of @p ep in bytes
     * @return  The new handle
     * @retval  NULL    Error (e.g. out of resources)
     */
    coap_ws_handle_t * (*open_handle)(const coap_ws_cbs_t *cbs, void *arg,
                                      const void *ep, size_t ep_len);
    /**
     * @brief   Close a WebSocket handle
     * @param[in,out]   handle  The WebSocket handle to close
     *
     * @note    The implementation is expected to reclaim any still open
     *          connection belonging to this handle
     */
    void (*close_handle)(coap_ws_handle_t *handle);
    /**
     * @brief   Establish a new connection
     * @param[in,out]   handle      The handle to open a connection over
     * @param[in]       ep          Endpoint to connect to
     * @param[in]       ep_len      Size of @p ep in bytes
     * @param[in]       arg         Store this into @ref coap_ws_conn_t::arg
     *
     * @post    @p arg is stored into the returned connection before this
     *          function returns and before any event callback are called
     *
     * @return  Pointer to the new connection
     * @retval  NULL    Connection failed
     */
    coap_ws_conn_t * (*connect)(coap_ws_handle_t *handle,
                                const void *ep, size_t ep_len, void *arg);
    /**
     * @brief   Close a WebSocket connection
     * @param[in,out]   conn        Connection to close
     *
     * @note    This will triger a call to @ref coap_ws_cbs_t::conn_reclaim_cb
     */
    void (*close)(coap_ws_conn_t *conn);
    /**
     * @brief   Get the remote endpoint of the given connection
     * @param[in]   conn    The connection to get the remote endpoint of
     * @param[out]  ep      Store a pointer to the endpoint representation in here
     *
     * @return  Size of the endpoint
     */
    size_t (*remote_ep)(const coap_ws_conn_t *conn, const void **ep);
    /**
     * @brief   Send a single message over the given connection
     * @param[in,out]   conn    Connection to send a message over
     * @param[in]       iol     Message to send
     *
     * @note    The message to send needs to contain the concatenation of all
     *          data chunks in @p iol.
     * @retval  0   Success
     * @retval  <0  Error (failed to send (part of) the message)
     */
    int (*sendv)(coap_ws_conn_t *conn, const iolist_t *iol);
};

#if defined(MODULE_SOCK_UDP) || defined(DOXYGEN)
typedef struct {
    coap_ws_conn_t conn;
    sock_udp_ep_t remote;
} coap_ws_over_udp_yolo_conn_t;

typedef struct coap_ws_over_udp_yolo_init_arg {
    coap_ws_handle_t handle;
    const coap_ws_cbs_t *cbs;
    sock_udp_t sock;
    coap_ws_over_udp_yolo_conn_t conns[4];
    void (*cleanup)(struct coap_ws_over_udp_yolo_init_arg *arg);
} coap_ws_over_udp_yolo_init_arg_t;

/**
 * @brief   Providing a WebSocket like interface using UDP, YOLO!
 *
 * @warning A WebSocket interface needs to provide a reliable, ordered,
 *          duplication free, connection based transfer of messages. UDP
 *          is unreliable, unordered, connection-less message transfer prone
 *          to duplicates. But ... YOLO!
 *
 * @note    This will only be provided with module `nanocoap_ws_udp_yolo`.
 *
 * This can be good enough for testing between two machines with a direct
 * link-layer connection between and the link layer implementing retransmissions
 * and acknowledgements.
 *
 * Using it for anything but testing has a high chance of foot-shooting.
 */
extern const coap_ws_transport_t coap_ws_over_udp_yolo;
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NET_NANOCOAP_WS_H */
