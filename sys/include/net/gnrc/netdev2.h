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
 * @brief     Provides the glue code for @ref gnrc on top of @ref drivers_netdev_netdev2
 * @{
 *
 * @file
 * @brief     netdev2 gnrc glue code interface
 *
 * This interface is supposed to provide common adaption code between the
 * low-level network device interface "netdev2" and the gnrc network stack.
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

#include "kernel_types.h"
#include "net/netdev2.h"
#include "net/gnrc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NETDEV2_MSG_TYPE_EVENT 0x1234

/**
 * @brief Structure holding gnrc netdev2 adapter state
 *
 * This structure is supposed to hold any state parameters needed
 * to use a netdev2 device from gnrc.
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
} gnrc_netdev2_t;

/**
 * @brief Initialize gnrc netdev2 handler thread
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
