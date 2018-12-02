/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2018 Ken Bannister <kb2ma@runbox.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_owsn_sock
 * @{
 *
 * @file
 * @brief       OpenWSN implementation of @ref net_sock_udp
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <errno.h>

#include "net/sock/udp.h"
#include "net/udp.h"
#include "owsn_sock_internal.h"
#include "od.h"

#include "opendefs.h"
#include "02a-MAClow/IEEE802154E.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/packetfunctions.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
    assert(sock);
    assert(local == NULL || local->port != 0);
    assert(remote == NULL || remote->port != 0);
    if ((local != NULL) && (remote != NULL) &&
        (local->netif != SOCK_ADDR_ANY_NETIF) &&
        (remote->netif != SOCK_ADDR_ANY_NETIF) &&
        (local->netif != remote->netif)) {
        return -EINVAL;
    }
    /* init sock.local */
    memset(&sock->local, 0, sizeof(sock_udp_ep_t));
    if (local != NULL) {
        if (owsn_af_not_supported(local->family)) {
            return -EAFNOSUPPORT;
        }
        memcpy(&sock->local, local, sizeof(sock_udp_ep_t));
    }
    /* init sock.remote */
    memset(&sock->remote, 0, sizeof(sock_udp_ep_t));
    if (remote != NULL) {
        if (owsn_af_not_supported(remote->family)) {
            return -EAFNOSUPPORT;
        }
        if (owsn_ep_addr_any((const sock_ip_ep_t *)remote)) {
            return -EINVAL;
        }
        owsn_ep_set((sock_ip_ep_t *)&sock->remote,
                    (sock_ip_ep_t *)remote, sizeof(sock_udp_ep_t));
    }
    if (local != NULL) {
        /* listen only with local given */
        owsn_sock_create(local->port);
    }
    sock->flags = flags;
    return 0;
}

void sock_udp_close(sock_udp_t *sock)
{
    assert(sock != NULL);
}

int sock_udp_get_local(sock_udp_t *sock, sock_udp_ep_t *local)
{
    assert(sock && local);
    if (sock->local.family == AF_UNSPEC) {
        return -EADDRNOTAVAIL;
    }
    memcpy(local, &sock->local, sizeof(sock_udp_ep_t));
    return 0;
}

int sock_udp_get_remote(sock_udp_t *sock, sock_udp_ep_t *remote)
{
    assert(sock && remote);
    if (sock->remote.family == AF_UNSPEC) {
        return -ENOTCONN;
    }
    memcpy(remote, &sock->remote, sizeof(sock_udp_ep_t));
    return 0;
}

ssize_t sock_udp_recv(sock_udp_t *sock, void *data, size_t max_len,
                      uint32_t timeout, sock_udp_ep_t *remote)
{
    (void)sock;
    (void)remote;
    OpenQueueEntry_t *pkt;
    sock_ip_ep_t tmp;
    size_t pkt_len;
    ssize_t res;

    res = owsn_sock_recv(timeout, &pkt, &tmp);
    if (res < 0) {
        return res;
    }

    /* read pkt */
    pkt_len = pkt->length;
    if (pkt_len > max_len) {
        openqueue_freePacketBuffer(pkt);
        return -ENOBUFS;
    }

    if (remote != NULL) {
        /* return remote to possibly block if wrong remote */
        memcpy(remote, &tmp, sizeof(tmp));
        remote->port = pkt->l4_sourcePortORicmpv6Type;
    }

    memcpy(data, &pkt->payload[0], pkt_len);
    openqueue_freePacketBuffer(pkt);

    return (ssize_t)pkt_len;
}

ssize_t sock_udp_send(sock_udp_t *sock, const void *data, size_t len,
                      const sock_udp_ep_t *remote)
{
    OpenQueueEntry_t *pkt;
    uint16_t src_port = 0, dst_port;
    sock_ip_ep_t local;
    sock_udp_ep_t remote_cpy;
    sock_ip_ep_t *rem;

    assert((sock != NULL) || (remote != NULL));
    assert((len == 0) || (data != NULL)); /* (len != 0) => (data != NULL) */

    /* don't run if not in synch */
    if (ieee154e_isSynch() == FALSE) {
        DEBUG("owsn_sock: Node not in synch, exit");
        return -ENOTCONN;
    }

    /* validate remote */
    if (remote != NULL) {
        if (remote->port == 0) {
            return -EINVAL;
        }
        else if (owsn_ep_addr_any((const sock_ip_ep_t *)remote)) {
            return -EINVAL;
        }
        else if (owsn_af_not_supported(remote->family)) {
            return -EAFNOSUPPORT;
        }
        else if ((sock != NULL) &&
                 (sock->local.netif != SOCK_ADDR_ANY_NETIF) &&
                 (remote->netif != SOCK_ADDR_ANY_NETIF) &&
                 (sock->local.netif != remote->netif)) {
            return -EINVAL;
        }
    }
    else if (sock->remote.family == AF_UNSPEC) {
        return -ENOTCONN;
    }
    if ((sock == NULL) || (sock->local.family == AF_UNSPEC)) {
        memset(&local, 0, sizeof(local));
        if (sock != NULL) {
            /* temporary; use dynamic port */
            src_port = sock->remote.port;
            /* bind sock object implicitly */
            sock->local.port = src_port;
            if (remote == NULL) {
                sock->local.family = sock->remote.family;
            }
            else {
                sock->local.family = remote->family;
            }
            owsn_sock_create(src_port);
        }
    }
    else {
        src_port = sock->local.port;
        memcpy(&local, &sock->local, sizeof(local));
    }
    /* sock can't be NULL at this point */
    if (remote == NULL) {
        rem = (sock_ip_ep_t *)&sock->remote;
        dst_port = sock->remote.port;
    }
    else {
        rem = (sock_ip_ep_t *)&remote_cpy;
        owsn_ep_set(rem, (sock_ip_ep_t *)remote, sizeof(sock_udp_ep_t));
        dst_port = remote->port;
    }

    /* check for matching address families in local and remote */
    if (local.family == AF_UNSPEC) {
        local.family = rem->family;
    }
    else if (local.family != rem->family) {
        return -EINVAL;
    }

    /* get a free packet buffer */
    pkt = openqueue_getFreePacketBuffer(COMPONENT_UINJECT);
    if (pkt == NULL) {
        DEBUG("owsn_sock: could not create packet buffer");
        return -ENOMEM;
    }
    pkt->owner = COMPONENT_UINJECT;
    pkt->creator = COMPONENT_UINJECT;
    pkt->l4_destination_port = dst_port;
    pkt->l4_sourcePortORicmpv6Type = src_port;
    pkt->l3_destinationAdd.type = ADDR_128B;
    memcpy(&pkt->l3_destinationAdd.addr_128b[0], (void *)&rem->addr.ipv6[0], 16);
    /* add payload */
    packetfunctions_reserveHeaderSize(pkt, len);
    memcpy(&pkt->payload[0], data, len);

    if (ENABLE_DEBUG) {
        printf("owsn_sock: sending %u bytes to port %u\n", len, dst_port);
        printf("owsn_sock: dst addr:\n");
        od_hex_dump(&pkt->l3_destinationAdd.addr_128b[0], 16, OD_WIDTH_DEFAULT);
        printf("owsn_sock: payload:\n");
        od_hex_dump(&pkt->payload[0], len, OD_WIDTH_DEFAULT);
    }
    owsn_sock_send(pkt);

    return pkt->length;
}

/** @} */
