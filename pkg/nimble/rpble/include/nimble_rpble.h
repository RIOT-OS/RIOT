/*
 * Copyright (C) 2019-2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ble_nimble_netif_rpble RPL-over-BLE for NimBLE
 * @ingroup     net_ble_nimble
 * @brief       RPL-over-BLE for Nimble implementation
 *
 * # About
 * This module implements a BLE connection manager the manages BLE connections
 * for (multi-hop) IP over BLE networks based on meta data provided by RPL.
 *
 *
 * # Concept
 * In their initial state, after power up or reboot, nodes start to scan for
 * BLE advertisements containing a specific advertising data (AD) field which
 * holds information about RPL DODAGs. For a configured amount of time, the node
 * ranks all senders of the received advertising packets based on a given
 * metric. After this time, the node selects the best fitting parent based on
 * this ranking and tries to connect to that peer.
 *
 * After a node has successfully opened a connection to its parent node, the
 * node starts to advertise its own RPL context data to accept connections
 * from potential child nodes.
 *
 * This approach leads to a BLE network topology that is equal to the IP routing
 * topology created by RPL on top.
 *
 * ## Advertising data structure
 * To include RPL context information into (legacy) BLE advertisements, it must
 * be encoded into the BLE advertising data format. This implementation uses
 * a custom sub-format that is included into the 16-bit UUID Service Data
 * (type: 0x16) field (Supplement to Bluetooth Core Specification CSSv8, 1.11).
 * The 16-bit UUID in this field is set to the IPSS service ID (0x1820).
 *
 * The following sub-format is used to encode the RPL context data:
 * ```
 * byte     1: instance ID (1b)
 * byte  2-17: DODAG ID (16b)
 * byte    18: DODAG version (1b)
 * byte    19: RPL role (1b)
 * byte 20-21: rank (2b)
 * byte    22: number of free BLE connection slots (1b)
 * ```
 *
 * ## Ranking of potential parents
 * The currently implemented ranking metric is very simple: the potential parent
 * node with the smallest rank is chosen. Additionally, nodes advertise the
 * number of free BLE connection slots. In case where multiple nodes advertise
 * the same RPL rank, the one with the largest number of open BLE connection
 * slots is selected. The idea behind this is to balance the number of BLE
 * connections per node, and with that also to balance the RPL DODAG.
 *
 *
 * # Usage
 * To use this module, simply include `nimble_rpble` into your build. If nothing
 * is explicitly configured, the module will use the default configuration as
 * specified in `pkg/nimble/rpble/include/nimble_rpble_params.h`.
 *
 * Once a node is configured as RPL root (e.g. using the `rpl root ..` shell
 * command), it will automatically start to advertise itself. Non-RPL-root nodes
 * will automatically scan for potential parent nodes and join the network as
 * soon as they find fitting neighbors.
 *
 *
 * ## Implementation status
 * In its current state, the implementation only works for environments where a
 * single RPL network with a single DODAG are present. The DODAG ID, instance
 * ID, and DODAG version are currently pretty much ignored when scanning for
 * potential parents.
 *
 * @{
 *
 * @file
 * @brief       Interface for the nimble_rpble module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_RPBLE_H
#define NIMBLE_RPBLE_H

#include "nimble_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   rpble configuration parameters
 */
typedef struct {
    uint32_t scan_itvl;         /**< scan interval when scanning for parents */
    uint32_t scan_win;          /**< scan window when scanning for parents */
    uint32_t adv_itvl;          /**< advertising interval used when advertising
                                 *   RPL context to child nodes */
    uint32_t conn_scan_itvl;    /**< scan interval when connecting to parent */
    uint32_t conn_scan_win;     /**< scan window when connecting to parent */
    uint32_t conn_scan_to;      /**< timeout when connecting to parent */
    uint32_t conn_itvl_min;     /**< lower bound of connection interval range */
    uint32_t conn_itvl_max;     /**< upper bound of connection interval range */
    uint16_t conn_latency;      /**< used slave latency for parent connection */
    uint32_t conn_super_to;     /**< used supervision timeout for parent
                                 *   connection */

    /* time the node searches and ranks potential parents
     * -> itvl := rand([eval_itvl:2*eval_itvl)) */
    uint32_t eval_itvl_min;     /**< amount of time a node searches for
                                 *   potential parents, lower bound */
    uint32_t eval_itvl_max;     /**< amount of time a node searches for
                                 *   potential parents, upper bound */
} nimble_rpble_cfg_t;

/**
 * @brief   RPL DODAG information
 */
typedef struct {
    uint8_t inst_id;            /**< instance ID */
    uint8_t dodag_id[16];       /**< DODAG ID */
    uint8_t version;            /**< DODAG version */
    uint8_t role;               /**< RPL role of the node */
    uint16_t rank;              /**< the node's rank in the DODAG */
} nimble_rpble_ctx_t;

/**
 * @brief   Initialize the nimble_rpble module with the given parameters
 *
 * @note    This function must be called only once, typically during system
 *          initialization
 *
 * @param[in] cfg       configuration parameters
 *
 * @return  0 on success
 */
int nimble_rpble_init(const nimble_rpble_cfg_t *cfg);

/**
 * @brief   Update the used timing parameters
 *
 * @param[in] cfg       configuration parameters
 *
 * @return  0 on success
 */
int nimble_rpble_param_update(const nimble_rpble_cfg_t *cfg);

/**
 * @brief   Register a callback that is called on BLE events
 *
 * The registered callback function is a simple pass-through of nimble_netif
 * events. The callback is executed in the context of NimBLE's host thread.
 *
 * @param[in] cb        event callback to register, set to NULL to remove
 */
int nimble_rpble_eventcb(nimble_netif_eventcb_t cb);

/**
 * @brief   Update the current RPL context
 *
 * @note    This function is meant to be called only by the RPL implementation
 *
 * @param[in] ctx       current DODAG state

 * @return  0 on success
 * @return  -EALREADY if the given context did not change
 */
int nimble_rpble_update(const nimble_rpble_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_RPBLE_H */
/** @} */
