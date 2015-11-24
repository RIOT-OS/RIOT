/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <utlist.h>
#include <errno.h>
#include "msg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/tcp.h"
#include "net/gnrc/tcp/tcb.h"
#include "tcp_internal/pkt.h"
#include "tcp_internal/helper.h"
#include "tcp_internal/option.h"
#include "tcp_internal/eventloop.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int8_t _pkt_build_reset_from_pkt(gnrc_pktsnip_t** out_pkt, gnrc_pktsnip_t* in_pkt)
{
    gnrc_pktsnip_t *tcp_snp = NULL;
    gnrc_pktsnip_t *ip6_snp = NULL;
    tcp_hdr_t* tcp_hdr_in;
    tcp_hdr_t tcp_hdr_out;

    /* Extract headers */
    LL_SEARCH_SCALAR(in_pkt, tcp_snp, type, GNRC_NETTYPE_TCP);
    tcp_hdr_in = (tcp_hdr_t *)tcp_snp->data;
#ifdef MODULE_GNRC_IPV6
    LL_SEARCH_SCALAR(in_pkt, ip6_snp, type, GNRC_NETTYPE_IPV6);
    ipv6_hdr_t *ip6_hdr = (ipv6_hdr_t *)ip6_snp->data;
#endif

    /* Setup Header information */
    tcp_hdr_out.src_port = tcp_hdr_in->dst_port;
    tcp_hdr_out.dst_port = tcp_hdr_in->src_port;
    tcp_hdr_out.checksum = byteorder_htons(0);
    tcp_hdr_out.window = byteorder_htons(0);
    tcp_hdr_out.urgent_ptr = byteorder_htons(0);

    /* Seq/Ackno and control flags depend on inputs ACK-Flag */
    if(byteorder_ntohs(tcp_hdr_in->off_ctl) | MSK_ACK){
        tcp_hdr_out.off_ctl = byteorder_htons((OPTION_OFFSET_BASE << 12) | MSK_RST);
        tcp_hdr_out.seq_num = tcp_hdr_in->ack_num;
        tcp_hdr_out.ack_num = byteorder_htonl(0);
    }else{
        tcp_hdr_out.off_ctl = byteorder_htons((OPTION_OFFSET_BASE << 12) | MSK_RST_ACK);
        tcp_hdr_out.seq_num = byteorder_htonl(0);
        tcp_hdr_out.ack_num = byteorder_htonl(byteorder_ntohl(tcp_hdr_in->seq_num) + _pkt_get_seg_len(in_pkt));
    }

    /* Allocate new tcp header */
    tcp_snp = gnrc_pktbuf_add(NULL, &tcp_hdr_out, OPTION_OFFSET_BASE * 4, GNRC_NETTYPE_TCP);
    if(tcp_snp == NULL){
        DEBUG("tcp: Can't allocate buffer for TCP Header\n.");
        return -ENOMEM;
    }else{
        *out_pkt = tcp_snp;
    }

    /* Build new network layer header */
#ifdef MODULE_GNRC_IPV6
    ip6_snp = gnrc_ipv6_hdr_build(tcp_snp, (uint8_t *) &(ip6_hdr->dst), sizeof(ip6_hdr->dst),
                             (uint8_t *) &(ip6_hdr->src), sizeof(ip6_hdr->src));
    if(ip6_snp == NULL){
        DEBUG("tcp: Can't allocate buffer for IPv6 Header.\n");
        gnrc_pktbuf_release(tcp_snp);
        return -ENOMEM;
    }else{
        *out_pkt = ip6_snp;
    }
#endif
    return 0;
}

int8_t _pkt_build(gnrc_tcp_tcb_t *tcb,
                  gnrc_pktsnip_t **out_pkt,
                  uint16_t *seq_con,
                  const uint16_t ctl,
                  const uint32_t seq_num,
                  const uint32_t ack_num,
                  uint8_t *payload,
                  const size_t payload_len)
{
    gnrc_pktsnip_t *pay_snp = NULL;
    gnrc_pktsnip_t *tcp_snp = NULL;
    gnrc_pktsnip_t *ip6_snp = NULL;
    tcp_hdr_t tcp_hdr;
    uint8_t nopts = 0;

    /* Add payload, if supplied */
    if(payload != NULL && payload_len > 0){
        pay_snp = gnrc_pktbuf_add(pay_snp, payload, payload_len, GNRC_NETTYPE_UNDEF);
        if(pay_snp == NULL){
            DEBUG("tcp: Can't allocate buffer for Payload Header\n.");
            return -ENOMEM;
        }
    }

    /* assemble tcp-header */
    tcp_hdr.src_port = byteorder_htons(tcb->local_port);
    tcp_hdr.dst_port = byteorder_htons(tcb->peer_port);
    tcp_hdr.checksum = byteorder_htons(0);
    tcp_hdr.seq_num = byteorder_htonl(seq_num);
    tcp_hdr.ack_num = byteorder_htonl(ack_num);
    tcp_hdr.window = byteorder_htons(tcb->rcv_wnd);
    tcp_hdr.urgent_ptr = byteorder_htons(0);

    /* tcp option handling */
    /* If this is a syn-message, send mss option */
    if(ctl & MSK_SYN){
        /* NOTE: MSS usually based on lower layers MTU */
        tcp_hdr.options[nopts] = byteorder_htonl(OPT_MSS(GNRC_TCP_MSS));
        nopts += 1;
    }
    tcp_hdr.off_ctl = byteorder_htons(((OPTION_OFFSET_BASE + nopts) << 12) | ctl);

    /* allocate tcp header */
    tcp_snp = gnrc_pktbuf_add(pay_snp, &tcp_hdr, (OPTION_OFFSET_BASE + nopts) * 4, GNRC_NETTYPE_TCP);
    if(tcp_snp == NULL){
        DEBUG("tcp: Can't allocate buffer for TCP Header\n.");
        gnrc_pktbuf_release(pay_snp);
        return -ENOMEM;
    }else{
        *(out_pkt) = tcp_snp;
    }

    /* Build network layer header */
#ifdef MODULE_GNRC_IPV6
    ip6_snp = gnrc_ipv6_hdr_build(tcp_snp, NULL, 0, tcb->peer_addr, sizeof(ipv6_addr_t)); // TODO don't send the tcb->peer_addr?
    if(ip6_snp == NULL){
        DEBUG("tcp: Can't allocate buffer for IPv6 Header.\n");
        gnrc_pktbuf_release(tcp_snp);
        return -ENOMEM;
    }else{
        *(out_pkt) = ip6_snp;
    }
#endif

    /* Calculate Sequence Space Number Consumption for this packet */
    if(ctl & MSK_SYN){
        *seq_con += 1;
    }
    if(ctl & MSK_FIN){
        *seq_con += 1;
    }
    *seq_con += payload_len;
    return 0;
}

int8_t _pkt_send(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *out_pkt, const uint16_t seq_con)
{
    /* Advance snd_nxt, by the consumed sequence space of the outgoing segment */
    tcb->snd_nxt += seq_con;
    gnrc_netapi_send(_tcp_pid, out_pkt);
    return 0;
}

/* TODO: Make this overflow-safe */
int8_t _pkt_chk_seq_num(gnrc_tcp_tcb_t *tcb, uint32_t snum, uint32_t slen)
{
    uint32_t rn = tcb->rcv_nxt;
    uint32_t rw = tcb->rcv_wnd;
    uint32_t sl = snum + slen -1;

    /* Possible case 1 */
    if(slen == 0 && rw == 0 && rn == snum){
        return true;
    }

    /* Possible case 2 */
    if(slen == 0 && rw > 0 && rn <= snum && snum < (rn + rw)){
        return true;
    }

    /* Possible case 3 */
    if(slen > 0 && rw > 0 && ((rn <= snum && snum < rn + rw) || (rn <= sl && sl < (rn + rw)))){
        return true;
    }

    /* Everthing else is not acceptable */
    return false;
}

uint32_t _pkt_get_seg_len(gnrc_pktsnip_t *pkt)
{
    uint32_t seg_len = 0;
    gnrc_pktsnip_t *snp = NULL;
    LL_SEARCH_SCALAR(pkt, snp, type, GNRC_NETTYPE_UNDEF);

    while(snp && snp->type == GNRC_NETTYPE_UNDEF){
        seg_len += snp->size;
        snp = snp->next;
    }
    return seg_len;
}

int8_t _pkt_setup_retransmit(gnrc_tcp_tcb_t* tcb, gnrc_pktsnip_t* pkt)
{
    gnrc_pktsnip_t* snp = NULL;
    uint32_t ctl = 0;
    uint32_t len = 0;
    uint32_t rtt = 10000000;

    /* Check if retransmit queue is full and pkt is not already in retransmit queue */
    if(tcb->pkt_retransmit != NULL && tcb->pkt_retransmit != pkt){
        return -ENOMEM;
    }

    /* Extract control bits and segment length */
    LL_SEARCH_SCALAR(pkt, snp, type, GNRC_NETTYPE_TCP);
    ctl = byteorder_ntohs(((tcp_hdr_t*) snp->data)->off_ctl);
    len = _pkt_get_seg_len(pkt);

    /* Check if pkt contains reset or is a pure ACK, return */
    if((ctl & MSK_RST) || (((ctl & MSK_SYN_FIN_ACK) == MSK_ACK) && len == 0)){
        return 0;
    }

    /* Assign pkt and increase users: every send attempt consumes a user */
    tcb->pkt_retransmit = pkt;
    gnrc_pktbuf_hold(pkt, 1);

    /* Setup retransmission timer, msg to TCP thread with ptr to tcb */
    tcb->msg_timeout.type = MSG_TYPE_RETRANSMISSION;
    tcb->msg_timeout.content.ptr = (void *)tcb;
    xtimer_set_msg(&tcb->timer_timeout, rtt, &tcb->msg_timeout, _tcp_pid);

    return 0;
}


void _pkt_acknowledge(gnrc_tcp_tcb_t* tcb, uint32_t ack)
{
    gnrc_pktsnip_t* snp = NULL;
    tcp_hdr_t* hdr = NULL;
    uint32_t seg = 0;

    /* Retransmission Queue is empty. Nothing to ACK there */
    if(tcb->pkt_retransmit == NULL){
        DEBUG("_pkt_acknowledge: There is no packet to ack\n");
        return;
    }

    /* There must be a packet, waiting to be acknowledged. */
    LL_SEARCH_SCALAR(tcb->pkt_retransmit, snp, type, GNRC_NETTYPE_TCP);
    hdr = (tcp_hdr_t*) snp->data;
    seg = byteorder_ntohl(hdr->seq_num) + _pkt_get_seg_len(tcb->pkt_retransmit) -1;

    /* If segment can be acknowledged -> stop timer, release packet from pktbuf. */
    if(seg < ack){
        xtimer_remove(&(tcb->timer_timeout));
        gnrc_pktbuf_release(tcb->pkt_retransmit);
        tcb->pkt_retransmit = NULL;
    }
}


uint16_t _pkt_calc_csum(const gnrc_pktsnip_t *hdr,
                        const gnrc_pktsnip_t *pseudo_hdr,
                        const gnrc_pktsnip_t *payload)
{
    uint16_t csum = 0;
    uint16_t len = (uint16_t) hdr->size;

    /* Process payload */
    while(payload && payload != hdr){
        csum = inet_csum(csum, (uint8_t *)payload->data, payload->size);
        len += (uint16_t)payload->size;
        payload = payload->next;
    }

    /* Process tcp-header, before checksum field(Byte 16 to 18) */
    csum = inet_csum(csum, (uint8_t *)hdr->data, 16);
    /* Process tcp-header, after checksum field */
    csum = inet_csum(csum, ((uint8_t *)hdr->data) + 18, hdr->size - 18);

    /* Process Network layer Header */
    switch (pseudo_hdr->type) {
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            csum = ipv6_hdr_inet_csum(csum, pseudo_hdr->data, PROTNUM_TCP, len);
            break;
#endif
        default:
            return 0;
    }
    return ~csum;
}
