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

#include <errno.h>

#include "log.h"
#include "net/af.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/netreg.h"
#include "net/udp.h"
#include "utlist.h"
#include "xtimer.h"

#include "sock_types.h"
#include "gnrc_sock_internal.h"

#ifdef MODULE_XTIMER
#define _TIMEOUT_MAGIC      (0xF38A0B63U)
#define _TIMEOUT_MSG_TYPE   (0x8474)

static void _callback_put(void *arg)
{
    msg_t timeout_msg = { .sender_pid = KERNEL_PID_UNDEF,
                          .type = _TIMEOUT_MSG_TYPE,
                          .content = { .value = _TIMEOUT_MAGIC } };
    gnrc_sock_reg_t *reg = arg;

    /* should be safe, because otherwise if mbox were filled this callback is
     * senseless */
    mbox_try_put(&reg->mbox, &timeout_msg);
}
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
        }
        if (reg->async_cb.generic) {
            reg->async_cb.generic(reg, SOCK_ASYNC_MSG_RECV);
        }
    }
}
#endif /* SOCK_HAS_ASYNC */

void gnrc_sock_create(gnrc_sock_reg_t *reg, gnrc_nettype_t type, uint32_t demux_ctx)
{
    mbox_init(&reg->mbox, reg->mbox_queue, SOCK_MBOX_SIZE);
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
                       uint32_t timeout, sock_ip_ep_t *remote)
{
    gnrc_pktsnip_t *pkt, *netif;
    msg_t msg;

    if (reg->mbox.cib.mask != (SOCK_MBOX_SIZE - 1)) {
        return -EINVAL;
    }
#ifdef MODULE_XTIMER
    xtimer_t timeout_timer;

    if ((timeout != SOCK_NO_TIMEOUT) && (timeout != 0)) {
        timeout_timer.callback = _callback_put;
        timeout_timer.arg = reg;
        xtimer_set(&timeout_timer, timeout);
    }
#endif
    if (timeout != 0) {
        mbox_get(&reg->mbox, &msg);
    }
    else {
        if (!mbox_try_get(&reg->mbox, &msg)) {
            return -EAGAIN;
        }
    }
#ifdef MODULE_XTIMER
    xtimer_remove(&timeout_timer);
#endif
    switch (msg.type) {
        case GNRC_NETAPI_MSG_TYPE_RCV:
            pkt = msg.content.ptr;
            break;
#ifdef MODULE_XTIMER
        case _TIMEOUT_MSG_TYPE:
            if (msg.content.value == _TIMEOUT_MAGIC) {
                return -ETIMEDOUT;
            }
#endif
            /* Falls Through. */
        default:
            return -EINVAL;
    }
    /* TODO: discern NETTYPE from remote->family (set in caller), when IPv4
     * was implemented */
    ipv6_hdr_t *ipv6_hdr = gnrc_ipv6_get_header(pkt);
    assert(ipv6_hdr != NULL);
    memcpy(&remote->addr, &ipv6_hdr->src, sizeof(ipv6_addr_t));
    remote->family = AF_INET6;
    netif = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
    if (netif == NULL) {
        remote->netif = SOCK_ADDR_ANY_NETIF;
    }
    else {
        gnrc_netif_hdr_t *netif_hdr = netif->data;
        /* TODO: use API in #5511 */
        remote->netif = (uint16_t)netif_hdr->if_pid;
    }
    *pkt_out = pkt; /* set out parameter */
    return 0;
}

ssize_t gnrc_sock_send(gnrc_pktsnip_t *payload, sock_ip_ep_t *local,
                       const sock_ip_ep_t *remote, uint8_t nh)
{
    gnrc_pktsnip_t *pkt;
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    gnrc_nettype_t type;
    size_t payload_len = gnrc_pkt_len(payload);
#ifdef MODULE_GNRC_NETERR
    unsigned status_subs = 0;
#endif

    if (local->family != remote->family) {
        gnrc_pktbuf_release(payload);
        return -EAFNOSUPPORT;
    }
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
        LL_PREPEND(pkt, netif);
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
#ifdef MODULE_GNRC_NETERR
    uint32_t last_status = GNRC_NETERR_SUCCESS;

    while (status_subs--) {
        msg_t err_report;
        err_report.type = 0;

        while (err_report.type != GNRC_NETERR_MSG_TYPE) {
            msg_try_receive(&err_report);
            if (err_report.type != GNRC_NETERR_MSG_TYPE) {
                msg_try_send(&err_report, sched_active_pid);
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
                        msg_try_send(&err_report, sched_active_pid);
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
