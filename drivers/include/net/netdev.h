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
 * 1. frame arrives for device
 * 2. The driver previously registered an ISR for handling received frames.
 *    This ISR then calls @ref netdev_t::event_callback "netdev->event_callback()"
 *    with `event:= `@ref NETDEV_EVENT_ISR (from Interrupt Service Routine)
 *    which wakes up event handler
 * 3. event handler calls @ref netdev_driver_t::isr "netdev->driver->isr()"
 *    (from thread context)
 * 4. @ref netdev_driver_t::isr "netdev->driver->isr()" calls
 *    @ref netdev_t::event_callback "netdev->event_callback()" with
 *    `event:= `@ref NETDEV_EVENT_RX_COMPLETE
 * 5. @ref netdev_t::event_callback "netdev->event_callback()" uses
 *    @ref netdev_driver_t::recv "netdev->driver->recv()" to fetch frame
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
#include <errno.h>

#include "iolist.h"
#include "net/netopt.h"
#include "kernel_defines.h"

#ifdef MODULE_L2FILTER
#include "net/l2filter.h"
#endif

/**
 * @name        Network device types
 * @anchor      net_netdev_type
 * @attention   When implementing a new type that is able to carry IPv6, have
 *              a look if you need to update @ref net_l2util as well.
 * @{
 */
enum {
    NETDEV_TYPE_UNKNOWN,
    NETDEV_TYPE_TEST,
    NETDEV_TYPE_RAW,
    NETDEV_TYPE_ETHERNET,
    NETDEV_TYPE_IEEE802154,
    NETDEV_TYPE_BLE,
    NETDEV_TYPE_CC110X,
    NETDEV_TYPE_LORA,
    NETDEV_TYPE_NRFMIN,
    NETDEV_TYPE_NRF24L01P_NG,
    NETDEV_TYPE_SLIP,
    NETDEV_TYPE_ESP_NOW,
};
/** @} */

/**
 * @brief   Possible event types that are send from the device driver to the
 *          upper layer
 */
typedef enum {
    NETDEV_EVENT_ISR,                       /**< driver needs it's ISR handled */
    NETDEV_EVENT_RX_STARTED,                /**< started to receive a frame */
    NETDEV_EVENT_RX_COMPLETE,               /**< finished receiving a frame */
    NETDEV_EVENT_TX_STARTED,                /**< started to transfer a frame */
    NETDEV_EVENT_TX_COMPLETE,               /**< transfer frame complete */
    /**
     * @brief   transfer frame complete and data pending flag
     *
     * @deprecated  Issue an NETDEV_EVENT_TX_COMPLETE event instead and pass
     *              the data pending info in netdev_driver_t::confirm_send
     *              via the `info` parameter
     */
    NETDEV_EVENT_TX_COMPLETE_DATA_PENDING,
    /**
     * @brief   ACK requested but not received
     *
     * @deprecated  Issue an NETDEV_EVENT_TX_COMPLETE event instead and return
     *              `-ECOMM` in netdev_driver_t::confirm_send. Via the `info`
     *              parameter additional details about the error can be passed
     */
    NETDEV_EVENT_TX_NOACK,
    /**
     * @brief   couldn't transfer frame
     *
     * @deprecated  Issue an NETDEV_EVENT_TX_COMPLETE event instead and return
     *              `-EBUSY` in netdev_driver_t::confirm_send.
     */
    NETDEV_EVENT_TX_MEDIUM_BUSY,
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
 * @brief   Received frame status information for most radios
 *
 * May be different for certain radios.
 */
struct netdev_radio_rx_info {
    int16_t rssi;       /**< RSSI of a received frame in dBm */
    uint8_t lqi;        /**< LQI of a received frame */
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
 * @brief   Driver types for netdev.
 *
 * @warning New entries must be added at the bottom of the list
 *          because the values need to remain constant to
 *          generate stable L2 addresses.
 * @{
 */
typedef enum {
    NETDEV_ANY = 0,         /**< Will match any device type */
    NETDEV_AT86RF215,
    NETDEV_AT86RF2XX,
    NETDEV_CC2538,
    NETDEV_DOSE,
    NETDEV_ENC28J60,
    NETDEV_KW41ZRF,
    NETDEV_MRF24J40,
    NETDEV_NRF802154,
    NETDEV_STM32_ETH,
    NETDEV_CC110X,
    NETDEV_SX127X,
    NETDEV_SAM0_ETH,
    NETDEV_ESP_NOW,
    NETDEV_NRF24L01P_NG,
    NETDEV_SOCKET_ZEP,
    NETDEV_SX126X,
    /* add more if needed */
} netdev_type_t;
/** @} */

/**
 * @brief   Will match any device index
 */
#define NETDEV_INDEX_ANY    (0xFF)

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
    const struct netdev_driver *driver;            /**< ptr to that driver's interface. */
    netdev_event_cb_t event_callback;              /**< callback for device events */
    void *context;                                 /**< ptr to network stack context */
#ifdef MODULE_NETDEV_LAYER
    netdev_t *lower;                               /**< ptr to the lower netdev layer */
#endif
#ifdef MODULE_L2FILTER
    l2filter_t filter[CONFIG_L2FILTER_LISTSIZE];   /**< link layer address filters */
#endif
#ifdef MODULE_NETDEV_REGISTER
    netdev_type_t type;                     /**< driver type used for netdev */
    uint8_t index;                          /**< instance number of the device */
#endif
};

/**
 * @brief Register a device with netdev.
 *        Must by called by the driver's setup function.
 *
 * @param[out] dev          the new netdev
 * @param[in]  type         the driver used for the netdev
 * @param[in]  index        the index in the config struct
 */
static inline void netdev_register(struct netdev *dev, netdev_type_t type, uint8_t index)
{
#ifdef MODULE_NETDEV_REGISTER
    dev->type  = type;
    dev->index = index;
#else
    (void) dev;
    (void) type;
    (void) index;
#endif
}

/**
 * @brief Structure to hold driver interface -> function mapping
 *
 * The send/receive functions expect/return a full ethernet
 * frame (dst mac, src mac, ethertype, payload, no checksum).
 */
typedef struct netdev_driver {
    /**
     * @brief   Start transmission of the given frame and return directly
     *
     * @pre     `(dev != NULL) && (iolist != NULL)`
     *
     * @param[in]   dev     Network device descriptor. Must not be NULL.
     * @param[in]   iolist  IO vector list to send. Elements of this list may
     *                      have iolist_t::iol_size == 0 and (in this case only)
     *                      iolist_t::iol_data == 0.
     *
     * @retval  -EBUSY      Driver is temporarily unable to send, e.g. because
     *                      an incoming frame on a half-duplex medium is
     *                      received
     * @retval  -ENETDOWN   Device is powered down
     * @retval  <0          Other error
     * @retval  0           Transmission successfully started
     * @retval  >0          Number of bytes transmitted (deprecated!)
     *
     * This function will cause the driver to start the transmission in an
     * async fashion. The driver will "own" the `iolist` until a subsequent
     * call to @ref netdev_driver_t::confirm_send returns something different
     * than `-EAGAIN`. The driver must signal completion using the
     * NETDEV_EVENT_TX_COMPLETE event, regardless of success or failure.
     *
     * Old drivers might not be ported to the new API and have
     * netdev_driver_t::confirm_send set to `NULL`. In that case the driver
     * will return the number of bytes transmitted on success (instead of `0`)
     * and will likely block until completion.
     */
    int (*send)(netdev_t *dev, const iolist_t *iolist);

    /**
     * @brief   Fetch the status of a transmission and perform any potential
     *          cleanup
     *
     * @param[in]   dev     Network device descriptor. Must not be NULL.
     * @param[out]  info    Device class specific type to fetch transmission
     *                      info. May be `NULL` if not needed by upper layer.
     *                      May be ignored by driver.
     *
     * @return  Number of bytes transmitted. (The size of the transmitted
     *          frame including all overhead, such as frame check sequence,
     *          bit stuffing, escaping, headers, trailers, preambles, start of
     *          frame delimiters, etc. May be an estimate for performance
     *          reasons.)
     * @retval  -EAGAIN     Transmission still ongoing. (Call later again!)
     * @retval  -ECOMM      Any kind of transmission error, such as collision
     *                      detected, layer 2 ACK timeout, etc.
     *                      Use @p info for more details
     * @retval  -EBUSY      Medium is busy. (E.g. Auto-CCA failed / timed out)
     * @retval  <0          Other error. (Please use a negative errno code.)
     *
     * @warning After netdev_driver_t::send was called and returned zero, this
     *          function must be called until it returns anything other than
     *          `-EAGAIN`.
     * @note    The driver will signal completion using the
     *          NETDEV_EVENT_TX_COMPLETE event. This function must not return
     *          `-EAGAIN` after that event was received.
     */
    int (*confirm_send)(netdev_t *dev, void *info);

    /**
     * @brief   Drop a received frame, **OR** get the length of a received
     *          frame, **OR** get a received frame.
     *
     * @pre     `(dev != NULL)`
     *
     * Supposed to be called from
     * @ref netdev_t::event_callback "netdev->event_callback()"
     *
     * If @p buf == NULL and @p len == 0, returns the frame size -- or an upper
     * bound estimation of the size -- without dropping the frame.
     * If @p buf == NULL and @p len > 0, drops the frame and returns the frame
     * size.
     *
     * If called with @p buf != NULL and @p len is smaller than the received
     * frame:
     *  - The received frame is dropped
     *  - The content in @p buf becomes invalid. (The driver may use the memory
     *    to implement the dropping - or may not change it.)
     *  - `-ENOBUFS` is returned
     *
     * @param[in]   dev     network device descriptor. Must not be NULL.
     * @param[out]  buf     buffer to write into or NULL to return the frame
     *                      size.
     * @param[in]   len     maximum number of bytes to read. If @p buf is NULL
     *                      the currently buffered frame is dropped when
     *                      @p len > 0. Must not be 0 when @p buf != NULL.
     * @param[out]  info    status information for the received frame. Might
     *                      be of different type for different netdev devices.
     *                      May be NULL if not needed or applicable.
     *
     * @retval  -ENOBUFS    if supplied buffer is too small
     * @return  number of bytes read if buf != NULL
     * @return  frame size (or upper bound estimation) if buf == NULL
     */
    int (*recv)(netdev_t *dev, void *buf, size_t len, void *info);

    /**
     * @brief   the driver's initialization function
     *
     * @pre     `(dev != NULL)`
     *
     * @param[in]   dev     network device descriptor. Must not be NULL.
     *
     * @retval  <0      on error
     * @retval  0       on success
     */
    int (*init)(netdev_t *dev);

    /**
     * @brief   a driver's user-space ISR handler
     *
     * @pre     `(dev != NULL)`
     *
     * This function will be called from a network stack's loop when being
     * notified by netdev_isr.
     *
     * It is supposed to call
     * @ref netdev_t::event_callback "netdev->event_callback()" for each
     * occurring event.
     *
     * See receive frame flow description for details.
     *
     * @param[in]   dev     network device descriptor. Must not be NULL.
     */
    void (*isr)(netdev_t *dev);

    /**
     * @brief   Get an option value from a given network device
     *
     * @pre     `(dev != NULL)`
     * @pre     for scalar types of @ref netopt_t @p max_len must be of exactly
     *          that length (see [netopt documentation](@ref net_netopt) for
     *          type)
     * @pre     for array types of @ref netopt_t @p max_len must greater or
     *          equal the required length (see
     *          [netopt documentation](@ref net_netopt) for type)
     * @pre     @p value must have the natural alignment of its type (see
     *          [netopt documentation](@ref net_netopt) for type)
     *
     * @param[in]   dev     network device descriptor
     * @param[in]   opt     option type
     * @param[out]  value   pointer to store the option's value in
     * @param[in]   max_len maximal amount of byte that fit into @p value
     *
     * @return  number of bytes written to @p value
     * @retval  -ENOTSUP    if @p opt is not provided by the device
     */
    int (*get)(netdev_t *dev, netopt_t opt,
               void *value, size_t max_len);

    /**
     * @brief   Set an option value for a given network device
     *
     * @pre     `(dev != NULL)`
     * @pre     for scalar types of @ref netopt_t @p value_len must be of
     *          exactly that length (see [netopt documentation](@ref net_netopt)
     *          for type)
     * @pre     for array types of @ref netopt_t @p value_len must lesser or
     *          equal the required length (see
     *          [netopt documentation](@ref net_netopt) for type)
     * @pre     @p value must have the natural alignment of its type (see
     *          [netopt documentation](@ref net_netopt) for type)
     *
     * @param[in]   dev         network device descriptor
     * @param[in]   opt         option type
     * @param[in]   value       value to set
     * @param[in]   value_len   the length of @p value
     *
     * @return  number of bytes written to @p value
     * @retval  -ENOTSUP    if @p opt is not configurable for the device
     * @retval  -EINVAL     if @p value is an invalid value with regards to
     *                      @p opt
     */
    int (*set)(netdev_t *dev, netopt_t opt,
               const void *value, size_t value_len);
} netdev_driver_t;

/**
 * @brief   Convenience function for declaring get() as not supported in general
 *
 * @param[in] dev           ignored
 * @param[in] opt           ignored
 * @param[in] value         ignored
 * @param[in] max_len       ignored
 *
 * @return  always returns `-ENOTSUP`
 */
static inline int netdev_get_notsup(netdev_t *dev, netopt_t opt,
                                    void *value, size_t max_len)
{
    (void)dev;
    (void)opt;
    (void)value;
    (void)max_len;
    return -ENOTSUP;
}

/**
 * @brief   Convenience function for declaring set() as not supported in general
 *
 * @param[in] dev           ignored
 * @param[in] opt           ignored
 * @param[in] value         ignored
 * @param[in] value_len     ignored
 *
 * @return  always returns `-ENOTSUP`
 */
static inline int netdev_set_notsup(netdev_t *dev, netopt_t opt,
                                    const void *value, size_t value_len)
{
    (void)dev;
    (void)opt;
    (void)value;
    (void)value_len;
    return -ENOTSUP;
}

/**
 * @brief Informs netdev there was an interrupt request from the network device.
 *
 *        This function calls @ref netdev_t::event_callback with
 *        NETDEV_EVENT_ISR event.
 *
 * @param netdev netdev instance of the device associated to the interrupt.
 */
static inline void netdev_trigger_event_isr(netdev_t *netdev)
{
    if (netdev->event_callback) {
        netdev->event_callback(netdev, NETDEV_EVENT_ISR);
    }
}
#ifdef __cplusplus
}
#endif

#endif /* NET_NETDEV_H */
/** @} */
