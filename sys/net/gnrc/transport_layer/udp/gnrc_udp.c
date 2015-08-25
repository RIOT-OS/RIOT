/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_udp
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
#include "net/ipv6/hdr.h"
#include "net/gnrc/udp.h"
#include "net/gnrc.h"
#include "net/inet_csum.h"


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
static char _stack[GNRC_UDP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_UDP_STACK_SIZE];
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
static uint16_t _calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr,
                           gnrc_pktsnip_t *payload)
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
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            csum = ipv6_hdr_inet_csum(csum, pseudo_hdr->data, PROTNUM_UDP, len);
            break;
#endif
        default:
            (void)len;
            return 0;
    }
    /* return inverted results */
    return ~csum;
}

static void _receive(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *udp, *ipv6;
    udp_hdr_t *hdr;
    uint32_t port;

    /* mark UDP header */
    udp = gnrc_pktbuf_start_write(pkt);
    if (udp == NULL) {
        DEBUG("udp: unable to get write access to packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = udp;
    udp = gnrc_pktbuf_mark(pkt, sizeof(udp_hdr_t), GNRC_NETTYPE_UDP);
    if (udp == NULL) {
        DEBUG("udp: error marking UDP header, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    /* mark payload as Type: UNDEF */
    pkt->type = GNRC_NETTYPE_UNDEF;
    /* get explicit pointer to UDP header */
    hdr = (udp_hdr_t *)udp->data;

    LL_SEARCH_SCALAR(pkt, ipv6, type, GNRC_NETTYPE_IPV6);

    assert(ipv6 != NULL);

    /* validate checksum */
    if (_calc_csum(udp, ipv6, pkt)) {
        DEBUG("udp: received packet with invalid checksum, dropping it\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* get port (netreg demux context) */
    port = (uint32_t)byteorder_ntohs(hdr->dst_port);

    /* send payload to receivers */
    if (!gnrc_netapi_dispatch_receive(GNRC_NETTYPE_UDP, port, pkt)) {
        DEBUG("udp: unable to forward packet as no one is interested in it\n");
        gnrc_pktbuf_release(pkt);
    }
}

static void _send(gnrc_pktsnip_t *pkt)
{
    udp_hdr_t *hdr;
    gnrc_pktsnip_t *udp_snip, *tmp;

    /* write protect first header */
    tmp = gnrc_pktbuf_start_write(pkt);
    if (tmp == NULL) {
        DEBUG("udp: cannot send packet: unable to allocate packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = tmp;
    udp_snip = tmp->next;

    /* get and write protect until udp snip */
    while ((udp_snip != NULL) && (udp_snip->type != GNRC_NETTYPE_UDP)) {
        udp_snip = gnrc_pktbuf_start_write(udp_snip);
        if (udp_snip == NULL) {
            DEBUG("udp: cannot send packet: unable to allocate packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        tmp->next = udp_snip;
        tmp = udp_snip;
        udp_snip = udp_snip->next;
    }

    assert(udp_snip != NULL);

    /* write protect UDP snip */
    udp_snip = gnrc_pktbuf_start_write(udp_snip);
    if (udp_snip == NULL) {
        DEBUG("udp: cannot send packet: unable to allocate packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    hdr = (udp_hdr_t *)udp_snip->data;
    /* fill in size field */
    hdr->length = byteorder_htons(gnrc_pkt_len(udp_snip));

    /* and forward packet to the network layer */
    if (!gnrc_netapi_dispatch_send(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("udp: cannot send packet: network layer not found\n");
        gnrc_pktbuf_release(pkt);
    }
}

static void *_event_loop(void *arg)
{
    (void)arg;
    msg_t msg, reply;
    msg_t msg_queue[GNRC_UDP_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t netreg;

    /* preset reply message */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t)-ENOTSUP;
    /* initialize message queue */
    msg_init_queue(msg_queue, GNRC_UDP_MSG_QUEUE_SIZE);
    /* register UPD at netreg */
    netreg.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    netreg.pid = thread_getpid();
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &netreg);

    /* dispatch NETAPI messages */
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("udp: GNRC_NETAPI_MSG_TYPE_RCV\n");
                _receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("udp: GNRC_NETAPI_MSG_TYPE_SND\n");
                _send((gnrc_pktsnip_t *)msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
            case GNRC_NETAPI_MSG_TYPE_GET:
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

int gnrc_udp_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr)
{
    uint16_t csum;

    if ((hdr == NULL) || (pseudo_hdr == NULL)) {
        return -EFAULT;
    }
    if (hdr->type != GNRC_NETTYPE_UDP) {
        return -EBADMSG;
    }

    csum = _calc_csum(hdr, pseudo_hdr, hdr->next);
    if (csum == 0) {
        return -ENOENT;
    }
    ((udp_hdr_t *)hdr->data)->checksum = byteorder_htons(csum);
    return 0;
}

gnrc_pktsnip_t *gnrc_udp_hdr_build(gnrc_pktsnip_t *payload,
                                   uint8_t *src, size_t src_len,
                                   uint8_t *dst, size_t dst_len)
{
    gnrc_pktsnip_t *res;
    udp_hdr_t *hdr;

    /* check parameters */
    if (src == NULL || dst == NULL ||
        src_len != sizeof(uint16_t) || dst_len != sizeof(uint16_t)) {
        return NULL;
    }
    /* allocate header */
    res = gnrc_pktbuf_add(payload, NULL, sizeof(udp_hdr_t), GNRC_NETTYPE_UDP);
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

int gnrc_udp_init(void)
{
    /* check if thread is already running */
    if (_pid == KERNEL_PID_UNDEF) {
        /* start UDP thread */
        _pid = thread_create(_stack, sizeof(_stack), GNRC_UDP_PRIO,
                             CREATE_STACKTEST, _event_loop, NULL, "udp");
    }
    return _pid;
}
