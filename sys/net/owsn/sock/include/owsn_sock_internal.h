/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_owsn_sock   OpenWSN-specific implementation of the sock API
 * @ingroup     net
 * @brief       Provides an implementation of the @ref net_sock for OpenWSN
 *
 * @{
 *
 * @file
 * @brief   OpenWSN-specific types and function definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef OWSN_SOCK_INTERNAL_H
#define OWSN_SOCK_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "mbox.h"
#include "net/af.h"
#include "net/sock/ip.h"

#include "opendefs.h"
#include "cross-layers/openqueue.h"

#include "sock_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Internal helper functions for OpenWSN
 * @internal
 * @{
 */
/**
 * @brief   Checks if address family is not supported
 * @internal
 */
static inline bool owsn_af_not_supported(int af)
{
    /* TODO: add AF_INET support */
    return (af != AF_INET6);
}

/**
 * @brief   Check if end point points to any address
 * @internal
 */
static inline bool owsn_ep_addr_any(const sock_ip_ep_t *ep)
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
 * @brief   Initializes a sock end-point from a given. Unlike GNRC, does *not*
 *          set interface if only one is present.
 * @internal
 */
static inline void owsn_ep_set(sock_ip_ep_t *out, const sock_ip_ep_t *in,
                               size_t in_size)
{
    memcpy(out, in, in_size);
}

/**
 * @brief   Create a sock internally
 * @internal
 */
void owsn_sock_create(uint32_t demux_ctx);

/**
 * @brief   Receive a packet internally
 * @internal
 */
ssize_t owsn_sock_recv(uint32_t timeout, OpenQueueEntry_t **pkt,
                       sock_ip_ep_t *remote);

/**
 * @brief   Send a packet internally
 * @internal
 */
ssize_t owsn_sock_send(OpenQueueEntry_t *pkt);
/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* OWSN_SOCK_INTERNAL_H */
/** @} */
