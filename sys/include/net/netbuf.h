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
#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Netbuf descriptor
 */
typedef struct {
    void *data;       /**< pointer to the data */
    size_t size;      /**< size of the buffer */
    void *ctx;        /**< context of the buffer */
} netbuf_t;

/**
 * @brief Set to 1 if the stack doesn't provide an allocation mechanism
 *        (see @ref netbuf_stack_alloc)
 */
#ifndef CONFIG_NETBUF_DISABLE_STACK_ALLOC
#define CONFIG_NETBUF_DISABLE_STACK_ALLOC (0)
#endif

/**
 * @brief Allocate a buffer from the network stack.
 *
 *        This function should populate at least `netbuf->data` and
 *        `netbuf->size`. If needed, `netbuf->ctx` should contain the context
 *        for freeing the buffer.
 *
 * @note  This function MUST be implemented by the network stack if
 *        CONFIG_NETBUF_DISABLE_STACK_ALLOC == 0. If
 *        CONFIG_NETBUF_DISABLE_STACK_ALLOC == 1, this function will be
 *        defined as a function that always returns NULL.
 *
 * @param[out]  netbuf  pointer to netbuf
 * @param[in]   size    size of the pkt to be allocated
 *
 * @return      pointer to the allocated buffer
 * @return      NULL if packet couldn't be allocated
 */
void *netbuf_stack_alloc(netbuf_t *netbuf, size_t size);

/**
 * @brief Initialize a netbuf with a given buffer and size.
 *
 * @param[out]  netbuf  pointer to the netbuf
 * @param[in]   data    pointer to the buffer. It can be NULL if
 *                      @ref netbuf_alloc should get the buffer from the stack
 * @param[in]   max_len Maximum length of the buffer. If `data` is NULL, this
 *                      value is not used.
 */
void netbuf_init(netbuf_t *netbuf, void *data, size_t max_len);

/**
 * @brief Allocate a packet with a given size
 *
 *        Depending on the value of `netbuf->data`, this function will either
 *        return a pointer to the currently existing buffer in `netbuf` or it
 *        will try to allocate a packet from the network stack using
 *        @ref netbuf_stack_alloc.
 *
 * @note  The netbuf MUST be initialized before calling this function
 *        (see @ref netbuf_init).
 * @note  The behavior of calling this function twice for the same `netbuf`
 *        is undefined.
 *
 * @param[in, out]  netbuf  pointer to the netbuf
 * @param[in]       size    size of the buffer to be allocated
 *
 * @return          `netbuf->data` if `netbuf->data` != NULL && `size` <= `netbuf->size`
 * @return          the result of @ref netbuf_stack_alloc if `netbuf->data` == NULL
 * @return          NULL otherwise.
 */
void *netbuf_alloc(netbuf_t *netbuf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* NET_NETBUF_H */
/** @} */
