/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup pkg_openthread_sokc
 * @{
 *
 * @file
 * @brief openthread-specific types
 *
 */
#ifndef SOCK_TYPES_H
#define SOCK_TYPES_H

#include "net/af.h"
#include <openthread.h>
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Raw IP sock type
 * @internal
 */
struct sock_ip {
    struct netconn *conn;
};

/**
 * @brief   TCP sock type
 * @internal
 */
struct sock_tcp {
    struct netconn *conn;
};

/**
 * @brief   TCP queue type
 */
struct sock_tcp_queue {
    struct netconn *conn;
};

/**
 * @brief   UDP sock type
 * @internal
 */
struct sock_udp {
    struct netconn *conn;
    sock_udp_ep_t local;                /**< local end-point */
    sock_udp_ep_t remote;               /**< remote end-point */
    uint16_t flags;                     /**< option flags */
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TYPES_H */
/** @} */
