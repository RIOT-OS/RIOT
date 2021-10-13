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

#include <assert.h>
#include <errno.h>
#include <string.h>

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

/**
 * @brief   Checks if a given UDP port is already used by another sock
 */
static bool _dyn_port_used(uint16_t port)
{
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
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
        if (ptr->local.port == port) {
            /* port already in use by another sock */
            return true;
        }
    }
#else
    (void) port;
#endif /* MODULE_GNRC_SOCK_CHECK_REUSE */
    return false;
}

/**
 * @brief   returns a UDP port, and checks for reuse if required
 *
 * implements "Another Simple Port Randomization Algorithm" as specified in
 * RFC 6056, see https://tools.ietf.org/html/rfc6056#section-3.3.2
 */
static uint16_t _get_dyn_port(sock_udp_t *sock)
{
    unsigned count = GNRC_SOCK_DYN_PORTRANGE_NUM;
    do {
        uint16_t port = GNRC_SOCK_DYN_PORTRANGE_MIN +
               (random_uint32() % GNRC_SOCK_DYN_PORTRANGE_NUM);
        if ((sock && (sock->flags & SOCK_FLAGS_REUSE_EP)) || !_dyn_port_used(port)) {
            return port;
        }
        --count;
    } while (count > 0);
    return GNRC_SOCK_DYN_PORTRANGE_ERR;
}

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
    assert(sock);
    assert(remote == NULL || remote->port != 0);
    if ((local != NULL) && (remote != NULL) &&
        (local->netif != SOCK_ADDR_ANY_NETIF) &&
        (remote->netif != SOCK_ADDR_ANY_NETIF) &&
        (local->netif != remote->netif)) {
        return -EINVAL;
    }
    memset(&sock->local, 0, sizeof(sock_udp_ep_t));
    if (local != NULL) {
        uint16_t port = local->port;

        if (gnrc_af_not_supported(local->family)) {
            return -EAFNOSUPPORT;
        }
        if (port == 0U) {
            port = _get_dyn_port(sock);
            if (port == GNRC_SOCK_DYN_PORTRANGE_ERR) {
                return -EADDRINUSE;
            }
        }
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
        else if (!(flags & SOCK_FLAGS_REUSE_EP)) {
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
        memcpy(&sock->local, local, sizeof(sock_udp_ep_t));
        sock->local.port = port;
    }
    memset(&sock->remote, 0, sizeof(sock_udp_ep_t));
    if (remote != NULL) {
        if (gnrc_af_not_supported(remote->family)) {
            return -EAFNOSUPPORT;
        }
        if (gnrc_ep_addr_any((const sock_ip_ep_t *)remote)) {
            return -EINVAL;
        }
        gnrc_ep_set((sock_ip_ep_t *)&sock->remote,
                    (sock_ip_ep_t *)remote, sizeof(sock_udp_ep_t));
    }
    if (local != NULL) {
        /* listen only with local given */
        gnrc_sock_create(&sock->reg, GNRC_NETTYPE_UDP, sock->local.port);
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

ssize_t sock_udp_recv_aux(sock_udp_t *sock, void *data, size_t max_len,
                         uint32_t timeout, sock_udp_ep_t *remote,
                         sock_udp_aux_rx_t *aux)
{
    void *pkt = NULL, *ctx = NULL;
    uint8_t *ptr = data;
    ssize_t res, ret = 0;
    bool nobufs = false;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    while ((res = sock_udp_recv_buf_aux(sock, &pkt, &ctx, timeout, remote,
                                        aux)) > 0) {
        if (res > (ssize_t)max_len) {
            nobufs = true;
            continue;
        }
        memcpy(ptr, pkt, res);
        ptr += res;
        ret += res;
    }
    return (nobufs) ? -ENOBUFS : ((res < 0) ? res : ret);
}

ssize_t sock_udp_recv_buf_aux(sock_udp_t *sock, void **data, void **buf_ctx,
                              uint32_t timeout, sock_udp_ep_t *remote,
                              sock_udp_aux_rx_t *aux)
{
    (void)aux;
    gnrc_pktsnip_t *pkt, *udp;
    udp_hdr_t *hdr;
    sock_ip_ep_t tmp;
    int res;
    gnrc_sock_recv_aux_t _aux = { 0 };

    assert((sock != NULL) && (data != NULL) && (buf_ctx != NULL));
    if (*buf_ctx != NULL) {
        *data = NULL;
        gnrc_pktbuf_release(*buf_ctx);
        *buf_ctx = NULL;
        return 0;
    }
    if (sock->local.family == AF_UNSPEC) {
        return -EADDRNOTAVAIL;
    }
    tmp.family = sock->local.family;
#if IS_USED(MODULE_SOCK_AUX_LOCAL)
    if ((aux != NULL) && (aux->flags & SOCK_AUX_GET_LOCAL)) {
        _aux.local = (sock_ip_ep_t *)&aux->local;
    }
#endif
#if IS_USED(MODULE_SOCK_AUX_TIMESTAMP)
    if ((aux != NULL) && (aux->flags & SOCK_AUX_GET_TIMESTAMP)) {
        _aux.timestamp = &aux->timestamp;
    }
#endif
#if IS_USED(MODULE_SOCK_AUX_RSSI)
    if ((aux != NULL) && (aux->flags & SOCK_AUX_GET_RSSI)) {
        _aux.rssi = &aux->rssi;
    }
#endif
    res = gnrc_sock_recv((gnrc_sock_reg_t *)sock, &pkt, timeout, &tmp, &_aux);
    if (res < 0) {
        return res;
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
#if IS_USED(MODULE_SOCK_AUX_LOCAL)
    if ((aux != NULL) && (aux->flags & SOCK_AUX_GET_LOCAL)) {
        aux->flags &= ~SOCK_AUX_GET_LOCAL;
        aux->local.port = sock->local.port;
    }
#endif
#if IS_USED(MODULE_SOCK_AUX_TIMESTAMP)
    if ((aux != NULL) && (_aux.flags & GNRC_SOCK_RECV_AUX_FLAG_TIMESTAMP)) {
        aux->flags &= ~SOCK_AUX_GET_TIMESTAMP;
    }
#endif
#if IS_USED(MODULE_SOCK_AUX_RSSI)
    if ((aux != NULL) && (_aux.flags & GNRC_SOCK_RECV_AUX_FLAG_RSSI)) {
        aux->flags &= ~SOCK_AUX_GET_RSSI;
    }
#endif
    *data = pkt->data;
    *buf_ctx = pkt;
    res = (int)pkt->size;
    return res;
}

ssize_t sock_udp_send_aux(sock_udp_t *sock, const void *data, size_t len,
                          const sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux)
{
    (void)aux;
    int res;
    gnrc_pktsnip_t *payload, *pkt;
    uint16_t src_port = 0, dst_port;
    sock_ip_ep_t local;
    sock_udp_ep_t remote_cpy;
    sock_ip_ep_t *rem;

    assert((sock != NULL) || (remote != NULL));
    assert((len == 0) || (data != NULL)); /* (len != 0) => (data != NULL) */

    if (remote != NULL) {
        if (remote->port == 0) {
            return -EINVAL;
        }
        else if (gnrc_ep_addr_any((const sock_ip_ep_t *)remote)) {
            return -EINVAL;
        }
        else if (gnrc_af_not_supported(remote->family)) {
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
    /* cppcheck-suppress nullPointerRedundantCheck
     * (reason: compiler evaluates lazily so this isn't a redundundant check and
     * cppcheck is being weird here anyways) */
    if ((sock == NULL) || (sock->local.family == AF_UNSPEC)) {
        /* no sock or sock currently unbound */
        memset(&local, 0, sizeof(local));
        if ((src_port = _get_dyn_port(sock)) == GNRC_SOCK_DYN_PORTRANGE_ERR) {
            return -EADDRINUSE;
        }
        /* cppcheck-suppress nullPointer
         * (reason: sock *can* be NULL at this place, cppcheck is weird here as
         * well, see above) */
        if (sock != NULL) {
            /* bind sock object implicitly */
            sock->local.port = src_port;
            if (remote == NULL) {
                sock->local.family = sock->remote.family;
            }
            else {
                sock->local.family = remote->family;
            }
            gnrc_sock_create(&sock->reg, GNRC_NETTYPE_UDP, src_port);
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
            /* prepend to current socks */
            sock->reg.next = (gnrc_sock_reg_t *)_udp_socks;
            _udp_socks = sock;
#endif /* MODULE_GNRC_SOCK_CHECK_REUSE */
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
        gnrc_ep_set(rem, (sock_ip_ep_t *)remote, sizeof(sock_udp_ep_t));
        dst_port = remote->port;
    }
    /* check for matching address families in local and remote */
    if (local.family == AF_UNSPEC) {
        local.family = rem->family;
    }
    else if (local.family != rem->family) {
        return -EINVAL;
    }
    /* generate payload and header snips */
    payload = gnrc_pktbuf_add(NULL, (void *)data, len, GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        return -ENOMEM;
    }
    pkt = gnrc_udp_hdr_build(payload, src_port, dst_port);
    if (pkt == NULL) {
        gnrc_pktbuf_release(payload);
        return -ENOMEM;
    }
    res = gnrc_sock_send(pkt, &local, rem, PROTNUM_UDP);
    if (res > 0) {
        res -= sizeof(udp_hdr_t);
    }
#ifdef SOCK_HAS_ASYNC
    if ((sock != NULL) && (sock->reg.async_cb.udp)) {
        sock->reg.async_cb.udp(sock, SOCK_ASYNC_MSG_SENT,
                               sock->reg.async_cb_arg);
    }
#endif  /* SOCK_HAS_ASYNC */
    return res;
}

#ifdef SOCK_HAS_ASYNC
void sock_udp_set_cb(sock_udp_t *sock, sock_udp_cb_t cb, void *arg)
{
    sock->reg.async_cb_arg = arg;
    sock->reg.async_cb.udp = cb;
}

#ifdef SOCK_HAS_ASYNC_CTX
sock_async_ctx_t *sock_udp_get_async_ctx(sock_udp_t *sock)
{
    return &sock->reg.async_ctx;
}
#endif  /* SOCK_HAS_ASYNC_CTX */
#endif  /* SOCK_HAS_ASYNC */

/** @} */
