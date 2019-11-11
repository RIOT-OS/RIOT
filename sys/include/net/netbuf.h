/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netbuf Network buffer abstraction
 * @ingroup     net
 * @brief       Provides an abstraction for network stack buffers
 * @{
 *
 * @file
 * @brief       Network buffer abstraction definition
 *
 * @author  Jose I. Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef NET_NETBUF_H
#define NET_NETBUF_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Netbuf descriptor
 */
typedef void netbuf_t;

/**
 * @brief   Allocate a network buffer with a given size.
 *
 * @note    Supposed to be implemented by the networking module.
 *
 * @param[in]       size    size of the buffer to be allocated.
 *
 * @return          pointer to the netbuf with the allocated buffer.
 * @return          NULL if there's not enough memory for allocation.
 */
netbuf_t *netbuf_alloc(size_t size);

/**
 * @brief   Get the @ref iolist_t representation of the network buffer.
 *
 * @pre     @p netbuf not NULL.
 * @pre     @p iol not NULL.
 *
 * @note    Supposed to be implemented by the networking module.
 *
 * @param[in]       netbuf  pointer to the netbuf.
 * @param[out]      iol     iolist representation of the netbuf.
 *
 * @return          @ref iolist_t representation on @p iol.
 */
void netbuf_get_iolist(netbuf_t *netbuf, iolist_t *iol);

/**
 * @brief Free the resources of a network buffer
 *
 * @note    Supposed to be implemented by the networking module.
 *
 * @param[in]       netbuf  pointer to the netbuf descriptor.
 */
void netbuf_free(netbuf_t *netbuf);

#ifdef __cplusplus
}
#endif

#endif /* NET_NETBUF_H */
/** @} */
