/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Ell-i open source co-operative
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_dev_eth_ll Low-Level Driver Inteface
 * @ingroup     net_ng_ethernet
 * @file
 * @brief       Low-level ethernet driver interface
 * @{
 *
 * This interface is supposed to be a low-level interface for ethernet drivers.
 * In order to be universally usable, it leaves out many implementation details
 * to the implementation of an actual network stack using this interface.
 *
 * In order to write a driver for this interface, you have to
 *
 * 1. create a (possibly const) eth_driver_t structure for your device type,
 *    implement it's functions
 *
 * 2. create a dev_eth_t structure, used as device state and handle, for each device
 *
 * In order to use this interface, you have to
 *
 * 1. implement dev_eth_isr, dev_eth_rx_handler and dev_eth_linkstate_handler
 * 2. run a loop that get's notified by dev_eth_isr
 *
 * A devices send function should always be able to send a frame (and make sure of proper locking).
 *
 * Receive packet flow:
 *
 * 1. Ethernet driver receives packet, executes driver's internal ISR.
 * 2. driver's internal ISR should do minimal acknowledging and house keeping and then
 *    call dev_eth_isr
 * 3. dev_eth_isr should notify a user of this API (e.g., the network stack's L2 thread)
 * 4. That thread executes the driver's user-space isr (dev->driver->isr)
 * 5. user space ISR handles less timing critical stuff, eventually calling
 *    dev_eth_linkstate_handler and / or dev_eth_rx_handler
 *
 * Check out the dev_eth test application as example.
 *
 * @file
 * @brief       Definitions low-level ethernet driver interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef DEV_ETH_H
#define DEV_ETH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ng_ethernet/hdr.h"

/**
 * @brief Structure to hold driver state
 *
 * Supposed to be extended by driver implementations.
 * The extended structure should contain all variable driver state.
 */
typedef struct dev_eth {
    const struct eth_driver *driver; /**< ptr to that driver's interface.
                                     driver->init() expects this to be present,
                                     so set this before using the device. */
} dev_eth_t;

/**
 * @brief Structure to hold driver interface -> function mapping
 *
 * The send/receive functions expect/return a full ethernet
 * frame (dst mac, src mac, ethertype, payload, no checksum).
 */
typedef struct eth_driver {
    /**
     * @brief Send ethernet frame
     *
     * Expects a full ethernet frame (dst mac, src mac, ethertype, payload,
     * no checksum).
     *
     * @param buf buffer to read from
     * @param len nr of bytes to send
     *
     * @return nr of bytes sent, or <=0 on error
     */
    int (*send)(dev_eth_t *dev, char* buf, int len);

    /**
     * @brief Get a received ethernet frame
     *
     * Supposed to be called from dev_eth_rx_handler().
     *
     * Make sure buf can hold the maximum expected ethernet frame size.
     *
     * @param buf buffer to write to
     * @param len maximum nr. of bytes to read
     *
     * @return nr of bytes read or <=0 on error
     */
    int (*recv)(dev_eth_t *dev, char* buf, int len);

    /**
     * @brief get the device's MAC address
     *
     * @param buf location to write to. Make sure this is can take least 6 bytes
     */
    void (*get_mac_addr)(dev_eth_t *dev, uint8_t *buf);

    /**
     * @brief get the device's promiscous status
     *
     * Default is not promiscous.
     * Promiscous means, receive all ethernet frames.
     * Not promiscous means only receive broadcast, multicast or frames with
     * the device's MAC address as dst address.
     *
     * @return 1 for promiscous, 0 for not promiscous
     */
    int (*get_promiscous)(dev_eth_t *dev);
    /**
     * @brief set the devices promiscous mode
     *
     * @param value 1 for promiscous, 0 for not promiscous
     * @return the new value (device might not support wanted mode)
     */
    int (*set_promiscous)(dev_eth_t *dev, int value);
    /**
     * @brief the driver's initialization function
     *
     * @return <=0 on error, >0 on success
     */
    int (*init)(dev_eth_t *dev);
    /**
     * @brief a driver's user-space ISR handler
     *
     * This function will be called from a network stack's loop when being notified
     * by dev_eth_isr.
     *
     * It is supposed to call dev_eth_rx_handler for each available received packed,
     * and dev_eth_linkstate_handler whenever a link state change event occurs.
     *
     * See receive packet flow description for details.
     */
    void (*isr)(dev_eth_t *dev);
} eth_driver_t;

/**
 * @brief Initialize a device given by dev (convenience function)
 *
 * The device given as parameter *must* be previously setup by the
 * drivers *_setup() function.
 *
 */
static inline int dev_eth_init(dev_eth_t *dev) {
    return dev->driver->init(dev);
}

/**
 * @brief global dev_eth interrupt handling function.
 *
 * This function should be called from your device's ISR from whithin ISR context.
 * It is supposed to wake up a waiting user-space event loop.
 */
extern void dev_eth_isr(dev_eth_t *dev);

/**
 * @brief dev_eth event callback for packets that were received.
 *
 * This function should be called from whithin your driver's isr()
 * (as defined in eth_driver_t), once for every packet that was received.
 *
 * It needs to call dev->driver->recv() in order to get received packet
 * from the driver.
 */
extern void dev_eth_rx_handler(dev_eth_t *dev);

/**
 * @brief dev_eth ethernet link state handler
 *
 * This function should be called from whithin your driver's isr()
 * (as defined in eth_driver_t) for every layer 2 link state change.
 *
 * @param dev device that triggered the event
 * @param newstate 1 for "link established", 0 for "link down"
 */
extern void dev_eth_linkstate_handler(dev_eth_t *dev, int newstate);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* DEV_ETH_H */
