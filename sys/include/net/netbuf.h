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
#include "iolist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Netbuf descriptor
 */
typedef void netbuf_ctx_t;

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
netbuf_ctx_t *netbuf_alloc(size_t size, void **buf);

/**
 * @brief Free the resources of a network buffer
 *
 * @note    Supposed to be implemented by the networking module.
 *
 * @param[in]       netbuf  pointer to the netbuf descriptor.
 */
void netbuf_free(netbuf_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* NET_NETBUF_H */
/** @} */
