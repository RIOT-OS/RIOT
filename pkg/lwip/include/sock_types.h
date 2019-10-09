/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup pkg_lwip_sock
 * @{
 *
 * @file
 * @brief lwIP-specific types
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SOCK_TYPES_H
#define SOCK_TYPES_H

#include "net/af.h"
#include "lwip/api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Raw IP sock type
 * @warning For network stack internal purposes only. Do not access members
 *          externally.
 * @internal
 */
struct sock_ip {
    struct netconn *conn;           /**< lwIP network connection object */
};

/**
 * @brief   TCP sock type
 * @warning For network stack internal purposes only. Do not access members
 *          externally.
 * @internal
 */
struct sock_tcp {
    struct netconn *conn;           /**< lwIP network connection object */
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
    struct netconn *conn;           /**< lwIP network connection object */
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
    struct netconn *conn;           /**< lwIP network connection object */
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TYPES_H */
/** @} */
