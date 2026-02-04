/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup pkg_lwip_sock
 * @{
 *
 * @file
 * @brief lwIP-specific types
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "net/af.h"
#include "lwip/api.h"
#ifdef SOCK_HAS_ASYNC
#include "net/sock/async/types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forward declaration
 * @internal
 */
typedef struct lwip_sock_base lwip_sock_base_t;

#ifdef SOCK_HAS_ASYNC
/**
 * @brief   Event callback for @ref lwip_sock_base_t
 * @internal
 */
typedef void (*lwip_sock_cb_t)(lwip_sock_base_t *sock,
                               sock_async_flags_t flags,
                               void *arg);
#endif  /* SOCK_HAS_ASYNC */

/**
 * @brief   Sock base type
 * @warning For network stack internal purposes only. Do not access members
 *          externally.
 * @internal
 */
struct lwip_sock_base {
    struct netconn *conn;           /**< lwIP network connection object */
#ifdef SOCK_HAS_ASYNC
    /**
     * @brief   Asynchronous upper layer callback
     *
     * @note    All have void return value and a (sock pointer, sock_async_flags_t)
     *          pair, so casting between these function pointers is okay.
     */
    union {
        lwip_sock_cb_t gen;         /**< generic version */
#ifdef MODULE_SOCK_IP
        sock_ip_cb_t ip;            /**< IP version */
#endif
#ifdef MODULE_SOCK_TCP
        sock_tcp_cb_t tcp;          /**< TCP version */
        /**
         * @brief   TCP queue version
         */
        sock_tcp_queue_cb_t tcp_queue;
#endif
#ifdef MODULE_SOCK_UDP
        sock_udp_cb_t udp;          /**< UDP version */
#endif
    } async_cb;
    void *async_cb_arg;             /**< asynchronous callback argument */
#ifdef SOCK_HAS_ASYNC_CTX
    sock_async_ctx_t async_ctx;     /**< asynchronous event context */
#endif  /* SOCK_HAS_ASYNC_CTX */
#endif  /* SOCK_HAS_ASYNC */
};

/**
 * @brief   Raw IP sock type
 * @warning For network stack internal purposes only. Do not access members
 *          externally.
 * @internal
 */
struct sock_ip {
    lwip_sock_base_t base;          /**< parent class */
};

/**
 * @brief   TCP sock type
 * @warning For network stack internal purposes only. Do not access members
 *          externally.
 * @internal
 */
struct sock_tcp {
    lwip_sock_base_t base;          /**< parent class */
    struct sock_tcp_queue *queue;   /**< Queue the sock might have been generated from */
    mutex_t mutex;                  /**< Mutex to protect the sock */
    struct pbuf *last_buf;          /**< Last received data */
    ssize_t last_offset;            /**< Offset in struct sock_tcp::last_buf since last read */
};

/**
 * @brief   TCP queue type
 * @warning For network stack internal purposes only. Do not access members
 *          externally.
 */
struct sock_tcp_queue {
    lwip_sock_base_t base;          /**< parent class */
    struct sock_tcp *array;         /**< Allocation array for sock objects to generate */
    mutex_t mutex;                  /**< Mutex to protect the queue */
    unsigned short len;             /**< Length of the struct sock_tcp_queue::array */
    unsigned short used;            /**< Used entries in struct sock_tcp_queue::array */
};

/**
 * @brief   UDP sock type
 * @warning For network stack internal purposes only. Do not access members
 *          externally.
 * @internal
 */
struct sock_udp {
    lwip_sock_base_t base;          /**< parent class */
};

#ifdef __cplusplus
}
#endif

/** @} */
