/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_nimble_statconn Statconn
 * @ingroup     pkg_nimble
 * @brief       Static connection manager for NimBLE netif that keeps opens
 *              connections on demand and takes care of keeping them open.
 *
 * @experimental
 *
 * # WARNING
 * This module is highly experimental! Expect bugs, instabilities and sudden API
 * changes :-)
 *
 *
 * # About
 * Statconn is the implementation of a static connection manager for the NimBLE
 * netif module. It initiates new connections when told and additionally takes
 * care of reopening connections in case of connection loss.
 *
 *
 * # Usage
 * A node can either be a master, the one initiating the connection, or a
 * slave, the one advertising its presence, w.r.t. to BLE connections. To open
 * a connection between two nodes, call nimble_statconn_add_slave() with the
 * BLE address of the slave node on the designated master node and call
 * nimble_statconn_add_master() with the address of the master node on the
 * designated slave node. From that point on, statconn will take care of
 * opening and maintaining the connection between these two nodes.
 *
 *
 * # Configuration
 * This module is configured completely statically. All relevant configuration
 * options are set at compile time using a couple of macros.
 *
 * @{
 *
 * @file
 * @brief       Simple static connection manager for NimBLE netif
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_STATCONN_H
#define NIMBLE_STATCONN_H

#include <stdint.h>

#include "nimble_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Advertising interval used when in advertising mode [in ms]
 */
#ifndef NIMBLE_STATCONN_ADV_ITVL_MS
#define NIMBLE_STATCONN_ADV_ITVL_MS         (90U)
#endif

/**
 * @brief   Scan window used when opening new connections [in ms]
 */
#ifndef NIMBLE_STATCONN_CONN_WIN_MS
#define NIMBLE_STATCONN_CONN_WIN_MS         (100U)
#endif

/**
 * @brief   Opening a new connection is aborted after this time [in ms]
 */
#ifndef NIMBLE_STATCONN_CONN_TIMEOUT_MS
#define NIMBLE_STATCONN_CONN_TIMEOUT_MS     (600U)
#endif

/**
 * @brief   Minimum connection interval used when opening a new connection. The
 *          actual used connection interval will be a random value ranging from
 *          @ref NIMBLE_STATCONN_CONN_ITVL_MIN_MS to
 *          @ref NIMBLE_STATCONN_CONN_ITVL_MAX_MS. Set both variables to the same
 *          value to use a fixed connection interval. [in ms]
 */
#ifndef NIMBLE_STATCONN_CONN_ITVL_MIN_MS
#define NIMBLE_STATCONN_CONN_ITVL_MIN_MS    (75U)
#endif

/**
 * @brief   Maximum connection interval to use. See
 *          @ref NIMBLE_STATCONN_CONN_ITVL_MIN_MS for more information. [in ms]
 */
#ifndef NIMBLE_STATCONN_CONN_ITVL_MAX_MS
#define NIMBLE_STATCONN_CONN_ITVL_MAX_MS    (75U)
#endif

/**
 * @brief   Slave latency used for new connections [# of connection events]
 */
#ifndef NIMBLE_STATCONN_CONN_LATENCY
#define NIMBLE_STATCONN_CONN_LATENCY        (0)
#endif

/**
 * @brief   Supervision timeout used for new connections [in ms]
 */
#ifndef NIMBLE_STATCONN_CONN_SUPERTO_MS
#define NIMBLE_STATCONN_CONN_SUPERTO_MS     (2500U)
#endif

/**
 * @brief   Return codes used by the statconn module
 */
enum {
    NIMBLE_STATCONN_OK      =  0,   /**< all groovy */
    NIMBLE_STATCONN_NOSLOT  = -1,   /**< no more connection slot available */
    NIMBLE_STATCONN_NOTCONN = -2,   /**< given address is not managed */
    NIMBLE_STATCONN_INUSE   = -3,   /**< given peer is already managed */
};

/**
 * @brief   Initialize the statconn module
 *
 * @warning This function **must** only be called once. Typically this is done
 *          during system initialization via auto-init.
 */
void nimble_statconn_init(void);

/**
 * @brief   Register a callback that is called on netif events
 *
 * The registered callback function is a simple pass-through of nimble_netif
 * events. The callback is executed in the context of NimBLE's host thread.
 *
 * @param[in] cb        event callback to register, may be NULL
 */
void nimble_statconn_eventcb(nimble_netif_eventcb_t cb);

/**
 * @brief   Connect to peer (master) with a given address as slave
 *
 * Adding a master will make this node advertise itself to wait for an incoming
 * connection by that master.
 *
 * @param[in] addr      BLE address of the peer
 *
 * @return  NIMBLE_STATCONN_OK if peer was successfully added
 * @return  NIMBLE_STATCONN_INUSE if the peer address is already in use
 * @return  NIMBLE_STATCONN_NOSLOT if no empty connection slot is available
 */
int nimble_statconn_add_master(const uint8_t *addr);

/**
 * @brief   Connect to a peer (slave) with a given address as master
 *
 * @param[in] addr      BLE address of the peer
 *
 * @return  NIMBLE_STATCONN_OK if peer was successfully added
 * @return  NIMBLE_STATCONN_INUSE if the peer address is already in use
 * @return  NIMBLE_STATCONN_NOSLOT if no empty connection slot is available
 */
int nimble_statconn_add_slave(const uint8_t *addr);

/**
 * @brief   Remove the connection to the given peer
 *
 * @param[in] addr      BLE address of the peer
 *
 * @return  NIMBLE_STATCONN_OK if peer was successfully removed
 * @return  NIMBLE_STATCONN_NOTCONN if given address is not managed
 */
int nimble_statconn_rm(const uint8_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_STATCONN_H */
/** @} */
