/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "compiler_hints.h"
#include "log.h"
#include "macros/math.h"
#include "net/af.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/tx_sync.h"
#include "net/ipv6/hdr.h"
#include "net/udp.h"
#include "utlist.h"
#include "sock_types.h"
#include "gnrc_sock_internal.h"

#if IS_USED(MODULE_ZTIMER_USEC) || IS_USED(MODULE_ZTIMER_MSEC)
#  include "ztimer.h"
#endif

#ifdef MODULE_FUZZING
extern gnrc_pktsnip_t *gnrc_pktbuf_fuzzptr;
gnrc_pktsnip_t *gnrc_sock_prevpkt = NULL;
#endif

#ifdef SOCK_HAS_ASYNC
static void _netapi_cb(uint16_t cmd, gnrc_pktsnip_t *pkt, void *ctx)
{
    if (cmd == GNRC_NETAPI_MSG_TYPE_RCV) {
        msg_t msg = { .type = GNRC_NETAPI_MSG_TYPE_RCV,
                      .content = { .ptr = pkt } };
        gnrc_sock_reg_t *reg = ctx;

        if (mbox_try_put(&reg->mbox, &msg) < 1) {
            LOG_WARNING("gnrc_sock: dropped message to %p (was full)\n",
                        (void *)&reg->mbox);
            /* packet could not be delivered so it should be dropped */
            gnrc_pktbuf_release(pkt);
            return;
        }
        if (reg->async_cb.generic) {
            reg->async_cb.generic(reg, SOCK_ASYNC_MSG_RECV, reg->async_cb_arg);
        }
    }
}
#endif /* SOCK_HAS_ASYNC */

void gnrc_sock_create(gnrc_sock_reg_t *reg, gnrc_nettype_t type, uint32_t demux_ctx)
{
    mbox_init(&reg->mbox, reg->mbox_queue, GNRC_SOCK_MBOX_SIZE);
#ifdef SOCK_HAS_ASYNC
    reg->async_cb.generic = NULL;
    reg->netreg_cb.cb = _netapi_cb;
    reg->netreg_cb.ctx = reg;
    gnrc_netreg_entry_init_cb(&reg->entry, demux_ctx, &reg->netreg_cb);
#else   /* SOCK_HAS_ASYNC */
    gnrc_netreg_entry_init_mbox(&reg->entry, demux_ctx, &reg->mbox);
#endif  /* SOCK_HAS_ASYNC */
    gnrc_netreg_register(type, &reg->entry);
}

ssize_t gnrc_sock_recv(gnrc_sock_reg_t *reg, gnrc_pktsnip_t **pkt_out,
                       uint32_t timeout, sock_ip_ep_t *remote,
                       gnrc_sock_recv_aux_t *aux)
{
    /* only used when some sock_aux_% module is used */
    (void)aux;
    gnrc_pktsnip_t *pkt, *netif;
    msg_t msg;

    /* The fuzzing module is only enabled when building a fuzzing
     * application from the fuzzing/ subdirectory. When using gnrc_sock
     * the fuzzer assumes that gnrc_sock_recv is called in a loop. If it
     * is called again and the previous return value was the special
     * crafted fuzzing packet, the fuzzing application terminates.
     *
     * sock_async_event has its on fuzzing termination condition. */
#if defined(MODULE_FUZZING) && !defined(MODULE_SOCK_ASYNC_EVENT)
    if (gnrc_sock_prevpkt && gnrc_sock_prevpkt == gnrc_pktbuf_fuzzptr) {
        exit(EXIT_SUCCESS);
    }
#endif

    if (mbox_size(&reg->mbox) != GNRC_SOCK_MBOX_SIZE) {
        return -EINVAL;
    }

#if defined(DEVELHELP) && IS_ACTIVE(SOCK_HAS_ASYNC)
    if ((timeout != 0) && (reg->async_cb.generic)) {
        /* this warning is a false positive when sock_*_recv() was not called from
         * the asynchronous handler */
        LOG_WARNING("gnrc_sock: timeout != 0 within the asynchronous callback lead "
                    "to unexpected delays within the asynchronous handler.\n");
    }
#endif

    if (timeout == SOCK_NO_TIMEOUT) {
        mbox_get(&reg->mbox, &msg);
    }
    else if (timeout == 0) {
        if (!mbox_try_get(&reg->mbox, &msg)) {
            return -EAGAIN;
        }
    }
    else {
        /* Preferring low power over us precision here if both options are
         * possible. This is typically the better trade-off, as even on fast
         * networks round-trip-times are typically measured in ms rather than
         * in us */
        if (IS_USED(MODULE_ZTIMER_MSEC)) {
            /* rounding up seems more sensible here */
            uint32_t timeout_ms = (timeout + US_PER_MS - 1) / US_PER_MS;
            if (ztimer_mbox_get_timeout(ZTIMER_MSEC, &reg->mbox, &msg, timeout_ms)) {
                return -ETIMEDOUT;
            }
        }
        else if (IS_USED(MODULE_ZTIMER_USEC)) {
            if (ztimer_mbox_get_timeout(ZTIMER_USEC, &reg->mbox, &msg, timeout)) {
                return -ETIMEDOUT;
            }
        }
        else {
            /* cannot do timeout without a timer */
            assert(0);
            return -ENOTSUP;
        }
    }
    switch (msg.type) {
        case GNRC_NETAPI_MSG_TYPE_RCV:
            pkt = msg.content.ptr;
            break;
        default:
            return -EINVAL;
    }
    /* TODO: discern NETTYPE from remote->family (set in caller), when IPv4
     * was implemented */
    ipv6_hdr_t *ipv6_hdr = gnrc_ipv6_get_header(pkt);
    assert(ipv6_hdr != NULL);
    memcpy(&remote->addr, &ipv6_hdr->src, sizeof(ipv6_addr_t));
    remote->family = AF_INET6;
#if IS_USED(MODULE_SOCK_AUX_LOCAL)
    if (aux->local != NULL) {
        memcpy(&aux->local->addr, &ipv6_hdr->dst, sizeof(ipv6_addr_t));
        aux->local->family = AF_INET6;
    }
#endif /* MODULE_SOCK_AUX_LOCAL */
    netif = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
    if (netif == NULL) {
        remote->netif = SOCK_ADDR_ANY_NETIF;
    }
    else {
        gnrc_netif_hdr_t *netif_hdr = netif->data;
        /* TODO: use API in #5511 */
        remote->netif = (uint16_t)netif_hdr->if_pid;
#if IS_USED(MODULE_SOCK_AUX_TIMESTAMP)
        if (aux->timestamp != NULL) {
            if (gnrc_netif_hdr_get_timestamp(netif_hdr, aux->timestamp) == 0) {
                aux->flags |= GNRC_SOCK_RECV_AUX_FLAG_TIMESTAMP;
            }
        }
#endif /* MODULE_SOCK_AUX_TIMESTAMP */
#if IS_USED(MODULE_SOCK_AUX_RSSI)
        if ((aux->rssi) && (netif_hdr->rssi != GNRC_NETIF_HDR_NO_RSSI)) {
            aux->flags |= GNRC_SOCK_RECV_AUX_FLAG_RSSI;
            *aux->rssi = netif_hdr->rssi;
        }
#endif /* MODULE_SOCK_AUX_RSSI */
    }
    *pkt_out = pkt; /* set out parameter */

#if IS_ACTIVE(SOCK_HAS_ASYNC)
    if (reg->async_cb.generic && mbox_avail(&reg->mbox)) {
        reg->async_cb.generic(reg, SOCK_ASYNC_MSG_RECV, reg->async_cb_arg);
    }
#endif
#ifdef MODULE_FUZZING
    gnrc_sock_prevpkt = pkt;
#endif

    return 0;
}

ssize_t gnrc_sock_send(gnrc_pktsnip_t *payload, sock_ip_ep_t *local,
                       const sock_ip_ep_t *remote, uint8_t nh,
                       gnrc_sock_send_aux_t *aux)
{
    (void)aux; /* usage of aux depends on selected modules */
    gnrc_pktsnip_t *pkt;
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    gnrc_nettype_t type;
    size_t payload_len = gnrc_pkt_len(payload);
#ifdef MODULE_GNRC_NETERR
    unsigned status_subs = 0;
#endif
#if IS_USED(MODULE_GNRC_TX_SYNC)
    gnrc_tx_sync_t tx_sync;
#endif

    if (local->family != remote->family) {
        gnrc_pktbuf_release(payload);
        return -EAFNOSUPPORT;
    }

#if IS_USED(MODULE_GNRC_TX_SYNC)
    if (gnrc_tx_sync_append(payload, &tx_sync)) {
        gnrc_pktbuf_release(payload);
        return -ENOMEM;
    }
#endif

    switch (local->family) {
#ifdef SOCK_HAS_IPV6
        case AF_INET6: {
            ipv6_hdr_t *hdr;
            pkt = gnrc_ipv6_hdr_build(payload, (ipv6_addr_t *)&local->addr.ipv6,
                                      (ipv6_addr_t *)&remote->addr.ipv6);
            if (pkt == NULL) {
                return -ENOMEM;
            }
            if (payload->type == GNRC_NETTYPE_UNDEF) {
                payload->type = GNRC_NETTYPE_IPV6;
                type = GNRC_NETTYPE_IPV6;
            }
            else {
                type = payload->type;
            }
            hdr = pkt->data;
            hdr->nh = nh;
            break;
        }
#endif
        default:
            (void)nh;
            gnrc_pktbuf_release(payload);
            return -EAFNOSUPPORT;
    }
    if (local->netif != SOCK_ADDR_ANY_NETIF) {
        /* TODO: use API in #5511 */
        iface = (kernel_pid_t)local->netif;
    }
    else if (remote->netif != SOCK_ADDR_ANY_NETIF) {
        /* TODO: use API in #5511 */
        iface = (kernel_pid_t)remote->netif;
    }
    if (iface != KERNEL_PID_UNDEF) {
        gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
        gnrc_netif_hdr_t *netif_hdr;

        if (netif == NULL) {
            gnrc_pktbuf_release(pkt);
            return -ENOMEM;
        }
        netif_hdr = netif->data;
        netif_hdr->if_pid = iface;
        pkt = gnrc_pkt_prepend(pkt, netif);
    }
#ifdef MODULE_GNRC_NETERR
    /* cppcheck-suppress uninitvar
     * (reason: pkt is initialized in AF_INET6 case above, otherwise function
     * will return early) */
    for (gnrc_pktsnip_t *ptr = pkt; ptr != NULL; ptr = ptr->next) {
        /* no error should occur since pkt was created here */
        gnrc_neterr_reg(ptr);
        status_subs++;
    }
#endif
    if (!gnrc_netapi_dispatch_send(type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        /* this should not happen, but just in case */
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }

#if IS_USED(MODULE_GNRC_TX_SYNC)
    gnrc_tx_sync(&tx_sync);
#  if IS_USED(MODULE_NETDEV_TX_INFO_TIMESTAMP) && IS_USED(MODULE_SOCK_AUX_TIMESTAMP)
    if (aux->timestamp && (tx_sync.tx_info.flags & NETDEV_TX_INFO_FLAG_TIMESTAMP)) {
        *aux->timestamp = tx_sync.tx_info.timestamp;
        aux->flags |= GNRC_SOCK_SEND_AUX_FLAG_TIMESTAMP;
    }
#  endif
#endif

#ifdef MODULE_GNRC_NETERR
    uint32_t last_status = GNRC_NETERR_SUCCESS;

    while (status_subs--) {
        msg_t err_report;
        err_report.type = 0;

        while (err_report.type != GNRC_NETERR_MSG_TYPE) {
            msg_try_receive(&err_report);
            if (err_report.type != GNRC_NETERR_MSG_TYPE) {
                msg_try_send(&err_report, thread_getpid());
            }
        }
        if (err_report.content.value != last_status) {
            int res = (int)(-err_report.content.value);

            for (unsigned i = 0; i < status_subs; i++) {
                err_report.type = 0;
                /* remove remaining status reports from queue */
                while (err_report.type != GNRC_NETERR_MSG_TYPE) {
                    msg_try_receive(&err_report);
                    if (err_report.type != GNRC_NETERR_MSG_TYPE) {
                        msg_try_send(&err_report, thread_getpid());
                    }
                }
            }
            return res;
        }
        last_status = err_report.content.value;
    }
#endif
    return payload_len;
}

/** @} */
