/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     examples
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "uwb/uwb.h"
#include "uwb/uwb_ftypes.h"

/**
 * @brief   Block after a request is sent
 */
#ifndef CONFIG_TWR_SHELL_BLOCKING
#define CONFIG_TWR_SHELL_BLOCKING           1
#endif

/**
 * @brief   Minimum idle time to enable putting the radio to sleep
 */
#ifndef CONFIG_TWR_MIN_IDLE_SLEEP_MS
#define CONFIG_TWR_MIN_IDLE_SLEEP_MS        20
#endif

/*
 * @brief   Block after a request is sent
 */
#ifndef CONFIG_TWR_PRINTF_INITIATOR_ONLY
#define CONFIG_TWR_PRINTF_INITIATOR_ONLY    1
#endif

/**
 * @brief   uwb rng data structure
 */
typedef struct uwb_core_rng_data_t {
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
} uwb_core_rng_data_t;

/**
 * @brief TWR algorithms
 */
typedef enum {
    TWR_PROTOCOL_NONE   = 0,
    TWR_PROTOCOL_SS     = UWB_DATA_CODE_SS_TWR,     /**< single sided twr */
    TWR_PROTOCOL_SS_ACK = UWB_DATA_CODE_SS_TWR_ACK, /**< single sided twr with
                                                         hw ACK as response */
    TWR_PROTOCOL_SS_EXT = UWB_DATA_CODE_SS_TWR_EXT, /**< single sided twr with
                                                         extended frames */
    TWR_PROTOCOL_DS     = UWB_DATA_CODE_DS_TWR,     /**< double sided twr */
    TWR_PROTOCOL_DS_ACK = UWB_DATA_CODE_DS_TWR_EXT, /**< double sided twr with
                                                         extended frames */
} twr_protocol_t;

/**
 * @brief   Initialize the uwb_rng wrapper
 */
void uwb_core_rng_init(void);

/**
 * @brief   Listen for rng requests
 */
void uwb_core_rng_listen_enable(void);

/**
 * @brief   Do not listen for rng requests
 */
void uwb_core_rng_listen_disable(void);

/**
 * @brief   Start performing range requests
 *
 * @param[in]   addr        short address of destination
 * @param[in]   proto       twr protocol to use
 * @param[in]   interval    interval between requests in ms
 * @param[in]   count       number of requests to perform
 */
void uwb_core_rng_start(uint16_t addr, twr_protocol_t proto, uint32_t interval,
                        uint32_t count);

/**
 * @brief   Returns remaining rng requests
 */
uint32_t uwb_core_rng_req_remaining(void);

#ifdef __cplusplus
}
#endif
