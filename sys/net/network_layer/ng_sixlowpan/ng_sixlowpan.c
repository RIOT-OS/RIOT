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
 */

#include "kernel_types.h"
#include "net/gnrc.h"
#include "thread.h"
#include "utlist.h"

#include "net/ng_ipv6/hdr.h"
#include "net/ng_sixlowpan.h"
#include "net/ng_sixlowpan/frag.h"
#include "net/ng_sixlowpan/iphc.h"
#include "net/ng_sixlowpan/netif.h"
#include "net/sixlowpan.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

static kernel_pid_t _pid = KERNEL_PID_UNDEF;

#if ENABLE_DEBUG
static char _stack[NG_SIXLOWPAN_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[NG_SIXLOWPAN_STACK_SIZE];
#endif


/* handles NG_NETAPI_MSG_TYPE_RCV commands */
static void _receive(ng_pktsnip_t *pkt);
/* handles NG_NETAPI_MSG_TYPE_SND commands */
static void _send(ng_pktsnip_t *pkt);
/* Main event loop for 6LoWPAN */
static void *_event_loop(void *args);

kernel_pid_t ng_sixlowpan_init(void)
{
    if (_pid > KERNEL_PID_UNDEF) {
        return _pid;
    }

    _pid = thread_create(_stack, sizeof(_stack), NG_SIXLOWPAN_PRIO,
                         CREATE_STACKTEST, _event_loop, NULL, "6lo");

    return _pid;
}

static void _receive(ng_pktsnip_t *pkt)
{
    ng_pktsnip_t *payload;
    uint8_t *dispatch;

    /* seize payload as a temporary variable */
    payload = ng_pktbuf_start_write(pkt);   /* need to duplicate since pkt->next
                                             * might get replaced */

    if (payload == NULL) {
        DEBUG("6lo: can not get write access on received packet\n");
#if defined(DEVELHELP) && ENABLE_DEBUG
        ng_pktbuf_stats();
#endif
        ng_pktbuf_release(pkt);
        return;
    }

    pkt = payload;  /* reset pkt from temporary variable */

    LL_SEARCH_SCALAR(pkt, payload, type, NG_NETTYPE_SIXLOWPAN);

    if ((payload == NULL) || (payload->size < 1)) {
        DEBUG("6lo: Received packet has no 6LoWPAN payload\n");
        ng_pktbuf_release(pkt);
        return;
    }

    dispatch = payload->data;

    if (dispatch[0] == SIXLOWPAN_UNCOMP) {
        ng_pktsnip_t *sixlowpan;
        DEBUG("6lo: received uncompressed IPv6 packet\n");
        payload = ng_pktbuf_start_write(payload);

        if (payload == NULL) {
            DEBUG("6lo: can not get write access on received packet\n");
#if defined(DEVELHELP) && ENABLE_DEBUG
            ng_pktbuf_stats();
#endif
            ng_pktbuf_release(pkt);
            return;
        }

        /* packet is uncompressed: just mark and remove the dispatch */
        sixlowpan = ng_pktbuf_mark(payload, sizeof(uint8_t), NG_NETTYPE_SIXLOWPAN);

        if (sixlowpan == NULL) {
            DEBUG("6lo: can not mark 6LoWPAN dispatch\n");
            ng_pktbuf_release(pkt);
            return;
        }

        pkt = ng_pktbuf_remove_snip(pkt, sixlowpan);
    }
#ifdef MODULE_NG_SIXLOWPAN_FRAG
    else if (sixlowpan_frag_is((sixlowpan_frag_t *)dispatch)) {
        DEBUG("6lo: received 6LoWPAN fragment\n");
        ng_sixlowpan_frag_handle_pkt(pkt);
        return;
    }
#endif
#ifdef MODULE_NG_SIXLOWPAN_IPHC
    else if (sixlowpan_iphc_is(dispatch)) {
        size_t dispatch_size;
        ng_pktsnip_t *sixlowpan;
        ng_pktsnip_t *ipv6 = ng_pktbuf_add(NULL, NULL, sizeof(ipv6_hdr_t),
                                           NG_NETTYPE_IPV6);
        if ((ipv6 == NULL) ||
            (dispatch_size = ng_sixlowpan_iphc_decode(ipv6, pkt, 0)) == 0) {
            DEBUG("6lo: error on IPHC decoding\n");
            if (ipv6 != NULL) {
                ng_pktbuf_release(ipv6);
            }
            ng_pktbuf_release(pkt);
            return;
        }
        sixlowpan = ng_pktbuf_mark(pkt, dispatch_size, NG_NETTYPE_SIXLOWPAN);
        if (sixlowpan == NULL) {
            DEBUG("6lo: error on marking IPHC dispatch\n");
            ng_pktbuf_release(ipv6);
            ng_pktbuf_release(pkt);
            return;
        }

        /* Remove IPHC dispatch */
        ng_pktbuf_remove_snip(pkt, sixlowpan);
        /* Insert IPv6 header instead */
        ipv6->next = pkt->next;
        pkt->next = ipv6;
    }
#endif
    else {
        DEBUG("6lo: dispatch %02" PRIx8 " ... is not supported\n",
              dispatch[0]);
        ng_pktbuf_release(pkt);
        return;
    }

    payload->type = NG_NETTYPE_IPV6;

    if (!ng_netapi_dispatch_receive(NG_NETTYPE_IPV6, NG_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("6lo: No receivers for this packet found\n");
        ng_pktbuf_release(pkt);
    }
}

static inline bool _add_uncompr_disp(ng_pktsnip_t *pkt)
{
    ng_pktsnip_t *sixlowpan;
    uint8_t *disp;

    DEBUG("6lo: Send uncompressed\n");

    sixlowpan = ng_pktbuf_add(NULL, NULL, sizeof(uint8_t), NG_NETTYPE_SIXLOWPAN);

    if (sixlowpan == NULL) {
        return false;
    }
    sixlowpan->next = pkt->next;
    pkt->next = sixlowpan;
    disp = sixlowpan->data;
    disp[0] = SIXLOWPAN_UNCOMP;
    return true;
}

static void _send(ng_pktsnip_t *pkt)
{
    ng_netif_hdr_t *hdr;
    ng_pktsnip_t *pkt2;
    ng_sixlowpan_netif_t *iface;
    /* datagram_size: pure IPv6 packet without 6LoWPAN dispatches or compression */
    size_t datagram_size;

    if ((pkt == NULL) || (pkt->size < sizeof(ng_netif_hdr_t))) {
        DEBUG("6lo: Sending packet has no netif header\n");
        ng_pktbuf_release(pkt);
        return;
    }

    if ((pkt->next == NULL) || (pkt->next->type != NG_NETTYPE_IPV6)) {
        DEBUG("6lo: Sending packet has no IPv6 header\n");
        ng_pktbuf_release(pkt);
        return;
    }

    pkt2 = ng_pktbuf_start_write(pkt);

    if (pkt2 == NULL) {
        DEBUG("6lo: no space left in packet buffer\n");
        ng_pktbuf_release(pkt);
        return;
    }

    hdr = pkt2->data;
    iface = ng_sixlowpan_netif_get(hdr->if_pid);
    datagram_size = ng_pkt_len(pkt2->next);

    if (iface == NULL) {
        DEBUG("6lo: Can not get 6LoWPAN specific interface information.\n");
        ng_pktbuf_release(pkt);
        return;
    }

#ifdef MODULE_NG_SIXLOWPAN_IPHC
    if (iface->iphc_enabled) {
        if (!ng_sixlowpan_iphc_encode(pkt2)) {
            DEBUG("6lo: error on IPHC encoding\n");
            ng_pktbuf_release(pkt2);
            return;
        }
        /* IPHC dispatch does not count on dispatch length since it _shortens_
         * the datagram */
    }
    else {
        if (!_add_uncompr_disp(pkt2)) {
            /* adding uncompressed dispatch failed */
            DEBUG("6lo: no space left in packet buffer\n");
            ng_pktbuf_release(pkt2);
            return;
        }
    }
#else
    /* suppress clang-analyzer report about iface being not read */
    (void) iface;
    if (!_add_uncompr_disp(pkt2)) {
        /* adding uncompressed dispatch failed */
        DEBUG("6lo: no space left in packet buffer\n");
        ng_pktbuf_release(pkt2);
        return;
    }
#endif
    DEBUG("6lo: iface->max_frag_size = %" PRIu16 " for interface %"
          PRIkernel_pid "\n", iface->max_frag_size, hdr->if_pid);

    /* IP should not send anything here if it is not a 6LoWPAN interface,
     * so we don't need to check for NULL pointers */
    if (datagram_size <= iface->max_frag_size) {
        DEBUG("6lo: Send SND command for %p to %" PRIu16 "\n",
              (void *)pkt2, hdr->if_pid);
        ng_netapi_send(hdr->if_pid, pkt2);

        return;
    }
#ifdef MODULE_NG_SIXLOWPAN_FRAG
    else {
        DEBUG("6lo: Send fragmented (%u > %" PRIu16 ")\n",
              (unsigned int)datagram_size, iface->max_frag_size);
        ng_sixlowpan_frag_send(hdr->if_pid, pkt2, datagram_size);
    }
#else
    (void)datagram_size;
    DEBUG("6lo: packet too big (%u> %" PRIu16 ")\n",
          (unsigned int)datagram_size, iface->max_frag_size);
#endif
}

static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[NG_SIXLOWPAN_MSG_QUEUE_SIZE];
    ng_netreg_entry_t me_reg;

    (void)args;
    msg_init_queue(msg_q, NG_SIXLOWPAN_MSG_QUEUE_SIZE);

    me_reg.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;
    me_reg.pid = thread_getpid();

    /* register interest in all 6LoWPAN packets */
    ng_netreg_register(NG_NETTYPE_SIXLOWPAN, &me_reg);

    /* preinitialize ACK */
    reply.type = NG_NETAPI_MSG_TYPE_ACK;

    /* start event loop */
    while (1) {
        DEBUG("6lo: waiting for incoming message.\n");
        msg_receive(&msg);

        switch (msg.type) {
            case NG_NETAPI_MSG_TYPE_RCV:
                DEBUG("6lo: NG_NETDEV_MSG_TYPE_RCV received\n");
                _receive((ng_pktsnip_t *)msg.content.ptr);
                break;

            case NG_NETAPI_MSG_TYPE_SND:
                DEBUG("6lo: NG_NETDEV_MSG_TYPE_SND received\n");
                _send((ng_pktsnip_t *)msg.content.ptr);
                break;

            case NG_NETAPI_MSG_TYPE_GET:
            case NG_NETAPI_MSG_TYPE_SET:
                DEBUG("6lo: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                msg_reply(&msg, &reply);
                break;

            default:
                DEBUG("6lo: operation not supported\n");
                break;
        }
    }

    return NULL;
}

/** @} */
