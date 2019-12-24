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

#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/sixlowpan.h"
#include "net/gnrc/sixlowpan/frag.h"
#include "net/gnrc/sixlowpan/frag/rb.h"
#include "net/gnrc/sixlowpan/iphc.h"
#include "net/gnrc/netif.h"
#include "net/sixlowpan.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static kernel_pid_t _pid = KERNEL_PID_UNDEF;

#if ENABLE_DEBUG
static char _stack[GNRC_SIXLOWPAN_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_SIXLOWPAN_STACK_SIZE];
#endif


/* handles GNRC_NETAPI_MSG_TYPE_RCV commands */
static void _receive(gnrc_pktsnip_t *pkt);
/* handles GNRC_NETAPI_MSG_TYPE_SND commands */
static void _send(gnrc_pktsnip_t *pkt);
/* Main event loop for 6LoWPAN */
static void *_event_loop(void *args);

kernel_pid_t gnrc_sixlowpan_init(void)
{
    if (_pid > KERNEL_PID_UNDEF) {
        return _pid;
    }

    _pid = thread_create(_stack, sizeof(_stack), GNRC_SIXLOWPAN_PRIO,
                         THREAD_CREATE_STACKTEST, _event_loop, NULL, "6lo");

    return _pid;
}

kernel_pid_t gnrc_sixlowpan_get_pid(void)
{
    return _pid;
}

void gnrc_sixlowpan_dispatch_recv(gnrc_pktsnip_t *pkt, void *context,
                                  unsigned page)
{
    gnrc_nettype_t type;

    (void)context;
    (void)page;
#ifndef MODULE_GNRC_IPV6
    type = GNRC_NETTYPE_UNDEF;
    for (gnrc_pktsnip_t *ptr = pkt; (ptr || (type == GNRC_NETTYPE_UNDEF));
         ptr = ptr->next) {
        if ((ptr->next) && (ptr->next->type == GNRC_NETTYPE_NETIF)) {
            type = ptr->type;
            break;
        }
    }
#else   /* MODULE_GNRC_IPV6 */
    /* just assume normal IPv6 traffic */
    type = GNRC_NETTYPE_IPV6;
#endif  /* MODULE_GNRC_IPV6 */
    if (!gnrc_netapi_dispatch_receive(type,
                                      GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("6lo: No receivers for this packet found\n");
        gnrc_pktbuf_release(pkt);
    }
}

void gnrc_sixlowpan_dispatch_send(gnrc_pktsnip_t *pkt, void *context,
                                  unsigned page)
{
    (void)context;
    (void)page;
    assert(pkt->type == GNRC_NETTYPE_NETIF);
    gnrc_netif_hdr_t *hdr = pkt->data;
    if (gnrc_netapi_send(hdr->if_pid, pkt) < 1) {
        DEBUG("6lo: unable to send %p over interface %u\n", (void *)pkt,
              hdr->if_pid);
        gnrc_pktbuf_release(pkt);
    }
}

void gnrc_sixlowpan_multiplex_by_size(gnrc_pktsnip_t *pkt,
                                      size_t orig_datagram_size,
                                      gnrc_netif_t *netif,
                                      unsigned page)
{
    assert(pkt != NULL);
    assert(netif != NULL);
    size_t datagram_size = gnrc_pkt_len(pkt->next);
    DEBUG("6lo: iface->sixlo.max_frag_size = %u for interface %i\n",
          netif->sixlo.max_frag_size, netif->pid);
    if ((netif->sixlo.max_frag_size == 0) ||
        (datagram_size <= netif->sixlo.max_frag_size)) {
        DEBUG("6lo: Dispatch for sending\n");
        gnrc_sixlowpan_dispatch_send(pkt, NULL, page);
    }
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG
    else if (orig_datagram_size <= SIXLOWPAN_FRAG_MAX_LEN) {
        DEBUG("6lo: Send fragmented (%u > %u)\n",
              (unsigned int)datagram_size, netif->sixlo.max_frag_size);
        gnrc_sixlowpan_frag_fb_t *fbuf;

        fbuf = gnrc_sixlowpan_frag_fb_get();
        if (fbuf == NULL) {
            DEBUG("6lo: Not enough resources to fragment packet. "
                  "Dropping packet\n");
            gnrc_pktbuf_release_error(pkt, ENOMEM);
            return;
        }
        fbuf->pkt = pkt;
        fbuf->datagram_size = orig_datagram_size;
        fbuf->tag = gnrc_sixlowpan_frag_fb_next_tag();
        /* Sending the first fragment has an offset==0 */
        fbuf->offset = 0;
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_HINT
        fbuf->hint.fragsz = 0;
#endif

        gnrc_sixlowpan_frag_send(pkt, fbuf, page);
    }
#endif
    else {
        (void)orig_datagram_size;
        DEBUG("6lo: packet too big (%u > %u)\n",
              (unsigned int)datagram_size, netif->sixlo.max_frag_size);
        gnrc_pktbuf_release_error(pkt, EMSGSIZE);
    }
}

static void _receive(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *payload;
    uint8_t *dispatch;

    /* seize payload as a temporary variable */
    payload = gnrc_pktbuf_start_write(pkt); /* need to duplicate since pkt->next
                                             * might get replaced */

    if (payload == NULL) {
        DEBUG("6lo: can not get write access on received packet\n");
#if defined(DEVELHELP) && ENABLE_DEBUG
        gnrc_pktbuf_stats();
#endif
        gnrc_pktbuf_release(pkt);
        return;
    }

    pkt = payload;  /* reset pkt from temporary variable */

    payload = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_SIXLOWPAN);

    if ((payload == NULL) || (payload->size < 1)) {
        DEBUG("6lo: Received packet has no 6LoWPAN payload\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    dispatch = payload->data;

    if (dispatch[0] == SIXLOWPAN_UNCOMP) {
        gnrc_pktsnip_t *sixlowpan;
        DEBUG("6lo: received uncompressed IPv6 packet\n");
        payload = gnrc_pktbuf_start_write(payload);

        if (payload == NULL) {
            DEBUG("6lo: can not get write access on received packet\n");
#if defined(DEVELHELP) && ENABLE_DEBUG
            gnrc_pktbuf_stats();
#endif
            gnrc_pktbuf_release(pkt);
            return;
        }

        /* packet is uncompressed: just mark and remove the dispatch */
        sixlowpan = gnrc_pktbuf_mark(payload, sizeof(uint8_t), GNRC_NETTYPE_SIXLOWPAN);

        if (sixlowpan == NULL) {
            DEBUG("6lo: can not mark 6LoWPAN dispatch\n");
            gnrc_pktbuf_release(pkt);
            return;
        }

        pkt = gnrc_pktbuf_remove_snip(pkt, sixlowpan);
#if defined(MODULE_CCN_LITE)
        payload->type = GNRC_NETTYPE_CCN;
#elif defined(MODULE_GNRC_IPV6)
        payload->type = GNRC_NETTYPE_IPV6;
#else
        payload->type = GNRC_NETTYPE_UNDEF;
#endif
    }
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG
    else if (sixlowpan_frag_is((sixlowpan_frag_t *)dispatch)) {
        DEBUG("6lo: received 6LoWPAN fragment\n");
        gnrc_sixlowpan_frag_recv(pkt, NULL, 0);
        return;
    }
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
    else if (sixlowpan_iphc_is(dispatch)) {
        DEBUG("6lo: received 6LoWPAN IPHC compressed datagram\n");
        gnrc_sixlowpan_iphc_recv(pkt, NULL, 0);
        return;
    }
#endif
    else {
        DEBUG("6lo: dispatch %02x... is not supported\n", dispatch[0]);
        gnrc_pktbuf_release(pkt);
        return;
    }
    gnrc_sixlowpan_dispatch_recv(pkt, NULL, 0);
}

static inline bool _add_uncompr_disp(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *sixlowpan;
    uint8_t *disp;

    DEBUG("6lo: Send uncompressed\n");

    sixlowpan = gnrc_pktbuf_add(NULL, NULL, sizeof(uint8_t), GNRC_NETTYPE_SIXLOWPAN);

    if (sixlowpan == NULL) {
        return false;
    }
    sixlowpan->next = pkt->next;
    pkt->next = sixlowpan;
    disp = sixlowpan->data;
    disp[0] = SIXLOWPAN_UNCOMP;
    return true;
}

static void _send(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *tmp;
    gnrc_netif_t *netif;
    /* datagram_size: pure IPv6 packet without 6LoWPAN dispatches or compression */
    size_t datagram_size;

    if ((pkt == NULL) || (pkt->size < sizeof(gnrc_netif_hdr_t))) {
        DEBUG("6lo: Sending packet has no netif header\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

#ifdef MODULE_GNRC_IPV6
    if ((pkt->next == NULL) || (pkt->next->type != GNRC_NETTYPE_IPV6)) {
        DEBUG("6lo: Sending packet has no IPv6 header\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
#endif

    tmp = gnrc_pktbuf_start_write(pkt);

    if (tmp == NULL) {
        DEBUG("6lo: no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp;
    netif = gnrc_netif_hdr_get_netif(pkt->data);
    datagram_size = gnrc_pkt_len(pkt->next);

    if (netif == NULL) {
        DEBUG("6lo: Can not get 6LoWPAN specific interface information.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
    if (netif->flags & GNRC_NETIF_FLAGS_6LO_HC) {
        gnrc_sixlowpan_iphc_send(pkt, NULL, 0);
        return;
    }
#endif
    if (!_add_uncompr_disp(pkt)) {
        /* adding uncompressed dispatch failed */
        DEBUG("6lo: no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    gnrc_sixlowpan_multiplex_by_size(pkt, datagram_size, netif, 0);
}

static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[GNRC_SIXLOWPAN_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                            sched_active_pid);

    (void)args;
    msg_init_queue(msg_q, GNRC_SIXLOWPAN_MSG_QUEUE_SIZE);

    /* register interest in all 6LoWPAN packets */
    gnrc_netreg_register(GNRC_NETTYPE_SIXLOWPAN, &me_reg);

    /* preinitialize ACK */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    /* start event loop */
    while (1) {
        DEBUG("6lo: waiting for incoming message.\n");
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("6lo: GNRC_NETDEV_MSG_TYPE_RCV received\n");
                _receive(msg.content.ptr);
                break;

            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("6lo: GNRC_NETDEV_MSG_TYPE_SND received\n");
                _send(msg.content.ptr);
                break;

            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("6lo: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                msg_reply(&msg, &reply);
                break;
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_FB
            case GNRC_SIXLOWPAN_FRAG_FB_SND_MSG:
                DEBUG("6lo: send fragmented event received\n");
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG
                gnrc_sixlowpan_frag_send(NULL, msg.content.ptr, 0);
#else   /* MODULE_GNRC_SIXLOWPAN_FRAG_FB */
                DEBUG("6lo: No fragmentation implementation available to sent\n");
                assert(false);
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_FB */
                break;
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_RB
            case GNRC_SIXLOWPAN_FRAG_RB_GC_MSG:
                DEBUG("6lo: garbage collect reassembly buffer event received\n");
                gnrc_sixlowpan_frag_rb_gc();
                break;
#endif

            default:
                DEBUG("6lo: operation not supported\n");
                break;
        }
    }

    return NULL;
}

/** @} */
