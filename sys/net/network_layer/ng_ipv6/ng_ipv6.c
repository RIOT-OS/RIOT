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
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "cpu-conf.h"
#include "kernel_types.h"
#include "net/ng_netbase.h"
#include "net/ng_protnum.h"
#include "thread.h"
#include "utlist.h"

#include "net/ng_ipv6/nc.h"
#include "net/ng_ipv6/netif.h"

#include "net/ng_ipv6.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _MAX_L2_ADDR_LEN    (8U)

static char _stack[NG_IPV6_STACK_SIZE];
static kernel_pid_t _pid = KERNEL_PID_UNDEF;

#if ENABLE_DEBUG
static char addr_str[NG_IPV6_ADDR_MAX_STR_LEN];
#endif

/* handles NG_NETAPI_MSG_TYPE_RCV commands */
static void _receive(ng_pktsnip_t *pkt);
/* dispatches received IPv6 packet for upper layer */
static void _dispatch_rcv_pkt(ng_nettype_t type, uint32_t demux_ctx,
                              ng_pktsnip_t *pkt);
/* Sends packet over the appropriate interface(s).
 * prep_hdr: prepare header for sending (call to _fill_ipv6_hdr()), otherwise
 * assume it is already prepared */
static void _send(ng_pktsnip_t *pkt, bool prep_hdr);
/* Main event loop for IPv6 */
static void *_event_loop(void *args);

kernel_pid_t ng_ipv6_init(void)
{
    if (_pid == KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, NG_IPV6_STACK_SIZE, NG_IPV6_PRIO,
                             CREATE_STACKTEST, _event_loop, NULL, "ipv6");
    }

    return _pid;
}

void ng_ipv6_demux(kernel_pid_t iface, ng_pktsnip_t *pkt, uint8_t nh)
{
    int receiver_num;

    pkt->type = ng_nettype_from_protnum(nh);

    /* TODO: add ICMPv6 and extension header handling */
    (void)iface;    /* will be used by that */

    receiver_num = ng_netreg_num(pkt->type, NG_NETREG_DEMUX_CTX_ALL) +
                   ng_netreg_num(NG_NETTYPE_IPV6, nh);

    if (receiver_num == 0) {
        DEBUG("ipv6: unable to forward packet as no one is interested in it\n");
        ng_pktbuf_release(pkt);
        return;
    }

    ng_pktbuf_hold(pkt, receiver_num - 1);
    /* IPv6 is not interested anymore so `- 1` */
    _dispatch_rcv_pkt(pkt->type, NG_NETREG_DEMUX_CTX_ALL, pkt);
    _dispatch_rcv_pkt(NG_NETTYPE_IPV6, nh, pkt);
}

/* internal functions */
static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[NG_IPV6_MSG_QUEUE_SIZE];
    ng_netreg_entry_t me_reg;

    (void)args;
    msg_init_queue(msg_q, NG_IPV6_MSG_QUEUE_SIZE);

    me_reg.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;
    me_reg.pid = thread_getpid();

    /* register interest in all IPv6 packets */
    ng_netreg_register(NG_NETTYPE_IPV6, &me_reg);

    /* preinitialize ACK */
    reply.type = NG_NETAPI_MSG_TYPE_ACK;

    /* start event loop */
    while (1) {
        DEBUG("ipv6: waiting for incoming message.\n");
        msg_receive(&msg);

        switch (msg.type) {
            case NG_NETAPI_MSG_TYPE_RCV:
                DEBUG("ipv6: NG_NETAPI_MSG_TYPE_RCV received\n");
                _receive((ng_pktsnip_t *)msg.content.ptr);
                break;

            case NG_NETAPI_MSG_TYPE_SND:
                DEBUG("ipv6: NG_NETAPI_MSG_TYPE_SND received\n");
                _send((ng_pktsnip_t *)msg.content.ptr, true);
                break;

            case NG_NETAPI_MSG_TYPE_GET:
            case NG_NETAPI_MSG_TYPE_SET:
                DEBUG("ipv6: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                msg_reply(&msg, &reply);
                break;

            default:
                break;
        }
    }

    return NULL;
}

/* functions for sending */
static void _send_unicast(kernel_pid_t iface, uint8_t *dst_l2addr,
                          uint16_t dst_l2addr_len, ng_pktsnip_t *pkt)
{
    ng_pktsnip_t *netif;

    if (pkt->type == NG_NETTYPE_NETIF) {
        /* great: someone already added a netif_hdr_t we assume it's wrong
         * to keep it simple
         * XXX: alternative would be to check if ng_netif_hdr_t::dst_l2addr_len
         * is long enough and only then to throw away the header. This causes
         * to much overhead IMHO */
        DEBUG("ipv6: removed old interface header\n");
        pkt = ng_pktbuf_remove_snip(pkt, pkt);
    }

    DEBUG("ipv6: add to interface header to packet\n");
    netif = ng_netif_hdr_build(NULL, 0, dst_l2addr, dst_l2addr_len);

    if (netif == NULL) {
        DEBUG("ipv6: error on interface header allocation, dropping packet\n");
        ng_pktbuf_release(pkt);
        return;
    }

    /* add netif to front of the pkt list */
    LL_PREPEND(pkt, netif);

    DEBUG("ipv6: send unicast over interface %" PRIkernel_pid "\n", iface);
    /* and send to interface */
    ng_netapi_send(iface, pkt);
}

static int _fill_ipv6_hdr(kernel_pid_t iface, ng_pktsnip_t *ipv6,
                          ng_pktsnip_t *payload)
{
    int res;
    ng_ipv6_hdr_t *hdr = ipv6->data;

    hdr->len = byteorder_htons(ng_pkt_len(payload));
    DEBUG("ipv6: set payload length to %zu (network byteorder %04" PRIx16 ")\n",
          ng_pkt_len(payload), hdr->len.u16);

    /* check if e.g. extension header was not already marked */
    if (hdr->nh == NG_PROTNUM_RESERVED) {
        hdr->nh = ng_nettype_to_protnum(payload->type);

        /* if still reserved: mark no next header */
        if (hdr->nh == NG_PROTNUM_RESERVED) {
            hdr->nh = NG_PROTNUM_IPV6_NONXT;
        }
    }

    DEBUG("ipv6: set next header to %" PRIu8 "\n", hdr->nh);

    if (hdr->hl == 0) {
        hdr->hl = ng_ipv6_netif_get(iface)->cur_hl;
    }

    if (ng_ipv6_addr_is_unspecified(&hdr->src)) {
        ng_ipv6_addr_t *src = ng_ipv6_netif_find_best_src_addr(iface, &hdr->dst);

        if (src != NULL) {
            DEBUG("ipv6: set packet source to %s\n",
                  ng_ipv6_addr_to_str(addr_str, src, sizeof(addr_str)));
            memcpy(&hdr->src, src, sizeof(ng_ipv6_addr_t));
        }

        /* Otherwise leave unspecified */
    }

    DEBUG("ipv6: calculate checksum for upper header.\n");

#if NG_NETIF_NUMOF > 1
    if (payload->users > 1) {
        ng_pktsnip_t *ptr = ipv6;

        /* We deal with multiple interfaces here (multicast) => possible
         * different source addresses => duplication of payload needed */
        while (ptr != payload) {
            /* duplicate everything including payload */
            ptr->next = ng_pktbuf_start_write(ptr->next);

            if (ptr->next == NULL) {
                DEBUG("ipv6: unable to get write access to payload, drop it\n");
                return -ENOBUFS;
            }

            ptr = ptr->next;
        }
    }
#endif /* NG_NETIF_NUMOF */

    if ((res = ng_netreg_calc_csum(payload, ipv6)) < 0) {
        if (res != -ENOENT) {   /* if there is no checksum we are okay */
            DEBUG("ipv6: checksum calculation failed.\n");
            return res;
        }
    }

    return 0;
}

static inline void _send_multicast_over_iface(kernel_pid_t iface, ng_pktsnip_t *pkt,
                                              ng_pktsnip_t *netif)
{
    DEBUG("ipv6: send multicast over interface %" PRIkernel_pid "\n", ifs[i]);
    /* mark as multicast */
    ((ng_netif_hdr_t *)netif->data)->flags |= NG_NETIF_HDR_FLAGS_MULTICAST;
    /* and send to interface */
    ng_netapi_send(iface, pkt);
}

static void _send_multicast(kernel_pid_t iface, ng_pktsnip_t *pkt,
                            ng_pktsnip_t *ipv6, ng_pktsnip_t *payload,
                            bool prep_hdr)
{
    ng_pktsnip_t *netif;

#if NG_NETIF_NUMOF > 1
    /* netif header not present: send over all interfaces */
    if (iface == KERNEL_PID_UNDEF) {
        size_t ifnum;
        /* get list of interfaces */
        kernel_pid_t *ifs = ng_netif_get(&ifnum);

        /* throw away packet if no one is interested */
        if (ifnum == 0) {
            DEBUG("ipv6: no interfaces registered, dropping packet\n");
            ng_pktbuf_release(pkt);
            return;
        }

        /* send packet to link layer */
        ng_pktbuf_hold(pkt, ifnum - 1);

        for (size_t i = 0; i < ifnum; i++) {
            if (prep_hdr) {
                /* need to get second write access (duplication) to fill IPv6
                 * header interface-local */
                ipv6 = ng_pktbuf_start_write(ipv6);

                if (ipv6 == NULL) {
                    DEBUG("ipv6: unable to get write access to IPv6 header, "
                          "for interface %" PRIkernel_pid "\n", ifs[i]);
                    ng_pktbuf_release(pkt);
                    return;
                }

                if (_fill_ipv6_hdr(ifs[i], ipv6, payload) < 0) {
                    /* error on filling up header */
                    ng_pktbuf_release(pkt);
                    return;
                }
            }

            /* allocate interface header */
            netif = ng_netif_hdr_build(NULL, 0, NULL, 0);

            if (netif == NULL) {
                DEBUG("ipv6: error on interface header allocation, "
                      "dropping packet\n");
                ng_pktbuf_release(pkt);
                return;
            }

            LL_PREPEND(pkt, netif);

            _send_multicast_over_iface(iface, pkt, netif);
        }
    }
    else {
        /* iface != KERNEL_PID_UNDEF implies that netif header is present */
        if (prep_hdr) {
            if (_fill_ipv6_hdr(iface, ipv6, payload) < 0) {
                /* error on filling up header */
                ng_pktbuf_release(pkt);
                return;
            }
        }

        netif = pkt;

        _send_multicast_over_iface(iface, pkt, netif);
    }
#else   /* NG_NETIF_NUMOF */
    if (iface == KERNEL_PID_UNDEF) {
        /* allocate interface header */
        netif = ng_netif_hdr_build(NULL, 0, NULL, 0);

        if (netif == NULL) {
            DEBUG("ipv6: error on interface header allocation, "
                  "dropping packet\n");
            ng_pktbuf_release(pkt);
            return;
        }

        LL_PREPEND(pkt, netif);
    }
    else {
        netif = pkt;
    }

    if (prep_hdr) {
        if (_fill_ipv6_hdr(iface, ipv6, payload) < 0) {
            /* error on filling up header */
            ng_pktbuf_release(pkt);
            return;
        }
    }

    _send_multicast_over_iface(iface, pkt, netif);
#endif  /* NG_NETIF_NUMOF */
}

static void _send(ng_pktsnip_t *pkt, bool prep_hdr)
{
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    ng_pktsnip_t *ipv6, *payload;
    ng_ipv6_hdr_t *hdr;
    ng_ipv6_nc_t *nc_entry;

    /* seize payload as temporary variable */
    payload = ng_pktbuf_start_write(pkt);

    if (payload == NULL) {
        DEBUG("ipv6: unable to get write access to packet, dropping packet\n");
        ng_pktbuf_release(pkt);
        return;
    }

    pkt = payload;  /* Reset pkt from temporary variable */

    /* get IPv6 snip and (if present) generic interface header */
    if (pkt->type == NG_NETTYPE_NETIF) {
        /* If there is already a netif header (routing protocols and
         * neighbor discovery might add them to preset sending interface) */
        iface = ((ng_netif_hdr_t *)pkt->data)->if_pid;
        ipv6 = pkt->next;
    }
    else {
        ipv6 = pkt;
    }

    hdr = ipv6->data;
    payload = ipv6->next;       /* TODO: parse extension headers */

    if (ng_ipv6_addr_is_multicast(&hdr->dst)) {
        _send_multicast(iface, pkt, ipv6, payload, prep_hdr);
    }
    else {
        ng_ipv6_addr_t *next_hop = NULL;

        next_hop = &hdr->dst;   /* TODO: next hop determination */

        if ((nc_entry = ng_ipv6_nc_get_reachable(iface, next_hop)) == NULL) {
            DEBUG("ipv6: No link layer address for next_hop %s found.\n",
                  ng_ipv6_addr_to_str(addr_str, next_hop, sizeof(addr_str)));
            ng_pktbuf_release(pkt);
            return;
        }
        else {
            iface = nc_entry->iface;
        }

        if (iface == KERNEL_PID_UNDEF) {
            DEBUG("ipv6: no interface for %s registered, dropping packet\n",
                  ng_ipv6_addr_to_str(addr_str, next_hop, sizeof(addr_str)));
            ng_pktbuf_release(pkt);
            return;
        }

        if (prep_hdr) {
            if (_fill_ipv6_hdr(iface, ipv6, payload) < 0) {
                /* error on filling up header */
                ng_pktbuf_release(pkt);
                return;
            }
        }

        _send_unicast(iface, nc_entry->l2_addr, nc_entry->l2_addr_len, pkt);
    }
}

/* functions for receiving */
static inline bool _pkt_not_for_me(kernel_pid_t *iface, ng_ipv6_hdr_t *hdr)
{
    if (*iface == KERNEL_PID_UNDEF) {
        *iface = ng_ipv6_netif_find_by_addr(NULL, &hdr->dst);
        return (*iface == KERNEL_PID_UNDEF);
    }
    else {
        return (ng_ipv6_netif_find_addr(*iface, &hdr->dst) == NULL);
    }
}

static void _dispatch_rcv_pkt(ng_nettype_t type, uint32_t demux_ctx,
                              ng_pktsnip_t *pkt)
{
    msg_t msg;
    ng_netreg_entry_t *entry = ng_netreg_lookup(type, demux_ctx);

    msg.type = NG_NETAPI_MSG_TYPE_RCV;
    msg.content.ptr = (char *)pkt;

    while (entry) {
        DEBUG("ipv6: Send receive command for %p to %" PRIu16 "\n", (void *)pkt,
              entry->pid);
        msg_send(&msg, entry->pid);
        entry = ng_netreg_getnext(entry);
    }
}

static void _receive(ng_pktsnip_t *pkt)
{
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    ng_pktsnip_t *ipv6, *netif;
    ng_ipv6_hdr_t *hdr;

    LL_SEARCH_SCALAR(pkt, netif, type, NG_NETTYPE_NETIF);

    if (netif != NULL) {
        iface = ((ng_netif_hdr_t *)netif->data)->if_pid;
    }

    if ((pkt->next != NULL) && (pkt->next->type == NG_NETTYPE_IPV6) &&
        (pkt->next->size == sizeof(ng_ipv6_hdr_t))) {
        /* IP header was already marked. Take it. */
        ipv6 = pkt->next;

        if (!ng_ipv6_hdr_is_ipv6_hdr(ipv6->data)) {
            DEBUG("ipv6: Received packet was not IPv6, dropping packet\n");
            ng_pktbuf_release(pkt);
            return;
        }
    }
    else {
        if (!ng_ipv6_hdr_is_ipv6_hdr(pkt->data)) {
            DEBUG("ipv6: Received packet was not IPv6, dropping packet\n");
            ng_pktbuf_release(pkt);
            return;
        }

        /* seize ipv6 as a temporary variable */
        ipv6 = ng_pktbuf_start_write(pkt);

        if (ipv6 == NULL) {
            DEBUG("ipv6: unable to get write access to packet, drop it\n");
            ng_pktbuf_release(pkt);
            return;
        }

        pkt = ipv6;     /* reset pkt from temporary variable */

        ipv6 = ng_pktbuf_add(pkt, pkt->data, sizeof(ng_ipv6_hdr_t),
                             NG_NETTYPE_IPV6);

        if (ipv6 == NULL) {
            DEBUG("ipv6: error marking IPv6 header, dropping packet\n");
            ng_pktbuf_release(pkt);
            return;
        }
    }

    /* extract header */
    hdr = (ng_ipv6_hdr_t *)ipv6->data;

    DEBUG("ipv6: Received (src = %s, ",
          ng_ipv6_addr_to_str(addr_str, &(hdr->src), sizeof(addr_str)));
    DEBUG("dst = %s, next header = %" PRIu8 ", length = %" PRIu16 ")\n",
          ng_ipv6_addr_to_str(addr_str, &(hdr->dst), sizeof(addr_str)),
          hdr->nh, byteorder_ntohs(hdr->len));

    if (_pkt_not_for_me(&iface, hdr)) { /* if packet is not for me */
        DEBUG("ipv6: packet destination not this host\n");

#ifdef MODULE_NG_IPV6_ROUTER    /* only routers redirect */
        /* redirect to next hop */
        DEBUG("ipv6: decrement hop limit to %" PRIu8 "\n", hdr->hl - 1);

        /* TODO: check if receiving interface is router */
        if (--(hdr->hl) > 0) {  /* drop packets that *reach* Hop Limit 0 */
            ng_pktsnip_t *tmp = pkt;

            DEBUG("ipv6: forward packet to next hop\n");

            /* pkt might not be writable yet, if header was given above */
            pkt = ng_pktbuf_start_write(tmp);
            ipv6 = ng_pktbuf_start_write(ipv6);

            if ((ipv6 == NULL) || (pkt == NULL)) {
                DEBUG("ipv6: unable to get write access to packet: dropping it\n");
                ng_pktbuf_release(tmp);
                return;
            }

            ng_pktbuf_release(ipv6->next);  /* remove headers around IPV6 */
            ipv6->next = pkt;           /* reorder for sending */
            pkt->next = NULL;
            _send(ipv6, false);
        }
        else {
            DEBUG("ipv6: hop limit reached 0: drop packet\n");
            ng_pktbuf_release(pkt);
            return;
        }

#else  /* MODULE_NG_IPV6_ROUTER */
        DEBUG("ipv6: dropping packet\n");
        /* non rounting hosts just drop the packet */
        ng_pktbuf_release(pkt);
        return;
#endif /* MODULE_NG_IPV6_ROUTER */
    }

    /* IPv6 internal demuxing (ICMPv6, Extension headers etc.) */
    ng_ipv6_demux(iface, pkt, hdr->nh);
}


/** @} */
