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
 * @brief       Implementation of internal/eventloop.h
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 * @}
 */

#include <utlist.h>
#include <errno.h>
#include "net/af.h"
#include "net/tcp.h"
#include "internal/common.h"
#include "internal/pkt.h"
#include "internal/fsm.h"
#include "internal/eventloop.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief send function, pass paket down the network stack
 *
 * @param[in] pkt   paket to pass down the network stack
 *
 * @return   zero on success
 * @return   negative value on error
 * @return   -EBADMSG if tcp header is missing
 */
static int _send(gnrc_pktsnip_t *pkt)
{
    /* NOTE: Sending Direction: pkt = nw, nw->next = tcp, tcp->next = payload */
    gnrc_pktsnip_t *tcp;

    /* Search for tcp header */
    LL_SEARCH_SCALAR(pkt, tcp, type, GNRC_NETTYPE_TCP);
    if (tcp == NULL) {
        DEBUG("gnrc_tcp_eventloop : _send() : tcp header missing.\n");
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }

    /* Dispatch to network layer */
    if (!gnrc_netapi_dispatch_send(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("gnrc_tcp_eventloop : _send() : network layer not found\n");
        gnrc_pktbuf_release(pkt);
    }
    return 0;
}

/**
 * @brief recv function, receive packet from network layer.
 *
 * @param[in] pkt   incomming paket to process
 *
 * @return zero on success
 * @return negative value on error
 * @return -EACCES if not able to get write access to packet
 * @return -ERANGE if segment offset is less than 5
 * @return -ENOMSG if paket can't be marked
 * @return -EINVAL if checksum was invalid
 * @return -ENOTCONN if no module is interested in this context
 */
static int _receive(gnrc_pktsnip_t *pkt)
{
    /* NOTE: Receiving direction: pkt = payload, payload->next = tcp, tcp->next = nw */
    uint16_t ctl = 0;
    uint16_t src = 0;
    uint16_t dst = 0;
    uint8_t hdr_size = 0;
    uint8_t syn = 0;
    gnrc_pktsnip_t *ip = NULL;
    gnrc_pktsnip_t *reset = NULL;
    gnrc_tcp_tcb_t *tcb = NULL;
    tcp_hdr_t *hdr;

    /* Get write access to the TCP Header */
    gnrc_pktsnip_t *tcp = gnrc_pktbuf_start_write(pkt);
    if (tcp == NULL) {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : can't write to packet\n");
        gnrc_pktbuf_release(pkt);
        return -EACCES;
    }
    pkt = tcp;

#ifdef MODULE_GNRC_IPV6
    /* Get IP Header, discard packet if doesn't contain an ip header */
    LL_SEARCH_SCALAR(pkt, ip, type, GNRC_NETTYPE_IPV6);
    if (ip == NULL) {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : pkt contains no IP Header\n");
        gnrc_pktbuf_release(pkt);
        return 0;
    }
#endif

    /* Get TCP Header */
    LL_SEARCH_SCALAR(pkt, tcp, type, GNRC_NETTYPE_TCP);
    if (tcp == NULL) {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : pkt contains no TCP Header\n");
        gnrc_pktbuf_release(pkt);
        return 0;
    }

    /* Extract control bits, src and dst ports and check if SYN is set (not SYN+ACK) */
    hdr = (tcp_hdr_t *)tcp->data;
    ctl = byteorder_ntohs(hdr->off_ctl);
    src = byteorder_ntohs(hdr->src_port);
    dst = byteorder_ntohs(hdr->dst_port);
    syn = ((ctl & MSK_SYN_ACK) == MSK_SYN);

    /* Validate Offset */
    if (GET_OFFSET(ctl) < TCP_HDR_OFFSET_MIN) {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : unexpected Offset Value\n");
        gnrc_pktbuf_release(pkt);
        return -ERANGE;
    }

    /* Calculate tcp header size */
    hdr_size = GET_OFFSET(ctl) * 4;

    /* Mark TCP-Header, if it contains any payload */
    if ((pkt->type == GNRC_NETTYPE_TCP) && (pkt->size != hdr_size)) {
        tcp = gnrc_pktbuf_mark(pkt, hdr_size, GNRC_NETTYPE_TCP);
        if (tcp == NULL) {
            DEBUG("gnrc_tcp_eventloop.c : _receive() : Header marking failed\n");
            gnrc_pktbuf_release(pkt);
            return -ENOMSG;
        }
        pkt->type = GNRC_NETTYPE_UNDEF;
    }

    /* Validate Checksum */
    if (byteorder_ntohs(hdr->checksum) != _pkt_calc_csum(tcp, ip, pkt)) {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : Invalid checksum\n");
        gnrc_pktbuf_release(pkt);
        return -EINVAL;
    }

    /* Find tcb to de-multiplex this packet to */
    mutex_lock(&_list_tcb_lock);
    tcb = _list_tcb_head;
    while (tcb) {
#ifdef MODULE_GNRC_IPV6
        /* Check if current tcb is fitting for the incomming packet */
        if (ip->type == GNRC_NETTYPE_IPV6 && tcb->address_family == AF_INET6) {
            /* If SYN is set, a connection is listening on that port ... */
            ipv6_addr_t *tmp_addr = NULL;
            if (syn && tcb->local_port == dst && tcb->state == FSM_STATE_LISTEN) {
                /* ... and local addr is unspec or preconfigured */
                tmp_addr = &((ipv6_hdr_t *)ip->data)->dst;
                if (ipv6_addr_equal((ipv6_addr_t *) tcb->local_addr, (ipv6_addr_t *) tmp_addr) ||
                    ipv6_addr_is_unspecified((ipv6_addr_t *) tcb->local_addr)) {
                    break;
                }
            }

            /* If SYN is not set and the ports match ... */
            if (!syn && tcb->local_port == dst && tcb->peer_port == src) {
                /* .. and the IP-Addresses match */
                tmp_addr = &((ipv6_hdr_t * )ip->data)->src;
                if (ipv6_addr_equal((ipv6_addr_t *) tcb->peer_addr, (ipv6_addr_t *) tmp_addr)) {
                    break;
                }
            }
        }
#else
        /* Supress compiler warnings if TCP is build without IP-Layer */
        (void) syn;
        (void) src;
        (void) dst;
#endif
        tcb = tcb->next;
    }
    mutex_unlock(&_list_tcb_lock);

    /* Call FSM with event RCVD_PKT if a fitting connection was found */
    if (tcb != NULL) {
        _fsm(tcb, FSM_EVENT_RCVD_PKT, pkt, NULL, 0);
    }
    /* No fitting connection has been found. Respond with reset */
    else {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : Can't find fitting connection\n");
        if ((ctl & MSK_RST) != MSK_RST) {
            _pkt_build_reset_from_pkt(&reset, pkt);
            gnrc_netapi_send(gnrc_tcp_pid, reset);
        }
        return -ENOTCONN;
    }
    gnrc_pktbuf_release(pkt);
    return 0;
}

void *_event_loop(__attribute__((unused)) void *arg)
{
    msg_t msg;
    msg_t reply;
    msg_t msg_queue[TCP_EVENTLOOP_MSG_QUEUE_SIZE];

    /* Store pid */
    gnrc_tcp_pid = thread_getpid();

    /* Setup reply message */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t)-ENOTSUP;

    /* Init message queue*/
    msg_init_queue(msg_queue, TCP_EVENTLOOP_MSG_QUEUE_SIZE);

    /* Register GNRC_tcp in netreg */
    gnrc_netreg_entry_t entry;
    gnrc_netreg_entry_init_pid(&entry, GNRC_NETREG_DEMUX_CTX_ALL, gnrc_tcp_pid);
    gnrc_netreg_register(GNRC_NETTYPE_TCP, &entry);

    /* dispatch NETAPI Messages */
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            /* Pass Message up the network stack */
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : GNRC_NETAPI_MSG_TYPE_RCV\n");
                _receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Pass Message down the network stack */
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : GNRC_NETAPI_MSG_TYPE_SND\n");
                _send((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Option Set and Get Messages*/
            case GNRC_NETAPI_MSG_TYPE_SET:
            case GNRC_NETAPI_MSG_TYPE_GET:
                msg_reply(&msg, &reply);
                break;

            /* Retransmission Timer expired -> Call FSM with retransmission event */
            case MSG_TYPE_RETRANSMISSION:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : MSG_TYPE_RETRANSMISSION\n");
                _fsm((gnrc_tcp_tcb_t *)msg.content.ptr, FSM_EVENT_TIMEOUT_RETRANSMIT,
                     NULL, NULL, 0);
                break;

            /* Time Wait Timer expired -> Call FSM with timewait event */
            case MSG_TYPE_TIMEWAIT:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : MSG_TYPE_TIMEWAIT\n");
                _fsm((gnrc_tcp_tcb_t *)msg.content.ptr, FSM_EVENT_TIMEOUT_TIMEWAIT,
                     NULL, NULL, 0);
                break;

            default:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : received expected message\n");
        }
    }
    /* never reached */
    return NULL;
}
