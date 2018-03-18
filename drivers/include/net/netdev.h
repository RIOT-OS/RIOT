/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Ell-i open source co-operative
 *               2015-2017 Freie Universität Berlin
 *               2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    drivers_netdev_api Netdev - Network Device Driver API
 * @ingroup     drivers_netdev
 * @brief       This is a generic low-level network driver interface
 * @{
 *
 * # About
 *
 * This interface provides a uniform API for network stacks to interact with
 * network device drivers. This interface is designed in a way, that it is
 * completely agnostic to the used network stack. This way, device drivers for
 * network devices (e.g. IEEE802.15.4 radios, Ethernet devices, ...) have to
 * implemented once and can be used with any supported network stack in RIOT.
 *
 * The functions provided by the interface cover three major parts:
 *  1. sending and receiving of actual network data
 *  2. network device configuration through reading and setting device
 *     parameters
 *  3. event handling
 *
 *
 * # The Interrupt Context Problem
 *
 * Network devices are typically connected to the host CPU via some sort of bus,
 * most commonly via SPI. This type of connection has the
 * disadvantage, that the bus is not used by the network device alone, but it
 * may be shared with other devices. This makes it necessary to synchronize
 * access to the bus to prevent bus access collisions.
 *
 * To illustrate this behavior, let's look at a typical error situation, that
 * leads to a very hard to find and debug latent failure: say we have two
 * devices A and B on the same SPI bus. Our CPU is now transferring a chunk of
 * 100 bytes to device A. After 20 bytes were transferred, device B triggers
 * an external interrupt on the host CPU. The interrupt handling now typically
 * requires the reading of some sort of status register on the 'triggering'
 * device, device B in this case. So what would happen here, is that the device
 * driver for device B would initiate a new SPI transfer on the already used bus
 * to read B's status register -> BAM.
 *
 * The peripheral drivers for shared buses (i.e. SPI and I2C) implement access
 * synchronization using mutexes, which are locked and unlocked in the driver's
 * `require` and `release` functions. The problem is now, that this type of
 * synchronization does only work in thread context, but not in interrupt
 * context. With reasonable effort and resource usage, we have no means of
 * synchronizing the bus access also in interrupt context.
 *
 * The solution to this problem as implemented by this interface is **not to
 * call any function that interacts with a device directly from interrupt
 * context**. Unfortunately this requires some added complexity for
 * synchronization efforts between thread and interrupt context to be able to
 * handle device events (i.e. external interrupts). See section
 * @ref netdev_sec_events for more information.
 *
 *
 * # Context requirements
 *
 * The `netdev` interface expects the network device drivers to run in thread
 * context (see section above). The interface was however designed in a way, to
 * allow more than one device driver to be serviced in the same thread.
 *
 * The key design element for `netdev` is, that device drivers implementing this
 * interface are not able to run stand-alone in a thread, but need some
 * bootstrapping code. This bootstrapping code can be anything from a simple
 * msg_receive() loop (as done for the GNRC adaption) to a complete network
 * stack that works without messaging entirely but is build on function call
 * interfaces.
 *
 *
 * # Sending and Receiving
 *
 * Sending data using the `netdev` interface is straight forward: simply call
 * the drivers @ref netdev_driver_t::send "send()" function, passing it the data
 * that should be sent. The caller of the @ref netdev_driver_t::send "send()"
 * function (e.g. a network stack) must hereby make sure, that the data is in
 * the correct format expected by the specific network device driver. Typically,
 * the data needs to contain a pre-filled link layer header as e.g. an
 * IEEE802.15.4 or Ethernet header.
 *
 * Receiving data using the `netdev` interface requires typically four steps:
 * 1. wait for a @ref NETDEV_EVENT_RX_COMPLETE event
 * 2. call the @ref netdev_driver_t::recv "recv()" function with `buf := NULL`
 *    and `len := 0` to get the size of the received data
 * 3. allocate a large enough buffer in some way
 * 4. call the @ref netdev_driver_t::recv "recv()" function a second time,
 *    passing the buffer and reading the received data into this buffer
 *
 * This receive sequence can of course be simplified by skipping steps 2 and 3
 * when using fixed sized pre-allocated buffers or similar means. *
 *
 * @note    The @ref netdev_driver_t::send "send()" and
 *          @ref netdev_driver_t::recv "recv()" functions **must** never be
 *          called from interrupt context.
 *
 * # Device Configuration
 *
 * The `netdev` interface covers a wide variety of network devices, which differ
 * to some extend in their configuration parameters (e.g. radios vs. wired
 * interfaces, channel selection vs. link status detection). To cover this
 * variety, `netdev` provides a generic configuration interface by exposing
 * simple @ref netdev_driver_t::get "get()" and
 * @ref netdev_driver_t::set "set()" functions. These are based on a globally
 * defined and **extendable** list of options as defined in @ref netopt.h.
 *
 * Every device driver can choose the options which it supports for reading
 * and/or writing from this list. If an option is not supported by the device
 * driver, the driver simply returns `-ENOTSUP`.
 *
 * @note    The @ref netdev_driver_t::get "get()" and
 *          @ref netdev_driver_t::set "set()" functions **must** never be called
 *          from interrupt context.
 *
 *
 * # Events {#netdev_sec_events}
 *
 * Network devices typically signal events by triggering external
 * interrupts on certain dedicated GPIO pins (in case of external devices), or
 * signal them by triggering internal interrupts directly (in case of register
 * mapped devices). As stated above, we are not allowed to do any kind of
 * interaction with our network device that involves bus access when in
 * interrupt mode. To circumvent this, the
 *
 * 1. an interrupt is triggered
 * 2. the drivers interrupt routine calls the registered @ref
 *    netdev_t::event_callback "netdev->event_callback()" function with
 *    `event:=` @ref NETDEV_EVENT_ISR as argument
 * 3. the @ref netdev_t::event_callback "netdev->event_callback()" (as it is
 *    implemented by the 'user' code) notifies the thread that hosts the device
 *    driver. This can be done in many ways, e.g. by using messaging, mutexes,
 *    thread flags and more
 * 4. the hosting thread is scheduled and calls the `netdev` interfaces
 *    @ref netdev_driver_t::isr "isr()" function
 * 5. now the driver can actual start to handle the interrupt, by e.g. reading
 *    status registers and triggering any subsequent actions like signaling
 *    a @ref NETDEV_EVENT_RX_COMPLETE
 *
 * The way that is used for waking up the hosting thread and telling is to call
 * the @ref netdev_driver_t::isr "isr()" function is completely up to the
 * `netdev` external code and can be done in many ways (e.g. sending messages, #
 * setting thread flags, unlocking mutexes, etc.).
 *
 * Any event that is not of type @ref NETDEV_EVENT_ISR is expected to be
 * triggered from thread context. This enables the code that sits on top of
 * `netdev` to perform the necessary actions right away, as for example reading
 * the received data from the network device or similar.
 *
 * @note    The @ref netdev_event_cb_t function runs in interrupt context when
 *          called for @ref NETDEV_EVENT_ISR, but it **must** run in thread
 *          context for all other events.
 *
 *
 * # Example
 *
 * The following example illustrates a receive sequence triggered by an
 * external interrupt:
 *
 * 1. packet arrives for device
 * 2. The driver previously registered an ISR for handling received packets.
 *    This ISR then calls @ref netdev_t::event_callback "netdev->event_callback()"
 *    with `event:= `@ref NETDEV_EVENT_ISR (from Interrupt Service Routine)
 *    which wakes up event handler
 * 3. event handler calls @ref netdev_driver_t::isr "netdev->driver->isr()"
 *    (from thread context)
 * 4. @ref netdev_driver_t::isr "netdev->driver->isr()" calls
 *    @ref netdev_t::event_callback "netdev->event_callback()" with
 *    `event:= `@ref NETDEV_EVENT_RX_COMPLETE
 * 5. @ref netdev_t::event_callback "netdev->event_callback()" uses
 *    @ref netdev_driver_t::recv "netdev->driver->recv()" to fetch packet
 *
 * ![RX event example](riot-netdev-rx.svg)
 *
 * @file
 * @brief       Definitions low-level network driver interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_NETDEV_H
#define NET_NETDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "iolist.h"
#include "net/netopt.h"

#ifdef MODULE_NETSTATS_L2
#include "net/netstats.h"
#endif
#ifdef MODULE_L2FILTER
#include "net/l2filter.h"
#endif

enum {
    NETDEV_TYPE_UNKNOWN,
    NETDEV_TYPE_RAW,
    NETDEV_TYPE_ETHERNET,
    NETDEV_TYPE_IEEE802154,
    NETDEV_TYPE_BLE,
    NETDEV_TYPE_CC110X,
    NETDEV_TYPE_LORA,
    NETDEV_TYPE_NRFMIN,
    NETDEV_TYPE_SLIP,
};

/**
 * @brief   Possible event types that are send from the device driver to the
 *          upper layer
 */
typedef enum {
    NETDEV_EVENT_ISR,                       /**< driver needs it's ISR handled */
    NETDEV_EVENT_RX_STARTED,                /**< started to receive a packet */
    NETDEV_EVENT_RX_COMPLETE,               /**< finished receiving a packet */
    NETDEV_EVENT_TX_STARTED,                /**< started to transfer a packet */
    NETDEV_EVENT_TX_COMPLETE,               /**< transfer packet complete */
    NETDEV_EVENT_TX_COMPLETE_DATA_PENDING,  /**< transfer packet complete and data pending flag */
    NETDEV_EVENT_TX_NOACK,                  /**< ACK requested but not received */
    NETDEV_EVENT_TX_MEDIUM_BUSY,            /**< couldn't transfer packet */
    NETDEV_EVENT_LINK_UP,                   /**< link established */
    NETDEV_EVENT_LINK_DOWN,                 /**< link gone */
    NETDEV_EVENT_TX_TIMEOUT,                /**< timeout when sending */
    NETDEV_EVENT_RX_TIMEOUT,                /**< timeout when receiving */
    NETDEV_EVENT_CRC_ERROR,                 /**< wrong CRC */
    NETDEV_EVENT_FHSS_CHANGE_CHANNEL,       /**< channel changed */
    NETDEV_EVENT_CAD_DONE,                  /**< channel activity detection done */
    /* expand this list if needed */
} netdev_event_t;

/**
 * @brief   Received packet status information for most radios
 *
 * May be different for certain radios.
 */
struct netdev_radio_rx_info {
    int16_t rssi;       /**< RSSI of a received packet in dBm */
    uint8_t lqi;        /**< LQI of a received packet */
};

/**
 * @brief   Forward declaration for netdev struct
 */
typedef struct netdev netdev_t;

/**
 * @brief   Event callback for signaling event to upper layers
 *
 * @param[in] type          type of the event
 */
typedef void (*netdev_event_cb_t)(netdev_t *dev, netdev_event_t event);

/**
 * @brief Structure to hold driver state
 *
 * Supposed to be extended by driver implementations.
 * The extended structure should contain all variable driver state.
 *
 * Contains a field @p context which is not used by the drivers, but supposed to
 * be used by upper layers to store reference information.
 */
struct netdev {
    const struct netdev_driver *driver;     /**< ptr to that driver's interface. */
    netdev_event_cb_t event_callback;       /**< callback for device events */
    void* context;                          /**< ptr to network stack context */
#ifdef MODULE_NETDEV_LAYER
    netdev_t *lower;                        /**< ptr to the lower netdev layer */
#endif
#ifdef MODULE_NETSTATS_L2
    netstats_t stats;                       /**< transceiver's statistics */
#endif
#ifdef MODULE_L2FILTER
    l2filter_t filter[L2FILTER_LISTSIZE];   /**< link layer address filters */
#endif
};

/**
 * @brief Structure to hold driver interface -> function mapping
 *
 * The send/receive functions expect/return a full ethernet
 * frame (dst mac, src mac, ethertype, payload, no checksum).
 */
typedef struct netdev_driver {
    /**
     * @brief Send frame
     *
     * @pre `(dev != NULL) && (iolist != NULL`
     *
     * @param[in] dev       network device descriptor
     * @param[in] iolist    io vector list to send
     *
     * @return number of bytes sent, or `< 0` on error
     */
    int (*send)(netdev_t *dev, const iolist_t *iolist);

    /**
     * @brief Get a received frame
     *
     * @pre `(dev != NULL)`
     * @pre `(buf != NULL) && (len > 0)`
     *
     * Supposed to be called from
     * @ref netdev_t::event_callback "netdev->event_callback()"
     *
     * If buf == NULL and len == 0, returns the packet size without dropping it.
     * If buf == NULL and len > 0, drops the packet and returns the packet size.
     *
     * @param[in]   dev     network device descriptor
     * @param[out]  buf     buffer to write into or NULL
     * @param[in]   len     maximum number of bytes to read
     * @param[out] info     status information for the received packet. Might
     *                      be of different type for different netdev devices.
     *                      May be NULL if not needed or applicable.
     *
     * @return `< 0` on error
     * @return number of bytes read if buf != NULL
     * @return packet size if buf == NULL
     */
    int (*recv)(netdev_t *dev, void *buf, size_t len, void *info);

    /**
     * @brief the driver's initialization function
     *
     * @pre `(dev != NULL)`
     *
     * @return `< 0` on error, 0 on success
     */
    int (*init)(netdev_t *dev);

    /**
     * @brief a driver's user-space ISR handler
     *
     * @pre `(dev != NULL)`
     *
     * This function will be called from a network stack's loop when being
     * notified by netdev_isr.
     *
     * It is supposed to call
     * @ref netdev_t::event_callback "netdev->event_callback()" for each
     * occurring event.
     *
     * See receive packet flow description for details.
     *
     * @param[in]   dev     network device descriptor
     */
    void (*isr)(netdev_t *dev);

    /**
     * @brief   Get an option value from a given network device
     *
     * @pre `(dev != NULL)`
     *
     * @param[in]   dev     network device descriptor
     * @param[in]   opt     option type
     * @param[out]  value   pointer to store the option's value in
     * @param[in]   max_len maximal amount of byte that fit into @p value
     *
     * @return              number of bytes written to @p value
     * @return              `< 0` on error, 0 on success
     */
    int (*get)(netdev_t *dev, netopt_t opt,
               void *value, size_t max_len);

    /**
     * @brief   Set an option value for a given network device
     *
     * @pre `(dev != NULL)`
     *
     * @param[in] dev       network device descriptor
     * @param[in] opt       option type
     * @param[in] value     value to set
     * @param[in] value_len the length of @p value
     *
     * @return              number of bytes used from @p value
     * @return              `< 0` on error, 0 on success
     */
    int (*set)(netdev_t *dev, netopt_t opt,
               const void *value, size_t value_len);
} netdev_driver_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_NETDEV_H */
/** @} */
