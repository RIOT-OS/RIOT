/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_netdev   Network device driver interface
 * @ingroup     net
 * @{
 *
 * @file
 * @brief       Network device driver interface definition.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NG_NETDEV_H_
#define NG_NETDEV_H_

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "kernel.h"
#include "net/ng_pkt.h"
#include "net/ng_netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type for @ref msg_t if device fired an event
 */
#define NG_NETDEV_MSG_TYPE_EVENT    (0x0100)

/**
 * @brief   Possible event types that are send from the device driver to the
 *          MAC layer
 */
typedef enum {
    NETDEV_EVENT_RX_STARTED     = 0x0001,   /**< started to receive a packet */
    NETDEV_EVENT_RX_COMPLETE    = 0x0002,   /**< finished receiving a packet */
    NETDEV_EVENT_TX_STARTED     = 0x0004,   /**< started to transfer a packet */
    NETDEV_EVENT_TX_COMPLETE    = 0x0008,   /**< finished transferring packet */
    /* expand this list if needed */
} ng_netdev_event_t;

/**
 * @brief   Event callback for signaling event to a MAC layer
 *
 * @param[in] type          type of the event
 * @param[in] arg           event argument, can e.g. contain a pktsnip_t pointer
 */
typedef void (*ng_netdev_event_cb_t)(ng_netdev_event_t type, void *arg);

/**
 * @brief   Forward declaration of ng_netdev_t due to cyclic dependency to
 *          @ref ng_netdev_driver_t.
 *
 * @see ng_netdev
 */
typedef struct ng_netdev ng_netdev_t;

/**
 * @brief   Network device API definition.
 *
 * @details This is a set of functions that must be implemented by any driver
 *           for a network device.
 */
typedef struct {
    /**
     * @brief Send data via a given network device
     *
     * @param[in] dev       network device descriptor
     * @param[in] pkt       pointer to the data in the packet buffer
     *
     * @return              number of bytes that were actually send out
     * @return              -ENODEV if @p dev is invalid
     * @return              -ENOMSG if pkt is invalid
     * @return              -EOVERFLOW if the payload size of @p pkt exceeds the
     *                      payload size that can be handled by the device
     */
    int (*send_data)(ng_netdev_t *dev, ng_pktsnip_t *pkt);

    /**
     * @brief   Registers an event callback to a given network device
     *
     * @param[in] dev       network device descriptor
     * @param[in] cb        function that is called on events
     *
     * @return              0 on success
     * @return              -ENODEV if @p dev is invalid
     * @return              -ENOBUFS if maximum number of callbacks is exceeded
     */
    int (*add_event_callback)(ng_netdev_t *dev, ng_netdev_event_cb_t cb);

    /**
     * @brief   Unregisters an event callback from a given network device
     *
     * @param[in] dev       network device descriptor
     * @param[in] cb        callback function
     *
     * @return              0 on success
     * @return              -ENODEV if @p dev is invalid
     * @return              -ENOENT if callback was not registered
     */
    int (*rem_event_callback)(ng_netdev_t *dev, ng_netdev_event_cb_t cb);

    /**
     * @brief   Get an option value from a given network device
     *
     * @param[in] dev           network device descriptor
     * @param[in] opt           option type
     * @param[out] value        pointer to store the option's value in
     * @param[in] max_len       maximal amount of byte that fit into @p value
     *
     * @return              number of bytes written to @p value
     * @return              -ENODEV if @p dev is invalid
     * @return              -ENOTSUP if @p opt is not supported
     * @return              -EOVERFLOW if available space in @p value given in
     *                      @p max_len is too small to store the option value
     * @return              -ECANCELED if internal driver error occurred
     */
    int (*get)(ng_netdev_t *dev, ng_netconf_opt_t opt,
               void *value, size_t max_len);

    /**
     * @brief   Set an option value for a given network device
     *
     * @param[in] dev       network device descriptor
     * @param[in] opt       option type
     * @param[in] value     value to set
     * @param[in] value_len the length of @p value
     *
     * @return              number of bytes used from @p value
     * @return              -ENODEV if @p dev is invalid
     * @return              -ENOTSUP if @p opt is not supported
     * @return              -EINVAL if @p value is invalid
     * @return              -ECANCELED if internal driver error occurred
     */
    int (*set)(ng_netdev_t *dev, ng_netconf_opt_t opt,
               void *value, size_t value_len);

    /**
     * @brief   This function is called by a MAC layer when a message of type
     *          @ref NG_NETDEV_MSG_TYPE_EVENT was received
     *
     * @param[in] dev           network device descriptor
     * @param[in] event_type    event type, given by msg_t::content::value
     *                          in the received message
     */
    void (*isr_event)(ng_netdev_t *dev, uint32_t event_type);
} ng_netdev_driver_t;

/**
 * @brief   The netdev data-structure holds the minimum information needed for
 *          interaction with MAC layers and can be expanded with device
 *          specific fields
 *
 * The netdev structure is the parent for all network device driver descriptors.
 *
 * @see ng_netdev_t
 */
struct ng_netdev {
    ng_netdev_driver_t const *driver;   /**< pointer to the devices interface */
    ng_netdev_event_cb_t event_cb;      /**< netdev event callback */
    kernel_pid_t mac_pid;               /**< the driver's thread's PID */
};

#ifdef __cplusplus
}
#endif

#endif /* NG_NETDEV_H_ */
/** @} */
