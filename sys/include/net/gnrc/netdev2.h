/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  net_gnrc_netdev2   Adaption layer for GNRC on top of Netdev2
 * @ingroup   net_gnrc
 * @brief     Provides the glue code for @ref net_gnrc on top of @ref drivers_netdev_netdev2
 * @{
 *
 * @file
 * @brief     netdev2-GNRC glue code interface
 *
 * This interface is supposed to provide common adaption code between the
 * low-level network device interface "netdev2" and the GNRC network stack.
 *
 * GNRC sends around "gnrc_pktsnip_t" structures, but netdev can only handle
 * "struct iovec" structures when sending, or a flat buffer when receiving.
 *
 * The purpose of gnrc_netdev is to bring these two interfaces together.
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef GNRC_NETDEV2_H
#define GNRC_NETDEV2_H

#include <assert.h>
#include <stdint.h>

#include "kernel_types.h"
#include "net/netdev2.h"
#include "net/gnrc.h"
#include "net/gnrc/mac/types.h"
#include "net/ieee802154.h"
#include "net/gnrc/mac/mac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default priority for adaption layer's threads
 */
#ifndef GNRC_NETDEV2_MAC_PRIO
#define GNRC_NETDEV2_MAC_PRIO   (THREAD_PRIORITY_MAIN - 5)
#endif

/**
 * @brief   Type for @ref msg_t if device fired an event
 */
#define NETDEV2_MSG_TYPE_EVENT 0x1234

/**
 * @brief   Mask for @ref gnrc_mac_tx_feedback_t
 */
#define GNRC_NETDEV2_MAC_INFO_TX_FEEDBACK_MASK  (0x0003U)

/**
 * @brief   Flag to track if a transmission might have corrupted a received
 *          packet
 */
#define GNRC_NETDEV2_MAC_INFO_RX_STARTED        (0x0004U)

/**
 * @brief Structure holding GNRC netdev2 adapter state
 *
 * This structure is supposed to hold any state parameters needed
 * to use a netdev2 device from GNRC.
 *
 * It can be extended
 */
typedef struct gnrc_netdev2 {
    /**
     * @brief Send a pktsnip using this device
     *
     * This function should convert the pktsnip into a format
     * the underlying device understands and send it.
     */
    int (*send)(struct gnrc_netdev2 *dev, gnrc_pktsnip_t *snip);

    /**
     * @brief Receive a pktsnip from this device
     *
     * This function should receive a raw frame from the underlying
     * device and convert it into a pktsnip while adding a netif header
     * and possibly marking out higher-layer headers.
     */
    gnrc_pktsnip_t * (*recv)(struct gnrc_netdev2 *dev);

    /**
     * @brief netdev2 handle this adapter is working with
     */
    netdev2_t *dev;

    /**
     * @brief PID of this adapter for netapi messages
     */
    kernel_pid_t pid;

#ifdef MODULE_GNRC_MAC
    /**
     * @brief general information for the MAC protocol
     */
    uint16_t mac_info;

    /**
     * @brief device's l2 address
     */
    uint8_t  l2_addr[IEEE802154_LONG_ADDRESS_LEN];

    /**
     * @brief device's l2 address length
     */
    uint8_t  l2_addr_len;

#if ((GNRC_MAC_RX_QUEUE_SIZE != 0) || (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || defined(DOXYGEN)
    /**
     * @brief MAC internal object which stores reception parameters, queues, and
     *        state machines.
     */
    gnrc_mac_rx_t rx;
#endif /* ((GNRC_MAC_RX_QUEUE_SIZE != 0) || (GNRC_MAC_DISPATCH_BUFFER_SIZE != 0)) || defined(DOXYGEN) */

#if ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_NEIGHBOR_COUNT != 0)) || defined(DOXYGEN)
    /**
     * @brief MAC internal object which stores transmission parameters, queues, and
     *        state machines.
     */
    gnrc_mac_tx_t tx;
#endif /* ((GNRC_MAC_TX_QUEUE_SIZE != 0) || (GNRC_MAC_NEIGHBOR_COUNT == 0)) || defined(DOXYGEN) */
#endif /* MODULE_GNRC_MAC */
} gnrc_netdev2_t;

#ifdef MODULE_GNRC_MAC

/**
 * @brief get the 'rx_started' state of the device
 *
 * This function checks whether the device has started receiving a packet.
 *
 * @param[in] dev  ptr to netdev2 device
 *
 * @return         the rx_started state
 */
static inline bool gnrc_netdev2_get_rx_started(gnrc_netdev2_t *dev)
{
    return (dev->mac_info & GNRC_NETDEV2_MAC_INFO_RX_STARTED);
}

/**
 * @brief set the rx_started state of the device
 *
 * This function is intended to be called only in netdev2_t::event_callback().
 *
 * @param[in] dev  ptr to netdev2 device
 *
 */
static inline void gnrc_netdev2_set_rx_started(gnrc_netdev2_t *dev, bool rx_started)
{
    if (rx_started) {
        dev->mac_info |= GNRC_NETDEV2_MAC_INFO_RX_STARTED;
    }
    else {
        dev->mac_info &= ~GNRC_NETDEV2_MAC_INFO_RX_STARTED;
    }
}

/**
 * @brief get the transmission feedback of the device
 *
 * @param[in] dev  ptr to netdev2 device
 *
 * @return         the transmission feedback
 */
static inline gnrc_mac_tx_feedback_t gnrc_netdev2_get_tx_feedback(gnrc_netdev2_t *dev)
{
    return (gnrc_mac_tx_feedback_t)(dev->mac_info &
                                    GNRC_NETDEV2_MAC_INFO_TX_FEEDBACK_MASK);
}

/**
 * @brief set the transmission feedback of the device
 *
 * This function is intended to be called only in netdev2_t::event_callback().
 *
 * @param[in] dev  ptr to netdev2 device
 *
 */
static inline void gnrc_netdev2_set_tx_feedback(gnrc_netdev2_t *dev,
                                  gnrc_mac_tx_feedback_t txf)
{
    /* check if gnrc_mac_tx_feedback does not collide with
     * GNRC_NETDEV2_MAC_INFO_RX_STARTED */
    assert(!(txf & GNRC_NETDEV2_MAC_INFO_RX_STARTED));
    /* unset previous value */
    dev->mac_info &= ~GNRC_NETDEV2_MAC_INFO_TX_FEEDBACK_MASK;
    dev->mac_info |= (uint16_t)(txf & GNRC_NETDEV2_MAC_INFO_TX_FEEDBACK_MASK);
}
#endif

/**
 * @brief Initialize GNRC netdev2 handler thread
 *
 * @param[in] stack         ptr to preallocated stack buffer
 * @param[in] stacksize     size of stack buffer
 * @param[in] priority      priority of thread
 * @param[in] name          name of thread
 * @param[in] gnrc_netdev2  ptr to netdev2 device to handle in created thread
 *
 * @return pid of created thread
 * @return KERNEL_PID_UNDEF on error
 */
kernel_pid_t gnrc_netdev2_init(char *stack, int stacksize, char priority,
                               const char *name, gnrc_netdev2_t *gnrc_netdev2);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETDEV2_H */
/** @} */
