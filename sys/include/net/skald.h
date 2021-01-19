/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    ble_skald Skald, who advertises to the world
 * @ingroup     ble
 * @brief       Skald, a minimalistic BLE advertising stack
 *
 * # About
 *
 * Skald is a very minimalistic BLE implementation, implementing the
 * `broadcaster` role only. With this focus, the stack allows for setting up
 * different kind of beacons using an extremely low memory footprint.
 *
 * # Design Decisions and Limitations
 * - support for local addresses only (using `luid` to generate them)
 * - advertising interval is configured during compile time, override by setting
 *   `CFLAGS+=-DCONFIG_SKALD_INTERVAL=xxx`
 * - advertising channels are configured during compile time, override by
 *   setting `CFLAGS+=-DSKALD_ADV_CHAN={37,39}`
 *
 * # Implementation state
 * Supported:
 * - advertising of custom GAP payloads
 * - iBeacon (full support)
 * - Eddystone (partly supported)
 *
 * Limitations:
 * - currently Skald supports random static addresses only (generated using
 *   the `luid` module)
 *
 * @{
 * @file
 * @brief       Skald's basic interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_SKALD_H
#define NET_SKALD_H

#include <stdint.h>

#include "ztimer.h"
#include "net/ble.h"
#include "net/netdev/ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_skald_conf Skald compile configurations
 * @ingroup config
 * @{
 */
/**
 * @brief   Advertising interval in microseconds
 */
#ifndef CONFIG_SKALD_INTERVAL_MS
#define CONFIG_SKALD_INTERVAL_MS        (1000U)
#endif

/**
 * @brief   Configure advertising channel 37
 *
 * Set CONFIG_ADV_CH_37_DISABLE to disable channel 37
 */
#ifdef DOXYGEN
#define CONFIG_ADV_CH_37_DISABLE
#endif

/**
 * @brief   Configure advertising channel 38
 *
 * Set CONFIG_ADV_CH_38_DISABLE to disable channel 38
 */
#ifdef DOXYGEN
#define CONFIG_ADV_CH_38_DISABLE
#endif

/**
 * @brief   Configure advertising channel 39
 *
 * Set CONFIG_ADV_CH_39_DISABLE to disable channel 39
 */
#ifdef DOXYGEN
#define CONFIG_ADV_CH_39_DISABLE
#endif
/** @} */

/**
 * @brief   Define advertising channel 37 if @ref CONFIG_ADV_CH_37_DISABLE is
 *          not set
 */
#if !defined(CONFIG_ADV_CH_37_DISABLE) || defined(DOXYGEN)
#define ADV_CH_37 37,
#else
#define ADV_CH_37
#endif

/**
 * @brief   Define advertising channel 38 if @ref CONFIG_ADV_CH_38_DISABLE is
 *          not set
 */
#if !defined(CONFIG_ADV_CH_38_DISABLE) || defined(DOXYGEN)
#define ADV_CH_38 38,
#else
#define ADV_CH_38
#endif

/**
 * @brief   Define advertising channel 39 if @ref CONFIG_ADV_CH_39_DISABLE is
 *          not set
 */
#if !defined(CONFIG_ADV_CH_39_DISABLE) || defined(DOXYGEN)
#define ADV_CH_39 39
#else
#define ADV_CH_39
#endif

/**
 * @brief   List of advertising channels
 */
#ifndef SKALD_ADV_CHAN
#define SKALD_ADV_CHAN { ADV_CH_37 ADV_CH_38 ADV_CH_39 }
#endif

/**
 * @brief   UUID representation format used by Skald
 */
typedef struct {
    uint8_t u8[16];         /**< UUID with byte-wise access */
} skald_uuid_t;

/**
 * @brief   Advertising context holding the advertising data and state
 */
typedef struct {
    netdev_ble_pkt_t pkt;   /**< packet holding the advertisement (GAP) data */
    ztimer_t timer;         /**< timer for scheduling advertising events */
    ztimer_now_t last;      /**< last timer trigger (for offset compensation) */
    uint8_t cur_chan;       /**< keep track of advertising channels */
} skald_ctx_t;

/**
 * @brief   Initialize Skald and the underlying radio
 */
void skald_init(void);

/**
 * @brief   Start advertising the given packet
 *
 * The packet will be send out each advertising interval (see CONFIG_SKALD_INTERVAL) on
 * each of the defined advertising channels (see SKALD_ADV_CHAN).
 *
 * @param[in,out] ctx   start advertising this context
 */
void skald_adv_start(skald_ctx_t *ctx);

/**
 * @brief   Stop the ongoing advertisement
 *
 * @param[in,out] ctx   stop advertising this context
 */
void skald_adv_stop(skald_ctx_t *ctx);

/**
 * @brief   Generate a random public address
 *
 * @note    @p buf must be able to hold BLE_ADDR_LEN (6) bytes
 *
 * @param[out] buf      the generated address is written to this buffer
 */
void skald_generate_random_addr(uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* NET_SKALD_H */
/** @} */
