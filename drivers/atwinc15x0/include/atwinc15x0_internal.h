/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_atwinc15x0
 * @{
 *
 * @file
 * @brief       Internal definitions for the ATWINC15x0 WiFi netdev driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ATWINC15X0_INTERNAL_H
#define ATWINC15X0_INTERNAL_H

#include <stdbool.h>
#include <string.h>

#include "driver/include/m2m_types.h"
#include "atwinc15x0.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the single ATWINC15x0 device instance
 *
 * Since the vendor ATWINC15x0 host driver uses many global variables, only
 * a single ATWINC15x0 device can be used. Therefore, the RIOT driver only
 * supports a single instance of an ATWINC15x0 device.
 */
extern atwinc15x0_t *atwinc15x0;

/**
 * @brief   Minimum number of slots to scan a channel
 */
#define ATWINC1510_SCAN_SLOTS_MIN           2

/**
 * @brief   Default number of slots to scan a channel
 */
#define ATWINC1510_SCAN_SLOTS_DEF           M2M_SCAN_DEFAULT_NUM_SLOTS

/**
 * @brief   Maximum number of slots to scan a channel
 */
#define ATWINC1510_SCAN_SLOTS_MAX           255

/**
 * @brief   Time in ms to scan a slot in a channel
 */
#define ATWINC1510_SCAN_SLOT_TIME_MS_MIN    10

/**
 * @brief   Default time in ms to scan a slot in a channel
 */
#define ATWINC1510_SCAN_SLOT_TIME_MS_DEF    M2M_SCAN_DEFAULT_SLOT_TIME

/**
 * @brief   Maximum time in ms to scan a slot in a channel
 */
#define ATWINC1510_SCAN_SLOT_TIME_MS_MAX    250

/**
 * @brief   Default number of probes to send to scan a channel
 */
#define ATWINC1510_SCAN_PROBES_NUMOF_DEF    M2M_SCAN_DEFAULT_NUM_PROBE

/**
 * @brief   Default threshold in dbm for an AP to pass
 */
#define ATWINC1510_SCAN_THRESHOLD_DBM_DEF   (-99)

/**
 * @brief   Check if @p dev is scanning
 *
 * @param[in]       dev         ATWINC15x0 device
 *
 * @returns true if @p dev is scanning
 * @returns false if @p dev is not scanning
 */
static inline bool _atwinc15x0_is_scanning(const atwinc15x0_t *dev) {
    return dev->state == ATWINC15X0_STATE_CONNECTED_SCANNING ||
           dev->state == ATWINC15X0_STATE_DISCONNECTED_SCANNING;
}

/**
 * @brief   Set state to indicate that @p dev is scanning
 *
 * @param[in, out]  dev         ATWINC15x0 device
 */
static inline void _atwinc15x0_set_scanning(atwinc15x0_t *dev) {
    if (dev->state == ATWINC15X0_STATE_CONNECTED) {
        dev->state = ATWINC15X0_STATE_CONNECTED_SCANNING;
    }
    else if (dev->state == ATWINC15X0_STATE_DISCONNECTED) {
        dev->state = ATWINC15X0_STATE_DISCONNECTED_SCANNING;
    }
    else {
        assert(false);
    }
}

/**
 * @brief   Check if @p dev is connecting to an AP
 *
 * @param[in]       dev         ATWINC15x0 device
 *
 * @returns true if @p dev is connecting
 * @returns false if @p dev is not connecting
 */
static inline bool _atwinc15x0_is_connecting(const atwinc15x0_t *dev) {
    return dev->state == ATWINC15X0_STATE_CONNECTED_CONNECTING ||
           dev->state == ATWINC15X0_STATE_DISCONNECTED_CONNECTING;
}

/**
 * @brief   Set state to indicate that @p dev is connecting
 *          to an AP
 *
 * @param[in, out]  dev         ATWINC15x0 device
 */
static inline void _atwinc15x0_set_connecting(atwinc15x0_t *dev) {
    if (dev->state == ATWINC15X0_STATE_CONNECTED) {
        dev->state = ATWINC15X0_STATE_CONNECTED_CONNECTING;
    }
    else if (dev->state == ATWINC15X0_STATE_DISCONNECTED) {
        dev->state = ATWINC15X0_STATE_DISCONNECTED_CONNECTING;
    }
    else {
        assert(false);
    }
}

/**
 * @brief   Check if @p dev is disconnecting from an AP
 *
 * @param[in]       dev         ATWINC15x0 device
 *
 * @returns true if @p dev is disconnecting
 * @returns false if @p dev is not disconnecting
 */
static inline bool _atwinc15x0_is_disconnecting(const atwinc15x0_t *dev) {
    return dev->state == ATWINC15X0_STATE_DISCONNECTING;
}

/**
 * @brief   Set state to indicate that @p dev is disconnecting
 *          from an AP
 *
 * @param[in, out]  dev         ATWINC15x0 device
 */
static inline void _atwinc15x0_set_disconnecting(atwinc15x0_t *dev) {
    if (dev->state == ATWINC15X0_STATE_CONNECTED) {
        dev->state = ATWINC15X0_STATE_DISCONNECTING;
    }
}

/**
 * @brief   Check if @p dev is connected to an AP
 *
 * @param[in]       dev         ATWINC15x0 device
 *
 * @returns true if @p dev is connected
 * @returns false if @p dev is not connected
 */
static inline bool _atwinc15x0_is_connected(const atwinc15x0_t *dev) {
    return dev->state == ATWINC15X0_STATE_CONNECTED ||
           dev->state == ATWINC15X0_STATE_CONNECTED_SCANNING ||
           dev->state == ATWINC15X0_STATE_CONNECTED_CONNECTING;
}

/**
 * @brief   Set state to indicate that @p dev is connected
 *          to an AP
 *
 * @param[in, out]  dev         ATWINC15x0 device
 */
static inline void _atwinc15x0_set_connected(atwinc15x0_t *dev) {
    assert(dev->state == ATWINC15X0_STATE_DISCONNECTED ||
           dev->state == ATWINC15X0_STATE_DISCONNECTED_CONNECTING ||
           dev->state == ATWINC15X0_STATE_CONNECTED_SCANNING);
    dev->state = ATWINC15X0_STATE_CONNECTED;
}

/**
 * @brief   Set state to indicate that @p dev is disconnected
 *
 * @param[in, out]  dev         ATWINC15x0 device
 */
static inline void _atwinc15x0_set_disconnected(atwinc15x0_t *dev) {
    dev->state = ATWINC15X0_STATE_DISCONNECTED;
}

/**
 * @brief   Check if @p dev is currently performing an asynchronous operation
 *
 * @param[in]       dev          ATWINC15x0 device
 * @returns true if @p dev is busy
 * @returns false if @p dev is not busy
 */
static inline bool _atwinc15x0_is_busy(const atwinc15x0_t *dev) {
    return dev->state == ATWINC15X0_STATE_DISCONNECTING ||
           dev->state == ATWINC15X0_STATE_DISCONNECTED_SCANNING ||
           dev->state == ATWINC15X0_STATE_DISCONNECTED_CONNECTING ||
           dev->state == ATWINC15X0_STATE_CONNECTED_SCANNING ||
           dev->state == ATWINC15X0_STATE_CONNECTED_CONNECTING;
}

/**
 * @brief Check is @p dev is currently not performing an asynchronous operation
 *
 * @param[in]       dev                   ATWINC15x0 device
 *
 * @return true if @p dev is idle
 * @return false if @p dev is not idle
 */
static inline bool _atwinc15x0_is_idle(const atwinc15x0_t *dev) {
    return dev->state == ATWINC15X0_STATE_CONNECTED ||
           dev->state == ATWINC15X0_STATE_DISCONNECTED;
}

/**
 * @brief   Return from any busy state to corresponding idle state
 *
 * @param[in]       dev                   ATWINC15x0 device
 */
static inline void _atwinc15x0_set_idle(atwinc15x0_t *dev) {
    if (dev->state == ATWINC15X0_STATE_CONNECTED_SCANNING ||
        dev->state == ATWINC15X0_STATE_CONNECTED_CONNECTING) {
        _atwinc15x0_set_connected(dev);
    }
    else if (dev->state == ATWINC15X0_STATE_DISCONNECTED_SCANNING ||
             dev->state == ATWINC15X0_STATE_DISCONNECTED_CONNECTING) {
        _atwinc15x0_set_disconnected(dev);
    }
}

/**
 * @brief   Set state to indicate that @p dev is sleeping
 *
 * @param[in]       dev         ATWINC15x0 device
 */
static inline void _atwinc15x0_set_sleeping(atwinc15x0_t *dev) {
    assert(!_atwinc15x0_is_busy(dev));
    dev->state = ATWINC15X0_STATE_SLEEP;
}

/**
 * @brief   Check if @p dev is currently sleeping
 *
 * @param[in]       dev         ATWINC15x0 device
 * @return true if @p dev is sleeping
 * @return false if @p dev is not sleeping
 */
static inline bool _atwinc15x0_is_sleeping(const atwinc15x0_t *dev) {
    return dev->state == ATWINC15X0_STATE_SLEEP;
}

/**
 * @brief   Set member of currently connected AP SSID
 *
 * @pre     Module atwinc15x0_dynamic_connect is used
 *
 * @param[in, out]  dev         ATWINC15x0 device
 * @param[in]       ssid        SSID
 */
static inline void _atwinc15x0_sta_set_current_ssid(atwinc15x0_t *dev, const char *ssid) {
    (void)dev; (void)ssid;
#if IS_USED(MODULE_ATWINC15X0_DYNAMIC_CONNECT)
    strcpy(atwinc15x0->ssid, ssid);
#endif
}

/**
 * @brief   Get member of currently connected AP
 *
 * @pre     Module atwinc15x0_dynamic_connect is used
 *
 * @param[in]       dev         ATWINC15x0 device
 *
 * @returns SSID member
 */
static inline const char *_atwinc15x0_sta_get_current_ssid(const atwinc15x0_t *dev) {
    (void)dev;
#if IS_USED(MODULE_ATWINC15X0_DYNAMIC_CONNECT)
    return dev->ssid;
#endif
    return NULL;
}

/**
 * @brief   ATWINC15x0 device driver ISR
 */
void atwinc15x0_irq(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ATWINC15X0_INTERNAL_H */
