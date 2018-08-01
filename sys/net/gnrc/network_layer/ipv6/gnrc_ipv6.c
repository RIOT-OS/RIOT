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
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "cpu_conf.h"
#include "kernel_types.h"
#include "net/gnrc.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/gnrc/sixlowpan/nd.h"
#include "net/protnum.h"
#include "thread.h"
#include "utlist.h"

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/ipv6/whitelist.h"
#include "net/gnrc/ipv6/blacklist.h"

#include "net/gnrc/ipv6.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _MAX_L2_ADDR_LEN    (8U)

#if ENABLE_DEBUG
static char _stack[GNRC_IPV6_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_IPV6_STACK_SIZE];
#endif

#ifdef MODULE_FIB
#include "net/fib.h"
#include "net/fib/table.h"
/**
 * @brief buffer to store the entries in the IPv6 forwarding table
 */
static fib_entry_t _fib_entries[GNRC_IPV6_FIB_TABLE_SIZE];

/**
 * @brief the IPv6 forwarding table
 */
fib_table_t gnrc_ipv6_fib_table;
#endif

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

kernel_pid_t gnrc_ipv6_pid = KERNEL_PID_UNDEF;

/* handles GNRC_NETAPI_MSG_TYPE_RCV commands */
static void _receive(gnrc_pktsnip_t *pkt);
/* Sends packet over the appropriate interface(s).
 * prep_hdr: prepare header for sending (call to _fill_ipv6_hdr()), otherwise
 * assume it is already prepared */
static void _send(gnrc_pktsnip_t *pkt, bool prep_hdr);
/* Main event loop for IPv6 */
static void *_event_loop(void *args);

/* Handles encapsulated IPv6 packets: http://tools.ietf.org/html/rfc2473 */
static void _decapsulate(gnrc_pktsnip_t *pkt);

kernel_pid_t gnrc_ipv6_init(void)
{
    if (gnrc_ipv6_pid == KERNEL_PID_UNDEF) {
        gnrc_ipv6_pid = thread_create(_stack, sizeof(_stack), GNRC_IPV6_PRIO,
                                      THREAD_CREATE_STACKTEST,
                                      _event_loop, NULL, "ipv6");
    }

#ifdef MODULE_FIB
    gnrc_ipv6_fib_table.data.entries = _fib_entries;
    gnrc_ipv6_fib_table.table_type = FIB_TABLE_TYPE_SH;
    gnrc_ipv6_fib_table.size = GNRC_IPV6_FIB_TABLE_SIZE;
    fib_init(&gnrc_ipv6_fib_table);
#endif

    return gnrc_ipv6_pid;
}

static void _dispatch_next_header(gnrc_pktsnip_t *current, gnrc_pktsnip_t *pkt,
                                  uint8_t nh, bool interested);

/*
 *         current                 pkt
 *         |                       |
 *         v                       v
 * IPv6 <- IPv6_EXT <- IPv6_EXT <- UNDEF
 */
void gnrc_ipv6_demux(gnrc_netif_t *netif, gnrc_pktsnip_t *current,
                     gnrc_pktsnip_t *pkt, uint8_t nh)
{
    bool interested = false;

    current->type = gnrc_nettype_from_protnum(nh);

    switch (nh) {
#ifdef MODULE_GNRC_ICMPV6
        case PROTNUM_ICMPV6:
            assert(current == pkt);
            interested = true;
            break;
#endif
#ifdef MODULE_GNRC_IPV6_EXT
        case PROTNUM_IPV6_EXT_HOPOPT:
        case PROTNUM_IPV6_EXT_DST:
        case PROTNUM_IPV6_EXT_RH:
        case PROTNUM_IPV6_EXT_FRAG:
        case PROTNUM_IPV6_EXT_AH:
        case PROTNUM_IPV6_EXT_ESP:
        case PROTNUM_IPV6_EXT_MOB:
            interested = true;

            break;
#endif
        case PROTNUM_IPV6:
            assert(current == pkt);
            interested = true;

            break;
        default:
            (void)netif;
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC_NHC
            /* second statement is true for small 6LoWPAN NHC decompressed frames
             * since in this case it looks like
             *
             * * GNRC_NETTYPE_UNDEF <- pkt
             * v
             * * GNRC_NETTYPE_UDP <- current
             * v
             * * GNRC_NETTYPE_EXT
             * v
             * * GNRC_NETTYPE_IPV6
             */
            assert((current == pkt) || (current == pkt->next));
#else
            assert(current == pkt);
#endif
            break;
    }

    _dispatch_next_header(current, pkt, nh, interested);

    if (!interested) {
        return;
    }

    switch (nh) {
#ifdef MODULE_GNRC_ICMPV6
        case PROTNUM_ICMPV6:
            DEBUG("ipv6: handle ICMPv6 packet (nh = %u)\n", nh);
            gnrc_icmpv6_demux(netif, pkt);
            return;
#endif
#ifdef MODULE_GNRC_IPV6_EXT
        case PROTNUM_IPV6_EXT_HOPOPT:
        case PROTNUM_IPV6_EXT_DST:
        case PROTNUM_IPV6_EXT_RH:
        case PROTNUM_IPV6_EXT_FRAG:
        case PROTNUM_IPV6_EXT_AH:
        case PROTNUM_IPV6_EXT_ESP:
        case PROTNUM_IPV6_EXT_MOB:
            DEBUG("ipv6: handle extension header (nh = %u)\n", nh);

            gnrc_ipv6_ext_demux(netif, current, pkt, nh);

            return;
#endif
        case PROTNUM_IPV6:
            DEBUG("ipv6: handle encapsulated IPv6 packet (nh = %u)\n", nh);
            _decapsulate(pkt);
            return;
        default:
            assert(false);
            break;
    }

    assert(false);
}

ipv6_hdr_t *gnrc_ipv6_get_header(gnrc_pktsnip_t *pkt)
{
    ipv6_hdr_t *hdr = NULL;
    gnrc_pktsnip_t *tmp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    if ((tmp != NULL) && (tmp->data != NULL) && ipv6_hdr_is(tmp->data)) {
        hdr = ((ipv6_hdr_t*) tmp->data);
    }

    return hdr;
}

/* internal functions */
static void _dispatch_next_header(gnrc_pktsnip_t *current, gnrc_pktsnip_t *pkt,
                                  uint8_t nh, bool interested)
{
#ifdef MODULE_GNRC_IPV6_EXT
    const bool should_dispatch_current_type = ((current->type != GNRC_NETTYPE_IPV6_EXT) ||
                                               (current->next->type == GNRC_NETTYPE_IPV6));
#else
    const bool should_dispatch_current_type = (current->next->type == GNRC_NETTYPE_IPV6);
#endif

    DEBUG("ipv6: forward nh = %u to other threads\n", nh);

    /* dispatch IPv6 extension header only once */
    if (should_dispatch_current_type) {
        bool should_release = (!gnrc_netreg_lookup(GNRC_NETTYPE_IPV6, nh)) &&
                              (!interested);

        if (!should_release) {
            gnrc_pktbuf_hold(pkt, 1);   /* don't remove from packet buffer in
                                         * next dispatch */
        }
        if (gnrc_netapi_dispatch_receive(current->type,
                                         GNRC_NETREG_DEMUX_CTX_ALL,
                                         pkt) == 0) {
            gnrc_pktbuf_release(pkt);
        }

        if (should_release) {
            return;
        }
    }
    if (interested) {
        gnrc_pktbuf_hold(pkt, 1);   /* don't remove from packet buffer in
                                     * next dispatch */
    }
    if (gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6, nh, pkt) == 0) {
        gnrc_pktbuf_release(pkt);
    }
}

static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[GNRC_IPV6_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                            sched_active_pid);

    (void)args;
    msg_init_queue(msg_q, GNRC_IPV6_MSG_QUEUE_SIZE);

    /* register interest in all IPv6 packets */
    gnrc_netreg_register(GNRC_NETTYPE_IPV6, &me_reg);

    /* preinitialize ACK */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    /* start event loop */
    while (1) {
        DEBUG("ipv6: waiting for incoming message.\n");
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("ipv6: GNRC_NETAPI_MSG_TYPE_RCV received\n");
                _receive(msg.content.ptr);
                break;

            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("ipv6: GNRC_NETAPI_MSG_TYPE_SND received\n");
                _send(msg.content.ptr, true);
                break;

            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("ipv6: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                msg_reply(&msg, &reply);
                break;

            case GNRC_IPV6_NIB_SND_UC_NS:
            case GNRC_IPV6_NIB_SND_MC_NS:
            case GNRC_IPV6_NIB_SND_NA:
            case GNRC_IPV6_NIB_SEARCH_RTR:
            case GNRC_IPV6_NIB_REPLY_RS:
            case GNRC_IPV6_NIB_SND_MC_RA:
            case GNRC_IPV6_NIB_REACH_TIMEOUT:
            case GNRC_IPV6_NIB_DELAY_TIMEOUT:
            case GNRC_IPV6_NIB_ADDR_REG_TIMEOUT:
            case GNRC_IPV6_NIB_ABR_TIMEOUT:
            case GNRC_IPV6_NIB_PFX_TIMEOUT:
            case GNRC_IPV6_NIB_RTR_TIMEOUT:
            case GNRC_IPV6_NIB_RECALC_REACH_TIME:
            case GNRC_IPV6_NIB_REREG_ADDRESS:
            case GNRC_IPV6_NIB_DAD:
            case GNRC_IPV6_NIB_VALID_ADDR:
                DEBUG("ipv6: NIB timer event received\n");
                gnrc_ipv6_nib_handle_timer_event(msg.content.ptr, msg.type);
                break;
            default:
                break;
        }
    }

    return NULL;
}

static void _send_to_iface(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    assert(netif != NULL);
    ((gnrc_netif_hdr_t *)pkt->data)->if_pid = netif->pid;
    if (gnrc_pkt_len(pkt->next) > netif->ipv6.mtu) {
        DEBUG("ipv6: packet too big\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
#ifdef MODULE_NETSTATS_IPV6
    netif->ipv6.stats.tx_success++;
    netif->ipv6.stats.tx_bytes += gnrc_pkt_len(pkt->next);
#endif

#ifdef MODULE_GNRC_SIXLOWPAN
    if (gnrc_netif_is_6ln(netif)) {
        DEBUG("ipv6: send to 6LoWPAN instead\n");
        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_SIXLOWPAN, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
            DEBUG("ipv6: no 6LoWPAN thread found\n");
            gnrc_pktbuf_release(pkt);
        }
        return;
    }
#endif
    if (gnrc_netapi_send(netif->pid, pkt) < 1) {
        DEBUG("ipv6: unable to send packet\n");
        gnrc_pktbuf_release(pkt);
    }
}

static gnrc_pktsnip_t *_create_netif_hdr(uint8_t *dst_l2addr,
                                         unsigned dst_l2addr_len,
                                         gnrc_pktsnip_t *pkt,
                                         uint8_t flags)
{
    gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build(NULL, 0, dst_l2addr, dst_l2addr_len);
    gnrc_netif_hdr_t *hdr;

    if (netif_hdr == NULL) {
        DEBUG("ipv6: error on interface header allocation, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    hdr = netif_hdr->data;
    /* previous netif header might have been allocated by some higher layer
     * to provide some flags (provided to us via netif_flags). */
    hdr->flags = flags;

    /* add netif_hdr to front of the pkt list */
    LL_PREPEND(pkt, netif_hdr);

    return pkt;
}

static bool _is_ipv6_hdr(gnrc_pktsnip_t *hdr)
{
#ifdef MODULE_GNRC_IPV6_EXT
    return (hdr->type == GNRC_NETTYPE_IPV6) ||
           (hdr->type == GNRC_NETTYPE_IPV6_EXT);
#else
    return (hdr->type == GNRC_NETTYPE_IPV6);
#endif
}

static int _fill_ipv6_hdr(gnrc_netif_t *netif, gnrc_pktsnip_t *ipv6)
{
    int res;
    ipv6_hdr_t *hdr = ipv6->data;
    gnrc_pktsnip_t *payload, *prev;

    hdr->len = byteorder_htons(gnrc_pkt_len(ipv6->next));
    DEBUG("ipv6: set payload length to %u (network byteorder %04" PRIx16 ")\n",
          (unsigned)byteorder_ntohs(hdr->len), hdr->len.u16);

    /* check if e.g. extension header was not already marked */
    if (hdr->nh == PROTNUM_RESERVED) {
        hdr->nh = gnrc_nettype_to_protnum(ipv6->next->type);

        /* if still reserved: mark no next header */
        if (hdr->nh == PROTNUM_RESERVED) {
            hdr->nh = PROTNUM_IPV6_NONXT;
        }
    }

    DEBUG("ipv6: set next header to %u\n", hdr->nh);

    if (hdr->hl == 0) {
        if (netif == NULL) {
            hdr->hl = GNRC_NETIF_DEFAULT_HL;
        }
        else {
            hdr->hl = netif->cur_hl;
        }
    }

    if (ipv6_addr_is_unspecified(&hdr->src)) {
        if (ipv6_addr_is_loopback(&hdr->dst)) {
            ipv6_addr_set_loopback(&hdr->src);
        }
        else {
            ipv6_addr_t *src = gnrc_netif_ipv6_addr_best_src(netif, &hdr->dst,
                                                             false);

            if (src != NULL) {
                DEBUG("ipv6: set packet source to %s\n",
                      ipv6_addr_to_str(addr_str, src, sizeof(addr_str)));
                memcpy(&hdr->src, src, sizeof(ipv6_addr_t));
            }
            /* Otherwise leave unspecified */
        }
    }

    DEBUG("ipv6: write protect up to payload to calculate checksum\n");
    payload = ipv6;
    prev = ipv6;
    do {
        /* IPv6 header itself was already write-protected in caller function,
         * just write protect extension headers and payload header */
        if ((payload = gnrc_pktbuf_start_write(payload->next)) == NULL) {
            DEBUG("ipv6: unable to get write access to IPv6 extension or payload header\n");
            /* packet duplicated to this point will be released by caller,
             * original packet by other subscriber */
            return -ENOMEM;
        }
        prev->next = payload;
        prev = payload;
    } while (_is_ipv6_hdr(payload));
    DEBUG("ipv6: calculate checksum for upper header.\n");
    if ((res = gnrc_netreg_calc_csum(payload, ipv6)) < 0) {
        if (res != -ENOENT) {   /* if there is no checksum we are okay */
            DEBUG("ipv6: checksum calculation failed.\n");
            /* packet will be released by caller */
            return res;
        }
    }

    return 0;
}

static bool _safe_fill_ipv6_hdr(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt,
                                bool prep_hdr)
{
    if (prep_hdr && (_fill_ipv6_hdr(netif, pkt) < 0)) {
        /* error on filling up header */
        gnrc_pktbuf_release(pkt);
        return false;
    }
    return true;
}

/* functions for sending */
static void _send_unicast(gnrc_pktsnip_t *pkt, bool prep_hdr,
                          gnrc_netif_t *netif, ipv6_hdr_t *ipv6_hdr,
                          uint8_t netif_hdr_flags)
{
    gnrc_ipv6_nib_nc_t nce;

    DEBUG("ipv6: send unicast\n");
    if (gnrc_ipv6_nib_get_next_hop_l2addr(&ipv6_hdr->dst, netif, pkt,
                                          &nce) < 0) {
        /* packet is released by NIB */
        DEBUG("ipv6: no link-layer address or interface for next hop to %s",
              ipv6_addr_to_str(addr_str, &ipv6_hdr->dst, sizeof(addr_str)));
        return;
    }
    netif = gnrc_netif_get_by_pid(gnrc_ipv6_nib_nc_get_iface(&nce));
    assert(netif != NULL);
    if (_safe_fill_ipv6_hdr(netif, pkt, prep_hdr)) {
        DEBUG("ipv6: add interface header to packet\n");
        if ((pkt = _create_netif_hdr(nce.l2addr, nce.l2addr_len, pkt,
                                     netif_hdr_flags)) == NULL) {
            return;
        }
        DEBUG("ipv6: send unicast over interface %" PRIkernel_pid "\n",
              netif->pid);
        /* and send to interface */
#ifdef MODULE_NETSTATS_IPV6
        netif->ipv6.stats.tx_unicast_count++;
#endif
        _send_to_iface(netif, pkt);
    }
}

static inline void _send_multicast_over_iface(gnrc_pktsnip_t *pkt,
                                              gnrc_netif_t *netif,
                                              uint8_t netif_hdr_flags)
{
    if ((pkt = _create_netif_hdr(NULL, 0, pkt,
                                 netif_hdr_flags |
                                 GNRC_NETIF_HDR_FLAGS_MULTICAST)) == NULL) {
        return;
    }
    DEBUG("ipv6: send multicast over interface %" PRIkernel_pid "\n", netif->pid);
#ifdef MODULE_NETSTATS_IPV6
    netif->ipv6.stats.tx_mcast_count++;
#endif
    /* and send to interface */
    _send_to_iface(netif, pkt);
}

static void _send_multicast(gnrc_pktsnip_t *pkt, bool prep_hdr,
                            gnrc_netif_t *netif, uint8_t netif_hdr_flags)
{
    size_t ifnum = 0;

    if (netif == NULL) {
        ifnum = gnrc_netif_numof();
        /* throw away packet if no one is interested */
        if (ifnum == 0) {
            DEBUG("ipv6: no interfaces registered, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
    }


#if GNRC_NETIF_NUMOF > 1
    /* interface not given: send over all interfaces */
    if (netif == NULL) {
        /* send packet to link layer */
        gnrc_pktbuf_hold(pkt, ifnum - 1);

        while ((netif = gnrc_netif_iter(netif))) {
            if (prep_hdr) {
                DEBUG("ipv6: prepare IPv6 header for sending\n");
                /* need to get second write access (duplication) to fill IPv6
                 * header interface-local */
                gnrc_pktsnip_t *tmp = gnrc_pktbuf_start_write(pkt);

                if (tmp == NULL) {
                    DEBUG("ipv6: unable to get write access to IPv6 header, "
                          "for interface %" PRIkernel_pid "\n", netif->pid);
                    gnrc_pktbuf_release(pkt);
                    return;
                }
                if (_fill_ipv6_hdr(netif, tmp) < 0) {
                    /* error on filling up header */
                    if (tmp != pkt) {
                        gnrc_pktbuf_release(tmp);
                    }
                    gnrc_pktbuf_release(pkt);
                    return;
                }
            }
            _send_multicast_over_iface(pkt, netif, netif_hdr_flags);
        }
    }
    else {
        if (_safe_fill_ipv6_hdr(netif, pkt, prep_hdr)) {
            _send_multicast_over_iface(pkt, netif, netif_hdr_flags);
        }
    }
#else   /* GNRC_NETIF_NUMOF */
    (void)ifnum; /* not used in this build branch */
    if (netif == NULL) {
        netif = gnrc_netif_iter(NULL);

        /* allocate interface header */
        if ((pkt = _create_netif_hdr(NULL, 0, pkt, netif_hdr_flags)) == NULL) {
            return;
        }
    }
    if (_safe_fill_ipv6_hdr(netif, pkt, prep_hdr)) {
        _send_multicast_over_iface(pkt, netif, netif_hdr_flags);
    }
#endif  /* GNRC_NETIF_NUMOF */
}

static void _send_to_self(gnrc_pktsnip_t *pkt, bool prep_hdr,
                          gnrc_netif_t *netif)
{
    uint8_t *rcv_data;
    gnrc_pktsnip_t *ptr = pkt, *rcv_pkt;

    if (!_safe_fill_ipv6_hdr(netif, pkt, prep_hdr)) {
        return;
    }
    rcv_pkt = gnrc_pktbuf_add(NULL, NULL, gnrc_pkt_len(pkt), GNRC_NETTYPE_IPV6);

    if (rcv_pkt == NULL) {
        DEBUG("ipv6: error on generating loopback packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    rcv_data = rcv_pkt->data;

    /* "reverse" packet (by making it one snip as if received from NIC) */
    while (ptr != NULL) {
        memcpy(rcv_data, ptr->data, ptr->size);
        rcv_data += ptr->size;
        ptr = ptr->next;
    }

    gnrc_pktbuf_release(pkt);

    DEBUG("ipv6: packet is addressed to myself => loopback\n");

    if (gnrc_netapi_receive(gnrc_ipv6_pid, rcv_pkt) < 1) {
        DEBUG("ipv6: unable to deliver packet\n");
        gnrc_pktbuf_release(rcv_pkt);
    }
}

static void _send(gnrc_pktsnip_t *pkt, bool prep_hdr)
{
    gnrc_netif_t *netif = NULL;
    gnrc_pktsnip_t *tmp_pkt;
    ipv6_hdr_t *ipv6_hdr;
    uint8_t netif_hdr_flags = 0U;

    /* get IPv6 snip and (if present) generic interface header */
    if (pkt->type == GNRC_NETTYPE_NETIF) {
        /* If there is already a netif header (routing protocols and
         * neighbor discovery might add them to preset sending interface or
         * higher layers wants to provide flags to the interface ) */
        const gnrc_netif_hdr_t *netif_hdr = pkt->data;

        netif = gnrc_netif_get_by_pid(((gnrc_netif_hdr_t *)pkt->data)->if_pid);
        /* discard broadcast and multicast flags because those could be
         * potentially wrong (dst is later checked to assure that multicast is
         * set if dst is a multicast address) */
        netif_hdr_flags = netif_hdr->flags &
                          ~(GNRC_NETIF_HDR_FLAGS_BROADCAST |
                            GNRC_NETIF_HDR_FLAGS_MULTICAST);

        tmp_pkt = gnrc_pktbuf_start_write(pkt);
        if (tmp_pkt == NULL) {
            DEBUG("ipv6: unable to get write access to netif header, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        /* discard to avoid complex checks for correctness (will be re-added
         * with correct addresses anyway as for the case were there is no
         * netif header provided)
         * Also re-establish temporary pointer used for write protection as
         * actual pointer */
        pkt = gnrc_pktbuf_remove_snip(tmp_pkt, tmp_pkt);
    }
    if (pkt->type != GNRC_NETTYPE_IPV6) {
        DEBUG("ipv6: unexpected packet type\n");
        gnrc_pktbuf_release_error(pkt, EINVAL);
        return;
    }
    tmp_pkt = gnrc_pktbuf_start_write(pkt);
    if (tmp_pkt == NULL) {
        DEBUG("ipv6: unable to get write access to IPv6 header, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp_pkt;

    ipv6_hdr = pkt->data;

    if (ipv6_addr_is_multicast(&ipv6_hdr->dst)) {
        _send_multicast(pkt, prep_hdr, netif, netif_hdr_flags);
    }
    else {
        gnrc_netif_t *tmp_netif = gnrc_netif_get_by_ipv6_addr(&ipv6_hdr->dst);

        if (ipv6_addr_is_loopback(&ipv6_hdr->dst) ||    /* dst is loopback address */
            /* or dst registered to a local interface */
            (tmp_netif != NULL)) {
            _send_to_self(pkt, prep_hdr, tmp_netif);
        }
        else {
            _send_unicast(pkt, prep_hdr, netif, ipv6_hdr, netif_hdr_flags);
        }
    }
}

/* functions for receiving */
static inline bool _pkt_not_for_me(gnrc_netif_t **netif, ipv6_hdr_t *hdr)
{
    if (ipv6_addr_is_loopback(&hdr->dst)) {
        return false;
    }
    else if ((!ipv6_addr_is_link_local(&hdr->dst)) ||
             (*netif == NULL)) {
        *netif = gnrc_netif_get_by_ipv6_addr(&hdr->dst);
        return (*netif == NULL);
    }
    else {
        return (gnrc_netif_get_by_ipv6_addr(&hdr->dst) == NULL);
    }
}

static void _receive(gnrc_pktsnip_t *pkt)
{
    gnrc_netif_t *netif = NULL;
    gnrc_pktsnip_t *ipv6, *netif_hdr, *first_ext;
    ipv6_hdr_t *hdr;

    assert(pkt != NULL);

    netif_hdr = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);

    if (netif_hdr != NULL) {
        netif = gnrc_netif_get_by_pid(((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid);
#ifdef MODULE_NETSTATS_IPV6
        assert(netif != NULL);
        netstats_t *stats = &netif->ipv6.stats;
        stats->rx_count++;
        stats->rx_bytes += (gnrc_pkt_len(pkt) - netif_hdr->size);
#endif
    }

    first_ext = pkt;

    for (ipv6 = pkt; ipv6 != NULL; ipv6 = ipv6->next) { /* find IPv6 header if already marked */
        if ((ipv6->type == GNRC_NETTYPE_IPV6) && (ipv6->size == sizeof(ipv6_hdr_t)) &&
            (ipv6->data != NULL) && (ipv6_hdr_is(ipv6->data))) {
            break;
        }

        first_ext = ipv6;
    }

    if (ipv6 == NULL) {
        if ((pkt->data == NULL) || !ipv6_hdr_is(pkt->data)) {
            DEBUG("ipv6: Received packet was not IPv6, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
#ifdef MODULE_GNRC_IPV6_WHITELIST
        if (!gnrc_ipv6_whitelisted(&((ipv6_hdr_t *)(pkt->data))->src)) {
            DEBUG("ipv6: Source address not whitelisted, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
#endif
#ifdef MODULE_GNRC_IPV6_BLACKLIST
        if (gnrc_ipv6_blacklisted(&((ipv6_hdr_t *)(pkt->data))->src)) {
            DEBUG("ipv6: Source address blacklisted, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
#endif
        /* seize ipv6 as a temporary variable */
        ipv6 = gnrc_pktbuf_start_write(pkt);

        if (ipv6 == NULL) {
            DEBUG("ipv6: unable to get write access to packet, drop it\n");
            gnrc_pktbuf_release(pkt);
            return;
        }

        pkt = ipv6;     /* reset pkt from temporary variable */

        ipv6 = gnrc_pktbuf_mark(pkt, sizeof(ipv6_hdr_t), GNRC_NETTYPE_IPV6);

        first_ext = pkt;
        pkt->type = GNRC_NETTYPE_UNDEF; /* snip is no longer IPv6 */

        if (ipv6 == NULL) {
            DEBUG("ipv6: error marking IPv6 header, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
    }
#ifdef MODULE_GNRC_IPV6_WHITELIST
    else if (!gnrc_ipv6_whitelisted(&((ipv6_hdr_t *)(ipv6->data))->src)) {
        /* if ipv6 header already marked*/
        DEBUG("ipv6: Source address not whitelisted, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
#endif
#ifdef MODULE_GNRC_IPV6_BLACKLIST
    else if (gnrc_ipv6_blacklisted(&((ipv6_hdr_t *)(ipv6->data))->src)) {
        /* if ipv6 header already marked*/
        DEBUG("ipv6: Source address blacklisted, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
#endif

    /* extract header */
    hdr = (ipv6_hdr_t *)ipv6->data;

    uint16_t ipv6_len = byteorder_ntohs(hdr->len);

    if ((ipv6_len == 0) && (hdr->nh != PROTNUM_IPV6_NONXT)) {
        /* this doesn't even make sense */
        DEBUG("ipv6: payload length 0, but next header not NONXT\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    /* if available, remove any padding that was added by lower layers
     * to fulfill their minimum size requirements (e.g. ethernet) */
    else if ((ipv6 != pkt) && (ipv6_len < pkt->size)) {
        gnrc_pktbuf_realloc_data(pkt, byteorder_ntohs(hdr->len));
    }
    else if (ipv6_len > (gnrc_pkt_len_upto(pkt, GNRC_NETTYPE_IPV6) - sizeof(ipv6_hdr_t))) {
        DEBUG("ipv6: invalid payload length: %d, actual: %d, dropping packet\n",
              (int) byteorder_ntohs(hdr->len),
              (int) (gnrc_pkt_len_upto(pkt, GNRC_NETTYPE_IPV6) - sizeof(ipv6_hdr_t)));
        gnrc_pktbuf_release(pkt);
        return;
    }

    DEBUG("ipv6: Received (src = %s, ",
          ipv6_addr_to_str(addr_str, &(hdr->src), sizeof(addr_str)));
    DEBUG("dst = %s, next header = %u, length = %" PRIu16 ")\n",
          ipv6_addr_to_str(addr_str, &(hdr->dst), sizeof(addr_str)),
          hdr->nh, byteorder_ntohs(hdr->len));

    if (_pkt_not_for_me(&netif, hdr)) { /* if packet is not for me */
        DEBUG("ipv6: packet destination not this host\n");

#ifdef MODULE_GNRC_IPV6_ROUTER    /* only routers redirect */
        /* redirect to next hop */
        DEBUG("ipv6: decrement hop limit to %u\n", (uint8_t) (hdr->hl - 1));

        /* RFC 4291, section 2.5.6 states: "Routers must not forward any
         * packets with Link-Local source or destination addresses to other
         * links."
         */
        if ((ipv6_addr_is_link_local(&(hdr->src))) || (ipv6_addr_is_link_local(&(hdr->dst)))) {
            DEBUG("ipv6: do not forward packets with link-local source or"
                  " destination address\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        /* TODO: check if receiving interface is router */
        else if (--(hdr->hl) > 0) {  /* drop packets that *reach* Hop Limit 0 */
            gnrc_pktsnip_t *reversed_pkt = NULL, *ptr = pkt;

            DEBUG("ipv6: forward packet to next hop\n");

            /* pkt might not be writable yet, if header was given above */
            ipv6 = gnrc_pktbuf_start_write(ipv6);
            if (ipv6 == NULL) {
                DEBUG("ipv6: unable to get write access to packet: dropping it\n");
                gnrc_pktbuf_release(pkt);
                return;
            }

            /* remove L2 headers around IPV6 */
            netif_hdr = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
            if (netif_hdr != NULL) {
                gnrc_pktbuf_remove_snip(pkt, netif_hdr);
            }

            /* reverse packet snip list order */
            while (ptr != NULL) {
                gnrc_pktsnip_t *next;
                ptr = gnrc_pktbuf_start_write(ptr);     /* duplicate if not already done */
                if (ptr == NULL) {
                    DEBUG("ipv6: unable to get write access to packet: dropping it\n");
                    gnrc_pktbuf_release(reversed_pkt);
                    gnrc_pktbuf_release(pkt);
                    return;
                }
                next = ptr->next;
                ptr->next = reversed_pkt;
                reversed_pkt = ptr;
                ptr = next;
            }
            _send(reversed_pkt, false);
            return;
        }
        else {
            DEBUG("ipv6: hop limit reached 0: drop packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }

#else  /* MODULE_GNRC_IPV6_ROUTER */
        DEBUG("ipv6: dropping packet\n");
        /* non rounting hosts just drop the packet */
        gnrc_pktbuf_release(pkt);
        return;
#endif /* MODULE_GNRC_IPV6_ROUTER */
    }

    /* IPv6 internal demuxing (ICMPv6, Extension headers etc.) */
    gnrc_ipv6_demux(netif, first_ext, pkt, hdr->nh);
}

static void _decapsulate(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *ptr = pkt;

    pkt->type = GNRC_NETTYPE_UNDEF; /* prevent payload (the encapsulated packet)
                                     * from being removed */

    /* Remove encapsulating IPv6 header */
    while ((ptr->next != NULL) && (ptr->next->type == GNRC_NETTYPE_IPV6)) {
        gnrc_pktbuf_remove_snip(pkt, pkt->next);
    }

    pkt->type = GNRC_NETTYPE_IPV6;

    _receive(pkt);
}

/** @} */
