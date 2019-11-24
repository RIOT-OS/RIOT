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
 * @brief       Implementation of internal/fsm.h
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 * @}
 */

#include <utlist.h>
#include <errno.h>
#include "random.h"
#include "net/af.h"
#include "net/gnrc.h"
#include "internal/common.h"
#include "internal/pkt.h"
#include "internal/option.h"
#include "internal/rcvbuf.h"
#include "internal/fsm.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Helper macro for LL_SEARCH to compare TCBs
 */
#define TCB_EQUAL(a,b)      ((a) != (b))

/**
 * @brief Checks if a given port number is currently used by a TCB as local_port.
 *
 * @note Must be called from a context where the TCB list is locked.
 *
 * @param[in] port_number   Port number that should be checked.
 *
 * @returns   Zero if @p port_number is currently not used.
 *            1 if @p port_number is used by an active connection.
 */
static int _is_local_port_in_use(const uint16_t port_number)
{
    gnrc_tcp_tcb_t *iter = NULL;
    LL_SEARCH_SCALAR(_list_tcb_head, iter, local_port, port_number);
    return (iter != NULL);
}

/**
 * @brief Generate random unused local port above the well-known ports (> 1024).
 *
 * @returns   Generated port number.
 */
static uint16_t _get_random_local_port(void)
{
    uint16_t ret = 0;
    do {
        ret = random_uint32();
        if (ret < 1024) {
            continue;
        }
    } while(_is_local_port_in_use(ret));
    return ret;
}

/**
 * @brief Clears retransmit queue.
 *
 * @param[in,out] tcb   TCB holding the retransmit queue.
 *
 * @return   Zero on success.
 */
static int _clear_retransmit(gnrc_tcp_tcb_t *tcb)
{
    if (tcb->pkt_retransmit != NULL) {
        gnrc_pktbuf_release(tcb->pkt_retransmit);
        xtimer_remove(&(tcb->tim_tout));
        tcb->pkt_retransmit = NULL;
    }
    return 0;
}

/**
 * @brief Restarts timewait timer.
 *
 * @param[in,out] tcb   TCB holding the timer struct to reset.
 *
 * @return   Zero on success.
 */
static int _restart_timewait_timer(gnrc_tcp_tcb_t *tcb)
{
    xtimer_remove(&tcb->tim_tout);
    tcb->msg_tout.type = MSG_TYPE_TIMEWAIT;
    tcb->msg_tout.content.ptr = (void *)tcb;
    xtimer_set_msg(&tcb->tim_tout, 2 * GNRC_TCP_MSL, &tcb->msg_tout, gnrc_tcp_pid);
    return 0;
}

/**
 * @brief Transition from current FSM state into another state.
 *
 * @param[in,out] tcb     TCB holding the FSM state.
 * @param[in]     state   State to transition in.
 *
 * @return   Zero on success.
 */
static int _transition_to(gnrc_tcp_tcb_t *tcb, fsm_state_t state)
{
    DEBUG("_transition_to: %d\n", state);

    gnrc_tcp_tcb_t *iter = NULL;

    switch (state) {
        case FSM_STATE_CLOSED:
            /* Clear retransmit queue */
            _clear_retransmit(tcb);

            /* Remove connection from active connections */
            mutex_lock(&_list_tcb_lock);
            LL_DELETE(_list_tcb_head, tcb);
            mutex_unlock(&_list_tcb_lock);

            /* Free potentially allocated receive buffer */
            _rcvbuf_release_buffer(tcb);
            tcb->status |= STATUS_NOTIFY_USER;
            break;

        case FSM_STATE_LISTEN:
            /* Clear address info */
#ifdef MODULE_GNRC_IPV6
            if (tcb->address_family == AF_INET6) {
                if (tcb->status & STATUS_ALLOW_ANY_ADDR) {
                    ipv6_addr_set_unspecified((ipv6_addr_t *) tcb->local_addr);
                }
                ipv6_addr_set_unspecified((ipv6_addr_t *) tcb->peer_addr);
            }
#endif
            tcb->peer_port = PORT_UNSPEC;

            /* Allocate receive buffer */
            if (_rcvbuf_get_buffer(tcb) == -ENOMEM) {
                return -ENOMEM;
            }

            /* Add connection to active connections (if not already active) */
            mutex_lock(&_list_tcb_lock);
            LL_SEARCH(_list_tcb_head, iter, tcb, TCB_EQUAL);
            if (iter == NULL) {
                LL_PREPEND(_list_tcb_head, tcb);
            }
            mutex_unlock(&_list_tcb_lock);
            break;

        case FSM_STATE_SYN_SENT:
            /* Allocate rceveive buffer */
            if (_rcvbuf_get_buffer(tcb) == -ENOMEM) {
                return -ENOMEM;
            }

            /* Add connection to active connections (if not already active) */
            mutex_lock(&_list_tcb_lock);
            LL_SEARCH(_list_tcb_head, iter, tcb, TCB_EQUAL);
            /* If connection is not already active: Check port number, append TCB */
            if (iter == NULL) {
                /* Check if port number was specified */
                if (tcb->local_port != PORT_UNSPEC) {
                    /* Check if given port number is use: return error and release buffer */
                    if (_is_local_port_in_use(tcb->local_port)) {
                        mutex_unlock(&_list_tcb_lock);
                        _rcvbuf_release_buffer(tcb);
                        return -EADDRINUSE;
                    }
                }
                /* Pick random port */
                else {
                    tcb->local_port = _get_random_local_port();
                }
                LL_PREPEND(_list_tcb_head, tcb);
            }
            mutex_unlock(&_list_tcb_lock);
            break;

        case FSM_STATE_SYN_RCVD:
        case FSM_STATE_ESTABLISHED:
        case FSM_STATE_CLOSE_WAIT:
            tcb->status |= STATUS_NOTIFY_USER;
            break;

        case FSM_STATE_TIME_WAIT:
            _restart_timewait_timer(tcb);
            break;

        default:
            break;
    }
    tcb->state = state;
    return 0;
}

/**
 * @brief FSM handling function for opening a TCP connection.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 *
 * @returns   Zero on success.
 *            -ENOMEM if receive buffer could not be allocated.
 *            -EADDRINUSE if given local port number is already in use.
 */
static int _fsm_call_open(gnrc_tcp_tcb_t *tcb)
{
    int ret = 0;

    DEBUG("gnrc_tcp_fsm.c : _fsm_call_open()\n");
    tcb->rcv_wnd = GNRC_TCP_DEFAULT_WINDOW;

    if (tcb->status & STATUS_PASSIVE) {
        /* Passive open, T: CLOSED -> LISTEN */
        if (_transition_to(tcb, FSM_STATE_LISTEN) == -ENOMEM) {
            _transition_to(tcb, FSM_STATE_CLOSED);
            return -ENOMEM;
        }
    }
    else {
        /* Active Open, set TCB values, send SYN, T: CLOSED -> SYN_SENT */
        tcb->iss = random_uint32();
        tcb->snd_nxt = tcb->iss;
        tcb->snd_una = tcb->iss;

        /* Transition FSM to SYN_SENT */
        ret = _transition_to(tcb, FSM_STATE_SYN_SENT);
        if (ret < 0) {
            _transition_to(tcb, FSM_STATE_CLOSED);
            return ret;
        }

        /* Send SYN */
        gnrc_pktsnip_t *out_pkt = NULL;
        uint16_t seq_con = 0;
        _pkt_build(tcb, &out_pkt, &seq_con, MSK_SYN, tcb->iss, 0, NULL, 0);
        _pkt_setup_retransmit(tcb, out_pkt, false);
        _pkt_send(tcb, out_pkt, seq_con, false);
    }
    return ret;
}

/**
 * @brief FSM Handling function for sending data.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 * @param[in,out] buf   Buffer containing data to send.
 * @param[in]     len   Maximum Number of Bytes to send from @p buf.
 *
 * @returns   Number of successfully transmitted bytes.
 */
static int _fsm_call_send(gnrc_tcp_tcb_t *tcb, void *buf, size_t len)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_call_send()\n");

    size_t payload = (tcb->snd_una + tcb->snd_wnd) - tcb->snd_nxt;

    /* Check if window is open and all packets were transmitted */
    if (payload > 0 && tcb->snd_wnd > 0 && tcb->pkt_retransmit == NULL) {
        /* Calculate segment size */
        payload = (payload < GNRC_TCP_MSS) ? payload : GNRC_TCP_MSS;
        payload = (payload < tcb->mss) ? payload : tcb->mss;
        payload = (payload < len) ? payload : len;

        /* Calculate payload size for this segment */
        gnrc_pktsnip_t *out_pkt = NULL;
        uint16_t seq_con = 0;
        _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK | MSK_PSH, tcb->snd_nxt, tcb->rcv_nxt, buf, payload);
        _pkt_setup_retransmit(tcb, out_pkt, false);
        _pkt_send(tcb, out_pkt, seq_con, false);
        return payload;
    }
    return 0;
}

/**
 * @brief FSM handling function for receiving data.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 * @param[in,out] buf   Buffer to store received data into.
 * @param[in]     len   Maximum number of bytes to receive.
 *
 * @returns   Number of successfully received bytes.
 */
static int _fsm_call_recv(gnrc_tcp_tcb_t *tcb, void *buf, size_t len)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_call_recv()\n");

    if (ringbuffer_empty(&tcb->rcv_buf)) {
        return 0;
    }

    /* Read data into 'buf' up to 'len' bytes from receive buffer */
    size_t rcvd = ringbuffer_get(&(tcb->rcv_buf), buf, len);

    /* If receive buffer can store more than GNRC_TCP_MSS: open window to available buffer size */
    if (ringbuffer_get_free(&tcb->rcv_buf) >= GNRC_TCP_MSS) {
        tcb->rcv_wnd = ringbuffer_get_free(&(tcb->rcv_buf));

        /* Send ACK to anounce window update */
        gnrc_pktsnip_t *out_pkt = NULL;
        uint16_t seq_con = 0;
        _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
        _pkt_send(tcb, out_pkt, seq_con, false);
    }
    return rcvd;
}

/**
 * @brief FSM handling function for starting connection teardown sequence.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 *
 * @returns   Zero on success.
 */
static int _fsm_call_close(gnrc_tcp_tcb_t *tcb)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_call_close()\n");

    if (tcb->state == FSM_STATE_SYN_RCVD || tcb->state == FSM_STATE_ESTABLISHED ||
        tcb->state == FSM_STATE_CLOSE_WAIT) {

        /* Send FIN packet */
        gnrc_pktsnip_t *out_pkt = NULL;
        uint16_t seq_con = 0;
        _pkt_build(tcb, &out_pkt, &seq_con, MSK_FIN_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
        _pkt_setup_retransmit(tcb, out_pkt, false);
        _pkt_send(tcb, out_pkt, seq_con, false);
    }

    if (tcb->state == FSM_STATE_LISTEN) {
        _transition_to(tcb, FSM_STATE_CLOSED);
    }
    else if (tcb->state == FSM_STATE_SYN_RCVD || tcb->state == FSM_STATE_ESTABLISHED) {
        _transition_to(tcb, FSM_STATE_FIN_WAIT_1);
    }
    else if (tcb->state == FSM_STATE_CLOSE_WAIT) {
        _transition_to(tcb, FSM_STATE_LAST_ACK);
    }
    return 0;
}

/**
 * @brief FSM handling function for forcefull connection teardown sequence.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 *
 * @returns   Zero on success.
 */
static int _fsm_call_abort(gnrc_tcp_tcb_t *tcb)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_call_abort()\n");

    /* A reset must be sent in case the TCB state is in one of those cases */
    if (tcb->state == FSM_STATE_SYN_RCVD || tcb->state == FSM_STATE_ESTABLISHED ||
        tcb->state == FSM_STATE_FIN_WAIT_1 || tcb->state == FSM_STATE_FIN_WAIT_2 ||
        tcb->state == FSM_STATE_CLOSE_WAIT) {

        /* Send RST packet without retransmit */
        gnrc_pktsnip_t *out_pkt = NULL;
        uint16_t seq_con = 0;
        _pkt_build(tcb, &out_pkt, &seq_con, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
        _pkt_send(tcb, out_pkt, seq_con, false);
    }

    /* From here on any state must transition into CLOSED state */
    _transition_to(tcb, FSM_STATE_CLOSED);

    return 0;
}

/**
 * @brief FSM handling function for processing of an incoming TCP packet.
 *
 * @param[in,out] tcb      TCB holding the connection information.
 * @param[in]     in_pkt   Incoming packet.
 *
 * @returns   Zero on success.
 *            -ENOMEM if receive buffer could not be allocated.
 */
static int _fsm_rcvd_pkt(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *in_pkt)
{
    gnrc_pktsnip_t *out_pkt = NULL;  /* Outgoing packet */
    uint16_t seq_con = 0;            /* Sequence number consumption of outgoing packet */
    gnrc_pktsnip_t *snp = NULL;      /* Temporary packet snip */
    gnrc_tcp_tcb_t *lst = NULL;      /* Temporary pointer to TCB */
    uint16_t ctl = 0;                /* Control bits of the incoming packet */
    uint32_t seg_seq = 0;            /* Sequence number of the incoming packet*/
    uint32_t seg_ack = 0;            /* Acknowledgment number of the incoming packet */
    uint32_t seg_wnd = 0;            /* Receive window of the incoming packet */
    uint32_t seg_len = 0;            /* Segment length of the incoming packet */
    uint32_t pay_len = 0;            /* Payload length of the incoming packet */

    DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt()\n");
    /* Search for TCP header. */
    LL_SEARCH_SCALAR(in_pkt, snp, type, GNRC_NETTYPE_TCP);
    tcp_hdr_t *tcp_hdr = (tcp_hdr_t *) snp->data;

    /* Parse packet options, return if they are malformed */
    if (_option_parse(tcb, tcp_hdr) < 0) {
        return 0;
    }

    /* Extract header values */
    ctl = byteorder_ntohs(tcp_hdr->off_ctl);
    seg_seq = byteorder_ntohl(tcp_hdr->seq_num);
    seg_ack = byteorder_ntohl(tcp_hdr->ack_num);
    seg_wnd = byteorder_ntohs(tcp_hdr->window);

    /* Extract network layer header */
#ifdef MODULE_GNRC_IPV6
    LL_SEARCH_SCALAR(in_pkt, snp, type, GNRC_NETTYPE_IPV6);
    if (snp == NULL) {
        DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt() : incoming packet had no IPv6 header\n");
        return 0;
    }
    void *ip = snp->data;
#endif

    /* Handle state LISTEN */
    if (tcb->state == FSM_STATE_LISTEN) {
        /* 1) Check RST: if RST is set: return */
        if (ctl & MSK_RST) {
            return 0;
        }
        /* 2) Check ACK: if ACK is set: send RST with seq_no = ack_no and return */
        if (ctl & MSK_ACK) {
            _pkt_build_reset_from_pkt(&out_pkt, in_pkt);
            _pkt_send(tcb, out_pkt, 0, false);
            return 0;
        }
        /* 3) Check SYN: if SYN is set prepare for incoming connection */
        if (ctl & MSK_SYN) {
            uint16_t src = byteorder_ntohs(tcp_hdr->src_port);
            uint16_t dst = byteorder_ntohs(tcp_hdr->dst_port);

            /* Check if SYN request is handled by another connection */
            lst = _list_tcb_head;
            while (lst) {
                /* Compare port numbers and network layer addresses */
                if (lst->local_port == dst && lst->peer_port == src) {
#ifdef MODULE_GNRC_IPV6
                    if (snp->type == GNRC_NETTYPE_IPV6 && lst->address_family == AF_INET6) {
                        ipv6_addr_t *dst_addr = &((ipv6_hdr_t *)ip)->dst;
                        ipv6_addr_t *src_addr = &((ipv6_hdr_t *)ip)->src;

                        if (ipv6_addr_equal((ipv6_addr_t *)lst->local_addr, dst_addr) &&
                            ipv6_addr_equal((ipv6_addr_t *)lst->peer_addr, src_addr)) {
                            break;
                        }
                    }
#endif
                }
                lst = lst->next;
            }
            /* Return if connection is already handled (port and addresses match) */
            if (lst != NULL) {
                DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt() : Connection already handled\n");
                return 0;
            }

            /* SYN request is valid, fill TCB with connection information */
#ifdef MODULE_GNRC_IPV6
            if (snp->type == GNRC_NETTYPE_IPV6 && tcb->address_family == AF_INET6) {
                memcpy(tcb->local_addr, &((ipv6_hdr_t *)ip)->dst, sizeof(ipv6_addr_t));
                memcpy(tcb->peer_addr, &((ipv6_hdr_t *)ip)->src, sizeof(ipv6_addr_t));

                /* In case peer_addr is link local: Store interface Id in tcb */
                if (ipv6_addr_is_link_local((ipv6_addr_t *) tcb->peer_addr)) {
                    gnrc_pktsnip_t *tmp = NULL;
                    LL_SEARCH_SCALAR(in_pkt, tmp, type, GNRC_NETTYPE_NETIF);
                    /* cppcheck-suppress knownConditionTrueFalse
                     * (reason: tmp *can* be != NULL after LL_SEARCH_SCALAR) */
                    if (tmp == NULL) {
                        DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt() :\
                               incoming packet had no netif header\n");
                        return 0;
                    }
                    tcb->ll_iface = ((gnrc_netif_hdr_t *)tmp->data)->if_pid;
                }
            }
#else
            DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt() : Received address was not stored\n");
            return 0;
#endif

            tcb->local_port = dst;
            tcb->peer_port = src;
            tcb->irs = byteorder_ntohl(tcp_hdr->seq_num);
            tcb->rcv_nxt = tcb->irs + 1;
            tcb->iss = random_uint32();
            tcb->snd_una = tcb->iss;
            tcb->snd_nxt = tcb->iss;
            tcb->snd_wnd = seg_wnd;

            /* Send SYN+ACK: seq_no = iss, ack_no = rcv_nxt, T: LISTEN -> SYN_RCVD */
            _pkt_build(tcb, &out_pkt, &seq_con, MSK_SYN_ACK, tcb->iss, tcb->rcv_nxt, NULL, 0);
            _pkt_setup_retransmit(tcb, out_pkt, false);
            _pkt_send(tcb, out_pkt, seq_con, false);
            _transition_to(tcb, FSM_STATE_SYN_RCVD);
        }
        return 0;
    }
    /* Handle state SYN_SENT */
    else if (tcb->state == FSM_STATE_SYN_SENT) {
        /* 1) Check ACK */
        if (ctl & MSK_ACK) {
            /* If ACK is not acceptable ...*/
            if (seg_ack <= tcb->iss || seg_ack > tcb->snd_nxt) {
                /* ... send reset, if RST is not set else return */
                if ((ctl & MSK_RST) != MSK_RST) {
                    _pkt_build(tcb, &out_pkt, &seq_con, MSK_RST, seg_ack, 0, NULL, 0);
                    _pkt_send(tcb, out_pkt, seq_con, false);
                }
                return 0;
            }
        }
        /* 2) Check RST: If RST set ... */
        if (ctl & MSK_RST) {
            /* ... and ACK: Translate to CLOSED, if not return */
            if (ctl & MSK_ACK) {
                _transition_to(tcb, FSM_STATE_CLOSED);
            }
            return 0;
        }
        /* 3) Check SYN: Set TCB values accordingly */
        if (ctl & MSK_SYN) {
            tcb->rcv_nxt = seg_seq + 1;
            tcb->irs = seg_seq;
            if (ctl & MSK_ACK) {
                tcb->snd_una = seg_ack;
                _pkt_acknowledge(tcb, seg_ack);
            }
            /* Set local network layer address accordingly */
#ifdef MODULE_GNRC_IPV6
            if (snp->type == GNRC_NETTYPE_IPV6 && tcb->address_family == AF_INET6) {
                memcpy(tcb->local_addr, &((ipv6_hdr_t *)ip)->dst, sizeof(ipv6_addr_t));
            }
#else
            DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt() : Received address was not stored\n");
            return 0;
#endif

            /* SYN has been ACKed. Send ACK, T: SYN_SENT -> ESTABLISHED */
            if (tcb->snd_una > tcb->iss) {
                _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
                _pkt_send(tcb, out_pkt, seq_con, false);
                _transition_to(tcb, FSM_STATE_ESTABLISHED);
            }
            /* Simultaneous SYN received. Send SYN+ACK, T: SYN_SENT -> SYN_RCVD */
            else {
                _pkt_build(tcb, &out_pkt, &seq_con, MSK_SYN_ACK, tcb->iss, tcb->rcv_nxt, NULL, 0);
                _pkt_setup_retransmit(tcb, out_pkt, false);
                _pkt_send(tcb, out_pkt, seq_con, false);
                _transition_to(tcb, FSM_STATE_SYN_RCVD);
            }
            tcb->snd_wnd = seg_wnd;
            tcb->snd_wl1 = seg_seq;
            tcb->snd_wl2 = seg_ack;
        }
        return 0;
    }
    /* Handle other states */
    else {
        seg_len = _pkt_get_seg_len(in_pkt);
        pay_len = _pkt_get_pay_len(in_pkt);
        /* 1) Verify sequence number ... */
        if (_pkt_chk_seq_num(tcb, seg_seq, pay_len)) {
            /* ... if invalid, and RST not set, reply with pure ACK, return */
            if ((ctl & MSK_RST) != MSK_RST) {
                _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
                _pkt_send(tcb, out_pkt, seq_con, false);
            }
            return 0;
        }
        /* 2) Check RST: If RST is set ... */
        if (ctl & MSK_RST) {
            /* .. and state is SYN_RCVD and the connection is passive: SYN_RCVD -> LISTEN */
            if (tcb->state == FSM_STATE_SYN_RCVD && (tcb->status & STATUS_PASSIVE)) {
                if (_transition_to(tcb, FSM_STATE_LISTEN) == -ENOMEM) {
                    _transition_to(tcb, FSM_STATE_CLOSED);
                    return -ENOMEM;
                }
            }
            else {
                _transition_to(tcb, FSM_STATE_CLOSED);
            }
            return 0;
        }
        /* 3) Check SYN: If SYN is set ... */
        if (ctl & MSK_SYN) {
            /* ... send RST, seq_no = snd_nxt, ack_no = rcv_nxt */
            _pkt_build(tcb, &out_pkt, &seq_con, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
            _pkt_send(tcb, out_pkt, seq_con, false);
            _transition_to(tcb, FSM_STATE_CLOSED);
            return 0;
        }
        /* 4) Check ACK */
        if (!(ctl & MSK_ACK)) {
            return 0;
        }
        else {
            if (tcb->state == FSM_STATE_SYN_RCVD) {
                if (LSS_32_BIT(tcb->snd_una, seg_ack) && LEQ_32_BIT(seg_ack, tcb->snd_nxt)) {
                    tcb->snd_wnd = seg_wnd;
                    tcb->snd_wl1 = seg_seq;
                    tcb->snd_wl2 = seg_ack;
                    _transition_to(tcb, FSM_STATE_ESTABLISHED);
                }
                else {
                    _pkt_build(tcb, &out_pkt, &seq_con, MSK_RST, seg_ack, 0, NULL, 0);
                    _pkt_send(tcb, out_pkt, seq_con, false);
                }
            }
            /* Acknowledgment processing */
            if (tcb->state == FSM_STATE_ESTABLISHED || tcb->state == FSM_STATE_FIN_WAIT_1 ||
                tcb->state == FSM_STATE_FIN_WAIT_2 || tcb->state == FSM_STATE_CLOSE_WAIT ||
                tcb->state == FSM_STATE_CLOSING || tcb->state == FSM_STATE_LAST_ACK) {
                /* Acknowledge previously sent data */
                if (LSS_32_BIT(tcb->snd_una, seg_ack) && LEQ_32_BIT(seg_ack, tcb->snd_nxt)) {
                    tcb->snd_una = seg_ack;
                    _pkt_acknowledge(tcb, seg_ack);
                }
                /* ACK received for something not yet sent: Reply with pure ACK */
                else if (LSS_32_BIT(tcb->snd_nxt, seg_ack)) {
                    _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt,
                               NULL, 0);
                    _pkt_send(tcb, out_pkt, seq_con, false);
                    return 0;
                }
                /* Update receive window */
                if (LEQ_32_BIT(tcb->snd_una, seg_ack) && LEQ_32_BIT(seg_ack, tcb->snd_nxt)) {
                    if (LSS_32_BIT(tcb->snd_wl1, seg_seq) || (tcb->snd_wl1 == seg_seq &&
                        LEQ_32_BIT(tcb->snd_wl2, seg_ack))) {
                        tcb->snd_wnd = seg_wnd;
                        tcb->snd_wl1 = seg_seq;
                        tcb->snd_wl2 = seg_ack;

                        /* Signal user after window update */
                        tcb->status |= STATUS_NOTIFY_USER;
                    }
                }
                /* Additional processing */
                /* Check additionally if previously sent FIN was acknowledged */
                if (tcb->state == FSM_STATE_FIN_WAIT_1) {
                    if (tcb->pkt_retransmit == NULL) {
                        _transition_to(tcb, FSM_STATE_FIN_WAIT_2);
                    }
                }
                /* If retransmission queue is empty, acknowledge close operation */
                if (tcb->state == FSM_STATE_FIN_WAIT_2) {
                    if (tcb->pkt_retransmit == NULL) {
                        /* Optional: Unblock user close operation */
                    }
                }
                /* If our FIN has been acknowledged: Transition to TIME_WAIT */
                if (tcb->state == FSM_STATE_CLOSING) {
                    if (tcb->pkt_retransmit == NULL) {
                        _transition_to(tcb, FSM_STATE_TIME_WAIT);
                    }
                }
                /* If our FIN was acknowledged and status is LAST_ACK: close connection */
                if (tcb->state == FSM_STATE_LAST_ACK) {
                    if (tcb->pkt_retransmit == NULL) {
                        _transition_to(tcb, FSM_STATE_CLOSED);
                        return 0;
                    }
                }
            }
        }
        /* 5) Check URG */
        /* NOTE: Add urgent pointer processing here ... */

        /* 6) Process payload, if existing */
        if (pay_len > 0) {
            /* Check if state is valid for payload receiving */
            if (tcb->state == FSM_STATE_ESTABLISHED || tcb->state == FSM_STATE_FIN_WAIT_1 ||
                tcb->state == FSM_STATE_FIN_WAIT_2) {
                /* Search for begin of payload */
                LL_SEARCH_SCALAR(in_pkt, snp, type, GNRC_NETTYPE_UNDEF);

                /* Accept only data that is expected, to be received */
                if (tcb->rcv_nxt == seg_seq) {
                    /* Copy contents into receive buffer */
                    while (snp && snp->type == GNRC_NETTYPE_UNDEF) {
                        tcb->rcv_nxt += ringbuffer_add(&(tcb->rcv_buf), snp->data, snp->size);
                        snp = snp->next;
                    }
                    /* Shrink receive window */
                    tcb->rcv_wnd = ringbuffer_get_free(&(tcb->rcv_buf));
                    /* Notify owner because new data is available */
                    tcb->status |= STATUS_NOTIFY_USER;
                }
                /* Send ACK, if FIN processing sends ACK already */
                /* NOTE: this is the place to add payload piggybagging in the future */
                if (!(ctl & MSK_FIN)) {
                    _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt,
                               NULL, 0);
                    _pkt_send(tcb, out_pkt, seq_con, false);
                }
            }
        }
        /* 7) Check FIN */
        if (ctl & MSK_FIN) {
            if (tcb->state == FSM_STATE_CLOSED || tcb->state == FSM_STATE_LISTEN ||
                tcb->state == FSM_STATE_SYN_SENT) {
                return 0;
            }
            /* Advance rcv_nxt over FIN bit */
            tcb->rcv_nxt = seg_seq + seg_len;
            _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
            _pkt_send(tcb, out_pkt, seq_con, false);

            if (tcb->state == FSM_STATE_SYN_RCVD || tcb->state == FSM_STATE_ESTABLISHED) {
                _transition_to(tcb, FSM_STATE_CLOSE_WAIT);
            }
            else if (tcb->state == FSM_STATE_FIN_WAIT_1) {
                if (tcb->pkt_retransmit == NULL) {
                    _transition_to(tcb, FSM_STATE_TIME_WAIT);
                }
                else {
                    _transition_to(tcb, FSM_STATE_CLOSING);
                }
            }
            else if (tcb->state == FSM_STATE_FIN_WAIT_2) {
                _transition_to(tcb, FSM_STATE_TIME_WAIT);
            }
            else if (tcb->state == FSM_STATE_TIME_WAIT) {
                _restart_timewait_timer(tcb);
            }
        }
    }
    return 0;
}

/**
 * @brief FSM handling function for timewait timeout handling.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 *
 * @returns   Zero on success.
 */
static int _fsm_timeout_timewait(gnrc_tcp_tcb_t *tcb)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_timeout_timewait()\n");
    _transition_to(tcb, FSM_STATE_CLOSED);
    return 0;
}

/**
 * @brief FSM handling function for retransmissions.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 *
 * @returns   Zero on success.
 */
static int _fsm_timeout_retransmit(gnrc_tcp_tcb_t *tcb)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_timeout_retransmit()\n");
    if (tcb->pkt_retransmit != NULL) {
        _pkt_setup_retransmit(tcb, tcb->pkt_retransmit, true);
        _pkt_send(tcb, tcb->pkt_retransmit, 0, true);
    }
    else {
        DEBUG("gnrc_tcp_fsm.c : _fsm_timeout_retransmit() : Retransmit queue is empty\n");
    }
    return 0;
}

/**
 * @brief FSM handling function for connection timeout handling.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 *
 * @returns   Zero on success.
 */
static int _fsm_timeout_connection(gnrc_tcp_tcb_t *tcb)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_timeout_connection()\n");
    _transition_to(tcb, FSM_STATE_CLOSED);
    return 0;
}

/**
 * @brief FSM handling function for probe sending.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 *
 * @returns   Zero on success.
 */
static int _fsm_send_probe(gnrc_tcp_tcb_t *tcb)
{
    gnrc_pktsnip_t *out_pkt = NULL;  /* Outgoing packet */
    uint8_t probe_pay[] = {1};       /* Probe payload */

    DEBUG("gnrc_tcp_fsm.c : _fsm_send_probe()\n");
    /* The probe sends a already acknowledged sequence no. with a garbage byte. */
    _pkt_build(tcb, &out_pkt, NULL, MSK_ACK, tcb->snd_una - 1, tcb->rcv_nxt, probe_pay,
               sizeof(probe_pay));
    _pkt_send(tcb, out_pkt, 0, false);
    return 0;
}

/**
 * @brief FSM Handling Function for clearing the retransmit queue.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 *
 * @returns   Zero on success.
 */
static int _fsm_clear_retransmit(gnrc_tcp_tcb_t *tcb)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_clear_retransmit()\n");
    _clear_retransmit(tcb);
    return 0;
}

/**
 * @brief FSM function (not synchronized).
 *
 * @param[in,out] tcb     TCB holding the connection information.
 * @param[in]     event   Current event that triggers fsm translation.
 * @param[in]     in_pkt  Packet that triggered fsm event. Only in case of RCVD_PKT.
 * @param[in,out] buf     Buffer for send and receive functions.
 * @param[in]     len     Number of bytes to send or receive in @p buf.
 *
 * @returns   Zero on success.
 *           -ENOMEM if receive buffer could not be allocated.
 *           -EADDRINUSE if given local port number in @p tcb is already in use.
 *           -EOPNOTSUPP if event is not implemented.
 */
static int _fsm_unprotected(gnrc_tcp_tcb_t *tcb, fsm_event_t event, gnrc_pktsnip_t *in_pkt,
                            void *buf, size_t len)
{
    int ret = 0;

    DEBUG("gnrc_tcp_fsm.c : _fsm_unprotected()\n");
    switch (event) {
        case FSM_EVENT_CALL_OPEN :
            ret = _fsm_call_open(tcb);
            break;
        case FSM_EVENT_CALL_SEND :
            ret = _fsm_call_send(tcb, buf, len);
            break;
        case FSM_EVENT_CALL_RECV :
            ret = _fsm_call_recv(tcb, buf, len);
            break;
        case FSM_EVENT_CALL_CLOSE :
            ret = _fsm_call_close(tcb);
            break;
        case FSM_EVENT_CALL_ABORT :
            ret = _fsm_call_abort(tcb);
            break;
        case FSM_EVENT_RCVD_PKT :
            ret = _fsm_rcvd_pkt(tcb, in_pkt);
            break;
        case FSM_EVENT_TIMEOUT_TIMEWAIT :
            ret = _fsm_timeout_timewait(tcb);
            break;
        case FSM_EVENT_TIMEOUT_RETRANSMIT :
            ret = _fsm_timeout_retransmit(tcb);
            break;
        case FSM_EVENT_TIMEOUT_CONNECTION :
            ret = _fsm_timeout_connection(tcb);
            break;
        case FSM_EVENT_SEND_PROBE :
            ret = _fsm_send_probe(tcb);
            break;
        case FSM_EVENT_CLEAR_RETRANSMIT :
            ret = _fsm_clear_retransmit(tcb);
            break;
    }
    return ret;
}

int _fsm(gnrc_tcp_tcb_t *tcb, fsm_event_t event, gnrc_pktsnip_t *in_pkt, void *buf, size_t len)
{
    /* Lock FSM */
    mutex_lock(&(tcb->fsm_lock));

    /* Call FSM */
    tcb->status &= ~STATUS_NOTIFY_USER;
    int32_t result = _fsm_unprotected(tcb, event, in_pkt, buf, len);

    /* Notify blocked thread if something interesting happened */
    if ((tcb->status & STATUS_NOTIFY_USER) && (tcb->status & STATUS_WAIT_FOR_MSG)) {
        msg_t msg;
        msg.type = MSG_TYPE_NOTIFY_USER;
        mbox_try_put(&(tcb->mbox), &msg);
    }
    /* Unlock FSM */
    mutex_unlock(&(tcb->fsm_lock));
    return result;
}
