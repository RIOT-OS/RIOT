/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <errno.h>

#include "csp/csp.h"
#include "csp/csp_id.h"
#include "csp/csp_types.h"
#include "csp_port.h"

#include "sock_types.h"
#include "net/sock/udp.h"
#include "net/netif.h"
#include "net/ipv6/addr.h"
#include "mutex.h"
#include "utlist.h"
#include "timex.h"
#define LOG_LEVEL   LOG_DEBUG
#include "log.h"

#ifdef MODULE_ZTIMER_MSEC
#include "ztimer.h"
#endif

#define _MSG_TYPE_RECV_PKT (0x1601)
#ifdef MODULE_ZTIMER_MSEC
#define _TIMEOUT_MAGIC     (0xF38A0B63U)
#define _TIMEOUT_MSG_TYPE  (0x8474)
#endif /* MODULE_ZTIMER_USEC */

/* sock linked list */
static sock_udp_t *_udp_socket_list;
/* linked list lock */
static mutex_t _sock_list_lock;

#ifdef MODULE_ZTIMER_MSEC
static void _timeout_cb(void *arg)
{
    msg_t timeout_msg = { .sender_pid = KERNEL_PID_UNDEF,
                          .type = _TIMEOUT_MSG_TYPE,
                          .content = { .value = _TIMEOUT_MAGIC } };
    mbox_t *mbox = arg;

    /* should be safe, because otherwise if mbox were filled this callback is
     * senseless */
    mbox_try_put(mbox, &timeout_msg);
}
#endif /* MODULE_ZTIMER_MSEC */

static uint16_t _csp_addr_from_ipv6_addr(const sock_udp_ep_t *ep)
{
    return ep->addr.ipv6[sizeof(ep->addr) - 1] + (ep->addr.ipv6[sizeof(ep->addr) - 2] << 8);
}

static bool _sock_valid_af(uint8_t af)
{
    /* only AF_INET6 is supported*/
    return (af == AF_INET6);
}

static bool _sock_valid_addr(const sock_udp_ep_t *ep)
{
    assert(ep != NULL);
    const uint8_t *p = (uint8_t *)&ep->addr;

    for (uint8_t i = 0; i < sizeof(ep->addr) - 2; i++) {
        if (p[i] != 0) {
            return false;
        }
    }
    uint16_t csp_addr = _csp_addr_from_ipv6_addr(ep);

    return csp_addr < csp_id_get_max_nodeid() && csp_addr != 0;
}

static void _set_ipv6_from_csp_addr(uint16_t csp_addr, sock_udp_ep_t *ep)
{
    memset(&ep->addr, '\0', sizeof(ep->addr));
    ep->addr.ipv6[sizeof(ep->addr) - 1] = (uint8_t)csp_addr;
    ep->addr.ipv6[sizeof(ep->addr) - 2] = (uint8_t)(csp_addr >> 8);
}

void _receive_callback(csp_packet_t *pkt)
{
    sock_udp_t *current;

    mutex_lock(&_sock_list_lock);
    current = _udp_socket_list;
    while (current != NULL) {
        LOG_DEBUG("[csp] sock: %" PRIx16 ":%" PRId16 " remote: %" PRIx16 ":%" PRId16 "\n",
                  _csp_addr_from_ipv6_addr(&current->gen_sock.local), current->gen_sock.local.port,
                  pkt->id.dst, pkt->id.dport
                  );
        if (current->gen_sock.local.port == pkt->id.dport) {
            msg_t msg;
            msg.type = _MSG_TYPE_RECV_PKT;
            msg.content.ptr = pkt;
            int ret = mbox_try_put(&current->mbox, &msg);
            if (ret < 1) {
                LOG_ERROR(
                    "csp_sock: dropped message to %p (was full)\n",
                    (void *)&current->mbox);
            }
            /* unlock mutex before callback execution */
            mutex_unlock(&_sock_list_lock);
#ifdef SOCK_HAS_ASYNC
            if (current->async_cb != NULL) {
                current->async_cb(current, SOCK_ASYNC_MSG_RECV, NULL);
            }
#endif /* SOCK_HAS_ASYNC */
            break;
        }
        current = current->next;
    }

    if (current == NULL) {
        /* unlock mutex if no associated sock was found */
        mutex_unlock(&_sock_list_lock);
        csp_buffer_free(pkt);
        LOG_ERROR("[csp] sock: no associated socket found %" PRIx16 ":%" PRId16 "\n", pkt->id.dst,
                  pkt->id.dport);
    }
}

void sock_udp_init(void)
{
    _udp_socket_list = NULL;
    mutex_init(&_sock_list_lock);
}

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
    /* sanity checks */
    assert(sock);
    assert(remote == NULL || remote->port != 0);
    /* check that if set netifs match */
    if ((local != NULL) && (remote != NULL) &&
        (local->netif != SOCK_ADDR_ANY_NETIF) &&
        (remote->netif != SOCK_ADDR_ANY_NETIF) &&
        (local->netif != remote->netif)) {
        return -EINVAL;
    }
    /* check local */
    memset(&sock->gen_sock.local, 0, sizeof(sock_udp_ep_t));
    if (local != NULL) {
        if (!_sock_valid_af(local->family)) {
            return -EAFNOSUPPORT;
        }
        uint8_t port = local->port;
        if (port > csp_id_get_max_port() && port != CSP_PORT_ANY) {
            LOG_DEBUG("[csp_sock_udp]: port must be < %d\n", csp_id_get_max_port());
            return -EINVAL;
        }
        if (port == CSP_PORT_UNSET) {
            port = csp_port_get_dyn_free();
            if (port == CSP_PORT_UNSET) {
                return -EADDRINUSE;
            }
        }
        if (csp_port_in_use(port)) {
            return -EADDRINUSE;
        }
        if (local->netif != SOCK_ADDR_ANY_NETIF) {
            if (!netif_get_by_id(local->netif)) {
                return -EINVAL;
            }
        }
        memcpy(&sock->gen_sock.local, local, sizeof(sock_udp_ep_t));
        /* grab address from netif */
        sock->gen_sock.local.port = port;
    }
    /* check remote */
    memset(&sock->gen_sock.remote, 0, sizeof(sock_udp_ep_t));
    if (remote != NULL) {
        if (!_sock_valid_af(remote->family)) {
            return -EAFNOSUPPORT;
        }
        if (!_sock_valid_addr(remote)) {
            LOG_DEBUG("[csp_sock_udp]: invalid address\n");
            return -EINVAL;
        }
        if (remote->netif != SOCK_ADDR_ANY_NETIF) {
            if (!netif_get_by_id(remote->netif)) {
                return -EINVAL;
            }
        }
        memcpy(&sock->gen_sock.remote, remote, sizeof(sock_udp_ep_t));
    }

    /* if all ok and local != NULL, listen */
    if (local != NULL) {
        if (csp_bind_callback(_receive_callback, sock->gen_sock.local.port)) {
            LOG_DEBUG("[csp_sock_udp]: failed bind on %d\n", sock->gen_sock.local.port);
            return -ENOMEM;
        }
        /* start mailbox */
        mbox_init(&sock->mbox, sock->mbox_queue, CSP_SOCK_MBOX_SIZE);
#ifdef SOCK_HAS_ASYNC
        sock->async_cb = NULL;
#endif
        /* update socket list */
        mutex_lock(&_sock_list_lock);
        LL_PREPEND(_udp_socket_list, sock);
        mutex_unlock(&_sock_list_lock);
    }

    /* set flags */
    sock->gen_sock.flags = flags;
    return 0;
}

ssize_t sock_udp_recv_aux(sock_udp_t *sock, void *data, size_t max_len,
                          uint32_t timeout, sock_udp_ep_t *remote,
                          sock_udp_aux_rx_t *aux)
{
    void *pkt = NULL, *ctx = NULL;
    ssize_t res;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    res = sock_udp_recv_buf_aux(sock, &pkt, &ctx, timeout, remote, aux);
    /* set data to copy, check if enough buffer space */
    if (res >= 0) {
        ssize_t tmp;
        if ((ssize_t)max_len >= res) {
            memset(data, 0, max_len);
            if (res > 0) {
                /* copy received data */
                memcpy(data, pkt, res);
                /* free packet */
                tmp = sock_udp_recv_buf_aux(sock, &pkt, &ctx, timeout, remote,
                                            aux);
                assert(tmp == 0);
            }
            (void)tmp;
        }
        else {
            res = -ENOBUFS;
            /* free packet */
            tmp = sock_udp_recv_buf_aux(sock, &pkt, &ctx, timeout, remote, aux);
            assert(tmp == 0);
        }
    }
    return res;
}

ssize_t sock_udp_recv_buf_aux(sock_udp_t *sock, void **data, void **buf_ctx,
                              uint32_t timeout, sock_udp_ep_t *remote,
                              sock_udp_aux_rx_t *aux)
{
    (void)aux;
    csp_packet_t *packet;
    sock_udp_ep_t ep;

    msg_t msg;

    assert((sock != NULL) && (data != NULL) && (buf_ctx != NULL));

    if (*buf_ctx != NULL) {
        *data = NULL;
        csp_buffer_free(*buf_ctx);
        *buf_ctx = NULL;
        return 0;
    }
    if (sock->gen_sock.local.port == CSP_PORT_UNSET) {
        return -EADDRNOTAVAIL;
    }
    else {
        if (mbox_size(&sock->mbox) != CSP_SOCK_MBOX_SIZE) {
            return -EINVAL;
        }
    #ifdef MODULE_ZTIMER_MSEC
        ztimer_t timer;

        if ((timeout != SOCK_NO_TIMEOUT) && (timeout != 0)) {
            timer.callback = _timeout_cb;
            timer.arg = &sock->mbox;
            ztimer_set(ZTIMER_MSEC, &timer, timeout / US_PER_MS);
        }
    #endif /* MODULE_ZTIMER_MSEC */
        if (timeout != 0) {
            mbox_get(&sock->mbox, &msg);
        }
        else {
            if (!mbox_try_get(&sock->mbox, &msg)) {
                return -EAGAIN;
            }
        }
    #ifdef MODULE_ZTIMER_MSEC
        if ((timeout != SOCK_NO_TIMEOUT) && (timeout != 0)) {
            ztimer_remove(ZTIMER_MSEC, &timer);
        }
    #endif /* MODULE_ZTIMER_USEC */
        switch (msg.type) {
        case _MSG_TYPE_RECV_PKT:
            packet = msg.content.ptr;
            break;
    #ifdef MODULE_ZTIMER_MSEC
        case _TIMEOUT_MSG_TYPE:
            if (msg.content.value == _TIMEOUT_MAGIC) {
                return -ETIMEDOUT;
            }
    #endif /* MODULE_ZTIMER_MSEC */
        /* Falls Through. */
        default:
            return -EINVAL;
        }
    }

    if (packet == NULL) {
        return -ETIMEDOUT;
    }

    ep.port = packet->id.sport;
    ep.family = AF_INET6;
    _set_ipv6_from_csp_addr(packet->id.src, &ep);
    ep.netif = 1; /* support a single netif for sock */

    if (remote != NULL) {
        /* return remote if requested */
        memcpy(remote, &ep, sizeof(sock_udp_ep_t));
    }

    if ((sock->gen_sock.remote.port != CSP_PORT_UNSET) &&
        /* check remote end-point if set */
        ((sock->gen_sock.remote.port != ep.port) ||
         ((memcmp(&sock->gen_sock.remote.addr, &ipv6_addr_unspecified,
                  sizeof(ipv6_addr_t)) != 0) &&
          (memcmp(&sock->gen_sock.remote.addr, &ep.addr,
                  sizeof(ipv6_addr_t)) != 0)))) {
        csp_buffer_free(packet);
        return -EPROTO;
    }

    *data = packet->data;
    *buf_ctx = packet;
    return packet->length;
}

ssize_t sock_udp_sendv_aux(sock_udp_t *sock, const iolist_t *snips,
                          const sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux)
{
    (void)aux;
    uint16_t dst_addr;
    uint16_t src_port = 0, dst_port;
    sock_ip_ep_t local;
    size_t payload_len = 0;
    uint8_t *payload_dst;

    assert((sock != NULL) || (remote != NULL));

    /* check remote */
    if (remote != NULL) {
        if (remote->port == CSP_PORT_UNSET) {
            LOG_DEBUG("[csp_sock_udp]: send, remote port is unset\n");
            return -EINVAL;
        }
        else if (!_sock_valid_addr((sock_udp_ep_t *)remote)) {
            LOG_DEBUG("[csp_sock_udp]: invalid address\n");
            return -EINVAL;
        }
        else if ((sock != NULL) &&
                 (sock->gen_sock.local.netif != SOCK_ADDR_ANY_NETIF) &&
                 (remote->netif != SOCK_ADDR_ANY_NETIF) &&
                 (sock->gen_sock.local.netif != remote->netif)) {
            return -EINVAL;
        }
    }
    /* if remote is null then check that sock has a remote */
    else if (sock->gen_sock.remote.port == CSP_PORT_UNSET) {
        return -ENOTCONN;
    }
    /* check local */
    if ((sock == NULL) || (sock->gen_sock.local.port == CSP_PORT_UNSET)) {
        /* no sock or sock currently unbound */
        memset(&local, 0, sizeof(local));
        if ((src_port = csp_port_get_dyn_free()) == CSP_PORT_UNSET) {
            return -EADDRINUSE;
        }
        if (sock != NULL) {
            sock->gen_sock.local.port = src_port;
            if (csp_bind_callback(_receive_callback, src_port)) {
                return -EINVAL;
            }
            /* start mailbox */
#ifdef SOCK_HAS_ASYNC
            sock->async_cb = NULL;
#endif
            mbox_init(&sock->mbox, sock->mbox_queue, CSP_SOCK_MBOX_SIZE);
            /* update socket list */
            mutex_lock(&_sock_list_lock);
            LL_PREPEND(_udp_socket_list, sock);
            mutex_unlock(&_sock_list_lock);
        }
    }
    else {
        /* set src port from sock */
        src_port = sock->gen_sock.local.port;
    }

    /* sock can't be NULL at this point */
    if (remote == NULL) {
        dst_addr = _csp_addr_from_ipv6_addr(&sock->gen_sock.remote);
        dst_port = sock->gen_sock.remote.port;
    }
    else {
        dst_addr = _csp_addr_from_ipv6_addr(remote);
        dst_port = remote->port;
    }

    /* calculate payload size */
    payload_len = iolist_size(snips);

    /* allocate packet buffer */
    csp_packet_t *packet = csp_buffer_get(payload_len + CSP_BUFFER_PACKET_OVERHEAD);
    if (packet == NULL) {
        return -ENOMEM;
    }

    /* copy packet content */
    packet->length = payload_len;
    payload_dst = packet->data;
    while (snips) {
        memcpy(payload_dst, snips->iol_base, snips->iol_len);
        payload_dst += snips->iol_len;
        snips = snips->iol_next;
    }
    /* send packet */
    csp_sendto(CSP_PRIO_NORM, dst_addr, dst_port, src_port, CSP_SO_NONE, packet);

    /* src address is filled by csp_send_direct */
    _set_ipv6_from_csp_addr(packet->id.src, &sock->gen_sock.local);
    return payload_len;
}

int sock_udp_get_local(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    if (sock->gen_sock.local.port == CSP_PORT_UNSET) {
        return -EADDRNOTAVAIL;
    }

    memcpy(ep, &sock->gen_sock.local, sizeof(sock_udp_ep_t));

    return 0;
}

int sock_udp_get_remote(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    if (sock->gen_sock.remote.port == CSP_PORT_UNSET) {
        return -ENOTCONN;
    }

    memcpy(ep, &sock->gen_sock.remote, sizeof(sock_udp_ep_t));

    return 0;
}

void sock_udp_close(sock_udp_t *sock)
{
    assert(sock != NULL);
    if (_udp_socket_list == NULL) {
        return;
    }
    if (sock) {
        if (mbox_size(&sock->mbox) != CSP_SOCK_MBOX_SIZE) {
            /* drop messages in mbox if any */
            msg_t msg;
            while (mbox_try_get(&sock->mbox, &msg)) {
                if (msg.type == _MSG_TYPE_RECV_PKT) {
                    csp_buffer_free(msg.content.ptr);
                }
            }
            /* remove sock from list */
            mutex_lock(&_sock_list_lock);
            LL_DELETE(_udp_socket_list, sock);
            mutex_unlock(&_sock_list_lock);
            sock->next = NULL;
        }
    }
    unsigned state = irq_disable();

    if (sock->gen_sock.local.port != CSP_PORT_UNSET) {
        csp_unbind_port(sock->gen_sock.local.port);
    }
    irq_restore(state);
}

#ifdef SOCK_HAS_ASYNC
void sock_udp_set_cb(sock_udp_t *sock, sock_udp_cb_t cb, void *cb_arg)
{
    sock->async_cb = cb;
    sock->async_cb_arg = cb_arg;
}

#ifdef SOCK_HAS_ASYNC_CTX
sock_async_ctx_t *sock_udp_get_async_ctx(sock_udp_t *sock)
{
    return &sock->async_ctx;
}
#endif  /* SOCK_HAS_ASYNC_CTX*/
#endif  /* SOCK_HAS_ASYNC */
