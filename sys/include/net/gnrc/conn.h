/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_conn   GNRC-specific implementation of the connection API
 * @ingroup     net_gnrc
 * @brief       Provides an implementation of the @ref net_conn by the
 *              @ref net_gnrc
 *
 * @{
 *
 * @file
 * @brief   GNRC-specific types and function definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_CONN_H
#define GNRC_CONN_H

#include <stdbool.h>
#include <stdint.h>
#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Connection base class
 * @internal
 */
typedef struct {
    gnrc_nettype_t l3_type;                     /**< Network layer type of the connection */
    gnrc_nettype_t l4_type;                     /**< Transport layer type of the connection */
    gnrc_netreg_entry_t netreg_entry;           /**< @p net_ng_netreg entry for the connection */
} conn_t;

/**
 * @brief   Raw connection type
 * @internal
 * @extends conn_t
 */
struct conn_ip {
    gnrc_nettype_t l3_type;                     /**< Network layer type of the connection. */
    gnrc_nettype_t l4_type;                     /**< Transport layer type of the connection.
                                                 *   Always GNRC_NETTYPE_UNDEF */
    gnrc_netreg_entry_t netreg_entry;           /**< @p net_ng_netreg entry for the connection */
    uint8_t local_addr[sizeof(ipv6_addr_t)];    /**< local IP address */
    size_t local_addr_len;                      /**< length of struct conn_ip::local_addr */
};

/**
 * @brief   UDP connection type
 * @internal
 * @extends conn_t
 */
struct conn_udp {
    gnrc_nettype_t l3_type;                     /**< Network layer type of the connection.
                                                 *   Always GNRC_NETTYPE_IPV6 */
    gnrc_nettype_t l4_type;                     /**< Transport layer type of the connection.
                                                 *   Always GNRC_NETTYPE_UDP */
    gnrc_netreg_entry_t netreg_entry;           /**< @p net_ng_netreg entry for the connection */
    uint8_t local_addr[sizeof(ipv6_addr_t)];    /**< local IP address */
    size_t local_addr_len;                      /**< length of struct conn_ip::local_addr */
};

/**
 * @brief  Bind connection to demux context
 *
 * @internal
 *
 * @param[out] entry    @ref net_ng_netreg entry.
 * @param[in] type      @ref net_ng_nettype.
 * @param[in] demux_ctx demux context (port or proto) for the connection.
 */
static inline void gnrc_conn_reg(gnrc_netreg_entry_t *entry, gnrc_nettype_t type,
                                 uint32_t demux_ctx)
{
    gnrc_netreg_entry_init_pid(entry, demux_ctx, sched_active_pid);
    gnrc_netreg_register(type, entry);
}

/**
 * @brief   Sets local address for a connection
 *
 * @internal
 *
 * @param[out] conn_addr    Pointer to the local address on the connection.
 * @param[in] addr          An IPv6 address.
 *
 * @return  true, if @p addr was a legal address (`::`, `::1` or an address assigned to any
 *          interface of this node) for the connection.
 * @return  false if @p addr was not a legal address for the connection.
 */
bool gnrc_conn6_set_local_addr(uint8_t *conn_addr, const ipv6_addr_t *addr);

/**
 * @brief   Generic recvfrom
 *
 * @internal
 *
 * @param[in] conn      Connection object.
 * @param[out] data     Pointer where the received data should be stored.
 * @param[in] max_len   Maximum space available at @p data.
 * @param[out] addr     NULL pointer or the sender's IP address. Must fit address of connection's
 *                      family if not NULL.
 * @param[out] addr_len Length of @p addr. May be NULL if @p addr is NULL.
 * @param[out] port     NULL pointer or the sender's port.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  -ENOMEM, if received data was more than max_len.
 * @returne -ETIMEDOUT, if more than 3 IPC messages were not @ref net_ng_netapi receive commands
 *          with the required headers in the packet
 */
int gnrc_conn_recvfrom(conn_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len,
                       uint16_t *port);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_CONN_H */
/** @} */
