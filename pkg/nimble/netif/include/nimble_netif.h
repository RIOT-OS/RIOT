/*
 * Copyright (C) 2018-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_nimble_netif GNRC netif Implementation
 * @ingroup     pkg_nimble
 * @brief       GNRC netif implementation for NimBLE, enabling the integration
 *              of NimBLE into GNRC
 *
 * # About
 * This NimBLE submodule provides a GNRC netif wrapper for integrating NimBLE
 * with GNRC and other network stacks using netif (e.g. CCNlite).
 *
 * # Concept
 * According to the IPv6-over-BLE standards (RFC7668 and IPSP), this module
 * exposes a (configurable) number of point-to-point BLE connections as a single
 * network device to BLE. Unicast traffic is only send using the corresponding
 * BLE connection. Multicast and Broadcast packets are duplicated and send via
 * each open BLE connection.
 *
 * # Structure
 * The netif implementation is able to handle multiple connections
 * simultaneously. The maximum number of concurrent connections is configured
 * during compile time, using NimBLEs MYNEWT_VAL_BLE_MAX_CONNECTIONS option.
 * Dependent on this value, the netif implementation takes care of allocation
 * all the memory needed. The API of this submodule uses simply integer values
 * to reference the used connection context (like file descriptors in linux).
 *
 * Like any other GNRC network device, the NimBLE netif wrapper runs in its own
 * thread. This thread is started and configured by the common netif code. All
 * send and get/set operations are handled by this thread. For efficiency
 * reasons, receiving of data is however handled completely in the NimBLE host
 * thread, from where the received data is directly passed on to the
 * corresponding GNRC thread.
 *
 * Although the wrapper hooks into GNRC using the netif interface, it does need
 * to implement parts of the netdev interface as well. This is done where
 * needed.
 *
 * # Usage
 * This submodule is designed to work fully asynchronous, in the same way as the
 * NimBLE interfaces are designed. All functions in this submodule will only
 * trigger the intended action. Once this action is complete, the module will
 * report the result asynchronously using the configured callback.
 *
 * So before using this module, make sure to register a callback using the
 * @ref nimble_netif_eventcb() function.
 *
 * After this, this module provides functions for managing BLE connections to
 * other devices. Once these connections are established, this module takes care
 * of mapping IP packets to the corresponding connections.
 *
 * @{
 *
 * @file
 * @brief       GNRC netif implementation for NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_NETIF_H
#define NIMBLE_NETIF_H

#include <stdint.h>

#include "net/ble.h"

#include "host/ble_hs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The maximum number of BLE connections that can be open concurrently
 *
 * @note    This value *must* be <= MYNEWT_VAL_BLE_MAX_CONNECTIONS
 */
#ifndef NIMBLE_NETIF_MAX_CONN
#define NIMBLE_NETIF_MAX_CONN       (MYNEWT_VAL_BLE_MAX_CONNECTIONS)
#endif

/**
 * @brief   Default L2CAP channel ID to use
 */
#ifndef NIMBLE_NETIF_CID
#define NIMBLE_NETIF_CID            (BLE_L2CAP_CID_IPSP)
#endif

/**
 * @brief   Default MTU size supported by the NimBLE netif wrapper
 */
/* NOTE: We do not use the @ref IPV6_MIN_MTU define here, as the iov6.h header
         pulls in some other RIOT headers that clash with NimBLE header (e.g.
 *       byteorder.h vs. endian.h) */
#ifndef NIMBLE_NETIF_MTU
#define NIMBLE_NETIF_MTU            (1280U)
#endif

/**
 * @brief   Set to > 0 to enforce different connection intervals for each of the
 *          nodes BLE connections
 *
 * Enabling this option will enforce that every BLE connection a node maintains,
 * independent of the nodes role, uses a different connection interval. The
 * value of NIMBLE_NETIF_CONN_ITVL_SPACING specifies the minimum spacing between
 * connection intervals as multiple of 1,25ms. E.g. a value of 2 will force each
 * connection to use a connection interval that is at least 2.5ms different from
 * all other used connection intervals.
 *
 * If a node is the coordinator of a connection, it will generate a connection
 * interval for each new connection based on a random value by adhering to the
 * spacing constraint.
 *
 * If a node is the subordinate of a new connection, it will check if the given
 * connection interval is fulfilling the spacing constraint with respect to
 * already existing connections of that node. If the connection interval of the
 * new connection is not properly spaced, the node will drop the connection
 * right away, giving the coordinator node the possibly to reconnect with a
 * different connection interval.
 */
#ifndef NIMBLE_NETIF_CONN_ITVL_SPACING
#define NIMBLE_NETIF_CONN_ITVL_SPACING          0
#endif

/**
 * @brief   Minimum spacing of connection interval when using randomized
 *          intervals, in multiples of 1.25ms
 */

/**
 * @brief   Return codes used by the NimBLE netif module
 */
enum {
    NIMBLE_NETIF_OK         =  0,   /**< everything went fine */
    NIMBLE_NETIF_NOTCONN    = -1,   /**< not connected */
    NIMBLE_NETIF_DEVERR     = -2,   /**< internal BLE stack error */
    NIMBLE_NETIF_BUSY       = -3,   /**< network device is busy */
    NIMBLE_NETIF_NOMEM      = -4,   /**< insufficient memory */
    NIMBLE_NETIF_NOTADV     = -5,   /**< not advertising */
    NIMBLE_NETIF_NOTFOUND   = -6,   /**< no fitting entry found */
};

/**
 * @brief   Event types triggered by the NimBLE netif module
 */
typedef enum {
    NIMBLE_NETIF_ACCEPTING,         /**< accepting incoming connections */
    NIMBLE_NETIF_ACCEPT_STOP,       /**< stop accepting incoming connections */
    NIMBLE_NETIF_INIT_MASTER,       /**< conn. procedure started (as master) */
    NIMBLE_NETIF_INIT_SLAVE,        /**< conn. procedure started (as slave) */
    NIMBLE_NETIF_CONNECTED_MASTER,  /**< connection established as master */
    NIMBLE_NETIF_CONNECTED_SLAVE,   /**< connection established as slave */
    NIMBLE_NETIF_CLOSED_MASTER,     /**< connection closed (we were master) */
    NIMBLE_NETIF_CLOSED_SLAVE,      /**< connection closed (we were slave) */
    NIMBLE_NETIF_ABORT_MASTER,      /**< connection est. abort (as master) */
    NIMBLE_NETIF_ABORT_SLAVE,       /**< connection est. abort (as slave) */
    NIMBLE_NETIF_CONN_UPDATED,      /**< connection parameter update done */
} nimble_netif_event_t;

/**
 * @brief   Flags describing the state of a single connection context
 */
enum {
    NIMBLE_NETIF_L2CAP_CLIENT       = 0x0001,   /**< L2CAP client */
    NIMBLE_NETIF_L2CAP_SERVER       = 0x0002,   /**< L2CAP server */
    NIMBLE_NETIF_L2CAP_CONNECTED    = 0x0003,   /**< L2CAP is connected */
    NIMBLE_NETIF_GAP_MASTER         = 0x0010,   /**< GAP master */
    NIMBLE_NETIF_GAP_SLAVE          = 0x0020,   /**< GAP slave */
    NIMBLE_NETIF_GAP_CONNECTED      = 0x0030,   /**< GAP is connected */
    NIMBLE_NETIF_ADV                = 0x0100,   /**< currently advertising */
    NIMBLE_NETIF_CONNECTING         = 0x4000,   /**< connection in progress */
    NIMBLE_NETIF_UNUSED             = 0x8000,   /**< context unused */
    NIMBLE_NETIF_ANY                = 0xffff,   /**< match any state */
};

/**
 * @brief   Event callback signature used for asynchronous event signaling
 *
 * @note    The event callback is always executed in NimBLE's host thread
 *
 * @param[in] handle        handle to the connection that triggered the event
 * @param[in] event         type of the event
 * @param[in] addr          BLE address of the peer in the effected connection
 */
typedef void(*nimble_netif_eventcb_t)(int handle,
                                      nimble_netif_event_t event,
                                      const uint8_t *addr);

/**
 * @brief   Initialize the netif implementation, spawns the netif thread
 *
 * This function is meant to be called once during system initialization, i.e.
 * auto-init.
 */
void nimble_netif_init(void);

/**
 * @brief   Register a global event callback, servicing all NimBLE connections
 *
 * @note    The event callback is always executed in NimBLE's host thread
 *
 * @param[in] cb            event callback to register, may be NULL
 */
void nimble_netif_eventcb(nimble_netif_eventcb_t cb);

/**
 * @brief   Open a BLE connection as BLE master
 *
 * @param[in] addr          address of the advertising BLE slave, in the NimBLE
 *                          addr format (little endian)
 * @param[in] conn_params   connection (timing) parameters, set to NULL to use
 *                          NimBLEs default parameters
 * @param[in] timeout       connect timeout [in ms]
 *
 * @return  the used connection handle on success
 * @return  NIMBLE_NETIF_BUSY if already connected to the given address or if
 *          a connection setup procedure is in progress
 * @return  NIMBLE_NETIF_NOMEM if no connection context memory is available
 * @return  NIMBLE_NETIF_NOTFOUND if unable to find valid connection interval
 */
int nimble_netif_connect(const ble_addr_t *addr,
                         struct ble_gap_conn_params *conn_params,
                         uint32_t timeout);

/**
 * @brief   Close the connection with the given handle
 *
 * @param[in] handle        handle for the connection to be closed
 *
 * @return  NIMBLE_NETIF_OK on success
 * @return  NIMBLE_NETIF_NOTFOUND if the handle is invalid
 * @return  NIMBLE_NETIF_NOTCONN if context for given handle is not connected
 */
int nimble_netif_close(int handle);

/**
 * @brief   Accept incoming connections by starting to advertise this node
 *
 * @param[in] ad            advertising data (in BLE AD format)
 * @param[in] ad_len        length of @p ad in bytes
 * @param[in] adv_params    advertising (timing) parameters to use
 *
 * @return  NIMBLE_NETIF_OK on success
 * @return  NIMBLE_NETIF_BUSY if already advertising
 * @return  NIMBLE_NETIF_NOMEM on insufficient connection memory
 */
int nimble_netif_accept(const uint8_t *ad, size_t ad_len,
                        const struct ble_gap_adv_params *adv_params);

/**
 * @brief   Stop accepting incoming connections (stop advertising)
 * *
 * @return  NIMBLE_NETIF_OK on success
 * @return  NIMBLE_NETIF_NOTADV if not currently advertising
 */
int nimble_netif_accept_stop(void);

/**
 * @brief   Update the connection parameters for the given connection
 *
 * @param[in] handle        connection handle
 * @param[in] conn_params   new connection parameters to apply
 *
 * @return  NIMBLE_NETIF_OK on success
 * @return  NIMBLE_NETIF_NOTCONN if handle does not point to a connection
 * @return  NIMBLE_NETIF_DEVERR if applying the given parameters failed
 */
int nimble_netif_update(int handle,
                        const struct ble_gap_upd_params *conn_params);

/**
 * @brief   Get the currently used channel map for the given connection as
 *          bitmap
 *
 * @param[in] handle        connection handle
 * @param[out] map          used channel map, map[0] denotes channels 0 to 7,
 *                          map[1] chan 8-15, ..., map[5] channels 33 to 36.
 *                          **must** be able to hold 5 bytes.
 *
 * @return  NIMBLE_NETIF_OK on success
 * @return  NIMBLE_NETIF_NOTCONN if handle dos not point to a connection
 * @return  NIMBLE_NETIF_DEVERR if reading the channel map failed otherwise
 */
int nimble_netif_used_chanmap(int handle, uint8_t map[5]);

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_NETIF_H */
/** @} */
