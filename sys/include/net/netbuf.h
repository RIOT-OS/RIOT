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
 * @brief   Get a network buffer with a given size.
 *
 * @note    Supposed to be implemented by the networking module.
 *
 * @param[in]       size    size of the buffer to be allocated.
 * @param[out]      ctx     allocation context.
 *
 * @return          pointer to the buffer.
 * @return          packet context in @p ctx.
 * @return          NULL on failure (e.g there's not enough memory for allocation)
 */
void *netbuf_get(size_t size, void **ctx);

/**
 * @brief Free the resources of a network buffer
 *
 * @note    Supposed to be implemented by the networking module.
 *
 * @param[in]       ctx  pointer to the allocation context.
 */
void netbuf_free(void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* NET_NETBUF_H */
/** @} */
