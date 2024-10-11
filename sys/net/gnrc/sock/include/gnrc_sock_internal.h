/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_sock
 *
 * @{
 *
 * @file
 * @brief   Internal GNRC-specific types and function definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_SOCK_INTERNAL_H
#define GNRC_SOCK_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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
 * @brief   Minimum port number in the dynamic portrange
 */
#define GNRC_SOCK_DYN_PORTRANGE_MIN (IANA_DYNAMIC_PORTRANGE_MIN)
/**
 * @brief   Maximum port number in the dynamic portrange
 */
#define GNRC_SOCK_DYN_PORTRANGE_MAX (IANA_DYNAMIC_PORTRANGE_MAX)

/**
 * @brief   Available ports in the range for dynamic source port allocation
 */
#define GNRC_SOCK_DYN_PORTRANGE_NUM (GNRC_SOCK_DYN_PORTRANGE_MAX - GNRC_SOCK_DYN_PORTRANGE_MIN + 1)

/**
 * @brief   Error value indicating that no free port could be found in the
 *          dynamic port range
 */
#define GNRC_SOCK_DYN_PORTRANGE_ERR (0)

/**
 * @brief   Check if remote address of a UDP packet matches the address the socket
 *          is bound to.
 */
#ifndef CONFIG_GNRC_SOCK_UDP_CHECK_REMOTE_ADDR
#define CONFIG_GNRC_SOCK_UDP_CHECK_REMOTE_ADDR (1)
#endif

/**
 * @brief   Structure to retrieve auxiliary data from @ref gnrc_sock_recv
 *
 * @details The members of this function depend on the modules used
 * @internal
 */
typedef struct {
#if IS_USED(MODULE_SOCK_AUX_LOCAL) || DOXYGEN
    /**
     * @brief   local IP address PDU was received on
     *
     * This member is only present if module `sock_aux_local` is used.
     */
    sock_ip_ep_t *local;
#endif
#if IS_USED(MODULE_SOCK_AUX_TIMESTAMP) || DOXYGEN
    uint64_t *timestamp;    /**< timestamp PDU was received at in nanoseconds */
#endif
#if IS_USED(MODULE_SOCK_AUX_RSSI) || DOXYGEN
    int16_t *rssi;          /**< RSSI value of received PDU */
#endif
    /**
     * @brief   Flags
     */
    uint8_t flags;
} gnrc_sock_recv_aux_t;

#define GNRC_SOCK_RECV_AUX_FLAG_TIMESTAMP   0x01    /**< Timestamp valid */
#define GNRC_SOCK_RECV_AUX_FLAG_RSSI        0x02    /**< RSSI valid */

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
 * @brief   Initializes a sock end-point from a given and sets the network
 *          interface implicitly if there is only one potential interface.
 * @internal
 */
static inline void gnrc_ep_set(sock_ip_ep_t *out, const sock_ip_ep_t *in,
                               size_t in_size)
{
    memcpy(out, in, in_size);
    if (out->netif != SOCK_ADDR_ANY_NETIF) {
        return;
    }

    /* set interface implicitly */
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    if ((netif != NULL) &&
        (gnrc_netif_highlander() || (gnrc_netif_iter(netif) == NULL))) {
        out->netif = netif->pid;
    }
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
                       sock_ip_ep_t *remote, gnrc_sock_recv_aux_t *aux);

/**
 * @brief   Send a packet internally
 * @internal
 */
ssize_t gnrc_sock_send(gnrc_pktsnip_t *payload, sock_ip_ep_t *local,
                       const sock_ip_ep_t *remote, uint8_t nh);
/** @internal
 * @}
 */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GNRC_SOCK_INTERNAL_H */
