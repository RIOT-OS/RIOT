/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc
 * @{
 *
 * @file
 * @brief       Implementation of internal/pkt.h
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 * @}
 */
#include <string.h>
#include <utlist.h>
#include <errno.h>
#include "byteorder.h"
#include "net/inet_csum.h"
#include "net/gnrc.h"
#include "internal/common.h"
#include "internal/option.h"
#include "internal/pkt.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Calculates the maximum of two unsigned numbers.
 *
 * @param[in] x   First comparrison value.
 * @param[in] y   Second comparrison value.
 *
 * @returns   X if x is larger than y, if not y is returned.
 */
static inline uint32_t _max(const uint32_t x, const uint32_t y)
{
  return (x > y) ? x : y;
}

int _pkt_build_reset_from_pkt(gnrc_pktsnip_t **out_pkt, gnrc_pktsnip_t *in_pkt)
{
    tcp_hdr_t tcp_hdr_out;

    /* Extract headers */
    gnrc_pktsnip_t *tcp_snp;
    LL_SEARCH_SCALAR(in_pkt, tcp_snp, type, GNRC_NETTYPE_TCP);
    tcp_hdr_t *tcp_hdr_in = (tcp_hdr_t *)tcp_snp->data;
#ifdef MODULE_GNRC_IPV6
    gnrc_pktsnip_t *ip6_snp;
    LL_SEARCH_SCALAR(in_pkt, ip6_snp, type, GNRC_NETTYPE_IPV6);
    ipv6_hdr_t *ip6_hdr = (ipv6_hdr_t *)ip6_snp->data;
#endif

    /* Setup header information */
    tcp_hdr_out.src_port = tcp_hdr_in->dst_port;
    tcp_hdr_out.dst_port = tcp_hdr_in->src_port;
    tcp_hdr_out.checksum = byteorder_htons(0);
    tcp_hdr_out.window = byteorder_htons(0);
    tcp_hdr_out.urgent_ptr = byteorder_htons(0);

    /* Seq/Ackno and control flags depend on inputs ACK-Flag */
    uint16_t ctl = byteorder_ntohs(tcp_hdr_in->off_ctl);
    if (ctl & MSK_ACK) {
        tcp_hdr_out.off_ctl = byteorder_htons((TCP_HDR_OFFSET_MIN << 12) | MSK_RST);
        tcp_hdr_out.seq_num = tcp_hdr_in->ack_num;
        tcp_hdr_out.ack_num = byteorder_htonl(0);
    }
    else {
        uint8_t seq_no = 0;
        tcp_hdr_out.off_ctl = byteorder_htons((TCP_HDR_OFFSET_MIN << 12) | MSK_RST_ACK);
        tcp_hdr_out.seq_num = byteorder_htonl(0);
        if (ctl & MSK_SYN) {
            seq_no += 1;
        }
        if (ctl & MSK_FIN) {
            seq_no += 1;
        }
        uint32_t tmp = byteorder_ntohl(tcp_hdr_in->seq_num);
        tcp_hdr_out.ack_num = byteorder_htonl(seq_no + tmp  + _pkt_get_pay_len(in_pkt));
    }

    /* Allocate new TCB header */
    tcp_snp = gnrc_pktbuf_add(NULL, &tcp_hdr_out, TCP_HDR_OFFSET_MIN * 4, GNRC_NETTYPE_TCP);
    if (tcp_snp == NULL) {
        DEBUG("gnrc_tcp_pkt.c : _pkt_build_reset_from_pkt() :\
               Can't alloc buffer for TCP Header\n.");
        *(out_pkt) = NULL;
        return -ENOMEM;
    }
    *out_pkt = tcp_snp;

    /* Build new network layer header */
#ifdef MODULE_GNRC_IPV6
    ip6_snp = gnrc_ipv6_hdr_build(tcp_snp, &(ip6_hdr->dst), &(ip6_hdr->src));
    if (ip6_snp == NULL) {
        DEBUG("gnrc_tcp_pkt.c : _pkt_build_reset_from_pkt() :\
               Can't alloc buffer for IPv6 Header.\n");
        gnrc_pktbuf_release(tcp_snp);
        *(out_pkt) = NULL;
        return -ENOMEM;
    }
    *out_pkt = ip6_snp;

    /* Add netif header in case the receiver addr sent from a link local address */
    if (ipv6_addr_is_link_local(&ip6_hdr->src)) {

        /* Search for netif header in received packet */
        gnrc_pktsnip_t *net_snp;
        LL_SEARCH_SCALAR(in_pkt, net_snp, type, GNRC_NETTYPE_NETIF);
        gnrc_netif_hdr_t *net_hdr = (gnrc_netif_hdr_t *)net_snp->data;

        /* Allocate new header and set interface id */
        net_snp = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
        if (net_snp == NULL) {
            DEBUG("gnrc_tcp_pkt.c : _pkt_build_reset_from_pkt() :\
                   Can't alloc buffer for netif Header.\n");
            gnrc_pktbuf_release(ip6_snp);
            *(out_pkt) = NULL;
            return -ENOMEM;
        }
        else {
            ((gnrc_netif_hdr_t *)net_snp->data)->if_pid = net_hdr->if_pid;
            LL_PREPEND(ip6_snp, net_snp);
            *(out_pkt) = net_snp;
        }
    }
#else
    DEBUG("gnrc_tcp_pkt.c : _pkt_build_reset_from_pkt() : Network Layer Module Missing\n");
#endif
    return 0;
}

int _pkt_build(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t **out_pkt, uint16_t *seq_con,
               const uint16_t ctl, const uint32_t seq_num, const uint32_t ack_num,
               void *payload, const size_t payload_len)
{
    gnrc_pktsnip_t *pay_snp = NULL;
    gnrc_pktsnip_t *tcp_snp = NULL;
    tcp_hdr_t tcp_hdr;
    uint8_t offset = TCP_HDR_OFFSET_MIN;

    /* Add payload, if supplied */
    if (payload != NULL && payload_len > 0) {
        pay_snp = gnrc_pktbuf_add(pay_snp, payload, payload_len, GNRC_NETTYPE_UNDEF);
        if (pay_snp == NULL) {
            DEBUG("gnrc_tcp_pkt.c : _pkt_build() : Can't allocate buffer for payload\n.");
            *(out_pkt) = NULL;
            return -ENOMEM;
        }
    }

    /* Fill TCP header */
    tcp_hdr.src_port = byteorder_htons(tcb->local_port);
    tcp_hdr.dst_port = byteorder_htons(tcb->peer_port);
    tcp_hdr.checksum = byteorder_htons(0);
    tcp_hdr.seq_num = byteorder_htonl(seq_num);
    tcp_hdr.ack_num = byteorder_htonl(ack_num);
    tcp_hdr.window = byteorder_htons(tcb->rcv_wnd);
    tcp_hdr.urgent_ptr = byteorder_htons(0);

    /* Calculate option field size. */
    /* Add MSS option if SYN is sent */
    if (ctl & MSK_SYN) {
        offset += 1;
    }
    /* Set offset and control bit accordingly */
    tcp_hdr.off_ctl = byteorder_htons(_option_build_offset_control(offset, ctl));

    tcp_snp = gnrc_pktbuf_add(pay_snp, &tcp_hdr, sizeof(tcp_hdr), GNRC_NETTYPE_TCP);
    if (tcp_snp == NULL) {
        DEBUG("gnrc_tcp_pkt.c : _pkt_build() : Can't allocate buffer for TCP Header\n.");
        gnrc_pktbuf_release(pay_snp);
        *(out_pkt) = NULL;
        return -ENOMEM;
    }
    else {
        /* Resize TCP header: size = offset * 4 bytes */
        if (gnrc_pktbuf_realloc_data(tcp_snp, offset * 4)) {
            DEBUG("gnrc_tcp_pkt.c : _pkt_build() : Couldn't set size of TCP header\n");
            gnrc_pktbuf_release(tcp_snp);
            *(out_pkt) = NULL;
            return -ENOMEM;
        }

        /* Add options if existing */
        if (TCP_HDR_OFFSET_MIN < offset) {
            uint8_t *opt_ptr = (uint8_t *) tcp_snp->data + sizeof(tcp_hdr);
            uint8_t opt_left = (offset - TCP_HDR_OFFSET_MIN) * sizeof(network_uint32_t);

            /* Init options field with 'End Of List' - option (0) */
            memset(opt_ptr, TCP_OPTION_KIND_EOL, opt_left);

            /* If SYN flag is set: Add MSS option */
            if (ctl & MSK_SYN) {
                network_uint32_t mss_option = byteorder_htonl(_option_build_mss(CONFIG_GNRC_TCP_MSS));
                memcpy(opt_ptr, &mss_option, sizeof(mss_option));
            }
            /* Increase opt_ptr and decrease opt_left, if other options are added */
            /* NOTE: Add additional options here */
        }
        *(out_pkt) = tcp_snp;
    }

    /* Build network layer header */
#ifdef MODULE_GNRC_IPV6
    ipv6_addr_t *src_addr = (ipv6_addr_t *) tcb->local_addr;
    ipv6_addr_t *dst_addr = (ipv6_addr_t *) tcb->peer_addr;

    gnrc_pktsnip_t *ip6_snp = gnrc_ipv6_hdr_build(tcp_snp, src_addr, dst_addr);
    if (ip6_snp == NULL) {
        DEBUG("gnrc_tcp_pkt.c : _pkt_build() : Can't allocate buffer for IPv6 Header.\n");
        gnrc_pktbuf_release(tcp_snp);
        *(out_pkt) = NULL;
        return -ENOMEM;
    }
    else {
        *(out_pkt) = ip6_snp;
    }

    /* Prepend network interface header if an interface id was specified */
    if (tcb->ll_iface > 0) {
        gnrc_pktsnip_t *net_snp = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
        if (net_snp == NULL) {
            DEBUG("gnrc_tcp_pkt.c : _pkt_build() : Can't allocate buffer for netif header.\n");
            gnrc_pktbuf_release(ip6_snp);
            *(out_pkt) = NULL;
            return -ENOMEM;
        }
        else {
            ((gnrc_netif_hdr_t *)net_snp->data)->if_pid = (kernel_pid_t)tcb->ll_iface;
            LL_PREPEND(ip6_snp, net_snp);
            *(out_pkt) = net_snp;
        }
    }
#else
        DEBUG("gnrc_tcp_pkt.c : _pkt_build_reset_from_pkt() : Network Layer Module Missing\n");
#endif

    /* Calculate sequence space number consumption for this packet */
    if (seq_con != NULL) {
        *seq_con = 0;
        if (ctl & MSK_SYN) {
            *seq_con += 1;
        }
        if (ctl & MSK_FIN) {
            *seq_con += 1;
        }
        *seq_con += payload_len;
    }
    return 0;
}

int _pkt_send(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *out_pkt, const uint16_t seq_con,
              const bool retransmit)
{
    if (out_pkt == NULL) {
        DEBUG("gnrc_tcp_pkt.c : _pkt_send() : Packet to send is null\n");
        return -EINVAL;
    }

    /* If this is no retransmission, advance sequence number and measure time */
    if (!retransmit) {
        tcb->retries = 0;
        tcb->snd_nxt += seq_con;
        tcb->rtt_start = xtimer_now().ticks32;
    }
    else {
        tcb->retries += 1;
    }

    /* Pass packet down the network stack */
    if (gnrc_netapi_send(gnrc_tcp_pid, out_pkt) < 1) {
        DEBUG("gnrc_tcp_pkt.c : _pkt_send() : unable to send packet\n");
        gnrc_pktbuf_release(out_pkt);
    }
    return 0;
}

int _pkt_chk_seq_num(const gnrc_tcp_tcb_t *tcb, const uint32_t seq_num, const uint32_t seg_len)
{
    uint32_t l_edge = tcb->rcv_nxt;
    uint32_t r_edge = tcb->rcv_nxt + tcb->rcv_wnd;
    uint32_t last_seq = seq_num + seg_len - 1;

    /* Possible case 1: */
    /* Segment contains no payload and receive window is closed and */
    /* sequence number is next expected number */
    if (seg_len == 0 && tcb->rcv_wnd == 0 && l_edge == seq_num) {
        return 0;
    }

    /* Possible case 2: */
    /* Segment contains no payload and receive window is open and */
    /* sequence number falls inside the receive window */
    if (seg_len == 0 && tcb->rcv_wnd > 0 && INSIDE_WND(l_edge, seq_num, r_edge)) {
        return 0;
    }

    /* Possible case 3: */
    /* Segment contains payload and receive window is open and */
    /* sequence number overlaps with receive window */
    if (seg_len > 0 && tcb->rcv_wnd > 0 && (INSIDE_WND(l_edge, seq_num, r_edge) ||
        INSIDE_WND(l_edge, last_seq, r_edge))) {
        return 0;
    }

    /* Everything else is not acceptable */
    return -1;
}

uint32_t _pkt_get_seg_len(gnrc_pktsnip_t *pkt)
{
    uint32_t seq = 0;
    uint16_t ctl = 0;
    gnrc_pktsnip_t *snp = NULL;

    LL_SEARCH_SCALAR(pkt, snp, type, GNRC_NETTYPE_TCP);
    tcp_hdr_t *hdr = (tcp_hdr_t *) snp->data;
    ctl = byteorder_ntohs(hdr->off_ctl);
    seq = _pkt_get_pay_len(pkt);
    if (ctl & MSK_SYN) {
        seq += 1;
    }
    if (ctl & MSK_FIN) {
        seq += 1;
    }
    return seq;
}

uint32_t _pkt_get_pay_len(gnrc_pktsnip_t *pkt)
{
    uint32_t seg_len = 0;
    gnrc_pktsnip_t *snp = NULL;

    LL_SEARCH_SCALAR(pkt, snp, type, GNRC_NETTYPE_UNDEF);
    while (snp && snp->type == GNRC_NETTYPE_UNDEF) {
        seg_len += snp->size;
        snp = snp->next;
    }
    return seg_len;
}

int _pkt_setup_retransmit(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *pkt, const bool retransmit)
{
    gnrc_pktsnip_t *snp = NULL;
    uint32_t ctl = 0;
    uint32_t len = 0;

    /* No packet received */
    if (pkt == NULL) {
        DEBUG("gnrc_tcp_pkt.c : _pkt_setup_retransmit() : pkt=NULL\n");
        return -EINVAL;
    }

    /* Check if retransmit queue is full and pkt is not already in retransmit queue */
    if (tcb->pkt_retransmit != NULL && tcb->pkt_retransmit != pkt) {
        DEBUG("gnrc_tcp_pkt.c : _pkt_setup_retransmit() : Nothing to do\n");
        return -ENOMEM;
    }

    /* Extract control bits and segment length */
    LL_SEARCH_SCALAR(pkt, snp, type, GNRC_NETTYPE_TCP);
    ctl = byteorder_ntohs(((tcp_hdr_t *) snp->data)->off_ctl);
    len = _pkt_get_pay_len(pkt);

    /* Check if pkt contains reset or is a pure ACK, return */
    if ((ctl & MSK_RST) || (((ctl & MSK_SYN_FIN_ACK) == MSK_ACK) && len == 0)) {
        return 0;
    }

    /* Assign pkt and increase users: every send attempt consumes a user */
    tcb->pkt_retransmit = pkt;
    gnrc_pktbuf_hold(pkt, 1);

    /* RTO adjustment */
    if (!retransmit) {
        /* If this is the first transmission: rto is 1 sec (Lower Bound) */
        if (tcb->srtt == RTO_UNINITIALIZED || tcb->rtt_var == RTO_UNINITIALIZED) {
            tcb->rto = CONFIG_GNRC_TCP_RTO_LOWER_BOUND;
        }
        else {
            tcb->rto = tcb->srtt + _max(CONFIG_GNRC_TCP_RTO_GRANULARITY, CONFIG_GNRC_TCP_RTO_K * tcb->rtt_var);
        }
    }
    else {
        /* If this is a retransmission: Double the rto (Timer Backoff) */
        tcb->rto *= 2;

        /* If the transmission has been tried five times, we assume srtt and rtt_var are bogus */
        /* New measurements must be taken the next time something is sent. */
        if (tcb->retries >= 5) {
            tcb->srtt = RTO_UNINITIALIZED;
            tcb->rtt_var = RTO_UNINITIALIZED;
        }
    }

    /* Perform boundary checks on current RTO before usage */
    if (tcb->rto < (int32_t) CONFIG_GNRC_TCP_RTO_LOWER_BOUND) {
        tcb->rto = CONFIG_GNRC_TCP_RTO_LOWER_BOUND;
    }
    else if (tcb->rto > (int32_t) CONFIG_GNRC_TCP_RTO_UPPER_BOUND) {
        tcb->rto = CONFIG_GNRC_TCP_RTO_UPPER_BOUND;
    }

    /* Setup retransmission timer, msg to TCP thread with ptr to TCB */
    tcb->msg_retransmit.type = MSG_TYPE_RETRANSMISSION;
    tcb->msg_retransmit.content.ptr = (void *) tcb;
    xtimer_set_msg(&tcb->timer_retransmit, tcb->rto, &tcb->msg_retransmit, gnrc_tcp_pid);
    return 0;
}

int _pkt_acknowledge(gnrc_tcp_tcb_t *tcb, const uint32_t ack)
{
    uint32_t seg = 0;
    gnrc_pktsnip_t *snp = NULL;
    tcp_hdr_t *hdr;

    /* Retransmission queue is empty. Nothing to ACK there */
    if (tcb->pkt_retransmit == NULL) {
        DEBUG("gnrc_tcp_pkt.c : _pkt_acknowledge() : There is no packet to ack\n");
        return -ENODATA;
    }

    LL_SEARCH_SCALAR(tcb->pkt_retransmit, snp, type, GNRC_NETTYPE_TCP);
    hdr = (tcp_hdr_t *) snp->data;

    /* There must be a packet, waiting to be acknowledged. */
    seg = byteorder_ntohl(hdr->seq_num) + _pkt_get_seg_len(tcb->pkt_retransmit) - 1;

    /* If segment can be acknowledged -> stop timer, release packet from pktbuf and update rto. */
    if (LSS_32_BIT(seg, ack)) {
        xtimer_remove(&(tcb->timer_retransmit));
        gnrc_pktbuf_release(tcb->pkt_retransmit);
        tcb->pkt_retransmit = NULL;

        /* Measure round trip time */
        int32_t rtt = xtimer_now().ticks32 - tcb->rtt_start;

        /* Use time only if there was no timer overflow and no retransmission (Karns Algorithm) */
        if (tcb->retries == 0 && rtt > 0) {
            /* If this is the first sample taken */
            if (tcb->srtt == RTO_UNINITIALIZED && tcb->rtt_var == RTO_UNINITIALIZED) {
                tcb->srtt = rtt;
                tcb->rtt_var = (rtt >> 1);
            }
            /* If this is a subsequent sample */
            else {
                tcb->rtt_var = (tcb->rtt_var / CONFIG_GNRC_TCP_RTO_B_DIV) * (CONFIG_GNRC_TCP_RTO_B_DIV-1);
                tcb->rtt_var += labs(tcb->srtt - rtt) / CONFIG_GNRC_TCP_RTO_B_DIV;
                tcb->srtt = (tcb->srtt / CONFIG_GNRC_TCP_RTO_A_DIV) * (CONFIG_GNRC_TCP_RTO_A_DIV-1);
                tcb->srtt += rtt / CONFIG_GNRC_TCP_RTO_A_DIV;
            }
        }
    }
    return 0;
}

uint16_t _pkt_calc_csum(const gnrc_pktsnip_t *hdr, const gnrc_pktsnip_t *pseudo_hdr,
                        const gnrc_pktsnip_t *payload)
{
    uint16_t csum = 0;
    uint16_t len = (uint16_t) hdr->size;

    if (pseudo_hdr == NULL) {
        return 0;
    }

    /* Process payload */
    while (payload && payload != hdr) {
        csum = inet_csum(csum, (uint8_t *)payload->data, payload->size);
        len += (uint16_t)payload->size;
        payload = payload->next;
    }

    /* Process TCP header, before checksum field(Byte 16 to 18) */
    csum = inet_csum(csum, (uint8_t *) hdr->data, 16);
    /* Process TCP header, after checksum field */
    csum = inet_csum(csum, ((uint8_t *) hdr->data) + 18, hdr->size - 18);

    /* Process network layer header */
    switch (pseudo_hdr->type) {
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            csum = ipv6_hdr_inet_csum(csum, pseudo_hdr->data, PROTNUM_TCP, len);
            break;
#endif
        default:
            /* Suppress compiler warnings */
            (void) len;
            return 0;
    }
    return ~csum;
}
