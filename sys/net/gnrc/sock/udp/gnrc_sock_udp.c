/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       GNRC implementation of @ref net_sock_udp
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>

#include "byteorder.h"
#include "net/af.h"
#include "net/protnum.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"
#include "net/sock/udp.h"
#include "net/udp.h"
#include "random.h"

#include "gnrc_sock_internal.h"

#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
static sock_udp_t *_udp_socks = NULL;
#endif

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
    memset(&sock->local, 0, sizeof(sock_udp_ep_t));
    if (local != NULL) {
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
        if (!(flags & SOCK_FLAGS_REUSE_EP)) {
            for (sock_udp_t *ptr = _udp_socks; ptr != NULL;
                 ptr = (sock_udp_t *)ptr->reg.next) {
                if (memcmp(&ptr->local, local, sizeof(sock_udp_ep_t)) == 0) {
                    return -EADDRINUSE;
                }
            }
        }
        /* prepend to current socks */
        sock->reg.next = (gnrc_sock_reg_t *)_udp_socks;
        _udp_socks = sock;
#endif
        if (gnrc_af_not_supported(local->family)) {
            return -EAFNOSUPPORT;
        }
        memcpy(&sock->local, local, sizeof(sock_udp_ep_t));
    }
    memset(&sock->remote, 0, sizeof(sock_udp_ep_t));
    if (remote != NULL) {
        if (gnrc_af_not_supported(remote->family)) {
            return -EAFNOSUPPORT;
        }
        if (gnrc_ep_addr_any((const sock_ip_ep_t *)remote)) {
            return -EINVAL;
        }
        memcpy(&sock->remote, remote, sizeof(sock_udp_ep_t));
    }
    if (local != NULL) {
        /* listen only with local given */
        gnrc_sock_create(&sock->reg, GNRC_NETTYPE_UDP,
                         local->port);
    }
    sock->flags = flags;
    return 0;
}

void sock_udp_close(sock_udp_t *sock)
{
    assert(sock != NULL);
    gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &sock->reg.entry);
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
    if (_udp_socks != NULL) {
        gnrc_sock_reg_t *head = (gnrc_sock_reg_t *)_udp_socks;
        LL_DELETE(head, (gnrc_sock_reg_t *)sock);
    }
#endif
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
    gnrc_pktsnip_t *pkt, *udp;
    udp_hdr_t *hdr;
    sock_ip_ep_t tmp;
    int res;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    if (sock->local.family == AF_UNSPEC) {
        return -EADDRNOTAVAIL;
    }
    tmp.family = sock->local.family;
    res = gnrc_sock_recv((gnrc_sock_reg_t *)sock, &pkt, timeout, &tmp);
    if (res < 0) {
        return res;
    }
    if (pkt->size > max_len) {
        gnrc_pktbuf_release(pkt);
        return -ENOBUFS;
    }
    udp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_UDP);
    assert(udp);
    hdr = udp->data;
    if (remote != NULL) {
        /* return remote to possibly block if wrong remote */
        memcpy(remote, &tmp, sizeof(tmp));
        remote->port = byteorder_ntohs(hdr->src_port);
    }
    if ((sock->remote.family != AF_UNSPEC) &&  /* check remote end-point if set */
        ((sock->remote.port != byteorder_ntohs(hdr->src_port)) ||
        /* We only have IPv6 for now, so just comparing the whole end point
         * should suffice */
        ((memcmp(&sock->remote.addr, &ipv6_addr_unspecified,
                 sizeof(ipv6_addr_t)) != 0) &&
         (memcmp(&sock->remote.addr, &tmp.addr, sizeof(ipv6_addr_t)) != 0)))) {
        gnrc_pktbuf_release(pkt);
        return -EPROTO;
    }
    memcpy(data, pkt->data, pkt->size);
    gnrc_pktbuf_release(pkt);
    return (int)pkt->size;
}

ssize_t sock_udp_send(sock_udp_t *sock, const void *data, size_t len,
                      const sock_udp_ep_t *remote)
{
    int res;
    gnrc_pktsnip_t *payload, *pkt;
    uint16_t src_port = 0, dst_port;
    sock_ip_ep_t local;
    sock_ip_ep_t rem;

    assert((sock != NULL) || (remote != NULL));
    assert((len == 0) || (data != NULL)); /* (len != 0) => (data != NULL) */
    if ((remote != NULL) && (sock != NULL) &&
        (sock->local.netif != SOCK_ADDR_ANY_NETIF) &&
        (remote->netif != SOCK_ADDR_ANY_NETIF) &&
        (sock->local.netif != remote->netif)) {
        return -EINVAL;
    }
    if ((remote != NULL) && ((remote->port == 0) ||
                             gnrc_ep_addr_any((const sock_ip_ep_t *)remote))) {
        return -EINVAL;
    }
    if ((remote == NULL) &&
        /* sock can't be NULL as per assertion above */
        (sock->remote.family == AF_UNSPEC)) {
        return -ENOTCONN;
    }
    /* compiler evaluates lazily so this isn't a redundundant check and cppcheck
     * is being weird here anyways */
    /* cppcheck-suppress nullPointerRedundantCheck */
    /* cppcheck-suppress nullPointer */
    if ((sock == NULL) || (sock->local.family == AF_UNSPEC)) {
        /* no sock or sock currently unbound */
        while (src_port == 0) {
            src_port = (uint16_t)(random_uint32() & UINT16_MAX);
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
            if ((sock == NULL) || !(sock->flags & SOCK_FLAGS_REUSE_EP)) {
                /* check if port already registered somewhere */
                for (sock_udp_t *ptr = _udp_socks; ptr != NULL;
                     ptr = (sock_udp_t *)ptr->reg.next) {
                    bool spec_addr = false;
                    for (unsigned i = 0; i < sizeof(ptr->local.addr); i++) {
                        const uint8_t *const p = (uint8_t *)&ptr->local.addr;
                        if (p[i] != 0) {
                            spec_addr = true;
                        }
                    }
                    if (spec_addr) {
                        continue;
                    }
                    if (ptr->local.port == src_port) {
                        /* we already have one of this port registered
                         * => generate a new one */
                        src_port = 0;
                    }
                }
            }
#endif
        }
        memset(&local, 0, sizeof(local));
        if (sock != NULL) {
            /* bind sock object implicitly */
            sock->local.port = src_port;
            if (remote == NULL) {
                sock->local.family = sock->remote.family;
            }
            else {
                sock->local.family = remote->family;
            }
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
            /* prepend to current socks */
            sock->reg.next = (gnrc_sock_reg_t *)_udp_socks;
            _udp_socks = sock;
#endif
            gnrc_sock_create(&sock->reg, GNRC_NETTYPE_UDP, src_port);
        }
    }
    else {
        src_port = sock->local.port;
        memcpy(&local, &sock->local, sizeof(local));
    }
    if (remote == NULL) {
        /* sock can't be NULL at this point */
        memcpy(&rem, &sock->remote, sizeof(rem));
        dst_port = sock->remote.port;
    }
    else {
        memcpy(&rem, remote, sizeof(rem));
        dst_port = remote->port;
    }
    if ((remote != NULL) && (remote->family == AF_UNSPEC) &&
        (sock != NULL) && (sock->remote.family != AF_UNSPEC)) {
        /* remote was set on create so take its family */
        rem.family = sock->remote.family;
    }
    else if ((remote != NULL) && gnrc_af_not_supported(remote->family)) {
        return -EAFNOSUPPORT;
    }
    else if ((local.family == AF_UNSPEC) && (rem.family != AF_UNSPEC)) {
        /* local was set to 0 above */
        local.family = rem.family;
    }
    else if ((local.family != AF_UNSPEC) && (rem.family == AF_UNSPEC)) {
        /* local was given on create, but remote family wasn't given by user and
         * there was no remote given on create, take from local */
        rem.family = local.family;
    }
    payload = gnrc_pktbuf_add(NULL, (void *)data, len, GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        return -ENOMEM;
    }
    pkt = gnrc_udp_hdr_build(payload, src_port, dst_port);
    if (pkt == NULL) {
        gnrc_pktbuf_release(payload);
        return -ENOMEM;
    }
    res = gnrc_sock_send(pkt, &local, &rem, PROTNUM_UDP);
    if (res <= 0) {
        return res;
    }
    return res - sizeof(udp_hdr_t);
}

/** @} */
