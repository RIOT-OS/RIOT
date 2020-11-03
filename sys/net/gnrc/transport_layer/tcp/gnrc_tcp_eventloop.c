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

#include <assert.h>
#include <utlist.h>
#include <errno.h>
#include "net/af.h"
#include "net/tcp.h"
#include "net/gnrc.h"
#include "include/gnrc_tcp_common.h"
#include "include/gnrc_tcp_pkt.h"
#include "include/gnrc_tcp_fsm.h"
#include "include/gnrc_tcp_eventloop.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define TCP_EVENTLOOP_MSG_QUEUE_SIZE (1 << CONFIG_GNRC_TCP_EVENTLOOP_MSG_QUEUE_SIZE_EXP)

static msg_t _eventloop_msg_queue[TCP_EVENTLOOP_MSG_QUEUE_SIZE];

/**
 * @brief Allocate memory for GNRC TCP thread stack.
 */
static char _stack[TCP_EVENTLOOP_STACK_SIZE + DEBUG_EXTRA_STACKSIZE];

/**
 * @brief Central evtimer for gnrc_tcp event loop
 */
static evtimer_t _tcp_msg_timer;

/**
 * @brief TCPs eventloop pid
 */
static kernel_pid_t _tcp_eventloop_pid = KERNEL_PID_UNDEF;

/**
 * @brief Send function, pass packet down the network stack.
 *
 * @param[in] pkt   Packet to send.
 *
 * @returns   Zero on success.
 *            Negative value on error.
 * @returns  -EBADMSG if required header is missing in @p pkt.
 */
static int _send(gnrc_pktsnip_t *pkt)
{
    TCP_DEBUG_ENTER;
    assert(pkt != NULL);

    /* NOTE: In sending direction: pkt = nw, nw->next = tcp, tcp->next = payload */
    /* Search for TCP header */
    gnrc_pktsnip_t *tcp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_TCP);
    gnrc_pktsnip_t *nw = NULL;

    if (tcp == NULL) {
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("-EBADMSG: Packet contains no TCP header.");
        TCP_DEBUG_LEAVE;
        return -EBADMSG;
    }

    /* Search for network layer */
#ifdef MODULE_GNRC_IPV6
    /* Get IPv6 header, discard packet if doesn't contain an ipv6 header */
    nw = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    if (nw == NULL) {
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("-EBADMSG: Packet contains no IPv6 header.");
        TCP_DEBUG_LEAVE;
        return -EBADMSG;
    }
#endif
    /* Dispatch packet to network layer */
    assert(nw != NULL);
    if (!gnrc_netapi_dispatch_send(nw->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("Can't dispatch to network layer.");
    }
    TCP_DEBUG_LEAVE;
    return 0;
}

/**
 * @brief Receive function, receive packet from network layer.
 *
 * @param[in] pkt   Incoming packet.
 *
 * @returns   Zero on success.
 *            Negative value on error.
 *            -EBADMSG if required header is missing or invalid in @p pkt.
 *            -EACCES if write access to packet was not acquired.
 *            -ERANGE if segment offset value is less than 5.
 *            -ENOMSG if packet couldn't be marked.
 *            -EINVAL if checksum was invalid.
 *            -ENOTCONN if no TCB is interested in @p pkt.
 */
static int _receive(gnrc_pktsnip_t *pkt)
{
    TCP_DEBUG_ENTER;
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
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("-EACCESS: Can't write to packet.");
        TCP_DEBUG_LEAVE;
        return -EACCES;
    }
    pkt = tcp;

#ifdef MODULE_GNRC_IPV6
    /* Get IPv6 header, discard packet if doesn't contain an ip header */
    ip = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    if (ip == NULL) {
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("-EBADMSG: Packet contains no IPv6 header.");
        TCP_DEBUG_LEAVE;
        return -EBADMSG;
    }
#endif

    /* Get TCP header */
    tcp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_TCP);
    if (tcp == NULL) {
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("-EBADMSG: Packet contains no TCP header.");
        TCP_DEBUG_LEAVE;
        return -EBADMSG;
    }

    if (tcp->size < sizeof(tcp_hdr_t)) {
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("-EBADMSG: Packet size less than minimal header size.");
        TCP_DEBUG_LEAVE;
        return -EBADMSG;
    }

    /* Extract control bits, src and dst ports and check if SYN is set (not SYN+ACK) */
    hdr = (tcp_hdr_t *)tcp->data;
    ctl = byteorder_ntohs(hdr->off_ctl);
    src = byteorder_ntohs(hdr->src_port);
    dst = byteorder_ntohs(hdr->dst_port);
    syn = ((ctl & MSK_SYN_ACK) == MSK_SYN);

    /* Validate offset */
    if (GET_OFFSET(ctl) < TCP_HDR_OFFSET_MIN) {
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("-ERANGE: Invalid TCP header offset.");
        TCP_DEBUG_LEAVE;
        return -ERANGE;
    }

    /* Calculate TCP header size */
    hdr_size = GET_OFFSET(ctl) * 4;

    /* Mark TCP header if it contains any payload */
    if ((pkt->type == GNRC_NETTYPE_TCP) && (pkt->size != hdr_size)) {
        tcp = gnrc_pktbuf_mark(pkt, hdr_size, GNRC_NETTYPE_TCP);
        if (tcp == NULL) {
            gnrc_pktbuf_release(pkt);
            TCP_DEBUG_ERROR("-ENOMSG: Header marking failed.");
            TCP_DEBUG_LEAVE;
            return -ENOMSG;
        }
        pkt->type = GNRC_NETTYPE_UNDEF;
        hdr = (tcp_hdr_t *)tcp->data;
    }

    /* Validate checksum */
    if (byteorder_ntohs(hdr->checksum) != _gnrc_tcp_pkt_calc_csum(tcp, ip, pkt)) {
#ifndef MODULE_FUZZING
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("-EINVAL: Invalid checksum.");
        TCP_DEBUG_LEAVE;
        return -EINVAL;
#endif
    }

    /* Find TCB to for this packet */
    _gnrc_tcp_common_tcb_list_t *list = _gnrc_tcp_common_get_tcb_list();
    mutex_lock(&list->lock);
    tcb = list->head;
    while (tcb) {
#ifdef MODULE_GNRC_IPV6
        /* Check if current TCB is fitting for the incoming packet */
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
        /* Suppress compiler warnings if TCP is built without network layer */
        TCP_DEBUG_ERROR("Missing network layer. Add module to makefile.");
        (void) syn;
        (void) src;
        (void) dst;
#endif
        tcb = tcb->next;
    }
    mutex_unlock(&list->lock);

    /* Call FSM with event RCVD_PKT if a fitting TCB was found */
    /* cppcheck-suppress knownConditionTrueFalse
     * (reason: tcb can be NULL at runtime)
     */
    if (tcb != NULL) {
        _gnrc_tcp_fsm(tcb, FSM_EVENT_RCVD_PKT, pkt, NULL, 0);
    }
    /* No fitting TCB has been found. Respond with reset */
    else {
        if ((ctl & MSK_RST) != MSK_RST) {
            _gnrc_tcp_pkt_build_reset_from_pkt(&reset, pkt);
            if (gnrc_netapi_send(_tcp_eventloop_pid, reset) < 1) {
                gnrc_pktbuf_release(reset);
                TCP_DEBUG_ERROR("Can't dispatch to network layer.");
            }
        }
        gnrc_pktbuf_release(pkt);
        TCP_DEBUG_ERROR("-ENOTCONN: Unable to find matching TCB.");
        TCP_DEBUG_LEAVE;
        return -ENOTCONN;
    }
    gnrc_pktbuf_release(pkt);
    TCP_DEBUG_LEAVE;
    return 0;
}

static void *_eventloop(__attribute__((unused)) void *arg)
{
    TCP_DEBUG_ENTER;
    msg_t msg;
    msg_t reply;

    /* Store pid */
    _tcp_eventloop_pid = thread_getpid();

    /* Setup reply message */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t)-ENOTSUP;

    /* Init message queue */
    msg_init_queue(_eventloop_msg_queue, TCP_EVENTLOOP_MSG_QUEUE_SIZE);

    /* Register GNRC TCPs handling thread in netreg */
    gnrc_netreg_entry_t entry;
    gnrc_netreg_entry_init_pid(&entry, GNRC_NETREG_DEMUX_CTX_ALL, _tcp_eventloop_pid);
    gnrc_netreg_register(GNRC_NETTYPE_TCP, &entry);

    /* dispatch NETAPI messages */
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            /* Pass message up the network stack */
            case GNRC_NETAPI_MSG_TYPE_RCV:
                TCP_DEBUG_INFO("Received GNRC_NETAPI_MSG_TYPE_RCV.");
                _receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Pass message down the network stack */
            case GNRC_NETAPI_MSG_TYPE_SND:
                TCP_DEBUG_INFO("Received GNRC_NETAPI_MSG_TYPE_SND.");
                _send((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Reply to option set and set messages*/
            case GNRC_NETAPI_MSG_TYPE_SET:
            case GNRC_NETAPI_MSG_TYPE_GET:
                msg_reply(&msg, &reply);
                break;

            /* Retransmission timer expired: Call FSM with retransmission event */
            case MSG_TYPE_RETRANSMISSION:
                TCP_DEBUG_INFO("Received MSG_TYPE_RETRANSMISSION.");
                _gnrc_tcp_fsm((gnrc_tcp_tcb_t *)msg.content.ptr,
                              FSM_EVENT_TIMEOUT_RETRANSMIT, NULL, NULL, 0);
                break;

            /* Timewait timer expired: Call FSM with timewait event */
            case MSG_TYPE_TIMEWAIT:
                TCP_DEBUG_INFO("Received MSG_TYPE_TIMEWAIT.");
                _gnrc_tcp_fsm((gnrc_tcp_tcb_t *)msg.content.ptr,
                              FSM_EVENT_TIMEOUT_TIMEWAIT, NULL, NULL, 0);
                break;

            default:
                TCP_DEBUG_ERROR("Received unexpected message.");
        }
    }
    /* Never reached */
    TCP_DEBUG_ERROR("This function should never exit.");
    TCP_DEBUG_LEAVE;
    return NULL;
}

void _gnrc_tcp_eventloop_sched(evtimer_msg_event_t *event, uint32_t offset,
                               uint16_t type, void *context)
{
    TCP_DEBUG_ENTER;
    event->event.offset = offset;
    event->msg.type = type;
    event->msg.content.ptr = context;
    evtimer_add_msg(&_tcp_msg_timer, event, _tcp_eventloop_pid);
    TCP_DEBUG_LEAVE;
}

void _gnrc_tcp_eventloop_unsched(evtimer_msg_event_t *event)
{
    TCP_DEBUG_ENTER;
    evtimer_del(&_tcp_msg_timer, (evtimer_event_t *)event);
    TCP_DEBUG_LEAVE;
}

int _gnrc_tcp_eventloop_init(void)
{
    TCP_DEBUG_ENTER;
    /* Guard: Check if thread is already running */
    if (_tcp_eventloop_pid != KERNEL_PID_UNDEF) {
        TCP_DEBUG_ERROR("-EEXIST: TCP eventloop already running.");
        TCP_DEBUG_LEAVE;
        return -EEXIST;
    }

    /* Initialize timers */
    evtimer_init_msg(&_tcp_msg_timer);

    kernel_pid_t pid = thread_create(_stack, sizeof(_stack), TCP_EVENTLOOP_PRIO,
                                     THREAD_CREATE_STACKTEST, _eventloop, NULL,
                                     "gnrc_tcp");
    TCP_DEBUG_LEAVE;
    return pid;
}
