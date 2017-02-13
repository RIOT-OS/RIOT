/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sock   GNRC-specific implementation of the sock API
 * @ingroup     net_gnrc
 * @brief       Provides an implementation of the @ref net_sock by the
 *              @ref net_gnrc
 *
 * @{
 *
 * @file
 * @brief   GNRC-specific types and function definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_SOCK_INTERNAL_H
#define GNRC_SOCK_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include "mbox.h"
#include "net/af.h"
#include "net/gnrc.h"
#include "net/gnrc/netreg.h"
#include "net/iana/portrange.h"
#include "net/sock/ip.h"

#include "sock_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default port range for dynamic source port allocation
 */
#define GNRC_SOCK_DYN_PORTRANGE_MIN (IANA_DYNAMIC_PORTRANGE_MIN)
#define GNRC_SOCK_DYN_PORTRANGE_MAX (IANA_DYNAMIC_PORTRANGE_MAX)
#define GNRC_SOCK_DYN_PORTRANGE_NUM (GNRC_SOCK_DYN_PORTRANGE_MAX - GNRC_SOCK_DYN_PORTRANGE_MIN + 1)
#define GNRC_SOCK_DYN_PORTRANGE_ERR (0)

/**
 * @brief   Offset for next dynamic port
 *
 * Currently set to a static (prime) offset, but could be random, too
 * see https://tools.ietf.org/html/rfc6056#section-3.3.3
 */
#define GNRC_SOCK_DYN_PORTRANGE_OFF (17U)

/**
 * @brief   Internal helper functions for GNRC
 * @internal
 * @{
 */
/**
 * @brief   Checks if address family is not supported
 * @internal
 */
static inline bool gnrc_af_not_supported(int af)
{
    /* TODO: add AF_INET support */
    return (af != AF_INET6);
}

/**
 * @brief   Check if end point points to any address
 * @internal
 */
static inline bool gnrc_ep_addr_any(const sock_ip_ep_t *ep)
{
    assert(ep != NULL);
    const uint8_t *p = (uint8_t *)&ep->addr;
    for (uint8_t i = 0; i < sizeof(ep->addr); i++) {
        if (p[i] != 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief   Create a sock internally
 * @internal
 */
void gnrc_sock_create(gnrc_sock_reg_t *reg, gnrc_nettype_t type, uint32_t demux_ctx);

/**
 * @brief   Receive a packet internally
 * @internal
 */
ssize_t gnrc_sock_recv(gnrc_sock_reg_t *reg, gnrc_pktsnip_t **pkt, uint32_t timeout,
                       sock_ip_ep_t *remote);

/**
 * @brief   Send a packet internally
 * @internal
 */
ssize_t gnrc_sock_send(gnrc_pktsnip_t *payload, sock_ip_ep_t *local,
                       const sock_ip_ep_t *remote, uint8_t nh);
/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* GNRC_SOCK_INTERNAL_H */
/** @} */
