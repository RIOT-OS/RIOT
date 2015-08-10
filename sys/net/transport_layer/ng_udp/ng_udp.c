/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_ng_udp
 * @{
 *
 * @file
 * @brief       UDP implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <errno.h>

#include "kernel.h"
#include "byteorder.h"
#include "msg.h"
#include "thread.h"
#include "utlist.h"
#include "net/ng_udp.h"
#include "net/ng_netbase.h"
#include "net/inet_csum.h"

#ifdef MODULE_NG_IPV6
#include "net/ng_ipv6/hdr.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Save the UDP's thread PID for later reference
 */
static kernel_pid_t _pid = KERNEL_PID_UNDEF;

/**
 * @brief   Allocate memory for the UDP thread's stack
 */
#if ENABLE_DEBUG
static char _stack[NG_UDP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[NG_UDP_STACK_SIZE];
#endif

/**
 * @brief   Calculate the UDP checksum dependent on the network protocol
 *
 * @note    If the checksum turns out to be 0x0000, the function returns 0xffff
 *          as specified in RFC768
 *
 * @param[in] pkt           pointer to the packet in the packet buffer
 * @param[in] pseudo_hdr    pointer to the network layer header
 * @param[in] payload       pointer to the payload
 *
 * @return                  the checksum of the pkt in host byte order
 * @return                  0 on error
 */
static uint16_t _calc_csum(ng_pktsnip_t *hdr, ng_pktsnip_t *pseudo_hdr,
                           ng_pktsnip_t *payload)
{
    uint16_t csum = 0;
    uint16_t len = (uint16_t)hdr->size;

    /* process the payload */
    while (payload && payload != hdr) {
        csum = inet_csum(csum, (uint8_t *)(payload->data), payload->size);
        len += (uint16_t)payload->size;
        payload = payload->next;
    }
    /* process applicable UDP header bytes */
    csum = inet_csum(csum, (uint8_t *)hdr->data, sizeof(udp_hdr_t));

    switch (pseudo_hdr->type) {
#ifdef MODULE_NG_IPV6
        case NG_NETTYPE_IPV6:
            csum = ng_ipv6_hdr_inet_csum(csum, pseudo_hdr->data,
                                         PROTNUM_UDP, len);
            break;
#endif
        default:
            (void)len;
            return 0;
    }
    /* return inverted results */
    return ~csum;
}

static void _receive(ng_pktsnip_t *pkt)
{
    ng_pktsnip_t *udp, *ipv6;
    udp_hdr_t *hdr;
    uint32_t port;

    /* mark UDP header */
    udp = ng_pktbuf_start_write(pkt);
    if (udp == NULL) {
        DEBUG("udp: unable to get write access to packet\n");
        ng_pktbuf_release(pkt);
        return;
    }
    pkt = udp;
    udp = ng_pktbuf_mark(pkt, sizeof(udp_hdr_t), NG_NETTYPE_UDP);
    if (udp == NULL) {
        DEBUG("udp: error marking UDP header, dropping packet\n");
        ng_pktbuf_release(pkt);
        return;
    }
    /* mark payload as Type: UNDEF */
    pkt->type = NG_NETTYPE_UNDEF;
    /* get explicit pointer to UDP header */
    hdr = (udp_hdr_t *)udp->data;

    LL_SEARCH_SCALAR(pkt, ipv6, type, NG_NETTYPE_IPV6);

    assert(ipv6 != NULL);

    /* validate checksum */
    if (_calc_csum(udp, ipv6, pkt)) {
        DEBUG("udp: received packet with invalid checksum, dropping it\n");
        ng_pktbuf_release(pkt);
        return;
    }

    /* get port (netreg demux context) */
    port = (uint32_t)byteorder_ntohs(hdr->dst_port);

    /* send payload to receivers */
    if (!ng_netapi_dispatch_receive(NG_NETTYPE_UDP, port, pkt)) {
        DEBUG("udp: unable to forward packet as no one is interested in it\n");
        ng_pktbuf_release(pkt);
    }
}

static void _send(ng_pktsnip_t *pkt)
{
    udp_hdr_t *hdr;
    ng_pktsnip_t *udp_snip;

    /* get udp snip and hdr */
    LL_SEARCH_SCALAR(pkt, udp_snip, type, NG_NETTYPE_UDP);

    assert(udp_snip != NULL);

    udp_snip = ng_pktbuf_start_write(udp_snip);
    if (udp_snip == NULL) {
        DEBUG("udp: cannot send packet: unable to allocate packet\n");
        ng_pktbuf_release(pkt);
        return;
    }
    hdr = (udp_hdr_t *)udp_snip->data;
    /* fill in size field */
    hdr->length = byteorder_htons(ng_pkt_len(udp_snip));

    /* and forward packet to the network layer */
    if (!ng_netapi_dispatch_send(pkt->type, NG_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("udp: cannot send packet: network layer not found\n");
        ng_pktbuf_release(pkt);
    }
}

static void *_event_loop(void *arg)
{
    (void)arg;
    msg_t msg, reply;
    msg_t msg_queue[NG_UDP_MSG_QUEUE_SIZE];
    ng_netreg_entry_t netreg;

    /* preset reply message */
    reply.type = NG_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t)-ENOTSUP;
    /* initialize message queue */
    msg_init_queue(msg_queue, NG_UDP_MSG_QUEUE_SIZE);
    /* register UPD at netreg */
    netreg.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;
    netreg.pid = thread_getpid();
    ng_netreg_register(NG_NETTYPE_UDP, &netreg);

    /* dispatch NETAPI messages */
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case NG_NETAPI_MSG_TYPE_RCV:
                DEBUG("udp: NG_NETAPI_MSG_TYPE_RCV\n");
                _receive((ng_pktsnip_t *)msg.content.ptr);
                break;
            case NG_NETAPI_MSG_TYPE_SND:
                DEBUG("udp: NG_NETAPI_MSG_TYPE_SND\n");
                _send((ng_pktsnip_t *)msg.content.ptr);
                break;
            case NG_NETAPI_MSG_TYPE_SET:
            case NG_NETAPI_MSG_TYPE_GET:
                msg_reply(&msg, &reply);
                break;
            default:
                DEBUG("udp: received unidentified message\n");
                break;
        }
    }

    /* never reached */
    return NULL;
}

int ng_udp_calc_csum(ng_pktsnip_t *hdr, ng_pktsnip_t *pseudo_hdr)
{
    uint16_t csum;

    if ((hdr == NULL) || (pseudo_hdr == NULL)) {
        return -EFAULT;
    }
    if (hdr->type != NG_NETTYPE_UDP) {
        return -EBADMSG;
    }

    csum = _calc_csum(hdr, pseudo_hdr, hdr->next);
    if (csum == 0) {
        return -ENOENT;
    }
    ((udp_hdr_t *)hdr->data)->checksum = byteorder_htons(csum);
    return 0;
}

ng_pktsnip_t *ng_udp_hdr_build(ng_pktsnip_t *payload,
                               uint8_t *src, size_t src_len,
                               uint8_t *dst, size_t dst_len)
{
    ng_pktsnip_t *res;
    udp_hdr_t *hdr;

    /* check parameters */
    if (src == NULL || dst == NULL ||
        src_len != sizeof(uint16_t) || dst_len != sizeof(uint16_t)) {
        return NULL;
    }
    /* allocate header */
    res = ng_pktbuf_add(payload, NULL, sizeof(udp_hdr_t), NG_NETTYPE_UDP);
    if (res == NULL) {
        return NULL;
    }
    /* initialize header */
    hdr = (udp_hdr_t *)res->data;
    hdr->src_port = byteorder_htons(*((uint16_t *)src));
    hdr->dst_port = byteorder_htons(*((uint16_t *)dst));
    hdr->checksum = byteorder_htons(0);
    return res;
}

int ng_udp_init(void)
{
    /* check if thread is already running */
    if (_pid == KERNEL_PID_UNDEF) {
        /* start UDP thread */
        _pid = thread_create(_stack, sizeof(_stack), NG_UDP_PRIO,
                             CREATE_STACKTEST, _event_loop, NULL, "udp");
    }
    return _pid;
}
