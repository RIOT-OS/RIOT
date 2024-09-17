/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Timothy Claeys <timothy.claeys@inria.fr>
 * @author  Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <errno.h>

#ifdef SOCK_HAS_ASYNC
#include "net/sock/async.h"
#endif
#include "net/ipv6/addr.h"
#include "net/iana/portrange.h"
#include "net/sock/udp.h"
#ifdef MODULE_ZTIMER_USEC
#include "ztimer.h"
#endif
#include "mutex.h"
#include "utlist.h"
#include "log.h"

#include "openwsn.h"
#include "opendefs.h"
#include "kernel/scheduler.h"
#include "cross-layers/packetfunctions.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/openrandom.h"
#include "cross-layers/idmanager.h"
#include "04-TRAN/udp.h"

#define _MSG_TYPE_RECV_PKT (0x1601)
#ifdef MODULE_ZTIMER_USEC
#define _TIMEOUT_MAGIC     (0xF38A0B63U)
#define _TIMEOUT_MSG_TYPE  (0x8474)
#endif /* MODULE_ZTIMER_USEC */

/* sock linked list */
static sock_udp_t *_udp_socket_list;
/* linked list lock */
static mutex_t _sock_list_lock;

#ifdef MODULE_ZTIMER_USEC
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
#endif /* MODULE_ZTIMER_USEC */

static void _sock_transmit_internal(void)
{
    OpenQueueEntry_t *pkt;

    pkt = openqueue_getPacketByComponent(COMPONENT_SOCK_TO_UDP);

    if (pkt == NULL) {
        LOG_ERROR("%s: not pkt in queue\n", __FUNCTION__);
        return;
    }

    udp_transmit(pkt);
}

static bool _sock_valid_af(uint8_t af)
{
    /* only AF_INET6 is supported*/
    return (af == AF_INET6);
}

static bool _sock_valid_addr(sock_udp_ep_t *ep)
{
    assert(ep != NULL);
    const uint8_t *p = (uint8_t *)&ep->addr;
    for (uint8_t i = 0; i < sizeof(ep->addr); i++) {
        if (p[i] != 0) {
            return true;
        }
    }
    return false;
}

static bool _sock_port_in_use(uint16_t port)
{
    for (sock_udp_t *ptr = _udp_socket_list; ptr != NULL;
         ptr = (sock_udp_t *)ptr->next) {
        if (ptr->gen_sock.local.port == port) {
            return true;
        }
    }
    return false;
}

/**
 * @brief   returns a UDP port, and checks for reuse if required
 *
 * implements "Another Simple Port Randomization Algorithm" as specified in
 * RFC 6056, see https://tools.ietf.org/html/rfc6056#section-3.3.2
 */
static uint16_t _get_dyn_port(void)
{
    unsigned num = (IANA_DYNAMIC_PORTRANGE_MAX - IANA_DYNAMIC_PORTRANGE_MIN);
    unsigned count = num;

    do {
        uint16_t port = IANA_DYNAMIC_PORTRANGE_MIN +
                        (openrandom_get16b() % num);
        if (!_sock_port_in_use(port)) {
            return port;
        }
        --count;
    } while (count > 0);
    return 0;
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
    /* if set netifs should match the only one available in OpenWSN*/
    if ((local != NULL) &&
        (local->netif != SOCK_ADDR_ANY_NETIF) &&
        (local->netif != CONFIG_OPENWSN_NETIF_ID)) {
        return -EINVAL;
    }
    if ((remote != NULL) &&
        (remote->netif != SOCK_ADDR_ANY_NETIF) &&
        (remote->netif != CONFIG_OPENWSN_NETIF_ID)) {
        return -EINVAL;
    }

    /* check local */
    memset(&sock->gen_sock.local, 0, sizeof(sock_udp_ep_t));
    if (local != NULL) {
        uint16_t port = local->port;
        if (!_sock_valid_af(local->family)) {
            return -EAFNOSUPPORT;
        }
        if (port == 0U) {
            port = _get_dyn_port();
            if (port == 0U) {
                return -EADDRINUSE;
            }
        }
        else if (!(flags & SOCK_FLAGS_REUSE_EP)) {
            /* a single IPV6 address is possible in OpenWSN, so only check
               if port is in use */
            if (_sock_port_in_use(port)) {
                return -EADDRINUSE;
            }
        }
        memcpy(&sock->gen_sock.local, local, sizeof(sock_udp_ep_t));
        sock->gen_sock.local.port = port;
    }

    /* check and set remote */
    memset(&sock->gen_sock.remote, 0, sizeof(sock_udp_ep_t));
    if (remote != NULL) {
        if (!_sock_valid_af(remote->family)) {
            return -EAFNOSUPPORT;
        }

        if (!_sock_valid_addr((sock_udp_ep_t *)remote)) {
            return -EINVAL;
        }
        memcpy(&sock->gen_sock.remote, remote, sizeof(sock_udp_ep_t));
    }

    /* if all ok and local != NULL, listen */
    if (local != NULL) {
        /* start mailbox */
        mbox_init(&sock->mbox, sock->mbox_queue, OPENWSN_SOCK_MBOX_SIZE);
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

ssize_t sock_udp_sendv_aux(sock_udp_t *sock,
                           const iolist_t *snips,
                           const sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux)
{
    (void)aux;
    OpenQueueEntry_t *pkt;
    open_addr_t dst_addr, src_addr;
    size_t payload_len = 0;
    uint8_t *payload_dst;

    memset(&dst_addr, 0, sizeof(open_addr_t));
    memset(&src_addr, 0, sizeof(open_addr_t));
    uint16_t src_port = 0, dst_port;

    /* asserts for sock_udp_send "pre" */
    assert((sock != NULL) || (remote != NULL));

    /* check remote */
    if (remote != NULL) {
        if (remote->port == 0) {
            return -EINVAL;
        }
        else if (!_sock_valid_af(remote->family)) {
            return -EAFNOSUPPORT;
        }
        else if (!_sock_valid_addr((sock_udp_ep_t *)remote)) {
            return -EINVAL;
        }
        else if ((sock != NULL) &&
                 (sock->gen_sock.local.netif != SOCK_ADDR_ANY_NETIF) &&
                 (remote->netif != SOCK_ADDR_ANY_NETIF) &&
                 (sock->gen_sock.local.netif != remote->netif)) {
            return -EINVAL;
        }
        /* if set netifs should match the only one available in OpenWSN*/
        if ((sock != NULL) &&
            (((sock->gen_sock.local.netif != SOCK_ADDR_ANY_NETIF) &&
              (sock->gen_sock.local.netif != CONFIG_OPENWSN_NETIF_ID)) ||
             ((remote->netif != SOCK_ADDR_ANY_NETIF) &&
              (remote->netif != CONFIG_OPENWSN_NETIF_ID)))) {
            return -EINVAL;
        }
        dst_addr.type = ADDR_128B;
        memcpy(&dst_addr.addr_128b, &remote->addr, LENGTH_ADDR128b);
        dst_port = remote->port;
    }
    /* if remote is null then check that sock has a remote */
    else if (sock->gen_sock.remote.family == AF_UNSPEC) {
        return -ENOTCONN;
    }
    /* set destination port and address based on sock remote */
    else {
        dst_port = sock->gen_sock.remote.port;
        dst_addr.type = ADDR_128B;
        memcpy(&dst_addr.addr_128b, &sock->gen_sock.remote.addr,
               LENGTH_ADDR128b);
    }

    if ((sock == NULL) || (sock->gen_sock.local.family == AF_UNSPEC)) {
        /* if no sock or unbound get a random port */
        if ((src_port = _get_dyn_port()) == 0) {
            return -EADDRINUSE;
        }
    }
    else {
        /* set src port from sock */
        src_port = sock->gen_sock.local.port;
        /* set addr if sock is not null or bound */
        src_addr.type = ADDR_128B;
        memcpy(&src_addr.addr_128b, &sock->gen_sock.local.addr,
               LENGTH_ADDR128b);
    }

    /* get pkt buffer and take owner ship */
    if ((pkt = openqueue_getFreePacketBuffer(COMPONENT_SOCK_TO_UDP)) == NULL) {
        return -ENOMEM;
    }
    pkt->owner = COMPONENT_SOCK_TO_UDP;
    pkt->creator = COMPONENT_SOCK_TO_UDP;

    /* set addresses and ports */
    pkt->l3_destinationAdd.type = ADDR_128B;
    pkt->l3_sourceAdd.type = ADDR_128B;
    memcpy(&pkt->l3_sourceAdd.addr_128b, &src_addr.addr_128b, LENGTH_ADDR128b);
    memcpy(&pkt->l3_destinationAdd.addr_128b, &dst_addr.addr_128b,
           LENGTH_ADDR128b);
    pkt->l4_destination_port = dst_port;
    pkt->l4_sourcePortORicmpv6Type = src_port;

    /* calculate payload size */
    payload_len = iolist_size(snips);

    /* set payload */
    if (packetfunctions_reserveHeader(&pkt, payload_len)) {
        openqueue_freePacketBuffer(pkt);
        return -ENOMEM;
    }

    /* copy payload into packet */
    payload_dst = pkt->payload;
    while (snips) {
        memcpy(payload_dst, snips->iol_base, snips->iol_len);
        payload_dst += snips->iol_len;
        snips = snips->iol_next;
    }
    pkt->l4_payload = pkt->payload;
    pkt->l4_length = pkt->length;

    /* push task to scheduler send */
    scheduler_push_task(_sock_transmit_internal, TASKPRIO_UDP);

    return payload_len;
}

void sock_udp_close(sock_udp_t *sock)
{
    assert(sock != NULL);
    if (_udp_socket_list == NULL) {
        return;
    }
    if (sock) {
        /* drop messages in mbox if any */
        msg_t msg;
        while (mbox_try_get(&sock->mbox, &msg)) {
            if (msg.type == _MSG_TYPE_RECV_PKT) {
                openqueue_freePacketBuffer(
                    (OpenQueueEntry_t*) msg.content.ptr);
            }
        }
        /* remove sock from list */
        mutex_lock(&_sock_list_lock);
        LL_DELETE(_udp_socket_list, sock);
        mutex_unlock(&_sock_list_lock);
        sock->next = NULL;
    }
}

int sock_udp_get_local(const sock_udp_t *sock, sock_udp_ep_t *ep)
{
    if (sock->gen_sock.local.family == AF_UNSPEC) {
        return -EADDRNOTAVAIL;
    }

    memcpy(ep, &sock->gen_sock.local, sizeof(sock_udp_ep_t));

    return 0;
}

int sock_udp_get_remote(const sock_udp_t *sock, sock_udp_ep_t *ep)
{
    if (sock->gen_sock.remote.family == AF_UNSPEC) {
        return -ENOTCONN;
    }

    memcpy(ep, &sock->gen_sock.remote, sizeof(sock_udp_ep_t));

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
            (void) tmp;
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
    (void) aux;
    sock_udp_ep_t ep;
    OpenQueueEntry_t *pkt = NULL;
    msg_t msg;

    assert((sock != NULL) && (data != NULL) && (buf_ctx != NULL));

    if (*buf_ctx != NULL) {
        *data = NULL;
        openqueue_freePacketBuffer(*buf_ctx);
        *buf_ctx = NULL;
        return 0;
    }
    if (sock->gen_sock.local.family == AF_UNSPEC) {
        return -EADDRNOTAVAIL;
    }

#ifdef MODULE_ZTIMER_USEC
    ztimer_t timer;

    if ((timeout != SOCK_NO_TIMEOUT) && (timeout != 0)) {
        timer.callback = _timeout_cb;
        timer.arg = &sock->mbox;
        ztimer_set(ZTIMER_USEC, &timer, timeout);
    }
#endif /* MODULE_ZTIMER_USEC */
    if (timeout != 0) {
        mbox_get(&sock->mbox, &msg);
    }
    else {
        if (!mbox_try_get(&sock->mbox, &msg)) {
            return -EAGAIN;
        }
    }
#ifdef MODULE_ZTIMER_USEC
    if ((timeout != SOCK_NO_TIMEOUT) && (timeout != 0)) {
        ztimer_remove(ZTIMER_USEC, &timer);
    }
#endif /* MODULE_ZTIMER_USEC */
    switch (msg.type) {
    case _MSG_TYPE_RECV_PKT:
        pkt = msg.content.ptr;
        break;
#ifdef MODULE_ZTIMER_USEC
    case _TIMEOUT_MSG_TYPE:
        if (msg.content.value == _TIMEOUT_MAGIC) {
            return -ETIMEDOUT;
        }
#endif /* MODULE_ZTIMER_USEC */
    /* Falls Through. */
    default:
        return -EINVAL;
    }

    if (pkt == NULL) {
        return -EINVAL;
    }

    /* copy source port and address to end point */
    ep.family = AF_INET6;
    ep.port = pkt->l4_sourcePortORicmpv6Type;
    ep.netif = CONFIG_OPENWSN_NETIF_ID;
    memcpy(&ep.addr, pkt->l3_sourceAdd.addr_128b, LENGTH_ADDR128b);

    if (remote != NULL) {
        /* return remote if requested */
        memcpy(remote, &ep, sizeof(sock_udp_ep_t));
    }

    if ((sock->gen_sock.remote.family != AF_UNSPEC) &&
        /* check remote end-point if set */
        ((sock->gen_sock.remote.port != pkt->l4_sourcePortORicmpv6Type) ||
         /* We only have IPv6 for now, so just comparing the whole end point
          * should suffice */
         ((memcmp(&sock->gen_sock.remote.addr, &ipv6_addr_unspecified,
                  sizeof(ipv6_addr_t)) != 0) &&
          (memcmp(&sock->gen_sock.remote.addr, &ep.addr,
                  sizeof(ipv6_addr_t)) != 0)))) {
        openqueue_freePacketBuffer(pkt);
        return -EPROTO;
    }

    *data = pkt->l4_payload;
    *buf_ctx = pkt;

    return pkt->l4_length;
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

void sock_receive_internal(void)
{
    OpenQueueEntry_t *pkt;
    sock_udp_t *current;

    pkt = openqueue_getPacketByComponent(COMPONENT_UDP_TO_SOCK);

    if (pkt == NULL) {
        LOG_ERROR("%s: not pkt in queue\n", __FUNCTION__);
        return;
    }

    mutex_lock(&_sock_list_lock);
    current = _udp_socket_list;

    while (current != NULL) {
        if (current->gen_sock.local.port == pkt->l4_destination_port &&
            idmanager_isMyAddress(&pkt->l3_destinationAdd)) {
            msg_t msg;
            msg.type = _MSG_TYPE_RECV_PKT;
            msg.content.ptr = pkt;
            int ret = mbox_try_put(&current->mbox, &msg);
            if (ret < 1) {
                LOG_ERROR(
                    "openwsn_sock: dropped message to %p (was full)\n",
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
        openqueue_freePacketBuffer(pkt);
        LOG_ERROR("%s: no associated socket found\n", __FUNCTION__);
    }
}

void sock_senddone_internal(OpenQueueEntry_t *msg, owerror_t error)
{
    /* In RIOT we can't know what message was sent... */
    (void)msg;
#ifdef SOCK_HAS_ASYNC
    OpenQueueEntry_t *pkt;
    sock_udp_t *current;

    pkt = openqueue_getPacketByComponent(COMPONENT_UDP);

    if (pkt == NULL) {
        LOG_ERROR("%s: not pkt in queue\n", __FUNCTION__);
        return;
    }

    mutex_lock(&_sock_list_lock);
    current = _udp_socket_list;
    while (current != NULL) {
        if (current->gen_sock.local.port == pkt->l4_sourcePortORicmpv6Type &&
            current->async_cb != NULL) {
            /* unlock mutex before callback execution */
            mutex_unlock(&_sock_list_lock);
            current->async_cb(current, SOCK_ASYNC_MSG_SENT, &error);
            break;
        }

        current = current->next;
    }

    if (current == NULL) {
        /* unlock mutex if no associated sock was found */
        mutex_unlock(&_sock_list_lock);
    }
#else /* SOCK_HAS_ASYNC */
    (void)error;
#endif /*SOCK_HAS_ASYNC */
}
