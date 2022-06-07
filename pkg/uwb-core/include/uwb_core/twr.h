/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_uwb_core_twr    TWR helper
 * @ingroup     pkg_uwb_core
 * @brief       Helper module to allow for simple ALOHA based TWR exchanges
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#ifndef UWB_CORE_TWR_H
#define UWB_CORE_TWR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "uwb/uwb.h"
#include "uwb_core.h"

/**
 * @brief   Block after a request is sent
 */
#ifndef CONFIG_TWR_SHELL_BLOCKING
#define CONFIG_TWR_SHELL_BLOCKING           1
#endif

/**
 * @brief   Minimum idle time to enable putting the radio to sleep
 *
 * @note    Only used if 'uwb_core_twr_sleep' is included
 */
#ifndef CONFIG_TWR_MIN_IDLE_SLEEP_MS
#define CONFIG_TWR_MIN_IDLE_SLEEP_MS        20
#endif

/**
 * @brief   The callback set with @ref uwb_core_twr_set_cb is only called for the
 *          TWR exchange initiator
 */
#ifndef CONFIG_TWR_NOTIFY_INITIATOR_ONLY
#define CONFIG_TWR_NOTIFY_INITIATOR_ONLY    1
#endif

/**
 * @brief   uwb rng data structure
 */
typedef struct uwb_core_twr_data {
#if IS_USED(MODULE_UWB_CORE_RNG_TRX_INFO)
    float tof;              /**< range request time of flight */
    float los;              /**< range request line of sight estimation [0..1] */
    float fppl;             /**< range request first path phase loss */
#endif
    uint32_t time;          /**< range request timestamp ms */
    int32_t d_cm;           /**< range request range estimation (cm) */
    uint16_t src;           /**< source short address */
    uint16_t dest;          /**< destination short address */
#if IS_USED(MODULE_UWB_CORE_RNG_TRX_INFO)
    int16_t rssi;           /**< range request rssi */
#endif
} uwb_core_twr_data_t;

/**
 * @brief   Callback signature triggered by this module for each completed TWR
 *          exchange.
 */
typedef void(*uwb_core_twr_cb_t)(void *data);

/**
 * @brief   Initialize the uwb_rng wrapper
 */
void uwb_core_twr_init(void);

/**
 * @brief   Listen for rng requests
 */
void uwb_core_twr_listen_enable(void);

/**
 * @brief   Do not listen for rng requests
 */
void uwb_core_twr_listen_disable(void);

/**
 * @brief   Start performing range requests
 *
 * @param[in]   addr        short address of destination
 * @param[in]   proto       twr protocol to use
 * @param[in]   interval    interval between requests in ms
 * @param[in]   count       number of requests to perform
 */
void uwb_core_twr_start(uint16_t addr, twr_protocol_t proto, uint32_t interval, uint32_t count);

/**
 * @brief   Returns remaining rng requests
 *
 * @return  Remaining requests.
 */
uint32_t uwb_core_twr_req_remaining(void);

/**
 * @brief   Checks current status
 *
 * @return true if devices is listening, false otherwise
 */
bool uwb_core_twr_listen_is_enabled(void);

/**
 * @brief   Sets callback to be called on completion of a rng request
 *
 * The callback is executed in the event queue returned by @ref uwb_core_get_eventq()
 *
 * @param[in]   cb          the callback
 */
void uwb_core_twr_set_cb(uwb_core_twr_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* UWB_CORE_TWR_H */
