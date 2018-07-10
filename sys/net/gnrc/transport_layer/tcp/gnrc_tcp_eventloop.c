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
#include "net/gnrc.h"
#include "internal/common.h"
#include "internal/pkt.h"
#include "internal/fsm.h"
#include "internal/eventloop.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

static msg_t _eventloop_msg_queue[TCP_EVENTLOOP_MSG_QUEUE_SIZE];

/**
 * @brief Send function, pass paket down the network stack.
 *
 * @param[in] pkt   Paket to send.
 *
 * @returns   Zero on success.
 *            Negative value on error.
 * @returns  -EBADMSG if TCP header is missing in @p pkt.
 */
static int _send(gnrc_pktsnip_t *pkt)
{
    assert(pkt != NULL);

    /* NOTE: In sending direction: pkt = nw, nw->next = tcp, tcp->next = payload */
    gnrc_pktsnip_t *tcp = NULL;
    gnrc_pktsnip_t *nw = NULL;

    /* Search for TCP header */
    LL_SEARCH_SCALAR(pkt, tcp, type, GNRC_NETTYPE_TCP);
    /* cppcheck-suppress knownConditionTrueFalse */
    if (tcp == NULL) {
        DEBUG("gnrc_tcp_eventloop : _send() : tcp header missing.\n");
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }

    /* Search for network layer */
#ifdef MODULE_GNRC_IPV6
    /* Get IPv6 header, discard packet if doesn't contain an ipv6 header */
    LL_SEARCH_SCALAR(pkt, nw, type, GNRC_NETTYPE_IPV6);
    if (nw == NULL) {
        DEBUG("gnrc_tcp_eventloop.c : _send() : pkt contains no ipv6 layer header\n");
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }
#endif

    /* Dispatch packet to network layer */
    assert(nw != NULL);
    if (!gnrc_netapi_dispatch_send(nw->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("gnrc_tcp_eventloop : _send() : network layer not found\n");
        gnrc_pktbuf_release(pkt);
    }
    return 0;
}

/**
 * @brief Receive function, receive packet from network layer.
 *
 * @param[in] pkt   Incomming paket.
 *
 * @returns   Zero on success.
 *            Negative value on error.
 *            -EACCES if write access to packet was not aquired.
 *            -ERANGE if segment offset value is less than 5.
 *            -ENOMSG if paket couldn't be marked.
 *            -EINVAL if checksum was invalid.
 *            -ENOTCONN if no TCB is interested in @p pkt.
 */
static int _receive(gnrc_pktsnip_t *pkt)
{
    /* NOTE: In receiving direction: pkt = payload, payload->next = tcp, tcp->next = nw */
    uint16_t ctl = 0;
    uint16_t src = 0;
    uint16_t dst = 0;
    uint8_t hdr_size = 0;
    uint8_t syn = 0;
    gnrc_pktsnip_t *ip = NULL;
    gnrc_pktsnip_t *reset = NULL;
    gnrc_tcp_tcb_t *tcb = NULL;
    tcp_hdr_t *hdr;

    /* Get write access to the TCP header */
    gnrc_pktsnip_t *tcp = gnrc_pktbuf_start_write(pkt);
    if (tcp == NULL) {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : can't write to packet\n");
        gnrc_pktbuf_release(pkt);
        return -EACCES;
    }
    pkt = tcp;

#ifdef MODULE_GNRC_IPV6
    /* Get IPv6 header, discard packet if doesn't contain an ip header */
    LL_SEARCH_SCALAR(pkt, ip, type, GNRC_NETTYPE_IPV6);
    if (ip == NULL) {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : pkt contains no IP Header\n");
        gnrc_pktbuf_release(pkt);
        return 0;
    }
#endif

    /* Get TCP header */
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

    /* Validate offset */
    if (GET_OFFSET(ctl) < TCP_HDR_OFFSET_MIN) {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : unexpected Offset Value\n");
        gnrc_pktbuf_release(pkt);
        return -ERANGE;
    }

    /* Calculate TCP header size */
    hdr_size = GET_OFFSET(ctl) * 4;

    /* Mark TCP header if it contains any payload */
    if ((pkt->type == GNRC_NETTYPE_TCP) && (pkt->size != hdr_size)) {
        tcp = gnrc_pktbuf_mark(pkt, hdr_size, GNRC_NETTYPE_TCP);
        if (tcp == NULL) {
            DEBUG("gnrc_tcp_eventloop.c : _receive() : Header marking failed\n");
            gnrc_pktbuf_release(pkt);
            return -ENOMSG;
        }
        pkt->type = GNRC_NETTYPE_UNDEF;
    }

    /* Validate checksum */
    if (byteorder_ntohs(hdr->checksum) != _pkt_calc_csum(tcp, ip, pkt)) {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : Invalid checksum\n");
        gnrc_pktbuf_release(pkt);
        return -EINVAL;
    }

    /* Find TCB to for this packet */
    mutex_lock(&_list_tcb_lock);
    tcb = _list_tcb_head;
    while (tcb) {
#ifdef MODULE_GNRC_IPV6
        /* Check if current TCB is fitting for the incomming packet */
        if (ip->type == GNRC_NETTYPE_IPV6 && tcb->address_family == AF_INET6) {
            /* If SYN is set, a connection is listening on that port ... */
            ipv6_addr_t *tmp_addr = NULL;
            if (syn && tcb->local_port == dst && tcb->state == FSM_STATE_LISTEN) {
                /* ... and local addr is unspec or pre configured */
                tmp_addr = &((ipv6_hdr_t *)ip->data)->dst;
                if (ipv6_addr_equal((ipv6_addr_t *) tcb->local_addr, (ipv6_addr_t *) tmp_addr) ||
                    ipv6_addr_is_unspecified((ipv6_addr_t *) tcb->local_addr)) {
                    break;
                }
            }

            /* If SYN is not set and the ports match ... */
            if (!syn && tcb->local_port == dst && tcb->peer_port == src) {
                /* .. and the IPv6 addresses match */
                tmp_addr = &((ipv6_hdr_t * )ip->data)->src;
                if (ipv6_addr_equal((ipv6_addr_t *) tcb->peer_addr, (ipv6_addr_t *) tmp_addr)) {
                    break;
                }
            }
        }
#else
        /* Supress compiler warnings if TCP is build without network layer */
        (void) syn;
        (void) src;
        (void) dst;
#endif
        tcb = tcb->next;
    }
    mutex_unlock(&_list_tcb_lock);

    /* Call FSM with event RCVD_PKT if a fitting TCB was found */
    if (tcb != NULL) {
        _fsm(tcb, FSM_EVENT_RCVD_PKT, pkt, NULL, 0);
    }
    /* No fitting TCB has been found. Respond with reset */
    else {
        DEBUG("gnrc_tcp_eventloop.c : _receive() : Can't find fitting tcb\n");
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

    /* Store pid */
    gnrc_tcp_pid = thread_getpid();

    /* Setup reply message */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t)-ENOTSUP;

    /* Init message queue */
    msg_init_queue(_eventloop_msg_queue, TCP_EVENTLOOP_MSG_QUEUE_SIZE);

    /* Register GNRC TCPs handling thread in netreg */
    gnrc_netreg_entry_t entry;
    gnrc_netreg_entry_init_pid(&entry, GNRC_NETREG_DEMUX_CTX_ALL, gnrc_tcp_pid);
    gnrc_netreg_register(GNRC_NETTYPE_TCP, &entry);

    /* dispatch NETAPI messages */
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            /* Pass message up the network stack */
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : GNRC_NETAPI_MSG_TYPE_RCV\n");
                _receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Pass message down the network stack */
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : GNRC_NETAPI_MSG_TYPE_SND\n");
                _send((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Reply to option set and set messages*/
            case GNRC_NETAPI_MSG_TYPE_SET:
            case GNRC_NETAPI_MSG_TYPE_GET:
                msg_reply(&msg, &reply);
                break;

            /* Retransmission timer expired: Call FSM with retransmission event */
            case MSG_TYPE_RETRANSMISSION:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : MSG_TYPE_RETRANSMISSION\n");
                _fsm((gnrc_tcp_tcb_t *)msg.content.ptr, FSM_EVENT_TIMEOUT_RETRANSMIT,
                     NULL, NULL, 0);
                break;

            /* Timewait timer expired: Call FSM with timewait event */
            case MSG_TYPE_TIMEWAIT:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : MSG_TYPE_TIMEWAIT\n");
                _fsm((gnrc_tcp_tcb_t *)msg.content.ptr, FSM_EVENT_TIMEOUT_TIMEWAIT,
                     NULL, NULL, 0);
                break;

            default:
                DEBUG("gnrc_tcp_eventloop.c : _event_loop() : received expected message\n");
        }
    }
    /* Never reached */
    return NULL;
}
