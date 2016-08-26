/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_conn_addr   Address abstractions
 * @ingroup     net_conn
 * @brief       Address abstractions for usage with @ref net_conn.
 * @{
 *
 * @file
 * @brief   Address abstraction definitions for @ref net_conn.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef CONN_ADDR_H_
#define CONN_ADDR_H_

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type to abstract both IPv4 and IPv6 addresses
 */
typedef union {
#ifdef CONN_HAS_IPV6
    ipv6_addr_t ipv6;       /**< IPv6 address mode */
#endif
    ipv4_addr_t ipv4;       /**< IPv4 address mode */
} conn_addr_ip_t;

#ifdef __cplusplus
}
#endif

#endif /* CONN_ADDR_H_ */
/** @} */
