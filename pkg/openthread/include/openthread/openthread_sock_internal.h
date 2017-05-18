/*
 * Copyright (C) 2017 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_openthread_sock   OPENTHREAD-specific implementation of the sock API
 * @ingroup     net_openthread
 * @brief       Provides an implementation of the @ref net_sock by the
 *              @ref net_openthread
 *
 * @{
 *
 * @file
 * @brief   OPENTHREAD-specific types and function definitions
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */
#ifndef OPENTHREAD_SOCK_INTERNAL_H
#define OPENTHREAD_SOCK_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include "mbox.h"
#include "net/af.h"
#include "net/openthread.h"
#include "net/openthread/netreg.h"
#include "net/iana/portrange.h"

#include "sock_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Minimum port number in the dynamic portrange
 */
#define OPENTHREAD_SOCK_DYN_PORTRANGE_MIN (IANA_DYNAMIC_PORTRANGE_MIN)
/**
 * @brief   Maximum port number in the dynamic portrange
 */
#define OPENTHREAD_SOCK_DYN_PORTRANGE_MAX (IANA_DYNAMIC_PORTRANGE_MAX)

/**
 * @brief   Available ports in the range for dynamic source port allocation
 */
#define OPENTHREAD_SOCK_DYN_PORTRANGE_NUM (OPENTHREAD_SOCK_DYN_PORTRANGE_MAX - OPENTHREAD_SOCK_DYN_PORTRANGE_MIN + 1)

/**
 * @brief   Error value indicating that no free port could be found in the
 *          dynamic port range
 */
#define OPENTHREAD_SOCK_DYN_PORTRANGE_ERR (0)

/**
 * @brief   Offset for next dynamic port
 *
 * Currently set to a static (prime) offset, but could be random, too
 * see https://tools.ietf.org/html/rfc6056#section-3.3.3
 */
#define OPENTHREAD_SOCK_DYN_PORTRANGE_OFF (17U)

/**
 * @brief   Internal helper functions for OPENTHREAD
 * @internal
 * @{
 */
/**
 * @brief   Checks if address family is not supported
 * @internal
 */
static inline bool openthread_af_not_supported(int af)
{
    /* TODO: add AF_INET support */
    return (af != AF_INET6);
}

/**
 * @brief   Check if end point points to any address
 * @internal
 */
static inline bool openthread_ep_addr_any(const sock_ip_ep_t *ep)
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
void openthread_sock_create(openthread_sock_reg_t *reg, openthread_nettype_t type, uint32_t demux_ctx);

/**
 * @brief   Receive a packet internally
 * @internal
 */
ssize_t openthread_sock_recv(openthread_sock_reg_t *reg, openthread_pktsnip_t **pkt, uint32_t timeout,
                       sock_ip_ep_t *remote);

/**
 * @brief   Send a packet internally
 * @internal
 */
ssize_t openthread_sock_send(openthread_pktsnip_t *payload, sock_ip_ep_t *local,
                       const sock_ip_ep_t *remote, uint8_t nh);
/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* OPENTHREAD_SOCK_INTERNAL_H */
/** @} */
