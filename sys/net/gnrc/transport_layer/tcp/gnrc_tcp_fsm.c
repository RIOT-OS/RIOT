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

#include "random.h"
#include "net/af.h"
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
 * @brief Checks if a given portnumber is currently used by a tcb as local_port.
 *
 * @param[in] portnumber   Portnumber that should be checked
 *
 * @note Must be called from a context where the tcb list ist locked.
 *
 * @return   Zero if @p portnumber is currently not used.
 * @return   1 if @p portnumber is used by an tcb.
 */
static int _is_local_port_in_use(const uint16_t portnumber)
{
    gnrc_tcp_tcb_t *iter = NULL;
    LL_FOREACH(_list_tcb_head, iter) {
        if (iter->local_port == portnumber) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Generate random unused local port above the well-known ports (> 1024)
 *
 * @return The generated port number
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
 * @brief clears retransmit queue
 *
 * @param[in/out] tcb   tcb containing the retransmit queue.
 *
 * @return zero on success
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
 * @brief restarts time wait timer
 *
 * @param[in/out] tcb   tcb containing the timer structure to use.
 *
 * @return Zero on success
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
 * @brief translates fsm into another state
 *
 * @param[in/out] tcb            tcb, that specifies connection
 * @param[in]     state          state to translate in
 * @param[out]    notify_owner   non-negative if the tcb owner should be notified
 *
 * @return zero on success
 */
static int _transition_to(gnrc_tcp_tcb_t *tcb, fsm_state_t state, bool *notify_owner)
{
    gnrc_tcp_tcb_t *iter = NULL;
    uint8_t found = 0;

    switch (state) {
        case FSM_STATE_CLOSED:
            /* Free Packets in Retransmit queue */
            _clear_retransmit(tcb);

            /* Remove from Connection from active connections */
            mutex_lock(&_list_tcb_lock);
            LL_FOREACH(_list_tcb_head, iter) {
                if (iter == tcb) {
                    found = 1;
                }
            }
            if (found) {
                LL_DELETE(_list_tcb_head, iter);
            }
            mutex_unlock(&_list_tcb_lock);

            /* Free potencially allocated Receive Buffer */
            _rcvbuf_release_buffer(tcb);
            *notify_owner = true;
            break;

        case FSM_STATE_LISTEN:
            /* Clear Adress Info */
            switch (tcb->address_family) {
#ifdef MODULE_GNRC_IPV6
                case AF_INET6:
                    if (tcb->status & STATUS_ALLOW_ANY_ADDR) {
                        ipv6_addr_set_unspecified((ipv6_addr_t *) tcb->local_addr);
                    }
                    ipv6_addr_set_unspecified((ipv6_addr_t *) tcb->peer_addr);
                    break;
#endif
                default:
                    DEBUG("gnrc_tcp_fsm.c : _transition_to() : Undefined Addresses\n");
                    break;
            }
            tcb->peer_port = PORT_UNSPEC;

            /* Allocate rcv Buffer */
            if (_rcvbuf_get_buffer(tcb) == -ENOMEM) {
                return -ENOMEM;
            }

            /* Add to Connection to active connections (if not already active) */
            mutex_lock(&_list_tcb_lock);
            LL_FOREACH(_list_tcb_head, iter) {
                if (iter == tcb) {
                    found = 1;
                }
            }
            if (!found) {
                LL_APPEND(_list_tcb_head, tcb);
            }
            mutex_unlock(&_list_tcb_lock);
            break;

        case FSM_STATE_SYN_SENT:
            /* Allocate rcv Buffer */
            if (_rcvbuf_get_buffer(tcb) == -ENOMEM) {
                return -ENOMEM;
            }

            /* Add to Connections to active connection (if not already active) */
            mutex_lock(&_list_tcb_lock);
            LL_FOREACH(_list_tcb_head, iter) {
                if (iter == tcb) {
                    found = 1;
                }
            }
            /* If not already active: Apped tcb but check portnumber first */
            if (!found) {
                /* Check if Port Number is not in use */
                if (tcb->local_port != PORT_UNSPEC) {

                    /* If Portnumber is used: return error and release buffer */
                    if (_is_local_port_in_use(tcb->local_port)) {
                        mutex_unlock(&_list_tcb_lock);
                        _rcvbuf_release_buffer(tcb);
                        return -EADDRINUSE;
                    }
                }
                /* Pick Random Port */
                else {
                    tcb->local_port = _get_random_local_port();
                }
                LL_APPEND(_list_tcb_head, tcb);
            }
            mutex_unlock(&_list_tcb_lock);
            break;

        case FSM_STATE_ESTABLISHED:
            *notify_owner = true;
            break;

        case FSM_STATE_CLOSE_WAIT:
            *notify_owner = true;
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
 * @brief FSM Handling Function for active and passive open
 *
 * @param[in/out] tcb            Specifies tcb to use fsm on.
 * @param[out]    notify_owner   non-negative if the tcb owner should be notified
 *
 * @return zero on success
 * @return -ENOMEM       Can't allocate receive buffer.
 * @return -EADDRINUSE   Given local port is already in use
 */
static int _fsm_call_open(gnrc_tcp_tcb_t *tcb, bool *notify_owner)
{
    gnrc_pktsnip_t *out_pkt = NULL;     /* Outgoing packet */
    uint16_t seq_con = 0;               /* Sequence number consumption (out_pkt) */
    int ret = 0;                        /* Return value */

    DEBUG("gnrc_tcp_fsm.c : _fsm_call_open()\n");
    tcb->rcv_wnd = GNRC_TCP_DEFAULT_WINDOW;

    if (tcb->status & STATUS_PASSIVE) {
        /* Passive Open, T: CLOSED -> LISTEN */
        if (_transition_to(tcb, FSM_STATE_LISTEN, notify_owner) == -ENOMEM) {
            _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
            return -ENOMEM;
        }
    }
    else {
        /* Active Open, init tcb values, send SYN, T: CLOSED -> SYN_SENT */
        tcb->iss = random_uint32();
        tcb->snd_nxt = tcb->iss;
        tcb->snd_una = tcb->iss;

        /* Translate to SYN_SENT */
        ret = _transition_to(tcb, FSM_STATE_SYN_SENT, notify_owner);
        if (ret < 0) {
            _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
            return ret;
        }

        /* Send SYN */
        _pkt_build(tcb, &out_pkt, &seq_con, MSK_SYN, tcb->iss, 0, NULL, 0);
        _pkt_setup_retransmit(tcb, out_pkt, false);
        _pkt_send(tcb, out_pkt, seq_con, false);
    }
    return ret;
}

/**
 * @brief FSM Handling Function for sending data.
 *
 * @param[in/out] tcb   Specifies tcb to use fsm on.
 * @param[in/out] buf   Buffer containing data to send.
 * @param[in]     len   Maximum Number of Bytes to send.
 *
 * @return number of bytes that was sent.
 */
static int _fsm_call_send(gnrc_tcp_tcb_t *tcb, void *buf, size_t len)
{
    gnrc_pktsnip_t *out_pkt = NULL;     /* Outgoing packet */
    uint16_t seq_con = 0;               /* Sequence number consumption (out_pkt) */

    DEBUG("gnrc_tcp_fsm.c : _fsm_call_send()\n");
    size_t payload = (tcb->snd_una + tcb->snd_wnd) - tcb->snd_nxt;

    /* We are allowed to send further bytes if window is open */
    if (payload > 0 && tcb->snd_wnd > 0 && tcb->pkt_retransmit == NULL) {
        /* Calculate segment size */
        payload = (payload < GNRC_TCP_MSS) ? payload : GNRC_TCP_MSS;
        payload = (payload < tcb->mss) ? payload : tcb->mss;
        payload = (payload < len) ? payload : len;

        /* Calculate payload size for this segment */
        _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, buf, payload);
        _pkt_setup_retransmit(tcb, out_pkt, false);
        _pkt_send(tcb, out_pkt, seq_con, false);
        return payload;
    }
    return 0;
}

/**
 * @brief FSM Handling Function for receiving data.
 *
 * @param[in/out] tcb   Specifies tcb to use fsm on.
 * @param[in/out] buf   buffer to store received data into.
 * @param[in]     len   Maximum Number of Bytes to receive.
 *
 * @return number of bytes that was received.
 */
static int _fsm_call_recv(gnrc_tcp_tcb_t *tcb, void *buf, size_t len)
{
    gnrc_pktsnip_t *out_pkt = NULL;     /* Outgoing packet */
    uint16_t seq_con = 0;               /* Sequence number consumption (out_pkt) */

    DEBUG("gnrc_tcp_fsm.c : _fsm_call_recv()\n");
    if (ringbuffer_empty(&tcb->rcv_buf)) {
        return 0;
    }

    /* Read up to the requesed amount of data */
    size_t rcvd = ringbuffer_get(&(tcb->rcv_buf), buf, len);

    /* If the buffer can store more than the GNRC_TCP_MSS: open Window to available buffersize */
    if (ringbuffer_get_free(&tcb->rcv_buf) >= GNRC_TCP_MSS) {
        tcb->rcv_wnd = ringbuffer_get_free(&(tcb->rcv_buf));

        /* Send ACK to update window on reopening */
        _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
        _pkt_send(tcb, out_pkt, seq_con, false);
    }
    return rcvd;
}

/**
 * @brief FSM Handling Function for initiating a teardown.
 *
 * @param[in/out] tcb            Specifies tcb to use fsm on.
 * @param[out]    notify_owner   non-negative if the tcb owner should be notified
 *
 * @return zero on success.
 */
static int _fsm_call_close(gnrc_tcp_tcb_t *tcb, bool *notify_owner)
{
    gnrc_pktsnip_t *out_pkt = NULL;     /* Outgoing packet */
    uint16_t seq_con = 0;               /* Sequence number consumption (out_pkt) */

    DEBUG("gnrc_tcp_fsm.c : _fsm_call_close()\n");
    if (tcb->state == FSM_STATE_SYN_RCVD || tcb->state == FSM_STATE_ESTABLISHED ||
        tcb->state == FSM_STATE_CLOSE_WAIT) {
        /* Send FIN packet */
        _pkt_build(tcb, &out_pkt, &seq_con, MSK_FIN_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
        _pkt_setup_retransmit(tcb, out_pkt, false);
        _pkt_send(tcb, out_pkt, seq_con, false);
    }
    switch (tcb->state) {
        case FSM_STATE_LISTEN:
            _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
            break;

        case FSM_STATE_SYN_RCVD:
        case FSM_STATE_ESTABLISHED:
            _transition_to(tcb, FSM_STATE_FIN_WAIT_1, notify_owner);
            break;

        case FSM_STATE_CLOSE_WAIT:
            _transition_to(tcb, FSM_STATE_LAST_ACK, notify_owner);
            break;

        default:
            break;
    }
    return 0;
}

/**
 * @brief FSM Handling Function for forcefull teardown
 *
 * @return -EOPNOTSUPP, because function is currently not implemented
 */
static int _fsm_call_abort(void)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_call_abort()\n");
    DEBUG("gnrc_tcp_fsm.c : _fsm_call_abort() : ABORT not implemented\n");
    return -EOPNOTSUPP;
}

/**
 * @brief FSM Handling Function for processing of a received packet
 *
 * @param[in/out] tcb            Specifies tcb to use fsm on.
 * @param[in]     in_pkt         Packet that should be processed.
 * @param[out]    notify_owner   non-negative if the tcb owner should be notified
 *
 * @return zero on success.
 * @return -ENOMEM Can't allocate receive buffer.
 */
static int _fsm_rcvd_pkt(gnrc_tcp_tcb_t *tcb, gnrc_pktsnip_t *in_pkt, bool *notify_owner)
{
    gnrc_pktsnip_t *out_pkt = NULL;  /* Outgoing packet */
    uint16_t seq_con = 0;            /* Sequence number consumption (out_pkt) */
    gnrc_pktsnip_t *snp = NULL;      /* Temporary Packet Snip */
    gnrc_tcp_tcb_t *lst = NULL;      /* Temporary tcb pointer */
    uint16_t ctl = 0;                /* Received control bits */
    uint32_t seg_seq = 0;            /* Received sequence number */
    uint32_t seg_ack = 0;            /* Received acknowledgment number */
    uint32_t seg_len = 0;            /* Segment length */
    uint32_t pay_len = 0;            /* Payload length */
    uint32_t seg_wnd = 0;            /* Segment window */

    DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt()\n");
    /* Search TCP header. */
    LL_SEARCH_SCALAR(in_pkt, snp, type, GNRC_NETTYPE_TCP);
    tcp_hdr_t *tcp_hdr = (tcp_hdr_t *) snp->data;

    /* Verify packet options, return if they were faulty */
    if (_option_parse(tcb, tcp_hdr) < 0) {
        return 0;
    }

    /* Extract header values */
    ctl = byteorder_ntohs(tcp_hdr->off_ctl);
    seg_seq = byteorder_ntohl(tcp_hdr->seq_num);
    seg_ack = byteorder_ntohl(tcp_hdr->ack_num);
    seg_wnd = byteorder_ntohs(tcp_hdr->window);

    /* Extract IPv6-Header */
#ifdef MODULE_GNRC_IPV6
    LL_SEARCH_SCALAR(in_pkt, snp, type, GNRC_NETTYPE_IPV6);
    if (snp == NULL) {
        DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt() : incomming packet had no ip header\n");
        return 0;
    }
    void *ip = snp->data;
#endif

    /* Handle state LISTEN */
    if (tcb->state == FSM_STATE_LISTEN) {
        /* 1) Check RST: if set, return */
        if (ctl & MSK_RST) {
            return 0;
        }
        /* 2) Check ACK: if set, send reset with seq_no = ack_no, return */
        if (ctl & MSK_ACK) {
            _pkt_build_reset_from_pkt(&out_pkt, in_pkt);
            _pkt_send(tcb, out_pkt, 0, false);
            return 0;
        }
        /* 3) Check SYN: Setup incoming connection*/
        if (ctl & MSK_SYN) {
            uint16_t src = byteorder_ntohs(tcp_hdr->src_port);
            uint16_t dst = byteorder_ntohs(tcp_hdr->dst_port);

            /* Check if SYN Request is handled by another connection */
            lst = _list_tcb_head;
            while (lst) {
                /* Compare Portnumbers and Network Layer Adresses */
                /* Note: Packets without ip-header were discarded earlier */
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

            /* SYN Request is valid, fill connection struct with connection information */
            /* Note: Packets without ipv6-header were discarded earlier */
#ifdef MODULE_GNRC_IPV6
            if (snp->type == GNRC_NETTYPE_IPV6 && tcb->address_family == AF_INET6) {
                memcpy(tcb->local_addr, &((ipv6_hdr_t *)ip)->dst, sizeof(ipv6_addr_t));
                memcpy(tcb->peer_addr, &((ipv6_hdr_t *)ip)->src, sizeof(ipv6_addr_t));
            }
#else
            DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt() : Received Address was not stored\n");
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
            _transition_to(tcb, FSM_STATE_SYN_RCVD, notify_owner);
        }
        return 0;
    }
    /* Handle state SYN_SENT */
    else if (tcb->state == FSM_STATE_SYN_SENT) {
        /* 1) Check ACK */
        if (ctl & MSK_ACK) {
            /* If ACK is not acceptable ...*/
            if (seg_ack <= tcb->iss || seg_ack > tcb->snd_nxt) {
                /* ... send Reset if RST is not set else return */
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
                _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
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
            /* Set the local address accordingly */
            /* Note: Packets without ipv6-header were discarded earlier */
#ifdef MODULE_GNRC_IPV6
            if (snp->type == GNRC_NETTYPE_IPV6 && tcb->address_family == AF_INET6) {
                memcpy(tcb->local_addr, &((ipv6_hdr_t *)ip)->dst, sizeof(ipv6_addr_t));
            }
#else
            DEBUG("gnrc_tcp_fsm.c : _fsm_rcvd_pkt() : Received Address was not stored\n");
            return 0;
#endif

            /* SYN has been ACKed, reply pure ACK, T: SYN_SENT -> ESTABLISHED */
            if (tcb->snd_una > tcb->iss) {
                _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
                _pkt_send(tcb, out_pkt, seq_con, false);
                _transition_to(tcb, FSM_STATE_ESTABLISHED, notify_owner);
            }
            /* Simultaneous SYN received send SYN+ACK, T: SYN_SENT -> SYN_RCVD */
            else {
                _pkt_build(tcb, &out_pkt, &seq_con, MSK_SYN_ACK, tcb->iss, tcb->rcv_nxt, NULL, 0);
                _pkt_setup_retransmit(tcb, out_pkt, false);
                _pkt_send(tcb, out_pkt, seq_con, false);
                _transition_to(tcb, FSM_STATE_SYN_RCVD, notify_owner);
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
        /* 1) Verify Sequence Number ... */
        if (!_pkt_chk_seq_num(tcb, seg_seq, pay_len)) {
            /* ... if invalid, and RST not set, reply with pure ACK, return */
            if ((ctl & MSK_RST) != MSK_RST) {
                _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
                _pkt_send(tcb, out_pkt, seq_con, false);
            }
            return 0;
        }
        /* 2) Check RST: If RST is set ... */
        if (ctl & MSK_RST) {
            /* .. and State is SYN_RCVD and passive Open: SYN_RCVD -> LISTEN */
            if (tcb->state == FSM_STATE_SYN_RCVD && (tcb->status & STATUS_PASSIVE)) {
                if (_transition_to(tcb, FSM_STATE_LISTEN, notify_owner) == -ENOMEM) {
                    _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
                    return -ENOMEM;
                }
            }
            else {
                _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
            }
            return 0;
        }
        /* 3) Check SYN: If SYN is set ... */
        if (ctl & MSK_SYN) {
            /* ... send RST, seq_no = snd_nxt, ack_no = rcv_nxt */
            _pkt_build(tcb, &out_pkt, &seq_con, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
            _pkt_send(tcb, out_pkt, seq_con, false);
            _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
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
                    _transition_to(tcb, FSM_STATE_ESTABLISHED, notify_owner);
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
                /* Sent data has been acknowledged */
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
                /* Update Window */
                if (LEQ_32_BIT(tcb->snd_una, seg_ack) && LEQ_32_BIT(seg_ack, tcb->snd_nxt)) {
                    if (LSS_32_BIT(tcb->snd_wl1, seg_seq) || (tcb->snd_wl1 == seg_seq &&
                        LEQ_32_BIT(tcb->snd_wl2, seg_ack))) {
                        tcb->snd_wnd = seg_wnd;
                        tcb->snd_wl1 = seg_seq;
                        tcb->snd_wl2 = seg_ack;

                        /* Signal User after Window Update */
                        *notify_owner = true;
                    }
                }
                /* Additional processing */
                /* Check additionaly if previous our sent FIN has been acknowledged */
                if (tcb->state == FSM_STATE_FIN_WAIT_1) {
                    if (tcb->pkt_retransmit == NULL) {
                        _transition_to(tcb, FSM_STATE_FIN_WAIT_2, notify_owner);
                    }
                }
                /* If retransmission queue is empty, acknowledge close operation */
                if (tcb->state == FSM_STATE_FIN_WAIT_2) {
                    if (tcb->pkt_retransmit == NULL) {
                        /* Optional: Unblock user close operation */
                    }
                }
                /* If our FIN has been acknowledged: Translate to TIME_WAIT */
                if (tcb->state == FSM_STATE_CLOSING) {
                    if (tcb->pkt_retransmit == NULL) {
                        _transition_to(tcb, FSM_STATE_TIME_WAIT, notify_owner);
                    }
                }
                /* If our FIN has been acknowledged: last ACK received, close connection */
                if (tcb->state == FSM_STATE_LAST_ACK) {
                    if (tcb->pkt_retransmit == NULL) {
                        _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
                        return 0;
                    }
                }
            }
        }
        /* 5) Check URG */
        /* NOTE: Add Urgent Pointer Processing here ... */
        /* 6) Process Payload, if existing */
        if (pay_len > 0) {
            /* Check if State is valid */
            if (tcb->state == FSM_STATE_ESTABLISHED || tcb->state == FSM_STATE_FIN_WAIT_1 ||
                tcb->state == FSM_STATE_FIN_WAIT_2) {
                /* Search for begin of payload "chain" */
                LL_SEARCH_SCALAR(in_pkt, snp, type, GNRC_NETTYPE_UNDEF);

                /* Add only Data that is expected, to be received */
                if (tcb->rcv_nxt == seg_seq) {
                    /* Copy contents in to buffer */
                    while (snp && snp->type == GNRC_NETTYPE_UNDEF) {
                        tcb->rcv_nxt += ringbuffer_add(&(tcb->rcv_buf), snp->data, snp->size);
                        snp = snp->next;
                    }
                    /* Shrink Receive Window */
                    tcb->rcv_wnd = ringbuffer_get_free(&(tcb->rcv_buf));
                    /* Notify Owner because new data is available */
                    *notify_owner = true;
                }
                /* Send pure ACK, if FIN doesn't this already */
                /* NOTE: this is the place to add piggybagging in the future */
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
            /* Advance rcv_nxt over FIN bit. */
            tcb->rcv_nxt = seg_seq + seg_len;
            _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
            _pkt_send(tcb, out_pkt, seq_con, false);

            if (tcb->state == FSM_STATE_SYN_RCVD || tcb->state == FSM_STATE_ESTABLISHED) {
                _transition_to(tcb, FSM_STATE_CLOSE_WAIT, notify_owner);
            }
            else if (tcb->state == FSM_STATE_FIN_WAIT_1) {
                if (tcb->pkt_retransmit == NULL) {
                    _transition_to(tcb, FSM_STATE_TIME_WAIT, notify_owner);
                }
                else {
                    _transition_to(tcb, FSM_STATE_CLOSING, notify_owner);
                }
            }
            else if (tcb->state == FSM_STATE_FIN_WAIT_2) {
                _transition_to(tcb, FSM_STATE_TIME_WAIT, notify_owner);
            }
            else if (tcb->state == FSM_STATE_TIME_WAIT) {
                _restart_timewait_timer(tcb);
            }
        }
    }
    return 0;
}

/**
 * @brief FSM Handling Function for timewait timeout handling
 *
 * @param[in/out] tcb            Specifies tcb to use fsm on.
 * @param[out]    notify_owner   non-negative if the tcb owner should be notified
 *
 * @return zero on success.
 */
static int _fsm_timeout_timewait(gnrc_tcp_tcb_t *tcb, bool *notify_owner)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_timeout_timewait()\n");
    _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
    return 0;
}

/**
 * @brief FSM Handling Function for retransmissions
 *
 * @param[in/out] tcb            Specifies tcb to use fsm on.
 *
 * @return zero on success.
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
 * @brief FSM Handling Function for connection timeout handling
 *
 * @param[in/out] tcb            Specifies tcb to use fsm on.
 * @param[out]    notify_owner   non-negative if the tcb owner should be notified
 *
 * @return zero on success.
 */
static int _fsm_timeout_connection(gnrc_tcp_tcb_t *tcb, bool *notify_owner)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_timeout_connection()\n");
    _transition_to(tcb, FSM_STATE_CLOSED, notify_owner);
    return 0;
}

/**
 * @brief FSM Handling Function for probe sending
 *
 * @param[in/out] tcb    tcb of this connection
 *
 * @return zero on success.
 */
static int _fsm_send_probe(gnrc_tcp_tcb_t *tcb)
{
    gnrc_pktsnip_t *out_pkt = NULL;  /* Outgoing packet */
    uint8_t probe_pay[] = {1};       /* Probe Payload */

    DEBUG("gnrc_tcp_fsm.c : _fsm_send_probe()\n");
    /* The Probe sends a already acknowledged Sequence No. with a garbage byte */
    _pkt_build(tcb, &out_pkt, NULL, MSK_ACK, tcb->snd_una - 1, tcb->rcv_nxt, probe_pay,
               sizeof(probe_pay));
    _pkt_send(tcb, out_pkt, 0, false);
    return 0;
}

/**
 * @brief FSM Handling Function for clearing the retransmit queue.
 *
 * @param[in/out] tcb   tcb of this connection.
 *
 * @return zero on success.
 */
static int _fsm_clear_retransmit(gnrc_tcp_tcb_t *tcb)
{
    DEBUG("gnrc_tcp_fsm.c : _fsm_clear_retransmit()\n");
    _clear_retransmit(tcb);
    return 0;
}

/**
 * @brief real fsm: needs to be protected from the outside
 *
 * @param[in/out] tcb            Specifies tcb to use fsm on.
 * @param[in]     event          current event that triggers fsm translation
 * @param[in]     in_pkt         packet that triggered fsm event. Only in case of RCVD_PKT
 * @param[in/out] buf            buffer for send and receive functions
 * @param[in]     len            number of bytes to send or receive atmost
 * @param[out]    notify_owner   non-negative if the tcb owner should be notified
 *
 * @return Zero on success
 * @return -ENOMEM       Can't allocate receive buffer.
 * @return -EADDRINUSE   Given local port is already in use
 * @return -EOPNOTSUPP   If event is not implemented
 */
static int _fsm_unprotected(gnrc_tcp_tcb_t *tcb, fsm_event_t event, gnrc_pktsnip_t *in_pkt,
                            void *buf, size_t len, bool *notify_owner)
{
    int ret = 0;

    DEBUG("gnrc_tcp_fsm.c : _fsm_unprotected()\n");
    switch (event) {
        case FSM_EVENT_CALL_OPEN :
            ret = _fsm_call_open(tcb, notify_owner);
            break;
        case FSM_EVENT_CALL_SEND :
            ret = _fsm_call_send(tcb, buf, len);
            break;
        case FSM_EVENT_CALL_RECV :
            ret = _fsm_call_recv(tcb, buf, len);
            break;
        case FSM_EVENT_CALL_CLOSE :
            ret = _fsm_call_close(tcb, notify_owner);
            break;
        case FSM_EVENT_CALL_ABORT :
            ret = _fsm_call_abort();
            break;
        case FSM_EVENT_RCVD_PKT :
            ret = _fsm_rcvd_pkt(tcb, in_pkt, notify_owner);
            break;
        case FSM_EVENT_TIMEOUT_TIMEWAIT :
            ret = _fsm_timeout_timewait(tcb, notify_owner);
            break;
        case FSM_EVENT_TIMEOUT_RETRANSMIT :
            ret = _fsm_timeout_retransmit(tcb);
            break;
        case FSM_EVENT_TIMEOUT_CONNECTION :
            ret = _fsm_timeout_connection(tcb, notify_owner);
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
    msg_t msg;
    int32_t result;
    bool notify_owner;

    /* Lock FSM */
    mutex_lock(&(tcb->fsm_lock));
    notify_owner = false;
    result = _fsm_unprotected(tcb, event, in_pkt, buf, len, &notify_owner);

    /* Notify owner if something interesting happend */
    if (notify_owner && tcb->owner != KERNEL_PID_UNDEF) {
        msg.type = MSG_TYPE_NOTIFY_USER;
        msg_send(&msg, tcb->owner);
    }
    /* Unlock FSM */
    mutex_unlock(&(tcb->fsm_lock));
    return result;
}
