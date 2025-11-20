/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    pkg_lwip_sock   lwIP-specific implementation of sock API
 * @ingroup     pkg_lwip
 * @brief       Provides an implementation of the @ref net_sock for the
 *              @ref pkg_lwip
 * @internal
 * @{
 *
 * @file
 * @brief       lwIP-specific function @ref net_sock definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdbool.h>
#include <stdint.h>

#include "net/af.h"
#include "net/sock.h"

#include "lwip/ip_addr.h"
#include "lwip/api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configures @ref sock_tcp_accept() timeout in milliseconds
 *          (0 by default, which means no timeout)
 */
#ifndef LWIP_SOCK_TCP_ACCEPT_TIMEOUT
#define LWIP_SOCK_TCP_ACCEPT_TIMEOUT    (0)
#endif

/**
 * @brief Internal helper functions for lwIP
 * @internal
 * @{
 */
int lwip_sock_create(struct netconn **conn, const struct _sock_tl_ep *local,
                      const struct _sock_tl_ep *remote, int proto,
                      uint16_t flags, int type);
uint16_t lwip_sock_bind_addr_to_netif(const ip_addr_t *bind_addr);
int lwip_sock_get_addr(struct netconn *conn, struct _sock_tl_ep *ep, u8_t local);
#if defined(MODULE_LWIP_SOCK_UDP) || defined(MODULE_LWIP_SOCK_IP)
int lwip_sock_recv(struct netconn *conn, uint32_t timeout, struct netbuf **buf);
#endif
ssize_t lwip_sock_sendv(struct netconn *conn, const iolist_t *snips,
                        int proto, const struct _sock_tl_ep *remote, int type);
static inline ssize_t lwip_sock_send(struct netconn *conn,
                                     const void *data, size_t len,
                                     int proto, const struct _sock_tl_ep *remote, int type)
{
    iolist_t snip = {
        .iol_base = (void *)data,
        .iol_len  = len,
    };

    return lwip_sock_sendv(conn, &snip, proto, remote, type);
}
/** @internal
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @internal
 * @} */
