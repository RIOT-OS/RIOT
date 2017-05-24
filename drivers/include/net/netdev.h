/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Ell-i open source co-operative
 *               2015 Freie Universität Berlin
 *               2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    drivers_netdev_api Network Device Driver API
 * @ingroup     drivers_netdev
 * @brief       This is the second version of a generic low-level network
 *              driver interface
 * @{
 *
 * This interface is supposed to be a low-level interface for network drivers.
 *
 * Example call flow:
 *
 * 1. packet arrives for device
 * 2. The driver previously registered an ISR for handling received packets.
 *    This ISR then calls @ref netdev_t::event_callback "netdev->event_callback()"
 *    with `event` := @ref NETDEV_EVENT_ISR (from Interrupt Service Routine)
 *    which wakes up event handler
 * 3. event handler calls @ref netdev_driver_t::isr "netdev->driver->isr()"
 *    (from thread context)
 * 4. @ref netdev_driver_t::isr "netdev->driver->isr()" calls
 *    @ref netdev_t::event_callback "netdev->event_callback()" with
 *    `event` := @ref NETDEV_EVENT_RX_COMPLETE
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
#include <sys/uio.h>

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
    NETDEV_TYPE_CC110X,
    NETDEV_TYPE_NRFMIN
};

/**
 * @brief   Possible event types that are send from the device driver to the
 *          upper layer
 */
typedef enum {
    NETDEV_EVENT_ISR,               /**< driver needs it's ISR handled */
    NETDEV_EVENT_RX_STARTED,        /**< started to receive a packet */
    NETDEV_EVENT_RX_COMPLETE,       /**< finished receiving a packet */
    NETDEV_EVENT_TX_STARTED,        /**< started to transfer a packet */
    NETDEV_EVENT_TX_COMPLETE,       /**< finished transferring packet */
    NETDEV_EVENT_TX_NOACK,          /**< ACK requested but not received */
    NETDEV_EVENT_TX_MEDIUM_BUSY,    /**< couldn't transfer packet */
    NETDEV_EVENT_LINK_UP,           /**< link established */
    NETDEV_EVENT_LINK_DOWN,         /**< link gone */
    /* expand this list if needed */
} netdev_event_t;

/**
 * @brief   Received packet status information for most radios
 *
 * May be different for certain radios.
 */
struct netdev_radio_rx_info {
    uint8_t rssi;       /**< RSSI of a received packet */
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
     * @pre `(dev != NULL)`
     * @pre `(count == 0) || (vector != NULL)`
     *      (`(count != 0) => (vector != NULL)`)
     *
     * @param[in] dev       network device descriptor
     * @param[in] vector    io vector array to send
     * @param[in] count     nr of entries in vector
     *
     * @return number of bytes sent, or `< 0` on error
     */
    int (*send)(netdev_t *dev, const struct iovec *vector, unsigned count);

    /**
     * @brief Get a received frame
     *
     * @pre `(dev != NULL)`
     * @pre `(buf != NULL) && (len > 0)`
     *
     * Supposed to be called from @ref netdev_t::event_callback().
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
     * This function will be called from a network stack's loop when being notified
     * by netdev_isr.
     *
     * It is supposed to call @ref netdev_t::event_callback() for each occuring event.
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
               void *value, size_t value_len);
} netdev_driver_t;

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NET_NETDEV_H */
