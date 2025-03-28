/*
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef NET_NETDEV_LAYER_H
#define NET_NETDEV_LAYER_H

/**
 * @ingroup     drivers_netdev_api
 * @{
 *
 * @file
 * @brief       Netdev layer helper functions.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netdev.h"
#include "assert.h"

/**
 * @brief   Add a netdev layer to the netdev layer stack
 *
 * @param[in] top     Last netdev device added to the stack.
 * @param[in] dev     New layer to push to the top of the stack.
 *
 * @return  The new top netdev layer of the netdev stack.
 */
static inline netdev_t *netdev_add_layer(netdev_t *top, netdev_t *dev)
{
    assert(top);
    assert(dev);
    dev->lower = top;
    top->context = dev;
    return dev;
}

/**
 * @brief   Passthrough init function.
 *
 * See also @ref netdev_driver for the extended description of this functions
 * behaviour
 *
 * @param[in]   dev     network device descriptor
 * @return              `< 0` on error, 0 on success
 */
int netdev_init_pass(netdev_t *dev);

/**
 * @brief   Passthrough isr function.
 *
 * See also @ref netdev_driver for the extended description of this functions
 * behaviour
 *
 * @param[in]   dev     network device descriptor
 */
void netdev_isr_pass(netdev_t *dev);

/**
 * @brief   Passthrough send function.
 *
 * See also @ref netdev_driver for the extended description of this functions
 * behaviour
 *
 * @param[in] dev       network device descriptor
 * @param[in] iolist    io vector list to send
 *
 * @return              number of bytes sent, or `< 0` on error
 */
int netdev_send_pass(netdev_t *dev, const iolist_t *iolist);

/**
 * @brief   Passthrough recv function.
 *
 * See also @ref netdev_driver for the extended description of this functions
 * behaviour
 *
 * @param[in]   dev     network device descriptor
 * @param[out]  buf     buffer to write into or NULL
 * @param[in]   len     maximum number of bytes to read
 * @param[out] info     status information for the received packet. Might
 *                      be of different type for different netdev devices.
 *                      May be NULL if not needed or applicable.
 *
 * @return              `< 0` on error
 * @return              number of bytes read if buf != NULL
 * @return              packet size if buf == NULL
 */
int netdev_recv_pass(netdev_t *dev, void *buf, size_t len, void *info);

/**
 * @brief   Passthrough get function.
 *
 * See also @ref netdev_driver for the extended description of this functions
 * behaviour
 *
 * @param[in]   dev     network device descriptor
 * @param[in]   opt     option type
 * @param[out]  value   pointer to store the option's value in
 * @param[in]   max_len maximal amount of byte that fit into @p value
 *
 * @return              number of bytes written to @p value
 * @return              `< 0` on error, 0 on success
 */
int netdev_get_pass(netdev_t *dev, netopt_t opt, void *value, size_t max_len);

/**
 * @brief   Passthrough set function.
 *
 * See also @ref netdev_driver for the extended description of this functions
 * behaviour
 *
 * @param[in] dev       network device descriptor
 * @param[in] opt       option type
 * @param[in] value     value to set
 * @param[in] value_len the length of @p value
 *
 * @return              number of bytes used from @p value
 * @return              `< 0` on error, 0 on success
 */
int netdev_set_pass(netdev_t *dev, netopt_t opt, const void *value, size_t value_len);

/**
 * @brief   Passthrough event callback function.
 *
 * @param[in] dev       network device descriptor
 * @param[in] event     type of the event
 */
void netdev_event_cb_pass(netdev_t *dev, netdev_event_t event);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_NETDEV_LAYER_H */
