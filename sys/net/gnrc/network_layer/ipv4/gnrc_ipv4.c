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
#include "net/gnrc/icmpv4.h"
#include "net/protnum.h"
#include "thread.h"
#include "utlist.h"

#include "net/arp.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/ipv4.h"
#include "net/gnrc/ipv4/arp/arp.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define _MAX_L2_ADDR_LEN    (8U)

#if ENABLE_DEBUG
static char _stack[GNRC_IPV4_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_IPV4_STACK_SIZE];
#endif


static char addr_str[IPV4_ADDR_MAX_STR_LEN];

kernel_pid_t gnrc_ipv4_pid = KERNEL_PID_UNDEF;

/* handles GNRC_NETAPI_MSG_TYPE_RCV commands */
static void _receive(gnrc_pktsnip_t *pkt);
/* Sends packet over the appropriate interface(s).
 * prep_hdr: prepare header for sending (call to _fill_ipv4_hdr()), otherwise
 * assume it is already prepared */
static void _send(gnrc_pktsnip_t *pkt, bool prep_hdr);

/* Main event loop for IPv4 */
static void *_event_loop(void *args);

kernel_pid_t gnrc_ipv4_init(void)
{
    gnrc_ipv4_route_delete_all();
    gnrc_ipv4_arp_init();

    if (gnrc_ipv4_pid == KERNEL_PID_UNDEF) {
        gnrc_ipv4_pid = thread_create(_stack, sizeof(_stack), GNRC_IPV4_PRIO,
                                      THREAD_CREATE_STACKTEST,
                                      _event_loop, NULL, "ipv4");
    }

    return gnrc_ipv4_pid;
}

/* internal functions */
/* TODO _dispatch_next_header rename to fit ipv4 names */
static void _dispatch_next_header(gnrc_pktsnip_t *pkt, unsigned nh, bool interested)
{
    const bool has_nh_subs = (gnrc_netreg_num(GNRC_NETTYPE_IPV4, nh) > 0) ||  interested;

    DEBUG("ipv4: forward protocol = %u to other threads\n", nh);

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
    if (gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV4, nh, pkt) == 0) {
        gnrc_pktbuf_release(pkt);
    }
}

static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[GNRC_IPV4_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                            sched_active_pid);

    (void)args;
    msg_init_queue(msg_q, GNRC_IPV4_MSG_QUEUE_SIZE);

    /* register interest in all IPv4 packets */
    gnrc_netreg_register(GNRC_NETTYPE_IPV4, &me_reg);

    /* preinitialize ACK */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    /* start event loop */
    while (1) {
        DEBUG("ipv4: waiting for incoming message.\n");
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("ipv4: GNRC_NETAPI_MSG_TYPE_RCV received\n");
                _receive(msg.content.ptr);
                break;

            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("ipv4: GNRC_NETAPI_MSG_TYPE_SND received\n");
                _send(msg.content.ptr, true);
                break;

            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("ipv4: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                msg_reply(&msg, &reply);
                break;

            default:
                break;
        }
    }

    return NULL;
}

static inline bool _is_ipv4_hdr(gnrc_pktsnip_t *hdr)
{
    return (hdr->type == GNRC_NETTYPE_IPV4);
}

static int _fill_ipv4_hdr(gnrc_netif_t *netif, gnrc_pktsnip_t *ipv4)
{
    int res;
    static uint16_t identification=0;
    ipv4_hdr_t *hdr = ipv4->data;
    gnrc_pktsnip_t *payload, *prev;

    // Set protocol
    if (hdr->protocol == PROTNUM_RESERVED) {
        if (ipv4->next == NULL) {
            hdr->protocol = PROTNUM_IPV6_NONXT;
        }
        else {
            hdr->protocol = gnrc_nettype_to_protnum(ipv4->next->type);

            /* if still reserved: mark no next header */
            if (hdr->protocol == PROTNUM_RESERVED) {
                hdr->protocol = PROTNUM_IPV6_NONXT;
            }
        }
    }

    // Total len
    hdr->tl = byteorder_htons(gnrc_pkt_len(ipv4));

    // TTL
    if (hdr->ttl == 0) {
        if (netif == NULL) {
            hdr->ttl = GNRC_NETIF_DEFAULT_HL;
        }
        else {
            hdr->ttl = netif->cur_hl;
        }
    }
    hdr->id = byteorder_htons(identification++);

    // Check source ip
    if (ipv4_addr_is_unspecified(&hdr->src)) {
        if (ipv4_addr_is_loopback(&hdr->dst)) {
            ipv4_addr_set_loopback(&hdr->src);
        }
        else {
/*
            ipv4_addr_t *src = gnrc_netif_ipv4_addr_best_src(netif, &hdr->dst,
                                                             false);
            ipv4_addr_t set_ipv4_addr = {{192, 168, 0, 222}}; // TODO implement gnrc_netif_ipv4_addr_best_src
            src = &set_ipv4_addr;
            if (src != NULL) {
                char addr_str[IPV4_ADDR_MAX_STR_LEN];
                DEBUG("ipv4: set packet source to %s\n",
                      ipv4_addr_to_str(addr_str, src, sizeof(addr_str)));
                memcpy(&hdr->src, src, sizeof(ipv4_addr_t));
            }
*/
            /* Otherwise leave unspecified */
        }
    }

    DEBUG("ipv4: write protect up to payload to calculate checksum\n");
    payload = ipv4;
    prev = ipv4;
    while (_is_ipv4_hdr(payload) && (payload->next != NULL)) {
        /* IPv4 header itself was already write-protected in caller function,
         * just write protect extension headers and payload header */
        if ((payload = gnrc_pktbuf_start_write(payload->next)) == NULL) {
            DEBUG("ipv4: unable to get write access to IPv4 extension or payload header\n");
            /* packet duplicated to this point will be released by caller,
             * original packet by other subscriber */
            return -ENOMEM;
        }
        prev->next = payload;
        prev = payload;
    }

    DEBUG("ipv4: calculate checksum for upper header.\n");
    if ((res = gnrc_netreg_calc_csum(payload, ipv4)) < 0) {
        if (res != -ENOENT) {   /* if there is no checksum we are okay */
            DEBUG("ipv4: checksum calculation failed.\n");
            /* packet will be released by caller */
            return res;
        }
    }

    DEBUG("ipv4: calculate checksum.\n");
    ipv4_hdr_inet_csum(ipv4->data);

    return 0;
}

static bool _safe_fill_ipv4_hdr(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt,
                                bool prep_hdr)
{
    if (prep_hdr && (_fill_ipv4_hdr(netif, pkt) < 0)) {
        /* error on filling up header */
        gnrc_pktbuf_release(pkt);
        return false;
    }
    return true;
}

static void _send_to_self(gnrc_pktsnip_t *pkt, bool prep_hdr,
                          gnrc_netif_t *netif)
{
    if (!_safe_fill_ipv4_hdr(netif, pkt, prep_hdr) ||
        /* no netif header so we just merge the whole packet. */
        (gnrc_pktbuf_merge(pkt) != 0)) {
        DEBUG("ipv4: error looping packet to sender.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    DEBUG("ipv4: packet is addressed to myself => loopback\n");

    if (gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV4,
                                     GNRC_NETREG_DEMUX_CTX_ALL,
                                     pkt) == 0) {
        DEBUG("ipv4: unable to deliver looped back packet\n");
        gnrc_pktbuf_release(pkt);
    }
}

static void _send_multicast(gnrc_pktsnip_t *pkt, bool prep_hdr,
                            gnrc_netif_t *netif, uint8_t netif_hdr_flags)
{
  gnrc_pktbuf_release_error(pkt, EINVAL);
}

static void _send_to_iface(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    const ipv4_hdr_t *hdr = pkt->next->data;

    assert(netif != NULL);
    ((gnrc_netif_hdr_t *)pkt->data)->if_pid = netif->pid;
    /*if (gnrc_pkt_len(pkt->next) > netif->ipv4.mtu) {
        DEBUG("ipv4: packet too big\n");
        //gnrc_icmpv4_error_pkt_too_big_send(netif->ipv4.mtu, pkt);
        gnrc_pktbuf_release_error(pkt, EMSGSIZE);
        return;
    }*/
    DEBUG("ipv4: Sending (src = %s, ",
          ipv4_addr_to_str(addr_str, &hdr->src, sizeof(addr_str)));
    DEBUG("dst = %s, next header = %u, length = %u)\n",
          ipv4_addr_to_str(addr_str, &hdr->dst, sizeof(addr_str)), hdr->protocol,
          byteorder_ntohs(hdr->tl));

    if (gnrc_netapi_send(netif->pid, pkt) < 1) {
        DEBUG("ipv4: unable to send packet\n");
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
        DEBUG("ipv4: error on interface header allocation, dropping packet\n");
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


static void _send_unicast(gnrc_pktsnip_t *pkt, bool prep_hdr,
                          gnrc_netif_t *netif, ipv4_hdr_t *ipv4_hdr,
                          uint8_t netif_hdr_flags)
{
    DEBUG("ipv4: send unicast\n");

    ipv4_addr_t hop;
    gnrc_ipv4_route_get_next_hop_l2addr(&ipv4_hdr->dst, &netif, pkt, &hop);
    if (netif == NULL) {
      DEBUG("ipv4: send unicast no route to host\n");
      gnrc_pktbuf_release_error(pkt, EHOSTUNREACH);
      return;
    }

    // prepare arp query
    arp_netapi_get_t arp_query;
    arp_query.ipv4 = hop;
    arp_query.iface = netif->pid;

    // send query
    uint16_t result = 1;
    printf("gnrc_ipv4: prepare arp query to gnrc_ipv4_arp\n");
    for(int i=0;i<=3;i++) {
        result = gnrc_netapi_get(gnrc_ipv4_arp_pid, 1, 0, &arp_query, sizeof(arp_query));
        if( result == 0 ) {
            printf("gnrc_ipv4: send arp query to gnrc_ipv4_arp\n");
            break;
        } else {
            printf("gnrc_ipv4: send arp query to gnrc_ipv4_arp error :%d\n", result);
            xtimer_usleep(100000); // 100ms
        }
    }

    if( result != 0 ) {
        gnrc_pktbuf_release_error(pkt, EHOSTUNREACH);
        return;
    }

    if (_safe_fill_ipv4_hdr(netif, pkt, prep_hdr)) {
        DEBUG("ipv4: add interface header to packet\n");
        if ((pkt = _create_netif_hdr(arp_query.mac, sizeof(arp_query.mac), pkt,
                                     netif_hdr_flags)) == NULL) {
            return;
        }

        DEBUG("ipv4: send unicast over interface %" PRIkernel_pid "\n",
              netif->pid);

        /* and send to interface */
#ifdef MODULE_NETSTATS_IPV4
        netif->ipv4.stats.tx_unicast_count++;
#endif
        _send_to_iface(netif, pkt);
    }
}

static void _send(gnrc_pktsnip_t *pkt, bool prep_hdr)
{
    gnrc_netif_t *netif = NULL;
    gnrc_pktsnip_t *tmp_pkt;
    ipv4_hdr_t *ipv4_hdr;
    uint8_t netif_hdr_flags = 0U;

    /* get IPv4 snip and (if present) generic interface header */
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
            DEBUG("ipv4: unable to get write access to netif header, dropping packet\n");
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

    // Ensure is IPv4
    if (pkt->type != GNRC_NETTYPE_IPV4) {
        DEBUG("ipv4: unexpected packet type\n");
        gnrc_pktbuf_release_error(pkt, EINVAL);
        return;
    }

    // Ensure is a dst IP is set
    if (ipv4_addr_is_unspecified(&((ipv4_hdr_t *)pkt->data)->dst)) {
        DEBUG("ipv4: destination address is unspecified address (0.0.0.0), "
              "dropping packet \n");
        gnrc_pktbuf_release_error(pkt, EINVAL);
        return;
    }

    tmp_pkt = gnrc_pktbuf_start_write(pkt);
    if (tmp_pkt == NULL) {
        DEBUG("ipv4: unable to get write access to IPv4 header, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp_pkt;

    ipv4_hdr = pkt->data;

    if (ipv4_addr_is_multicast(&ipv4_hdr->dst)) {
        _send_multicast(pkt, prep_hdr, netif, netif_hdr_flags);
    }
    else {
        gnrc_netif_t *tmp_netif = gnrc_netif_get_by_ipv4_addr(&ipv4_hdr->dst);

        if (ipv4_addr_is_loopback(&ipv4_hdr->dst) ||    /* dst is loopback address */
            /* or dst registered to a local interface */
            (tmp_netif != NULL)) {
            _send_to_self(pkt, prep_hdr, tmp_netif);
        }
        else {
            _send_unicast(pkt, prep_hdr, netif, ipv4_hdr, netif_hdr_flags);
        }
    }
}

static inline bool _gnrc_ipv4_is_interested(unsigned protocol) {
#ifdef MODULE_GNRC_ICMPV4
    return (protocol == PROTNUM_ICMP);
#else  /* MODULE_GNRC_ICMPV4 */
    return false;
#endif /* MODULE_GNRC_ICMPV4 */
}

static void _demux(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt, unsigned protocol)
{
    pkt->type = gnrc_nettype_from_protnum(protocol);
    _dispatch_next_header(pkt, protocol, _gnrc_ipv4_is_interested(protocol));
    switch (protocol) {
#ifdef MODULE_GNRC_ICMPV4
        case PROTNUM_ICMP:
            DEBUG("ipv4: handle ICMPv4 packet (protocol = %u)\n", protocol);
            gnrc_icmpv4_demux(netif, pkt);
            break;
#endif /* MODULE_GNRC_ICMPV4 */
        default:
            break;
    }
}

/* functions for receiving */
static inline bool _pkt_not_for_me(gnrc_netif_t **netif, ipv4_hdr_t *hdr)
{
    if (ipv4_addr_is_loopback(&hdr->dst)) {
        return false;
    }
    else if ((!ipv4_addr_is_link_local(&hdr->dst)) ||
             (*netif == NULL)) {
        *netif = gnrc_netif_get_by_ipv4_addr(&hdr->dst);
        return (*netif == NULL);
    }
    else {
        return (gnrc_netif_get_by_ipv4_addr(&hdr->dst) == NULL);
    }
}

static void _receive(gnrc_pktsnip_t *pkt)
{
    gnrc_netif_t *netif = NULL;
    gnrc_pktsnip_t *ipv4, *netif_hdr;
    ipv4_hdr_t *hdr;
    uint8_t protocol;

    assert(pkt != NULL);

    netif_hdr = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);

    if (netif_hdr != NULL) {
        netif = gnrc_netif_get_by_pid(((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid);
    }

    if ((pkt->data == NULL) || (pkt->size < sizeof(ipv4_hdr_t)) || !ipv4_hdr_is(pkt->data)) {
        DEBUG("ipv4: Received packet was not IPv4, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* seize ipv4 as a temporary variable */
    ipv4 = gnrc_pktbuf_start_write(pkt);

    if (ipv4 == NULL) {
        DEBUG("ipv4: unable to get write access to packet, drop it\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    pkt = ipv4;     /* reset pkt from temporary variable */

    ipv4 = gnrc_pktbuf_mark(pkt, sizeof(ipv4_hdr_t), GNRC_NETTYPE_IPV4);

    pkt->type = GNRC_NETTYPE_UNDEF; /* snip is no longer IPv4 */

    if (ipv4 == NULL) {
        DEBUG("ipv4: error marking IPv4 header, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    /* extract header */
    hdr = (ipv4_hdr_t *)ipv4->data;

    if (hdr->ttl == 0) {
        /* This is an illegal value in any case, not just in case of a
         * forwarding step, so *do not* check it together with ((--hdr->hl) > 0)
         * in forwarding code below */
        DEBUG("ipv4: packet was received with ttl 0\n");
        //gnrc_icmpv4_error_time_exc_send(ICMPV4_ERROR_TIME_EXC_HL, pkt);
        gnrc_pktbuf_release_error(pkt, ETIMEDOUT);
        return;
    }

    uint16_t ipv4_len = byteorder_ntohs(hdr->tl);
    uint8_t ipv4_hdr_len = ipv4_hdr_get_ihl(hdr)/8;
    uint8_t ipv4_payload_len = ipv4_len - ipv4_hdr_len;
    protocol = hdr->protocol;

    if (ipv4_len <= sizeof(ipv4_hdr_t)) {
        /* this doesn't even make sense */
        DEBUG("ipv4: payload length 0\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    /* if available, remove any padding that was added by lower layers
     * to fulfill their minimum size requirements (e.g. ethernet) */
    else if ((ipv4 != pkt) && (ipv4_payload_len < pkt->size)) {
        gnrc_pktbuf_realloc_data(pkt, ipv4_payload_len);
    }
    else if (ipv4_payload_len > (gnrc_pkt_len_upto(pkt, GNRC_NETTYPE_IPV4) - sizeof(ipv4_hdr_t))) {
        DEBUG("ipv4: invalid payload length: %d, actual: %d, dropping packet\n",
              ipv4_len,
              (int) (gnrc_pkt_len_upto(pkt, GNRC_NETTYPE_IPV4) - sizeof(ipv4_hdr_t)));
        //gnrc_icmpv4_error_param_prob_send(ICMPV4_ERROR_PARAM_PROB_HDR_FIELD, &(ipv4_hdr_len), pkt);
        gnrc_pktbuf_release_error(pkt, EINVAL);
        return;
    }

    /* verify checksum header */
    uint16_t csum;
    csum = (uint16_t) ~ inet_csum(0, (uint8_t *) hdr, sizeof(ipv4_hdr_t));
    if( byteorder_ntohs(hdr->csum) == csum ) {
        DEBUG("ipv4: wrong header checksum : %x (instead of %x)", byteorder_ntohs(hdr->csum), (~csum));
    }

    DEBUG("ipv4: Received (src = %s, ",
          ipv4_addr_to_str(addr_str, &(hdr->src), sizeof(addr_str)));
    DEBUG("dst = %s, protocol = %u, header length = %" PRIu16 ")\n",
          ipv4_addr_to_str(addr_str, &(hdr->dst), sizeof(addr_str)),
          protocol, ipv4_hdr_len);

    if (_pkt_not_for_me(&netif, hdr)) { /* if packet is not for me */
        DEBUG("ipv4: packet destination not this host\n");
        /* TODO Add routing code */
        DEBUG("ipv4: dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    DEBUG("ipv4: packet destination if for me !!!!!!!\n");

    _demux(netif, pkt, protocol);
}

/** @} */
