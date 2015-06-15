/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     dev_ble_nrf ble Low-Level Driver Inteface
 * @{
 *
 * @file
 * @brief       Low-level bluetooth low-energy interface
 *
 * @author      James Hollister <jhollisterjr@gmail.com>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef DEV_BLE_H
#define DEV_BLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Structure to hold driver state
 *
 * Supposed to be extended by driver implementations.
 * The extended structure should contain all variable driver state.
 */
typedef struct dev_ble {
    const struct ble_driver *driver; /**< ptr to that driver's interface.
                                     driver->init() expects this to be present,
                                     so set this before using the device. */
} dev_ble_t;

/**
 * @brief Structure to hold driver interface -> function mapping
 *
 */
typedef struct ble_driver {
    /**
     * @brief Send a ble frame
     *
     * @param buf buffer to read from
     * @param len nr of bytes to send
     *
     * @return nr of bytes sent, or <=0 on error
     */
    int (*send)(dev_ble_t *dev, char* buf, int len);

    /**
     * @brief Get a received ble frame
     *
     * @param buf buffer to write to
     * @param len maximum nr. of bytes to read
     *
     * @return nr of bytes read or <=0 on error
     */
    int (*recv)(dev_ble_t *dev, char* buf, int len);

    /**
     * @brief the driver's initialization function
     *
     * @return <=0 on error, >0 on success
     */
    int (*init)(dev_ble_t *dev);
    /**
     * @brief a driver's user-space ISR handler
     *
     */
    void (*isr)(dev_ble_t *dev);
} ble_driver_t;

/**
 * @brief Initialize a device given by dev (convenience function)
 *
 * The device given as parameter *must* be previously setup by the
 * drivers *_setup() function.
 *
 */
static inline int dev_ble_init(dev_ble_t *dev) {
    return dev->driver->init(dev);
}

/**
 * @brief global dev_ble interrupt handling function.
 *
 * This function should be called from your device's ISR from whithin ISR context.
 * It is supposed to wake up a waiting user-space event loop.
 */
extern void dev_ble_isr(dev_ble_t *dev);

/**
 * @brief dev_ble event callback for packets that were received.
 *
 * This function should be called from whithin your driver's isr()
 * (as defined in ble_driver_t), once for every packet that was received.
 *
 * It needs to call dev->driver->recv() in order to get received packet
 * from the driver.
 */
extern void dev_ble_rx_handler(dev_ble_t *dev);

/**
 * @brief dev_ble ble link state handler
 *
 * This function should be called from whithin your driver's isr()
 * (as defined in ble_driver_t) for every layer 2 link state change.
 *
 * @param dev device that triggered the event
 * @param newstate 1 for "link established", 0 for "link down"
 */
extern void dev_ble_linkstate_handler(dev_ble_t *dev, int newstate);
#ifdef __cplusplus
}
#endif
/** @} */
#endif /* DEV_BLE_H */
