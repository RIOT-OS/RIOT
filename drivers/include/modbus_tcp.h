/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_modbus_tcp Modbus TCP transport
 * @ingroup     drivers_modbus
 * @brief       Modbus TCP transport
 *
 * Modbus TCP transport implementation. The transport wraps each PDU in a
 * 7-byte MBAP header (transaction ID, protocol ID, length, unit ID) and
 * sends it over a sock_tcp connection. TCP guarantees reliable delivery and is
 * responsible for checksumming.
 *
 * The same transport descriptor is used for client and server roles, but it
 * cannot be used simultaneously. The @ref modbus_tcp_params_t::mode field
 * selects which side of the connection the transport sets up at initialization
 * time. Client mode connects to a remote end-point. Server mode listens on a
 * local end-point and serves a single concurrent connection. When one client
 * disconnects between two requests, the next connection is accepted
 * transparently, without surfacing an error to the application.
 *
 * A client keeps its connection open across requests, including after a
 * response timeout: the MBAP transaction ID is used to discard late responses
 * to earlier timed-out requests. The connection is only torn down (and
 * transparently re-established on the next request) when the byte stream can
 * no longer be trusted, for example on an error in the middle of a frame.
 *
 * Since the server serves a single connection at a time, a dead connection
 * (e.g. a client that vanished without closing) would block new connections
 * indefinitely. The optional @ref modbus_tcp_params_t::idle_timeout bounds
 * this: when no request arrives within that time, the active connection is
 * dropped and the next one is accepted.
 *
 * @{
 *
 * @file
 * @brief       Modbus TCP transport definitions
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <stdbool.h>
#include <stdint.h>

#include "net/sock/tcp.h"

#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Modbus TCP frame sizes
 * @{
 */
#define MODBUS_TCP_MBAP_SIZE        (7)     /**< size of MBAP header */
#define MODBUS_TCP_ADU_MAX_SIZE     (MODBUS_TCP_MBAP_SIZE + \
                                     MODBUS_PDU_MAX_SIZE)   /**< maximum size of TCP ADU
                                                                (MBAP + PDU) */
/** @} */

/**
 * @brief   Default Modbus TCP port
 */
#define MODBUS_TCP_DEFAULT_PORT     (502)

/**
 * @brief   Modbus TCP transport mode
 */
typedef enum {
    MODBUS_TCP_MODE_CLIENT,     /**< client mode (connects to a remote end-point) */
    MODBUS_TCP_MODE_SERVER,     /**< server mode (listens on a local end-point) */
} modbus_tcp_mode_t;

/**
 * @brief   Modbus TCP device parameters
 */
typedef struct {
    modbus_tcp_mode_t mode;     /**< client or server mode */
    sock_tcp_ep_t endpoint;     /**< remote end-point (client) or local end-point (server) */
    uint32_t response_timeout;  /**< client recv timeout (in usec), 0
                                     selects @c SOCK_NO_TIMEOUT (block forever) */
    uint32_t idle_timeout;      /**< drop the active connection when no
                                     request arrives within this time (in
                                     usec), 0 selects @c SOCK_NO_TIMEOUT
                                     (never drop idle connections) */
} modbus_tcp_params_t;

/**
 * @brief   Internal client state
 */
typedef struct {
    sock_tcp_t sock;            /**< the connected sock */
    bool connected;             /**< true while @ref sock holds a live connection */
} modbus_tcp_client_t;

/**
 * @brief   Internal server state
 */
typedef struct {
    sock_tcp_queue_t queue;     /**< listen queue */
    sock_tcp_t pool[1];         /**< backing storage for the queue */
    sock_tcp_t *active;         /**< currently accepted sock, or @c NULL */
} modbus_tcp_server_t;

/**
 * @brief   Modbus TCP device structure
 *
 * These fields are internal to the implementation, and should not be used
 * directly.
 */
typedef struct {
    uint8_t buf[MODBUS_TCP_ADU_MAX_SIZE];   /**< MBAP + PDU buffer */

    union {
        modbus_tcp_client_t client;         /**< client-mode state */
        modbus_tcp_server_t server;         /**< server-mode state */
    } state;                                /**< role-specific state */

    uint16_t transaction_id;                /**< running counter (client) or last received id
                                                 (server) */
} modbus_tcp_t;

/**
 * @brief   Modbus TCP transport descriptor
 */
extern const modbus_transport_desc_t modbus_tcp_transport;

#ifdef __cplusplus
}
#endif

/** @} */
