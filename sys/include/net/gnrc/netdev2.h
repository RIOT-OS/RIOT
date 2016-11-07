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
 * @author    Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef GNRC_NETDEV2_H
#define GNRC_NETDEV2_H

#include "kernel_types.h"
#include "net/netdev2.h"
#include "net/gnrc.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GNRC_NETDEV2_MAC_PRIO
#define GNRC_NETDEV2_MAC_PRIO   (THREAD_PRIORITY_MAIN - 5)
#endif

/**
 * @brief   Type for @ref msg_t if device fired an event
 */
#define NETDEV2_MSG_TYPE_EVENT 0x1234

/**
 * @brief   Queue of packets for link-layer retransmissions
 */
typedef struct {
    struct gnrc_pktqueue *next; /**< next node in queue */
    gnrc_pktsnip_t *pkt;        /**< pointer to the packet */
    uint8_t cnt;                /**< remaining number of max. retransmissions */
} netdev2_retrans_queue_t;

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
     * @brief   Get an option value from a given network device
     *
     * @param[in]   dev     network device descriptor
     * @param[in]   opt     option type
     * @param[out]  value   pointer to store the option's value in
     * @param[in]   max_len maximal amount of byte that fit into @p value
     *
     * @return              number of bytes written to @p value
     * @return              <0 on error
     */
    int (*get)(struct gnrc_netdev2 *dev, netopt_t opt,
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
     * @return              <0 on error
     */
    int (*set)(struct gnrc_netdev2 *dev, netopt_t opt,
               void *value, size_t value_len);

    /**
     * @brief netdev2 handle this adapter is working with
     */
    netdev2_t *dev;

    /**
     * @brief PID of this adapter for netapi messages
     */
    kernel_pid_t pid;

#ifdef MODULE_NETDEV_RETRANS
    /**
     * @brief head of queue for link-layer retransmissions
     */
    netdev2_retrans_queue_t *retrans_head;
#endif
} gnrc_netdev2_t;

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
