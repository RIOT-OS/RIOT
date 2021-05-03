/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @}
 */


#include "msg.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/udp.h"
#include "net/sock.h"
#include "sched.h"

#include "stack.h"

#define _MSG_QUEUE_SIZE     (4)

static msg_t _msg_queue[_MSG_QUEUE_SIZE];
static gnrc_netreg_entry_t _udp_handler;

void _net_init(void)
{
    msg_init_queue(_msg_queue, _MSG_QUEUE_SIZE);
    gnrc_netreg_entry_init_pid(&_udp_handler, GNRC_NETREG_DEMUX_CTX_ALL,
                               thread_getpid());
}

void _prepare_send_checks(void)
{
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &_udp_handler);
}

static gnrc_pktsnip_t *_build_udp_packet(const ipv6_addr_t *src,
                                         const ipv6_addr_t *dst,
                                         uint16_t src_port, uint16_t dst_port,
                                         void *data, size_t data_len,
                                         uint16_t netif,
                                         const inject_aux_t *aux)
{
    gnrc_pktsnip_t *netif_hdr_snip, *ipv6, *udp;
    udp_hdr_t *udp_hdr;
    ipv6_hdr_t *ipv6_hdr;
    uint16_t csum = 0;

    if ((netif > INT16_MAX) || ((sizeof(udp_hdr_t) + data_len) > UINT16_MAX)) {
        return NULL;
    }

    udp = gnrc_pktbuf_add(NULL, NULL, sizeof(udp_hdr_t) + data_len,
                          GNRC_NETTYPE_UNDEF);
    if (udp == NULL) {
        return NULL;
    }
    udp_hdr = udp->data;
    udp_hdr->src_port = byteorder_htons(src_port);
    udp_hdr->dst_port = byteorder_htons(dst_port);
    udp_hdr->length = byteorder_htons((uint16_t)udp->size);
    udp_hdr->checksum.u16 = 0;
    memcpy(udp_hdr + 1, data, data_len);
    csum = inet_csum(csum, (uint8_t *)udp->data, udp->size);
    ipv6 = gnrc_ipv6_hdr_build(NULL, src, dst);
    if (ipv6 == NULL) {
        return NULL;
    }
    ipv6_hdr = ipv6->data;
    ipv6_hdr->len = byteorder_htons((uint16_t)udp->size);
    ipv6_hdr->nh = PROTNUM_UDP;
    ipv6_hdr->hl = 64;
    csum = ipv6_hdr_inet_csum(csum, ipv6_hdr, PROTNUM_UDP, (uint16_t)udp->size);
    if (csum == 0xffff) {
        udp_hdr->checksum = byteorder_htons(csum);
    }
    else {
        udp_hdr->checksum = byteorder_htons(~csum);
    }
    udp = gnrc_pkt_append(udp, ipv6);
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
    return gnrc_pkt_append(udp, netif_hdr_snip);
}

bool _inject_packet_aux(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                        uint16_t src_port, uint16_t dst_port,
                        void *data, size_t data_len, uint16_t netif,
                        const inject_aux_t *aux)
{
    gnrc_pktsnip_t *pkt = _build_udp_packet(src, dst, src_port, dst_port,
                                            data, data_len, netif, aux);

    if (pkt == NULL) {
        return false;
    }
    return (gnrc_netapi_dispatch_receive(GNRC_NETTYPE_UDP,
                                         GNRC_NETREG_DEMUX_CTX_ALL, pkt) > 0);
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
                   uint16_t src_port, uint16_t dst_port,
                   void *data, size_t data_len, uint16_t iface,
                   bool random_src_port)
{
    gnrc_pktsnip_t *pkt, *ipv6, *udp;
    ipv6_hdr_t *ipv6_hdr;
    udp_hdr_t *udp_hdr;
    msg_t msg;

    msg_receive(&msg);
    if (msg.type != GNRC_NETAPI_MSG_TYPE_SND) {
        return false;
    }
    pkt = msg.content.ptr;
    if (iface != SOCK_ADDR_ANY_NETIF) {
        gnrc_netif_hdr_t *netif_hdr;

        if (pkt->type != GNRC_NETTYPE_NETIF) {
            return _res(pkt, false);
        }
        netif_hdr = pkt->data;
        if (netif_hdr->if_pid != (int)iface) {
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
    udp = gnrc_pktsnip_search_type(ipv6, GNRC_NETTYPE_UDP);
    if (udp == NULL) {
        return _res(pkt, false);
    }
    udp_hdr = udp->data;
    return _res(pkt, (memcmp(src, &ipv6_hdr->src, sizeof(ipv6_addr_t)) == 0) &&
                (memcmp(dst, &ipv6_hdr->dst, sizeof(ipv6_addr_t)) == 0) &&
                (ipv6_hdr->nh == PROTNUM_UDP) &&
                (random_src_port || (src_port == byteorder_ntohs(udp_hdr->src_port))) &&
                (dst_port == byteorder_ntohs(udp_hdr->dst_port)) &&
                (udp->next != NULL) &&
                (data_len == udp->next->size) &&
                (memcmp(data, udp->next->data, data_len) == 0));
}
