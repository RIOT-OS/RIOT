/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "stack.h"

#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netreg.h"
#include "net/sock.h"
#include "sched.h"

#define _MSG_QUEUE_SIZE     (4)

static msg_t _msg_queue[_MSG_QUEUE_SIZE];
static gnrc_netreg_entry_t _ip_handler;

void _net_init(void)
{
    msg_init_queue(_msg_queue, _MSG_QUEUE_SIZE);
    gnrc_netreg_entry_init_pid(&_ip_handler, GNRC_NETREG_DEMUX_CTX_ALL,
                               thread_getpid());
}

void _prepare_send_checks(void)
{
    gnrc_netreg_register(GNRC_NETTYPE_IPV6, &_ip_handler);
}

static gnrc_pktsnip_t *_build_ipv6_packet(const ipv6_addr_t *src,
                                          const ipv6_addr_t *dst, uint8_t nh,
                                          void *data, size_t data_len,
                                          uint16_t netif,
                                          const inject_aux_t *aux)
{
    gnrc_pktsnip_t *netif_hdr_snip, *ipv6, *payload;
    ipv6_hdr_t *ipv6_hdr;

    if ((netif > INT16_MAX) || (data_len > UINT16_MAX)) {
        return NULL;
    }

    payload = gnrc_pktbuf_add(NULL, data, data_len, GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        return NULL;
    }
    ipv6 = gnrc_ipv6_hdr_build(NULL, src, dst);
    if (ipv6 == NULL) {
        return NULL;
    }
    ipv6_hdr = ipv6->data;
    ipv6_hdr->len = byteorder_htons((uint16_t)payload->size);
    ipv6_hdr->nh = nh;
    ipv6_hdr->hl = 64;
    payload = gnrc_pkt_append(payload, ipv6);
    netif_hdr_snip = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (netif_hdr_snip == NULL) {
        return NULL;
    }
    gnrc_netif_hdr_t *netif_hdr = netif_hdr_snip->data;
    netif_hdr->if_pid = (kernel_pid_t)netif;
    if (aux) {
        gnrc_netif_hdr_set_timestamp(netif_hdr, aux->timestamp);
        netif_hdr->rssi = aux->rssi;
    }
    return gnrc_pkt_append(payload, netif_hdr_snip);
}


bool _inject_packet_aux(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                        uint8_t proto, void *data, size_t data_len,
                        uint16_t netif, const inject_aux_t *aux)
{
    gnrc_pktsnip_t *pkt = _build_ipv6_packet(src, dst, proto, data, data_len,
                                             netif, aux);

    if (pkt == NULL) {
        return false;
    }
    /* put directly in mbox, dispatching to IPv6 would result in the packet
     * being dropped, since dst is not on any interface */
    return (gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6, proto, pkt) > 0);
}

bool _check_net(void)
{
    return (gnrc_pktbuf_is_sane() && gnrc_pktbuf_is_empty());
}

static inline bool _res(gnrc_pktsnip_t *pkt, bool res)
{
    gnrc_pktbuf_release(pkt);
    return res;
}

bool _check_packet(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                   uint8_t proto, void *data, size_t data_len,
                   uint16_t netif)
{
    gnrc_pktsnip_t *pkt, *ipv6;
    ipv6_hdr_t *ipv6_hdr;
    msg_t msg;

    msg_receive(&msg);
    if (msg.type != GNRC_NETAPI_MSG_TYPE_SND) {
        return false;
    }
    pkt = msg.content.ptr;
    if (netif != SOCK_ADDR_ANY_NETIF) {
        gnrc_netif_hdr_t *netif_hdr;

        if (pkt->type != GNRC_NETTYPE_NETIF) {
            return _res(pkt, false);
        }
        netif_hdr = pkt->data;
        if (netif_hdr->if_pid != (int)netif) {
            return _res(pkt, false);
        }
        ipv6 = pkt->next;
    }
    else {
        ipv6 = pkt;
    }
    if (ipv6->type != GNRC_NETTYPE_IPV6) {
        return _res(pkt, false);
    }
    ipv6_hdr = ipv6->data;
    return _res(pkt, (memcmp(src, &ipv6_hdr->src, sizeof(ipv6_addr_t)) == 0) &&
                (memcmp(dst, &ipv6_hdr->dst, sizeof(ipv6_addr_t)) == 0) &&
                (ipv6_hdr->nh == proto) &&
                (ipv6->next != NULL) &&
                (data_len == ipv6->next->size) &&
                (memcmp(data, ipv6->next->data, data_len) == 0));
}

/** @} */
