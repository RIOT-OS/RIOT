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
#include "net/gnrc/ndp.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/gnrc/sixlowpan/nd.h"
#include "net/gnrc/sixlowpan/nd/router.h"
#include "net/protnum.h"
#include "thread.h"
#include "utlist.h"

#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/ipv6/whitelist.h"

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

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

kernel_pid_t gnrc_ipv6_pid = KERNEL_PID_UNDEF;

/* handles GNRC_NETAPI_MSG_TYPE_RCV commands */
static void _receive(gnrc_pktsnip_t *pkt);
/* dispatches received IPv6 packet for upper layer */
static void _dispatch_rcv_pkt(gnrc_nettype_t type, uint32_t demux_ctx,
                              gnrc_pktsnip_t *pkt);
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
                                      CREATE_STACKTEST, _event_loop, NULL, "ipv6");
    }

#ifdef MODULE_FIB
    gnrc_ipv6_fib_table.data.entries = _fib_entries;
    gnrc_ipv6_fib_table.table_type = FIB_TABLE_TYPE_SH;
    gnrc_ipv6_fib_table.size = GNRC_IPV6_FIB_TABLE_SIZE;
    fib_init(&gnrc_ipv6_fib_table);
#endif

    return gnrc_ipv6_pid;
}

void gnrc_ipv6_demux(kernel_pid_t iface, gnrc_pktsnip_t *pkt, uint8_t nh)
{
    int receiver_num;

    pkt->type = gnrc_nettype_from_protnum(nh);

    switch (nh) {
#ifdef MODULE_GNRC_ICMPV6
        case PROTNUM_ICMPV6:
            DEBUG("ipv6: handle ICMPv6 packet (nh = %" PRIu8 ")\n", nh);
            gnrc_icmpv6_demux(iface, pkt);
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
            DEBUG("ipv6: handle extension header (nh = %" PRIu8 ")\n", nh);
            if (!gnrc_ipv6_ext_demux(iface, pkt, nh)) {
                DEBUG("ipv6: unable to parse extension headers.\n");
                gnrc_pktbuf_release(pkt);
                return;
            }
#endif
        case PROTNUM_IPV6:
            DEBUG("ipv6: handle encapsulated IPv6 packet (nh = %" PRIu8 ")\n", nh);
            _decapsulate(pkt);
            break;
        default:
            (void)iface;
            break;
    }

    DEBUG("ipv6: forward nh = %" PRIu8 " to other threads\n", nh);
    receiver_num = gnrc_netreg_num(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL) +
                   gnrc_netreg_num(GNRC_NETTYPE_IPV6, nh);

    if (receiver_num == 0) {
        DEBUG("ipv6: unable to forward packet as no one is interested in it\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    gnrc_pktbuf_hold(pkt, receiver_num - 1);    /* IPv6 is not interested anymore so `- 1` */

    /* XXX can't use gnrc_netapi_dispatch_receive() twice here since a call to that function
     *     implicitly hands all rights to the packet to one of the receiving threads. As a result,
     *     the second call to gnrc_netapi_dispatch_receive() would be invalid */
    _dispatch_rcv_pkt(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt);
    _dispatch_rcv_pkt(GNRC_NETTYPE_IPV6, nh, pkt);
}

/* internal functions */
static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[GNRC_IPV6_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t me_reg;

    (void)args;
    msg_init_queue(msg_q, GNRC_IPV6_MSG_QUEUE_SIZE);

    me_reg.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    me_reg.pid = thread_getpid();

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
                _receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("ipv6: GNRC_NETAPI_MSG_TYPE_SND received\n");
                _send((gnrc_pktsnip_t *)msg.content.ptr, true);
                break;

            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("ipv6: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                msg_reply(&msg, &reply);
                break;

#ifdef MODULE_GNRC_NDP
            case GNRC_NDP_MSG_RTR_TIMEOUT:
                DEBUG("ipv6: Router timeout received\n");
                ((gnrc_ipv6_nc_t *)msg.content.ptr)->flags &= ~GNRC_IPV6_NC_IS_ROUTER;
                break;

            case GNRC_NDP_MSG_ADDR_TIMEOUT:
                DEBUG("ipv6: Router advertisement timer event received\n");
                gnrc_ipv6_netif_remove_addr(KERNEL_PID_UNDEF,
                                            (ipv6_addr_t *)msg.content.ptr);
                break;

            case GNRC_NDP_MSG_NBR_SOL_RETRANS:
                DEBUG("ipv6: Neigbor solicitation retransmission timer event received\n");
                gnrc_ndp_retrans_nbr_sol((gnrc_ipv6_nc_t *)msg.content.ptr);
                break;

            case GNRC_NDP_MSG_NC_STATE_TIMEOUT:
                DEBUG("ipv6: Neigbor cache state timeout received\n");
                gnrc_ndp_state_timeout((gnrc_ipv6_nc_t *)msg.content.ptr);
                break;
#endif
#ifdef MODULE_GNRC_NDP_ROUTER
            case GNRC_NDP_MSG_RTR_ADV_RETRANS:
                DEBUG("ipv6: Router advertisement retransmission event received\n");
                gnrc_ndp_router_retrans_rtr_adv((gnrc_ipv6_netif_t *)msg.content.ptr);
                break;
            case GNRC_NDP_MSG_RTR_ADV_DELAY:
                DEBUG("ipv6: Delayed router advertisement event received\n");
                gnrc_ndp_router_send_rtr_adv((gnrc_ipv6_nc_t *)msg.content.ptr);
                break;
#endif
#ifdef MODULE_GNRC_NDP_HOST
            case GNRC_NDP_MSG_RTR_SOL_RETRANS:
                DEBUG("ipv6: Router solicitation retransmission event received\n");
                gnrc_ndp_host_retrans_rtr_sol((gnrc_ipv6_netif_t *)msg.content.ptr);
                break;
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_ND
            case GNRC_SIXLOWPAN_ND_MSG_MC_RTR_SOL:
                DEBUG("ipv6: Multicast router solicitation event received\n");
                gnrc_sixlowpan_nd_mc_rtr_sol((gnrc_ipv6_netif_t *)msg.content.ptr);
                break;
            case GNRC_SIXLOWPAN_ND_MSG_UC_RTR_SOL:
                DEBUG("ipv6: Unicast router solicitation event received\n");
                gnrc_sixlowpan_nd_uc_rtr_sol((gnrc_ipv6_nc_t *)msg.content.ptr);
                break;
#   ifdef MODULE_GNRC_SIXLOWPAN_CTX
            case GNRC_SIXLOWPAN_ND_MSG_DELETE_CTX:
                DEBUG("ipv6: Delete 6LoWPAN context event received\n");
                gnrc_sixlowpan_ctx_remove(((((gnrc_sixlowpan_ctx_t *)msg.content.ptr)->flags_id) &
                                           GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK));
                break;
#   endif
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
            case GNRC_SIXLOWPAN_ND_MSG_ABR_TIMEOUT:
                DEBUG("ipv6: border router timeout event received\n");
                gnrc_sixlowpan_nd_router_abr_remove(
                        (gnrc_sixlowpan_nd_router_abr_t *)msg.content.ptr);
                break;
            case GNRC_SIXLOWPAN_ND_MSG_AR_TIMEOUT:
                DEBUG("ipv6: address registration timeout received\n");
                gnrc_sixlowpan_nd_router_gc_nc((gnrc_ipv6_nc_t *)msg.content.ptr);
                break;
            case GNRC_NDP_MSG_RTR_ADV_SIXLOWPAN_DELAY:
                DEBUG("ipv6: Delayed router advertisement event received\n");
                gnrc_ipv6_nc_t *nc_entry = (gnrc_ipv6_nc_t *)msg.content.ptr;
                gnrc_ndp_internal_send_rtr_adv(nc_entry->iface, NULL,
                                               &(nc_entry->ipv6_addr), false);
                break;
#endif
            default:
                break;
        }
    }

    return NULL;
}

static void _send_to_iface(kernel_pid_t iface, gnrc_pktsnip_t *pkt)
{
    ((gnrc_netif_hdr_t *)pkt->data)->if_pid = iface;
    gnrc_ipv6_netif_t *if_entry = gnrc_ipv6_netif_get(iface);

    assert(if_entry != NULL);
    if (gnrc_pkt_len(pkt->next) > if_entry->mtu) {
        DEBUG("ipv6: packet too big\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
#ifdef MODULE_GNRC_SIXLOWPAN
    if ((if_entry != NULL) && (if_entry->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN)) {
        DEBUG("ipv6: send to 6LoWPAN instead\n");
        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_SIXLOWPAN, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
            DEBUG("ipv6: no 6LoWPAN thread found");
            gnrc_pktbuf_release(pkt);
        }
        return;
    }
#endif
    if (gnrc_netapi_send(iface, pkt) < 1) {
        DEBUG("ipv6: unable to send packet\n");
        gnrc_pktbuf_release(pkt);
    }
}

/* functions for sending */
static void _send_unicast(kernel_pid_t iface, uint8_t *dst_l2addr,
                          uint16_t dst_l2addr_len, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *netif;

    if (pkt->type == GNRC_NETTYPE_NETIF) {
        /* great: someone already added a netif_hdr_t we assume it's wrong
         * to keep it simple
         * XXX: alternative would be to check if gnrc_netif_hdr_t::dst_l2addr_len
         * is long enough and only then to throw away the header. This causes
         * to much overhead IMHO */
        DEBUG("ipv6: removed old interface header\n");
        pkt = gnrc_pktbuf_remove_snip(pkt, pkt);
    }

    DEBUG("ipv6: add interface header to packet\n");
    netif = gnrc_netif_hdr_build(NULL, 0, dst_l2addr, dst_l2addr_len);

    if (netif == NULL) {
        DEBUG("ipv6: error on interface header allocation, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* add netif to front of the pkt list */
    LL_PREPEND(pkt, netif);

    DEBUG("ipv6: send unicast over interface %" PRIkernel_pid "\n", iface);
    /* and send to interface */
    _send_to_iface(iface, pkt);
}

static int _fill_ipv6_hdr(kernel_pid_t iface, gnrc_pktsnip_t *ipv6,
                          gnrc_pktsnip_t *payload)
{
    int res;
    ipv6_hdr_t *hdr = ipv6->data;

    hdr->len = byteorder_htons(gnrc_pkt_len(payload));
    DEBUG("ipv6: set payload length to %zu (network byteorder %04" PRIx16 ")\n",
          gnrc_pkt_len(payload), hdr->len.u16);

    /* check if e.g. extension header was not already marked */
    if (hdr->nh == PROTNUM_RESERVED) {
        hdr->nh = gnrc_nettype_to_protnum(payload->type);

        /* if still reserved: mark no next header */
        if (hdr->nh == PROTNUM_RESERVED) {
            hdr->nh = PROTNUM_IPV6_NONXT;
        }
    }

    DEBUG("ipv6: set next header to %" PRIu8 "\n", hdr->nh);

    if (hdr->hl == 0) {
        if (iface == KERNEL_PID_UNDEF) {
            hdr->hl = GNRC_IPV6_NETIF_DEFAULT_HL;
        }
        else {
            hdr->hl = gnrc_ipv6_netif_get(iface)->cur_hl;
        }
    }

    if (ipv6_addr_is_unspecified(&hdr->src)) {
        if (ipv6_addr_is_loopback(&hdr->dst)) {
            ipv6_addr_set_loopback(&hdr->src);
        }
        else {
            ipv6_addr_t *src = gnrc_ipv6_netif_find_best_src_addr(iface, &hdr->dst);

            if (src != NULL) {
                DEBUG("ipv6: set packet source to %s\n",
                      ipv6_addr_to_str(addr_str, src, sizeof(addr_str)));
                memcpy(&hdr->src, src, sizeof(ipv6_addr_t));
            }
            /* Otherwise leave unspecified */
        }
    }

    DEBUG("ipv6: calculate checksum for upper header.\n");

    if ((res = gnrc_netreg_calc_csum(payload, ipv6)) < 0) {
        if (res != -ENOENT) {   /* if there is no checksum we are okay */
            DEBUG("ipv6: checksum calculation failed.\n");
            return res;
        }
    }

    return 0;
}

static inline void _send_multicast_over_iface(kernel_pid_t iface, gnrc_pktsnip_t *pkt)
{
    DEBUG("ipv6: send multicast over interface %" PRIkernel_pid "\n", iface);
    /* mark as multicast */
    ((gnrc_netif_hdr_t *)pkt->data)->flags |= GNRC_NETIF_HDR_FLAGS_MULTICAST;
    /* and send to interface */
    _send_to_iface(iface, pkt);
}

static void _send_multicast(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                            gnrc_pktsnip_t *ipv6, gnrc_pktsnip_t *payload,
                            bool prep_hdr)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t ifnum = 0;

    if (iface == KERNEL_PID_UNDEF) {
        /* get list of interfaces */
        ifnum = gnrc_netif_get(ifs);

        /* throw away packet if no one is interested */
        if (ifnum == 0) {
            DEBUG("ipv6: no interfaces registered, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
    }


#if GNRC_NETIF_NUMOF > 1
    /* netif header not present: send over all interfaces */
    if (iface == KERNEL_PID_UNDEF) {
        assert(pkt == ipv6);
        /* send packet to link layer */
        gnrc_pktbuf_hold(pkt, ifnum - 1);

        for (size_t i = 0; i < ifnum; i++) {
            gnrc_pktsnip_t *netif;
            if (prep_hdr) {
                /* need to get second write access (duplication) to fill IPv6
                 * header interface-local */
                gnrc_pktsnip_t *tmp = gnrc_pktbuf_start_write(pkt);
                gnrc_pktsnip_t *ptr = tmp->next;
                ipv6 = tmp;

                if (ipv6 == NULL) {
                    DEBUG("ipv6: unable to get write access to IPv6 header, "
                          "for interface %" PRIkernel_pid "\n", ifs[i]);
                    gnrc_pktbuf_release(pkt);
                    return;
                }

                /* multiple interfaces => possibly different source addresses
                 * => different checksums => duplication of payload needed */
                while (ptr != payload->next) {
                    /* duplicate everything including payload */
                    tmp->next = gnrc_pktbuf_start_write(ptr);
                    if (tmp->next == NULL) {
                        DEBUG("ipv6: unable to get write access to payload, drop it\n");
                        gnrc_pktbuf_release(ipv6);
                        return;
                    }
                    tmp = tmp->next;
                    ptr = ptr->next;
                }

                if (_fill_ipv6_hdr(ifs[i], ipv6, tmp) < 0) {
                    /* error on filling up header */
                    gnrc_pktbuf_release(ipv6);
                    return;
                }
            }

            /* allocate interface header */
            netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);

            if (netif == NULL) {
                DEBUG("ipv6: error on interface header allocation, "
                      "dropping packet\n");
                gnrc_pktbuf_release(ipv6);
                return;
            }

            LL_PREPEND(ipv6, netif);

            _send_multicast_over_iface(ifs[i], ipv6);
        }
    }
    else {
        /* iface != KERNEL_PID_UNDEF implies that netif header is present */
        assert(pkt != ipv6);
        if (prep_hdr) {
            if (_fill_ipv6_hdr(iface, ipv6, payload) < 0) {
                /* error on filling up header */
                gnrc_pktbuf_release(pkt);
                return;
            }
        }

        _send_multicast_over_iface(iface, pkt);
    }
#else   /* GNRC_NETIF_NUMOF */
    (void)ifnum; /* not used in this build branch */
    if (iface == KERNEL_PID_UNDEF) {
        gnrc_pktsnip_t *netif;
        iface = ifs[0];

        /* allocate interface header */
        netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);

        if (netif == NULL) {
            DEBUG("ipv6: error on interface header allocation, "
                  "dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }

        LL_PREPEND(pkt, netif);
    }

    if (prep_hdr) {
        if (_fill_ipv6_hdr(iface, ipv6, payload) < 0) {
            /* error on filling up header */
            gnrc_pktbuf_release(pkt);
            return;
        }
    }

    _send_multicast_over_iface(iface, pkt);
#endif  /* GNRC_NETIF_NUMOF */
}

static inline kernel_pid_t _next_hop_l2addr(uint8_t *l2addr, uint8_t *l2addr_len,
                                            kernel_pid_t iface, ipv6_addr_t *dst,
                                            gnrc_pktsnip_t *pkt)
{
    kernel_pid_t found_iface;
#if defined(MODULE_GNRC_SIXLOWPAN_ND)
    (void)pkt;
    found_iface = gnrc_sixlowpan_nd_next_hop_l2addr(l2addr, l2addr_len, iface, dst);
    if (found_iface > KERNEL_PID_UNDEF) {
        return found_iface;
    }
#endif
#if defined(MODULE_GNRC_NDP_NODE)
    found_iface = gnrc_ndp_node_next_hop_l2addr(l2addr, l2addr_len, iface, dst, pkt);
#elif !defined(MODULE_GNRC_SIXLOWPAN_ND) && defined(MODULE_GNRC_IPV6_NC)
    (void)pkt;
    gnrc_ipv6_nc_t *nc = gnrc_ipv6_nc_get(iface, dst);
    if ((nc == NULL) || !gnrc_ipv6_nc_is_reachable(nc)) {
        return KERNEL_PID_UNDEF;
    }
    found_iface = nc->iface;
    *l2addr_len = nc->l2_addr_len;
    memcpy(l2addr, nc->l2_addr, nc->l2_addr_len);
#elif !defined(MODULE_GNRC_SIXLOWPAN_ND)
    found_iface = KERNEL_PID_UNDEF;
    (void)l2addr;
    (void)l2addr_len;
    (void)iface;
    (void)dst;
    (void)pkt;
    *l2addr_len = 0;
#endif
    return found_iface;
}

static void _send(gnrc_pktsnip_t *pkt, bool prep_hdr)
{
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    gnrc_pktsnip_t *ipv6, *payload;
    ipv6_addr_t *tmp;
    ipv6_hdr_t *hdr;
    /* get IPv6 snip and (if present) generic interface header */
    if (pkt->type == GNRC_NETTYPE_NETIF) {
        /* If there is already a netif header (routing protocols and
         * neighbor discovery might add them to preset sending interface) */
        iface = ((gnrc_netif_hdr_t *)pkt->data)->if_pid;
        /* seize payload as temporary variable */
        ipv6 = gnrc_pktbuf_start_write(pkt); /* write protect for later removal
                                              * in _send_unicast() */
        if (ipv6 == NULL) {
            DEBUG("ipv6: unable to get write access to netif header, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt = ipv6;  /* Reset pkt from temporary variable */

        ipv6 = pkt->next;
    }
    else {
        ipv6 = pkt;
    }
    /* seize payload as temporary variable */
    payload = gnrc_pktbuf_start_write(ipv6);
    if (payload == NULL) {
        DEBUG("ipv6: unable to get write access to IPv6 header, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    if (ipv6 != pkt) {      /* in case packet has netif header */
        pkt->next = payload;/* pkt is already write-protected so we can do that */
    }
    else {
        pkt = payload;      /* pkt is the IPv6 header so we just write-protected it */
    }
    ipv6 = payload;  /* Reset ipv6 from temporary variable */

    hdr = ipv6->data;
    payload = ipv6->next;

    if (ipv6_addr_is_multicast(&hdr->dst)) {
        _send_multicast(iface, pkt, ipv6, payload, prep_hdr);
    }
    else if ((ipv6_addr_is_loopback(&hdr->dst)) ||      /* dst is loopback address */
             ((iface == KERNEL_PID_UNDEF) && /* or dst registered to any local interface */
              ((iface = gnrc_ipv6_netif_find_by_addr(&tmp, &hdr->dst)) != KERNEL_PID_UNDEF)) ||
             ((iface != KERNEL_PID_UNDEF) && /* or dst registered to given interface */
              (gnrc_ipv6_netif_find_addr(iface, &hdr->dst) != NULL))) {
        uint8_t *rcv_data;
        gnrc_pktsnip_t *ptr = ipv6, *rcv_pkt;

        if (prep_hdr) {
            if (_fill_ipv6_hdr(iface, ipv6, payload) < 0) {
                /* error on filling up header */
                gnrc_pktbuf_release(pkt);
                return;
            }
        }

        rcv_pkt = gnrc_pktbuf_add(NULL, NULL, gnrc_pkt_len(ipv6), GNRC_NETTYPE_IPV6);

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
    else {
        uint8_t l2addr_len = GNRC_IPV6_NC_L2_ADDR_MAX;
        uint8_t l2addr[l2addr_len];

        iface = _next_hop_l2addr(l2addr, &l2addr_len, iface, &hdr->dst, pkt);

        if (iface == KERNEL_PID_UNDEF) {
            DEBUG("ipv6: error determining next hop's link layer address\n");
            gnrc_pktbuf_release(pkt);
            return;
        }

        if (prep_hdr) {
            if (_fill_ipv6_hdr(iface, ipv6, payload) < 0) {
                /* error on filling up header */
                gnrc_pktbuf_release(pkt);
                return;
            }
        }

        _send_unicast(iface, l2addr, l2addr_len, pkt);
    }
}

/* functions for receiving */
static inline bool _pkt_not_for_me(kernel_pid_t *iface, ipv6_hdr_t *hdr)
{
    if (ipv6_addr_is_loopback(&hdr->dst)) {
        return false;
    }
    else if (*iface == KERNEL_PID_UNDEF) {
        *iface = gnrc_ipv6_netif_find_by_addr(NULL, &hdr->dst);
        return (*iface == KERNEL_PID_UNDEF);
    }
    else {
        return (gnrc_ipv6_netif_find_addr(*iface, &hdr->dst) == NULL);
    }
}

static void _dispatch_rcv_pkt(gnrc_nettype_t type, uint32_t demux_ctx,
                              gnrc_pktsnip_t *pkt)
{
    gnrc_netreg_entry_t *entry = gnrc_netreg_lookup(type, demux_ctx);

    while (entry) {
        DEBUG("ipv6: Send receive command for %p to %" PRIu16 "\n", (void *)pkt,
              entry->pid);
        if (gnrc_netapi_receive(entry->pid, pkt) < 1) {
            DEBUG("ipv6: unable to deliver packet\n");
            gnrc_pktbuf_release(pkt);
        }
        entry = gnrc_netreg_getnext(entry);
    }
}

static void _receive(gnrc_pktsnip_t *pkt)
{
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    gnrc_pktsnip_t *ipv6, *netif;
    ipv6_hdr_t *hdr;

    assert(pkt != NULL);

    LL_SEARCH_SCALAR(pkt, netif, type, GNRC_NETTYPE_NETIF);

    if (netif != NULL) {
        iface = ((gnrc_netif_hdr_t *)netif->data)->if_pid;
    }

    if ((pkt->next != NULL) && (pkt->next->type == GNRC_NETTYPE_IPV6) &&
        (pkt->next->size == sizeof(ipv6_hdr_t))) {
        /* IP header was already marked. Take it. */
        ipv6 = pkt->next;

        if (!ipv6_hdr_is(ipv6->data)) {
            DEBUG("ipv6: Received packet was not IPv6, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
#ifdef MODULE_GNRC_IPV6_WHITELIST
        if (!gnrc_ipv6_whitelisted(&((ipv6_hdr_t *)(ipv6->data))->src)) {
            DEBUG("ipv6: Source address not whitelisted, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
#endif
    }
    else {
        if (!ipv6_hdr_is(pkt->data)) {
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
        /* seize ipv6 as a temporary variable */
        ipv6 = gnrc_pktbuf_start_write(pkt);

        if (ipv6 == NULL) {
            DEBUG("ipv6: unable to get write access to packet, drop it\n");
            gnrc_pktbuf_release(pkt);
            return;
        }

        pkt = ipv6;     /* reset pkt from temporary variable */

        ipv6 = gnrc_pktbuf_mark(pkt, sizeof(ipv6_hdr_t), GNRC_NETTYPE_IPV6);

        if (ipv6 == NULL) {
            DEBUG("ipv6: error marking IPv6 header, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
    }

    /* extract header */
    hdr = (ipv6_hdr_t *)ipv6->data;

    /* if available, remove any padding that was added by lower layers
     * to fulfill their minimum size requirements (e.g. ethernet) */
    if (byteorder_ntohs(hdr->len) < pkt->size) {
        gnrc_pktbuf_realloc_data(pkt, byteorder_ntohs(hdr->len));
    }

    DEBUG("ipv6: Received (src = %s, ",
          ipv6_addr_to_str(addr_str, &(hdr->src), sizeof(addr_str)));
    DEBUG("dst = %s, next header = %" PRIu8 ", length = %" PRIu16 ")\n",
          ipv6_addr_to_str(addr_str, &(hdr->dst), sizeof(addr_str)),
          hdr->nh, byteorder_ntohs(hdr->len));

    if (_pkt_not_for_me(&iface, hdr)) { /* if packet is not for me */
        DEBUG("ipv6: packet destination not this host\n");

#ifdef MODULE_GNRC_IPV6_ROUTER    /* only routers redirect */
        /* redirect to next hop */
        DEBUG("ipv6: decrement hop limit to %" PRIu8 "\n", hdr->hl - 1);

        /* RFC 4291, section 2.5.6 states: "Routers must not forward any
         * packets with Link-Local source or destination addresses to other
         * links."
         */
        if ((ipv6_addr_is_link_local(&(hdr->src))) || (ipv6_addr_is_link_local(&(hdr->dst)))) {
            DEBUG("ipv6: do not forward packets with link-local source or"\
                  " destination address\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        /* TODO: check if receiving interface is router */
        else if (--(hdr->hl) > 0) {  /* drop packets that *reach* Hop Limit 0 */
            gnrc_pktsnip_t *tmp = pkt;

            DEBUG("ipv6: forward packet to next hop\n");

            /* pkt might not be writable yet, if header was given above */
            pkt = gnrc_pktbuf_start_write(tmp);
            ipv6 = gnrc_pktbuf_start_write(ipv6);

            if ((ipv6 == NULL) || (pkt == NULL)) {
                DEBUG("ipv6: unable to get write access to packet: dropping it\n");
                gnrc_pktbuf_release(tmp);
                return;
            }

            gnrc_pktbuf_release(ipv6->next);    /* remove headers around IPV6 */
            ipv6->next = pkt;                   /* reorder for sending */
            pkt->next = NULL;
            _send(ipv6, false);
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
    gnrc_ipv6_demux(iface, pkt, hdr->nh);
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
