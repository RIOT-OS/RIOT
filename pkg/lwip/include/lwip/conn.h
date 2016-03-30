/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_lwip_conn   Connection type definitions for lwIP
 * @ingroup     pkg_lwip
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef LWIP_CONN_H_
#define LWIP_CONN_H_

#include "lwip/api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generic @ref net_conn object for lwIP (used internally)
 */
struct conn {
    struct netconn *lwip_conn;  /**< stack-internal connection object */
};

/**
 * @brief   @ref net_conn_ip definition for lwIP
 */
struct conn_ip {
    struct netconn *lwip_conn;  /**< stack-internal connection object */
};

/**
 * @brief   @ref net_conn_udp definition for lwIP
 */
struct conn_udp {
    struct netconn *lwip_conn;  /**< stack-internal connection object */
};

/**
 * @brief Internal consolidation functions
 * @{
 */
/**
 * @brief   consolidation function for @ref conn_ip_create() and @ref
 *          conn_udp_create()
 *
 * @internal
 */
int lwip_conn_create(struct netconn **netconn, const void *addr, size_t addr_len,
                     int family, int type, int proto, uint16_t port);

/**
 * @brief   consolidation function for @ref conn_ip_getlocaladdr() and @ref
 *          conn_udp_getlocaladdr()
 *
 * @internal
 */
int lwip_conn_getlocaladdr(struct netconn *netconn, void *addr, uint16_t *port);

/**
 * @brief   consolidation function for @ref conn_ip_recvfrom() and @ref
 *          conn_udp_recvfrom()
 *
 * @internal
 */
int lwip_conn_recvfrom(struct netconn *netconn, void *data, size_t max_len,
                       void *addr, size_t *addr_len, uint16_t *port);

/**
 * @brief   consolidation function for @ref conn_ip_sendto() and @ref
 *          conn_udp_sendto()
 *
 * @internal
 */
int lwip_conn_sendto(struct netconn *netconn, const void *data, size_t len,
                     const void *addr, size_t addr_len, uint16_t port);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_CONN_H_ */
/** @} */
