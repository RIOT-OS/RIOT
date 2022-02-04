/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_nimble_autoconn Autoconn
 * @ingroup     pkg_nimble
 * @brief       Simple connection manager that automatically opens BLE
 *              connections to any node that fits some given filter criteria
 *
 * @experimental
 *
 * # WARNING
 * This module is highly experimental! Expect bugs, instabilities and sudden API
 * changes :-)
 *
 *
 * # About
 * This NimBLE submodule implements a connection manager for BLE. It takes care
 * of scanning, advertising, and opening connections to neighboring nodes. For
 * this autoconn periodically switches between advertising and scanning mode,
 * hence from accepting incoming connection requests to scanning actively for
 * new neighbors.
 *
 *
 * # Concept
 * The IETF and BT SIG standards describing IP-over-BLE only describe how to
 * transfer IP data over L2CAP connection oriented channels. But they do not
 * say anything about when BLE connections should be established between two
 * BLE nodes in the first place. While this can be done manually (e.g. via RIOTs
 * `ble` shell command), this is certainly no option in massive M2M deployments.
 *
 * To enable nodes to automatically connect to their neighbors, autoconn
 * implements a naive strategy which makes nodes to connect to any neighbor they
 * see, as long as the neighbor signals a predefined set of capabilities.
 *
 * In particular, neighbors are simply filtered by looking at the 16-bit service
 * UUIDs included in the `Incomplete List of 16-bit Service UUIDs` field in the
 * advertising data that is received from neighbors.
 *
 * The logical network topology (as seen by IP) is formed by the established
 * BLE link layer connections. It is important to node, that the autoconn module
 * will form a random topology on the link layer, as no further context
 * information is used for the connection decisions. This can potentially lead
 * to fragmented, non-connected sub-networks in larger deployments!
 *
 *
 * # State Machine
 * Autoconn implements a state machine, that switches a nodes role periodically
 * between scanning and advertising. To make sure, that nodes always have a
 * chance to see each other, especially when booted at the same point in time,
 * the intervals of each role consist of a constant amount of time plus a
 * random interval. This way two nodes will eventually see each other and be
 * able to establish a connection.
 *
 * All timing values for the interval duration and the maximum amount of the
 * random offset are configurable.
 *
 *
 * # Usage
 * In the current state, the filtering of neighbors is hard coded into the
 * autoconn module. Two options are implemented:
 *
 * 1. connect to any neighbor capable of IP-over-BLE -> @ref BLE_GATT_SVC_IPSS
 *    UUID included in the BLE_GAP_AD_UUID16_INCOMP field of the received
 *    advertising data
 * 2. connect to any neighbor capable of NDN-over-BLE -> @ref BLE_GATT_SVC_NDNSS
 *    UUID included in the BLE_GAP_AD_UUID16_INCOMP field of the received
 *    advertising data
 *
 * The active filter used in autoconn is selected using one of two submodules
 * during build time:
 *
 * 1. `USEMDOULE += nimble_autoconn_ipsp`
 * 2. `USEMODULE += nibmle_autoconn_ndnsp'
 *
 * @note    The NDN support service (NDNSP) is defined by us and it is not at
 *          all standardized nor sanctioned by the BT SIG. For experimental use
 *          only...
 *
 *
 * # Implementation Status
 * - The filter function could be more powerful. It is probably a good idea to
 *   extend this module to allow for passing custom filter functions using a
 *   function pointer
 * - Currently this module does not allow to use NimBLE as IP-over-BLE node and
 *   as a GATT server concurrently. This could be enabled by adding an
 *   additional callback function which exposes some/all GAP events to a user
 *   application (i.e. BLE_GAP_EVENT_SUBSCRIBE, BLE_GAP_EVENT_NOTIFY_RX,
 *   BLE_GAP_EVENT_NOTIFY_TX).
 * - It might make sense to get rid of the periodic switching between scanning
 *   and advertising in favor of doing both in parallel. This would simplify the
 *   code (and configuration) quite a bit. But in the past, there were severe
 *   stability issues with NimBLE doing this, so it needs to be evaluated in the
 *   future if this is a feasible option.
 *
 * @{
 *
 * @file
 * @brief       Simple automated connection manager for NimBLE netif
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_AUTOCONN_H
#define NIMBLE_AUTOCONN_H

#include <stdint.h>

#include "nimble_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Return codes used by the autoconn module
 */
enum {
    NIMBLE_AUTOCONN_OK       =  0,  /**< like a walk in the park */
    NIMBLE_AUTOCONN_PARAMERR = -1,  /**< invalid parameters given */
    NIMBLE_AUTOCONN_ADERR    = -2,  /**< error generating advertising data */
};

/**
 * @brief   Set of configuration parameters needed to run autoconn
 */
typedef struct {
    /** amount of time spend in scanning mode [in ms] */
    uint32_t period_scan_ms;
    /** amount of time spend in advertising mode [in ms] */
    uint32_t period_adv_ms;
    /** a random value from 0 to this value is added to the duration of each
     *  scanning and advertising period [in ms] */
    uint32_t period_jitter_ms;
    /** advertising interval used when in advertising mode [in ms] */
    uint32_t adv_itvl_ms;
    /** scan interval applied while in scanning state [in ms] */
    uint32_t scan_itvl_ms;
    /** scan window applied while in scanning state [in ms] */
    uint32_t scan_win_ms;
    /** opening a new connection is aborted after this time [in ms] */
    uint32_t conn_timeout_ms;
    /** connection interval used when opening a new connection, lower bound.
     *  [in ms] */
    uint32_t conn_itvl_min_ms;
    /** connection interval, upper bound [in ms] */
    uint32_t conn_itvl_max_ms;
    /** slave latency used for new connections */
    uint16_t conn_latency;
    /** supervision timeout used for new connections [in ms] */
    uint32_t conn_super_to_ms;
    /** BLE PHY mode to use */
    nimble_phy_t phy_mode;
    /** node ID included in the advertising data, may be NULL */
    const char *node_id;
} nimble_autoconn_params_t;

/**
 * @brief   Initialize and enable the autoconn module
 *
 * @warning This function **must** only be called once. Typically this is during
 *          system initialization or at the beginning of the user application.
 *          Use nimble_autoconn_update() to update parameters at runtime.
 *
 * @warning Autoconn expects nimble_netif to be initialized. So make sure
 *          nimble_netif_init() was called before calling nimble_autoconn_init().
 *
 * @param[in] params        timing parameters to use
 * @param[in] ad            advertising data, if NULL it is generated
 * @param[in] adlen         length of @p ad in bytes
 */
int nimble_autoconn_init(const nimble_autoconn_params_t *params,
                         const uint8_t *ad, size_t adlen);

/**
 * @brief   Register a callback that is called on netif events
 *
 * The registered callback function is a simple pass-through of nimble_netif
 * events. The callback is executed in the context of NimBLE's host thread.
 *
 * @param[in] cb            event callback to register, may be NULL
 */
void nimble_autoconn_eventcb(nimble_netif_eventcb_t cb);

/**
 * @brief   Update the used parameters (timing and node ID)
 *
 * @param[in] params        new parameters to apply
 * @param[in] ad            advertising data, if NULL it is generated
 * @param[in] adlen         length of @p ad in bytes
 *
 * @return  NIMBLE_AUTOCONN_OK if everything went fine
 * @return  NIMBLE_AUTOCONN_INVALID if given parameters can not be applied
 */
int nimble_autoconn_update(const nimble_autoconn_params_t *params,
                           const uint8_t *ad, size_t adlen);

/**
 * @brief   Enable automated creation of new BLE connections
 */
void nimble_autoconn_enable(void);

/**
 * @brief   Disable the automated connection management
 *
 * @note    All existing connections are kept, only the scanning and advertising
 *          is canceled
 */
void nimble_autoconn_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_AUTOCONN_H */
/** @} */
