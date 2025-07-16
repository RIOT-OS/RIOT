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
#include <kernel_defines.h>
#include <stdbool.h>

#include "byteorder.h"
#include "cpu_conf.h"
#include "sched.h"
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

#ifdef MODULE_GNRC_IPV6_EXT_FRAG
#include "net/gnrc/ipv6/ext/frag.h"
#endif

#ifdef MODULE_FIB
#include "net/fib.h"
#include "net/fib/table.h"
#endif

#ifdef MODULE_GNRC_RPL_SRH
#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/srh.h"
#endif

#include "net/gnrc/ipv6.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define _MAX_L2_ADDR_LEN    (8U)

static char _stack[GNRC_IPV6_STACK_SIZE + DEBUG_EXTRA_STACKSIZE];
static msg_t _msg_q[GNRC_IPV6_MSG_QUEUE_SIZE];

#ifdef MODULE_FIB
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

#ifdef MODULE_GNRC_IPV6_EXT_FRAG
static void _send_by_netif_hdr(gnrc_pktsnip_t *pkt);
#endif  /* MODULE_GNRC_IPV6_EXT_FRAG */
/* Main event loop for IPv6 */
static void *_event_loop(void *args);

kernel_pid_t gnrc_ipv6_init(void)
{
    if (gnrc_ipv6_pid == KERNEL_PID_UNDEF) {
        gnrc_ipv6_pid = thread_create(_stack, sizeof(_stack), GNRC_IPV6_PRIO,
                                      0,
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

static void _dispatch_next_header(gnrc_pktsnip_t *pkt, unsigned nh,
                                  bool interested);

static inline bool _gnrc_ipv6_is_interested(unsigned nh) {
#ifdef MODULE_GNRC_ICMPV6
    return (nh == PROTNUM_ICMPV6);
#else  /* MODULE_GNRC_ICMPV6 */
    return false;
#endif /* MODULE_GNRC_ICMPV6 */
}

static void _demux(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt, unsigned nh)
{
    pkt->type = gnrc_nettype_from_protnum(nh);
    _dispatch_next_header(pkt, nh, _gnrc_ipv6_is_interested(nh));
    switch (nh) {
#ifdef MODULE_GNRC_ICMPV6
        case PROTNUM_ICMPV6:
            DEBUG("ipv6: handle ICMPv6 packet (nh = %u)\n", nh);
            gnrc_icmpv6_demux(netif, pkt);
            break;
#endif /* MODULE_GNRC_ICMPV6 */
        default:
            break;
    }
}

ipv6_hdr_t *gnrc_ipv6_get_header(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *tmp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    if (tmp == NULL) {
        return NULL;
    }

    assert(tmp->data != NULL);
    assert(tmp->size >= sizeof(ipv6_hdr_t));
    assert(ipv6_hdr_is(tmp->data));

    return ((ipv6_hdr_t*) tmp->data);
}

/* internal functions */
static void _dispatch_next_header(gnrc_pktsnip_t *pkt, unsigned nh,
                                  bool interested)
{
    const bool has_nh_subs = (gnrc_netreg_num(GNRC_NETTYPE_IPV6, nh) > 0) ||
                             interested;

    DEBUG("ipv6: forward nh = %u to other threads\n", nh);

    if (has_nh_subs) {
        gnrc_pktbuf_hold(pkt, 1);   /* don't remove from packet buffer in
                                     * next dispatch */
    }
    if (gnrc_netapi_dispatch_receive(pkt->type,
                                     GNRC_NETREG_DEMUX_CTX_ALL,
                                     pkt) == 0) {
        gnrc_pktbuf_release(pkt);
    }
    if (!has_nh_subs) {
        /* we should exit early. pkt was already released above */
        return;
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
    msg_t msg, reply;
    gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                            thread_getpid());

    (void)args;
    msg_init_queue(_msg_q, GNRC_IPV6_MSG_QUEUE_SIZE);

    /* initialize fragmentation data-structures */
#ifdef MODULE_GNRC_IPV6_EXT_FRAG
    gnrc_ipv6_ext_frag_init();
#endif  /* MODULE_GNRC_IPV6_EXT_FRAG */
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

#ifdef MODULE_GNRC_IPV6_EXT_FRAG
            case GNRC_IPV6_EXT_FRAG_RBUF_GC:
                gnrc_ipv6_ext_frag_rbuf_gc();
                break;
            case GNRC_IPV6_EXT_FRAG_CONTINUE:
                DEBUG("ipv6: continue fragmenting packet\n");
                gnrc_ipv6_ext_frag_send(msg.content.ptr);
                break;
            case GNRC_IPV6_EXT_FRAG_SEND:
                DEBUG("ipv6: send fragment\n");
                _send_by_netif_hdr(msg.content.ptr);
                break;
#endif  /* MODULE_GNRC_IPV6_EXT_FRAG */
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
            case GNRC_IPV6_NIB_IFACE_UP:
                gnrc_ipv6_nib_iface_up(msg.content.ptr);
                break;
            case GNRC_IPV6_NIB_IFACE_DOWN:
                gnrc_ipv6_nib_iface_down(msg.content.ptr, false);
                break;
            default:
                break;
        }
    }

    return NULL;
}

static void _send_to_iface(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    const ipv6_hdr_t *hdr = pkt->next->data;

    (void)hdr;  /* only used for DEBUG messages */
    assert(netif != NULL);
    gnrc_netif_hdr_set_netif(pkt->data, netif);
    if (gnrc_pkt_len(pkt->next) > netif->ipv6.mtu) {
        DEBUG("ipv6: packet too big\n");
        gnrc_icmpv6_error_pkt_too_big_send(netif->ipv6.mtu, pkt);
        gnrc_pktbuf_release_error(pkt, EMSGSIZE);
        return;
    }
    DEBUG("ipv6: Sending (src = %s, ",
          ipv6_addr_to_str(addr_str, &hdr->src, sizeof(addr_str)));
    DEBUG("dst = %s, next header = %u, length = %u)\n",
          ipv6_addr_to_str(addr_str, &hdr->dst, sizeof(addr_str)), hdr->nh,
          byteorder_ntohs(hdr->len));
#ifdef MODULE_NETSTATS_IPV6
    /* This is read from the netif thread. To prevent data corruptions, we
     * have to guarantee mutually exclusive access */
    unsigned irq_state = irq_disable();
    netif->ipv6.stats.tx_success++;
    netif->ipv6.stats.tx_bytes += gnrc_pkt_len(pkt->next);
    irq_restore(irq_state);
#endif

#ifdef MODULE_GNRC_SIXLOWPAN
    if (gnrc_netif_is_6lo(netif)) {
        DEBUG("ipv6: send to 6LoWPAN instead\n");
        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_SIXLOWPAN, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
            DEBUG("ipv6: no 6LoWPAN thread found\n");
            gnrc_pktbuf_release(pkt);
        }
        return;
    }
#endif
    if (gnrc_netif_send(netif, pkt) < 1) {
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
    return gnrc_pkt_prepend(pkt, netif_hdr);
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
        if (ipv6->next == NULL) {
            hdr->nh = PROTNUM_IPV6_NONXT;
        }
        else {
            hdr->nh = gnrc_nettype_to_protnum(ipv6->next->type);

            /* if still reserved: mark no next header */
            if (hdr->nh == PROTNUM_RESERVED) {
                hdr->nh = PROTNUM_IPV6_NONXT;
            }
        }
    }

    DEBUG("ipv6: set next header to %u\n", hdr->nh);

    if (hdr->hl == 0) {
        if (netif == NULL) {
            hdr->hl = CONFIG_GNRC_NETIF_DEFAULT_HL;
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
    else {
        bool invalid_src;
        int idx;

        gnrc_netif_acquire(netif);
        invalid_src = ((!ipv6_addr_is_loopback(&hdr->dst)) &&
                       (idx = gnrc_netif_ipv6_addr_idx(netif, &hdr->src)) >= 0) &&
            (gnrc_netif_ipv6_addr_get_state(netif, idx) != GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);
        gnrc_netif_release(netif);
        if (invalid_src) {
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
            gnrc_pktsnip_t *icmpv6 = gnrc_pktsnip_search_type(ipv6,
                                                              GNRC_NETTYPE_ICMPV6);
            icmpv6_hdr_t *icmpv6_hdr;

            if (icmpv6 != NULL) {
                icmpv6_hdr = icmpv6->data;
            }
            if ((icmpv6 == NULL) ||
                ((icmpv6_hdr->type != ICMPV6_RTR_SOL) &&
                 (icmpv6_hdr->type != ICMPV6_NBR_SOL))) {
                DEBUG("ipv6: preset packet source address %s is invalid\n",
                      ipv6_addr_to_str(addr_str, &hdr->src, sizeof(addr_str)));
                return -EADDRNOTAVAIL;
            }
#else   /* CONFIG_GNRC_IPV6_NIB_6LN */
            DEBUG("ipv6: preset packet source address %s is invalid\n",
                  ipv6_addr_to_str(addr_str, &hdr->src, sizeof(addr_str)));
            return -EADDRNOTAVAIL;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN */
        }
    }

    DEBUG("ipv6: write protect up to payload to calculate checksum\n");
    payload = ipv6;
    prev = ipv6;
    while (_is_ipv6_hdr(payload) && (payload->next != NULL)) {
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
    }
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
static bool _fragment_pkt_if_needed(gnrc_pktsnip_t *pkt,
                                    gnrc_netif_t *netif,
                                    bool from_me)
{
#ifdef MODULE_GNRC_IPV6_EXT_FRAG
    /* TODO: get path MTU when PMTU discovery is implemented */
    unsigned path_mtu = netif->ipv6.mtu;

    if (from_me && (gnrc_pkt_len(pkt->next) > path_mtu)) {
        gnrc_netif_hdr_t *hdr = pkt->data;
        hdr->if_pid = netif->pid;
        gnrc_ipv6_ext_frag_send_pkt(pkt, path_mtu);
        return true;
    }
#else   /* MODULE_GNRC_IPV6_EXT_FRAG */
    (void)pkt;
    (void)netif;
    (void)from_me;
#endif  /* MODULE_GNRC_IPV6_EXT_FRAG */
    return false;
}

#ifdef MODULE_GNRC_IPV6_EXT_FRAG
static void _send_by_netif_hdr(gnrc_pktsnip_t *pkt)
{
    assert(pkt->type == GNRC_NETTYPE_NETIF);
    gnrc_netif_t *netif = gnrc_netif_hdr_get_netif(pkt->data);

    _send_to_iface(netif, pkt);
}
#endif  /* MODULE_GNRC_IPV6_EXT_FRAG */

static void _send_unicast(gnrc_pktsnip_t *pkt, bool prep_hdr,
                          gnrc_netif_t *netif, ipv6_hdr_t *ipv6_hdr,
                          uint8_t netif_hdr_flags)
{
    gnrc_ipv6_nib_nc_t nce;

    DEBUG("ipv6: send unicast\n");
#ifdef MODULE_GNRC_RPL_SRH
    if (ipv6_addr_is_link_local(&ipv6_hdr->dst) && !ipv6_addr_is_link_local(&ipv6_hdr->src)) {
        /* Link-local address return netif = 0, use the netif of RPL*/
        gnrc_netif_t *iface = gnrc_netif_get_by_prefix(gnrc_rpl_get_root_dodag_id());
        netif = gnrc_netif_get_by_pid(iface->pid);
    }
#endif /* MODULE_GNRC_RPL_SRH */
    if (gnrc_ipv6_nib_get_next_hop_l2addr(&ipv6_hdr->dst, netif, pkt,
                                          &nce) < 0) {
        /* packet is released by NIB */
        DEBUG("ipv6: no link-layer address or interface for next hop to %s\n",
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
        /* prep_hdr => The packet is from me */
        if (_fragment_pkt_if_needed(pkt, netif, prep_hdr)) {
            DEBUG("ipv6: packet is fragmented\n");
            return;
        }
        DEBUG("ipv6: send unicast over interface %" PRIkernel_pid "\n",
              netif->pid);
        /* and send to interface */
#ifdef MODULE_NETSTATS_IPV6
        /* This is read from the netif thread. To prevent data corruptions, we
         * have to guarantee mutually exclusive access */
        unsigned irq_state = irq_disable();
        netif->ipv6.stats.tx_unicast_count++;
        irq_restore(irq_state);
#endif
        _send_to_iface(netif, pkt);
    }
}

static inline void _send_multicast_over_iface(gnrc_pktsnip_t *pkt,
                                              bool prep_hdr,
                                              gnrc_netif_t *netif,
                                              uint8_t netif_hdr_flags)
{
    if ((pkt = _create_netif_hdr(NULL, 0, pkt,
                                 netif_hdr_flags |
                                 GNRC_NETIF_HDR_FLAGS_MULTICAST)) == NULL) {
        return;
    }
    /* prep_hdr => The packet is from me */
    if (_fragment_pkt_if_needed(pkt, netif, prep_hdr)) {
        DEBUG("ipv6: packet is fragmented\n");
        return;
    }
    DEBUG("ipv6: send multicast over interface %" PRIkernel_pid "\n", netif->pid);
#ifdef MODULE_NETSTATS_IPV6
    /* This is read from the netif thread. To prevent data corruptions, we
     * have to guarantee mutually exclusive access */
    unsigned irq_state = irq_disable();
    netif->ipv6.stats.tx_mcast_count++;
    irq_restore(irq_state);
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

    if (!gnrc_netif_highlander()) {
        /* interface not given: send over all interfaces */
        if (netif == NULL) {
            /* the packet is replicated over all interfaces that is's being sent on */
            gnrc_pktbuf_hold(pkt, ifnum - 1);

            while ((netif = gnrc_netif_iter(netif))) {
                gnrc_pktsnip_t *send_pkt = pkt;
                /* for !prep_hdr just use pkt as we don't duplicate IPv6 header as
                 * it is already filled and thus isn't filled with potentially
                 * interface-specific data */
                if (prep_hdr) {
                    DEBUG("ipv6: prepare IPv6 header for sending\n");
                    /* need to get second write access (duplication) to fill IPv6
                     * header with interface-specific data */
                    send_pkt = gnrc_pktbuf_start_write(pkt);

                    if (send_pkt == NULL) {
                        DEBUG("ipv6: unable to get write access to IPv6 header, "
                              "for interface %" PRIkernel_pid "\n", netif->pid);
                        gnrc_pktbuf_release(pkt);
                        return;
                    }
                    if (_fill_ipv6_hdr(netif, send_pkt) < 0) {
                        /* error on filling up header */
                        if (send_pkt != pkt) {
                            gnrc_pktbuf_release(send_pkt);
                        }
                        gnrc_pktbuf_release(pkt);
                        return;
                    }
                }
                _send_multicast_over_iface(send_pkt, prep_hdr, netif,
                                           netif_hdr_flags);
            }
        }
        else {
            if (_safe_fill_ipv6_hdr(netif, pkt, prep_hdr)) {
                _send_multicast_over_iface(pkt, prep_hdr, netif, netif_hdr_flags);
            }
        }
    }
    else {
        (void)ifnum; /* not used in this build branch */
        if (netif == NULL) {
            netif = gnrc_netif_iter(NULL);

            /* allocate interface header */
            if ((pkt = _create_netif_hdr(NULL, 0, pkt, netif_hdr_flags)) == NULL) {
                return;
            }
        }
        if (_safe_fill_ipv6_hdr(netif, pkt, prep_hdr)) {
            _send_multicast_over_iface(pkt, prep_hdr, netif, netif_hdr_flags);
        }
    }
}

static void _send_to_self(gnrc_pktsnip_t *pkt, bool prep_hdr,
                          gnrc_netif_t *netif)
{
    /* _safe_fill_ipv6_hdr releases pkt on error */
    if (!_safe_fill_ipv6_hdr(netif, pkt, prep_hdr)) {
        DEBUG("ipv6: error looping packet to sender.\n");
        return;
    }
    /* no netif header so we just merge the whole packet. */
    else if (gnrc_pktbuf_merge(pkt) != 0) {
        DEBUG("ipv6: error looping packet to sender.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    DEBUG("ipv6: packet is addressed to myself => loopback\n");

    if (gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6,
                                     GNRC_NETREG_DEMUX_CTX_ALL,
                                     pkt) == 0) {
        DEBUG("ipv6: unable to deliver looped back packet\n");
        gnrc_pktbuf_release(pkt);
    }
}

#ifdef MODULE_GNRC_RPL_SRH
/* function for sending in non-storing mode */
static inline bool _pkt_from_me(ipv6_hdr_t *hdr)
{
    if (ipv6_addr_is_loopback(&hdr->dst) || ipv6_addr_is_unspecified(&hdr->src)) {
        return true;
    }

    else {
        return !(gnrc_netif_get_by_ipv6_addr(&hdr->src) == NULL);
    }
}
#endif /* MODULE_GNRC_RPL_SRH */

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

        netif = gnrc_netif_hdr_get_netif(pkt->data);
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
    if (ipv6_addr_is_unspecified(&((ipv6_hdr_t *)pkt->data)->dst)) {
        DEBUG("ipv6: destination address is unspecified address (::), "
              "dropping packet \n");
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

    ipv6_hdr = (ipv6_hdr_t *)pkt->data;
#ifdef MODULE_GNRC_RPL_SRH
    /* If the packet is from me, insert SRH if needed */
    if (get_is_root() &&
        !ipv6_addr_is_link_local(&(ipv6_hdr->dst)) &&
        _pkt_from_me(ipv6_hdr)) {
        if (gnrc_rpl_get_root_dodag_id() == NULL) {
            DEBUG("ipv6: no DODAG ID available, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt = gnrc_rpl_srh_insert(pkt, ipv6_hdr);
        if (pkt == NULL) {
            DEBUG("ipv6: SRH insertion failed, dropping packet\n");
            return;
        }

        /* Set the src to the global address
         * to avoid being set to the local one in _safe_fill_ipv6_hdr */
        memcpy(&ipv6_hdr->src, gnrc_rpl_get_root_dodag_id(),\
            sizeof(ipv6_addr_t));
    }
#endif /* MODULE_GNRC_RPL_SRH */

    if (pkt == NULL) {
        return;
    }
    if (ipv6_addr_is_multicast(&ipv6_hdr->dst)) {
        _send_multicast(pkt, prep_hdr, netif, netif_hdr_flags);
    }
    else {
        gnrc_netif_t *tmp_netif = gnrc_netif_get_by_ipv6_addr(&ipv6_hdr->dst);

        /* only consider link-local addresses on the interface we are sending on */
        if (tmp_netif != netif && ipv6_addr_is_link_local(&ipv6_hdr->dst)) {
            tmp_netif = NULL;
        }

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
    gnrc_pktsnip_t *ipv6, *netif_hdr;
    ipv6_hdr_t *hdr;
    uint8_t first_nh;

    assert(pkt != NULL);

    netif_hdr = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);

    if (netif_hdr != NULL) {
        netif = gnrc_netif_hdr_get_netif(netif_hdr->data);
#ifdef MODULE_NETSTATS_IPV6
        assert(netif != NULL);
        /* This is read from the netif thread. To prevent data corruptions, we
         * have to guarantee mutually exclusive access */
        unsigned irq_state = irq_disable();
        netstats_t *stats = &netif->ipv6.stats;
        stats->rx_count++;
        stats->rx_bytes += (gnrc_pkt_len(pkt) - netif_hdr->size);
        irq_restore(irq_state);
#endif
    }

    if ((pkt->data == NULL) || (pkt->size < sizeof(ipv6_hdr_t)) ||
        !ipv6_hdr_is(pkt->data)) {
        DEBUG("ipv6: Received packet was not IPv6, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
#ifdef MODULE_GNRC_IPV6_WHITELIST
    else if (!gnrc_ipv6_whitelisted(&((ipv6_hdr_t *)(pkt->data))->src)) {
        DEBUG("ipv6: Source address not whitelisted, dropping packet\n");
        gnrc_icmpv6_error_dst_unr_send(ICMPV6_ERROR_DST_UNR_PROHIB, pkt);
        gnrc_pktbuf_release(pkt);
        return;
    }
#endif
#ifdef MODULE_GNRC_IPV6_BLACKLIST
    else if (gnrc_ipv6_blacklisted(&((ipv6_hdr_t *)(pkt->data))->src)) {
        DEBUG("ipv6: Source address blacklisted, dropping packet\n");
        gnrc_icmpv6_error_dst_unr_send(ICMPV6_ERROR_DST_UNR_PROHIB, pkt);
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

    pkt->type = GNRC_NETTYPE_UNDEF; /* snip is no longer IPv6 */

    if (ipv6 == NULL) {
        DEBUG("ipv6: error marking IPv6 header, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    /* extract header */
    hdr = (ipv6_hdr_t *)ipv6->data;

    if (hdr->hl == 0) {
        /* This is an illegal value in any case, not just in case of a
         * forwarding step, so *do not* check it together with ((--hdr->hl) > 0)
         * in forwarding code below */
        DEBUG("ipv6: packet was received with hop-limit 0\n");
        gnrc_icmpv6_error_time_exc_send(ICMPV6_ERROR_TIME_EXC_HL, pkt);
        gnrc_pktbuf_release_error(pkt, ETIMEDOUT);
        return;
    }

    uint16_t ipv6_len = byteorder_ntohs(hdr->len);
    first_nh = hdr->nh;

    if ((ipv6_len == 0) && (first_nh != PROTNUM_IPV6_NONXT)) {
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
        gnrc_icmpv6_error_param_prob_send(ICMPV6_ERROR_PARAM_PROB_HDR_FIELD,
                                          &(hdr->len), pkt);
        gnrc_pktbuf_release_error(pkt, EINVAL);
        return;
    }

    DEBUG("ipv6: Received (src = %s, ",
          ipv6_addr_to_str(addr_str, &(hdr->src), sizeof(addr_str)));
    DEBUG("dst = %s, next header = %u, length = %" PRIu16 ")\n",
          ipv6_addr_to_str(addr_str, &(hdr->dst), sizeof(addr_str)),
          first_nh, byteorder_ntohs(hdr->len));

    if ((pkt = gnrc_ipv6_ext_process_hopopt(pkt, &first_nh)) == NULL) {
        DEBUG("ipv6: packet's extension header was erroneous or packet was "
              "consumed due to it\n");
        return;
    }
    if (_pkt_not_for_me(&netif, hdr)) { /* if packet is not for me */
        DEBUG("ipv6: packet destination not this host\n");

#ifdef MODULE_GNRC_IPV6_ROUTER    /* only routers redirect */
        /* redirect to next hop */
        DEBUG("ipv6: decrement hop limit to %u\n", (uint8_t) (hdr->hl - 1));

        /* RFC 4291, section 2.5.6 states: "Routers must not forward any
         * packets with Link-Local source or destination addresses to other
         * links."
         * In RPL Non-storing, we need to forward the packets with
         * link-local source address
         */
        bool route_local = true;
#ifdef MODULE_GNRC_RPL_SRH
        route_local = ipv6_addr_is_link_local(&(hdr->dst));
        DEBUG("ipv6: do not forward packets with link-local destination address\n");

#else /* MODULE_GNRC_RPL_SRH */
        route_local =
            ((ipv6_addr_is_link_local(&(hdr->src))) || (ipv6_addr_is_link_local(&(hdr->dst))));
        DEBUG("ipv6: do not forward packets with link-local source or"
              " destination address\n");
#endif /* MODULE_GNRC_RPL_SRH */
        if (route_local) {
#ifdef MODULE_GNRC_ICMPV6_ERROR
            if (ipv6_addr_is_link_local(&(hdr->src)) &&
                !ipv6_addr_is_link_local(&(hdr->dst))) {
                gnrc_icmpv6_error_dst_unr_send(ICMPV6_ERROR_DST_UNR_SCOPE, pkt);
            }
            else
            if (!ipv6_addr_is_multicast(&(hdr->dst))) {
                gnrc_icmpv6_error_dst_unr_send(ICMPV6_ERROR_DST_UNR_ADDR, pkt);
            }
#endif
            gnrc_pktbuf_release(pkt);
            return;
        }
        /* TODO: check if receiving interface is router */
        else if (--(hdr->hl) > 0) {  /* drop packets that *reach* Hop Limit 0 */
            DEBUG("ipv6: forward packet to next hop\n");

            /* remove L2 headers around IPV6 */
            if (netif_hdr != NULL) {
                gnrc_pktbuf_remove_snip(pkt, netif_hdr);
            }
            pkt = gnrc_pktbuf_reverse_snips(pkt);
            if (pkt == NULL) {
                DEBUG("ipv6: unable to reverse pkt from receive order to send "
                      "order; dropping it\n");
                return;
            }
#ifdef MODULE_GNRC_RPL_SRH
            if (get_is_root()) {
                pkt = gnrc_rpl_srh_insert(pkt, hdr);
            }
            _send(pkt, false);
#else /* MODULE_GNRC_RPL_SRH */
            _send(pkt, false);
#endif /* MODULE_GNRC_RPL_SRH */
            return;
        }
        else {
            DEBUG("ipv6: hop limit reached 0: drop packet\n");
            gnrc_icmpv6_error_time_exc_send(ICMPV6_ERROR_TIME_EXC_HL, pkt);
            gnrc_pktbuf_release_error(pkt, ETIMEDOUT);
            return;
        }

#else  /* MODULE_GNRC_IPV6_ROUTER */
        DEBUG("ipv6: dropping packet\n");
        /* non rounting hosts just drop the packet */
        gnrc_pktbuf_release(pkt);
        return;
#endif /* MODULE_GNRC_IPV6_ROUTER */
    }
    if ((pkt = gnrc_ipv6_ext_process_all(pkt, &first_nh)) == NULL) {
        DEBUG("ipv6: packet was consumed in extension header handling\n");
        return;
    }
    _demux(netif, pkt, first_nh);
}

/** @} */
