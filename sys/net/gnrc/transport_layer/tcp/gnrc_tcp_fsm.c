/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "msg.h"
#include "random.h"
#include "ringbuffer.h"
#include "net/gnrc/tcp.h"
#include "tcp_internal/fsm.h"
#include "tcp_internal/pkt.h"
#include "tcp_internal/demux.h"
#include "tcp_internal/option.h"
#include "tcp_internal/helper.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void _restart_timewait_timer(gnrc_tcp_tcb_t *tcb)
{
    /* Stop existing Timer, start new timer */
    xtimer_remove(&tcb->timer_timeout);
    tcb->msg_timeout.type = MSG_TYPE_TIMEWAIT;
    tcb->msg_timeout.content.ptr = (void *)tcb;
    xtimer_set_msg(&tcb->timer_timeout, 2*GNRC_TCP_MSL, &tcb->msg_timeout, _tcp_pid);
}

int8_t _translate_to(gnrc_tcp_tcb_t *tcb, gnrc_tcp_fsm_state_t state, bool *notify_owner)
{
    switch(state){
        case CLOSED:        _demux_remove(tcb);
                            _clear_retransmit(tcb);
                            *notify_owner = true;
                            break;

        case LISTEN:        _demux_change_context(tcb, PORT_UNSPEC, tcb->local_port);
                            break;

        case ESTABLISHED:   *notify_owner = true;
                            break;

        case CLOSE_WAIT:    *notify_owner = true;
                            break;

        case TIME_WAIT:     _restart_timewait_timer(tcb);
                            break;

        default:            break;
    }

    /* DEBUG */
    switch(tcb->state){
        case CLOSED:        DEBUG("STATE CHANGED: CLOSED -> "); break;
        case SYN_SENT:      DEBUG("STATE CHANGED: SYN_SENT -> "); break;
        case SYN_RCVD:      DEBUG("STATE CHANGED: SYN_RCVD -> "); break;
        case LISTEN:        DEBUG("STATE CHANGED: LISTEN -> "); break;
        case ESTABLISHED:   DEBUG("STATE CHANGED: ESTABLISHED -> "); break;
        case CLOSE_WAIT:    DEBUG("STATE CHANGED: CLOSE_WAIT -> "); break;
        case FIN_WAIT_1:    DEBUG("STATE CHANGED: FIN_WAIT_1 -> "); break;
        case FIN_WAIT_2:    DEBUG("STATE CHANGED: FIN_WAIT_2 -> "); break;
        case CLOSING:       DEBUG("STATE CHANGED: CLOSING -> "); break;
        case LAST_ACK:      DEBUG("STATE CHANGED: LAST_ACK -> "); break;
        case TIME_WAIT:     DEBUG("STATE CHANGED: TIME_WAIT -> "); break;
    }

    tcb->state = state;

    /* DEBUG */
    switch(tcb->state){
        case CLOSED:        DEBUG("CLOSED\n"); break;
        case SYN_SENT:      DEBUG("SYN_SENT\n"); break;
        case SYN_RCVD:      DEBUG("SYN_RCVD\n"); break;
        case LISTEN:        DEBUG("LISTEN\n"); break;
        case ESTABLISHED:   DEBUG("ESTABLISHED\n"); break;
        case CLOSE_WAIT:    DEBUG("CLOSE_WAIT\n"); break;
        case FIN_WAIT_1:    DEBUG("FIN_WAIT_1\n"); break;
        case FIN_WAIT_2:    DEBUG("FIN_WAIT_2\n"); break;
        case CLOSING:       DEBUG("CLOSING\n"); break;
        case LAST_ACK:      DEBUG("LAST_ACK\n"); break;
        case TIME_WAIT:     DEBUG("TIME_WAIT\n"); break;
    }

    return 0;
}

ssize_t _fsm_unprotected(gnrc_tcp_tcb_t *tcb, gnrc_tcp_fsm_event_t event, gnrc_pktsnip_t *in_pkt, void* buf, size_t nByte, bool *notify_owner)
{
    gnrc_pktsnip_t* out_pkt = NULL;   /* Outgoing packet */
    uint16_t seq_con = 0;             /* Sequence Number Space Consumption */

    /* Event CALL_OPEN: Passive or Active Open */
    if(event == CALL_OPEN){
        DEBUG("tcp_fsm: CALL_OPEN\n");
        _demux_register(tcb);
        tcb->rcv_wnd = DEFAULT_WINDOW;

        if(tcb->options & AI_PASSIVE){
            /* Passive Open, T: CLOSED -> LISTEN */
            _translate_to(tcb, LISTEN, notify_owner);
        }else{
            /* Active Open, init tcb values, send SYN, T: CLOSED -> SYN_SENT */
            tcb->snd_nxt = tcb->snd_una = tcb->iss = genrand_uint32();
            _pkt_build(tcb, &out_pkt, &seq_con, MSK_SYN, tcb->iss, 0, NULL, 0);
            _pkt_setup_retransmit(tcb, out_pkt);
            _pkt_send(tcb, out_pkt, seq_con);
            _translate_to(tcb, SYN_SENT, notify_owner);
        }
        return 0;
    }

    /* Event CALL_CLOSE */
    if(event == CALL_CLOSE){
        DEBUG("tcp_fsm: CALL_CLOSE\n");
        if(tcb->state == ESTABLISHED || tcb->state == CLOSE_WAIT){
            /* Send FIN packet */
            _pkt_build(tcb, &out_pkt, &seq_con, MSK_FIN_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
            _pkt_setup_retransmit(tcb, out_pkt);
            _pkt_send(tcb, out_pkt, seq_con);

            if(tcb->state == ESTABLISHED){
                _translate_to(tcb, FIN_WAIT_1, notify_owner);
            }else if(tcb->state == CLOSE_WAIT){
                _translate_to(tcb, LAST_ACK, notify_owner);
            }
        }
        return 0;
    }

    if(event == CALL_SEND){
        DEBUG("tcp_fsm: CALL_SEND\n");
        size_t payload = (tcb->snd_una + tcb->snd_wnd) - tcb->snd_nxt;

        /* We are allowed to send further bytes if window is open */
        if(payload > 0 && tcb->snd_wnd > 0 && tcb->pkt_retransmit == NULL){
            /* Calculate segment size */
            payload = (payload < GNRC_TCP_MSS ? payload : GNRC_TCP_MSS);
            payload = (payload < tcb->mss) ? payload : tcb->mss;
            payload = (payload < nByte) ? payload : nByte;

            /* Calculate payload size for this segment */
            _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, buf, payload);
            _pkt_setup_retransmit(tcb, out_pkt);
            _pkt_send(tcb, out_pkt, seq_con);
            return payload;
        }

        return 0;
    }

    if(event == CALL_RECV){
        DEBUG("tcp_fsm: CALL_RECV\n");
        if(ringbuffer_empty(&tcb->rcv_buf)){
            return 0;
        }

        /* Read up to the requesed amount of data */
        size_t rcvd = ringbuffer_get(&(tcb->rcv_buf), buf, nByte);

        /* If the buffer can store more than the GNRC_TCP_MSS: open Window to available buffersize */
        if(ringbuffer_get_free(&tcb->rcv_buf) >= GNRC_TCP_MSS){
            tcb->rcv_wnd = ringbuffer_get_free(&(tcb->rcv_buf));

            /* Send ACK to update window on reopening */
            _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, 0, 0);
            _pkt_send(tcb, out_pkt, seq_con);
        }
        return rcvd;
    }

    if(event == RCVD_PKT){
        DEBUG("tcp_fsm: RCVD_PKT\n");
        gnrc_pktsnip_t *snp = NULL;
        tcp_hdr_t *tcp_hdr = NULL;

        uint16_t src = 0;
        uint16_t dst = 0;
        uint16_t ctl = 0;
        uint32_t seg_seq = 0;
        uint32_t seg_ack = 0;
        uint32_t seg_urg = 0;
        uint32_t seg_len = 0;
        uint32_t seg_wnd = 0;

        /* Search TCP header. */
        LL_SEARCH_SCALAR(in_pkt, snp, type, GNRC_NETTYPE_TCP);
        tcp_hdr = (tcp_hdr_t *) snp->data;

        /* Verify packet options, return if they were faulty */
        if(_option_parse(tcb, tcp_hdr)<0){
            return 0;
        }

        /* Extract needed values from header */
        ctl = byteorder_ntohs(tcp_hdr->off_ctl);
        seg_seq = byteorder_ntohl(tcp_hdr->seq_num);
        seg_ack = byteorder_ntohl(tcp_hdr->ack_num);
        seg_urg = byteorder_ntohs(tcp_hdr->urgent_ptr);
        seg_wnd = byteorder_ntohs(tcp_hdr->window);

        /* Handle state LISTEN */
        if(tcb->state == LISTEN){
            /* 1) Check RST: if set, return */
            if(ctl & MSK_RST){
                return 0;
            }
#ifdef MODULE_GNRC_IPV6
            /* Store received address information into tcb */
            LL_SEARCH_SCALAR(in_pkt, snp, type, GNRC_NETTYPE_IPV6);
            ipv6_hdr_t *ip6 = (ipv6_hdr_t *)snp->data;
            memcpy(tcb->peer_addr, &(ip6->src), tcb->peer_addr_len);
#endif
            /* 2) Check ACK: if set, send reset with seq_no = ack_no, return */
            if(ctl & MSK_ACK){
                _pkt_build_reset_from_pkt(&out_pkt, in_pkt);
                _pkt_send(tcb, out_pkt, 0);
                return 0;
            }
            /* 3) Check SYN: Setup incoming connection*/
            if(ctl & MSK_SYN){
                src = byteorder_ntohs(tcp_hdr->src_port);
                dst = byteorder_ntohs(tcp_hdr->dst_port);

                /* Check if connection is already handled by another tcb */
                if(gnrc_netreg_num(GNRC_NETTYPE_TCP, _demux_build_context(src, dst)) > 0){
                    return 0;
                }
                /* Take responsibility for this connection, setup tcb */
                _demux_change_context(tcb, src, dst);
                tcb->irs = byteorder_ntohl(tcp_hdr->seq_num);
                tcb->rcv_nxt = tcb->irs + 1;
                tcb->snd_nxt = tcb->snd_una = tcb->iss = genrand_uint32();
                tcb->snd_wnd = seg_wnd;

                /* Send SYN+ACK: seq_no = iss, ack_no = rcv_nxt, T: LISTEN -> SYN_RCVD */
                _pkt_build(tcb, &out_pkt, &seq_con, MSK_SYN_ACK, tcb->iss, tcb->rcv_nxt, NULL, 0);
                _pkt_setup_retransmit(tcb, out_pkt);
                _pkt_send(tcb, out_pkt, seq_con);
                _translate_to(tcb, SYN_RCVD, notify_owner);
            }
            return 0;
        }

        /* Handle state SYN_SENT */
        else if(tcb->state == SYN_SENT){
            /* 1) Check ACK */
            if(ctl & MSK_ACK){
                /* If ACK is not acceptable ...*/
                if(seg_ack <= tcb->iss || seg_ack > tcb->snd_nxt){
                    /* ... send Reset if RST is not set else return */
                    if((ctl & MSK_RST) != MSK_RST){
                        _pkt_build(tcb, &out_pkt, &seq_con, MSK_RST, seg_ack, 0, NULL, 0);
                        _pkt_send(tcb, out_pkt, seq_con);
                    }
                    return 0;
                }
            }
            /* 2) Check RST: If RST set ... */
            if(ctl & MSK_RST){
                /* ... and ACK: Translate to CLOSED, if not return */
                if(ctl & MSK_ACK){
                    _translate_to(tcb, CLOSED, notify_owner);
                }
                return 0;
            }
            /* 3) Check SYN: Set TCB values accordingly */
            if(ctl & MSK_SYN){
                tcb->rcv_nxt = seg_seq + 1;
                tcb->irs = seg_seq;
                if(ctl & MSK_ACK){
                    tcb->snd_una = seg_ack;
                    _pkt_acknowledge(tcb, seg_ack);
                }

                /* SYN has been ACKed, reply pure ACK, T: SYN_SENT -> ESTABLISHED */
                if(tcb->snd_una > tcb->iss){
                    tcb->snd_wnd = seg_wnd;
                    tcb->snd_wl1 = seg_seq;
                    tcb->snd_wl2 = seg_ack;
                    _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
                    _pkt_send(tcb, out_pkt, seq_con);
                    _translate_to(tcb, ESTABLISHED, notify_owner);

                /* Simultaneous SYN received send SYN+ACK, T: SYN_SENT -> SYN_RCVD */
                }else{
                    _pkt_build(tcb, &out_pkt, &seq_con, MSK_SYN_ACK, tcb->iss, tcb->rcv_nxt, NULL, 0);
                    _pkt_setup_retransmit(tcb, out_pkt);
                    _pkt_send(tcb, out_pkt, seq_con);
                    _translate_to(tcb, SYN_RCVD, notify_owner);
                }
            }
            return 0;
        }

        /* Handle other states */
        else{
            seg_len = _pkt_get_seg_len(in_pkt);

             /* 1) Verify Sequence Number ... */
            if(!_pkt_chk_seq_num(tcb, seg_seq, seg_len)){
                /* ... if invalid, and RST not set, reply with pure ACK, return */
                if((ctl & MSK_RST) != MSK_RST){
                    _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
                    _pkt_send(tcb, out_pkt, seq_con);
                }
                return 0;
            }
            
            /* 2) Check RST: If RST is set ... */
            if(ctl & MSK_RST){
                /* .. and State is SYN_RCVD and passive Open: SYN_RCVD -> LISTEN */
                if(tcb->state == SYN_RCVD && (tcb->options & AI_PASSIVE)){
                    _translate_to(tcb, LISTEN, notify_owner);
                }else{
                    _translate_to(tcb, CLOSED, notify_owner);
                }
                return 0;
            }
            /* 3) Check SYN: If SYN is set ... */
            if(ctl & MSK_SYN){
                /* ... send RST, seq_no = snd_nxt, ack_no = rcv_nxt */
                _pkt_build(tcb, &out_pkt, &seq_con, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0);
                _pkt_send(tcb, out_pkt, seq_con);
                _translate_to(tcb, CLOSED, notify_owner);
                return 0;
            }
            /* 4) Check ACK */
            if(!(ctl & MSK_ACK)){
                return 0;
            }else{
                if(tcb->state == SYN_RCVD){
                    if(tcb->snd_una <= seg_ack && seg_ack <= tcb->snd_nxt){
                        tcb->snd_wnd = seg_wnd;
                        tcb->snd_wl1 = seg_seq;
                        tcb->snd_wl2 = seg_ack;
                        _translate_to(tcb, ESTABLISHED, notify_owner);
                    }else{
                        _pkt_build(tcb, &out_pkt, &seq_con, MSK_RST, seg_ack, 0, NULL, 0);
                        _pkt_send(tcb, out_pkt, seq_con);
                    }
                }

                /* Acknowledgment processing */
                if(tcb->state == ESTABLISHED || tcb->state == FIN_WAIT_1 || tcb->state == FIN_WAIT_2
                || tcb->state == CLOSE_WAIT || tcb->state == CLOSING || tcb->state == LAST_ACK){
                    /* New seg_acknowledgment received */
                    if(tcb->snd_una <= seg_ack && seg_ack <= tcb->snd_nxt){
                        tcb->snd_una = seg_ack;
                        _pkt_acknowledge(tcb, seg_ack);

                        /* Update Window */
                        if(tcb->snd_wl1 < seg_seq || (tcb->snd_wl1 == seg_seq && tcb->snd_wl2 <= seg_ack)){
                            tcb->snd_wnd = seg_wnd;
                            tcb->snd_wl1 = seg_seq;
                            tcb->snd_wl2 = seg_ack;
                            
                            /* Signal User because Window Update */
                            *notify_owner = true;
                        }
                    }
                    /* This is an old Duplicate, ignore it */
                    else if(seg_ack < tcb->snd_una){
                        return 0;
                    }
                    /* ACK received for something not yet sent: Reply with pure ACK */
                    else if(tcb->snd_nxt > seg_ack){
                        _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
                        _pkt_send(tcb, out_pkt, seq_con);
                        return 0;
                    }

                    /* Additional processing */
                    /* Check additionaly if previous our sent FIN has been acknowledged */
                    if(tcb->state == FIN_WAIT_1){
                        if(tcb->pkt_retransmit == NULL){
                            _translate_to(tcb, FIN_WAIT_2, notify_owner);
                        }
                    }
                    
                    /* If retransmission queue is empty, acknowledge close operation */
                    if(tcb->state == FIN_WAIT_2){
                        if(tcb->pkt_retransmit == NULL){
                        /* Optional: Unblock user close operation */
                        }
                    }

                    /* If our FIN has been acknowledged: Translate to TIME_WAIT */
                    if(tcb->state == CLOSING){
                        if(tcb->pkt_retransmit == NULL){
                            _translate_to(tcb, TIME_WAIT, notify_owner);
                        }
                    }

                    /* If our FIN has been acknowledged: last ACK received, close connection */
                    if(tcb->state == LAST_ACK){
                        if(tcb->pkt_retransmit == NULL){
                            _translate_to(tcb, CLOSED, notify_owner);
                            return 0;
                        }
                    }
                } 

            }
            /* 5) Check URG */
            if(ctl & MSK_URG){
                if(tcb->state == ESTABLISHED || tcb->state == FIN_WAIT_1 || tcb->state == FIN_WAIT_2){
                    tcb->rcv_up = (tcb->rcv_up > seg_urg) ? tcb->rcv_up : seg_urg;
                    /* TODO: Signal Userspace */
                }
            }

            /* 6) Process Payload, if existing */
            if(seg_len > 0){
                /* Check if State is valid */
                if(tcb->state == ESTABLISHED || tcb->state == FIN_WAIT_1 || tcb->state == FIN_WAIT_2){
                    /* Search for begin of payload "chain" */
                    LL_SEARCH_SCALAR(in_pkt, snp, type, GNRC_NETTYPE_UNDEF);

                    /* Add only Data that is expected, to be received */
                    if(tcb->rcv_nxt == seg_seq){
                        /* Copy contents in to buffer */
                        while(snp && snp->type == GNRC_NETTYPE_UNDEF){
                            tcb->rcv_nxt += ringbuffer_add( &(tcb->rcv_buf), snp->data, snp->size);
                            snp = snp->next;
                        }

                        /* Shrink Receive Window */
                        tcb->rcv_wnd = ringbuffer_get_free(&(tcb->rcv_buf));
                        /* Notify Owner because new data is available */
                        *notify_owner = true;
                    }

                    /* Send pure ACK, if FIN doesn't this already */ 
                    /* TODO: this is the place for piggybagging */
                    if(!(ctl & MSK_FIN)){
                        _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
                        _pkt_send(tcb, out_pkt, seq_con);
                    }
                }
            }

            /* 7) Check FIN */
            if(ctl & MSK_FIN){
                if(tcb->state == CLOSED || tcb->state == LISTEN || tcb->state == SYN_SENT){
                    return 0;
                }

                /* TOOO: Signal User, Connection closing */
                /* Acknowledge, received data + FIN Bit */
                tcb->rcv_nxt += 1;
                _pkt_build(tcb, &out_pkt, &seq_con, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0);
                _pkt_send(tcb, out_pkt, seq_con);

                if(tcb->state == SYN_RCVD || tcb->state == ESTABLISHED){
                    _translate_to(tcb, CLOSE_WAIT, notify_owner);
                }else if(tcb->state == FIN_WAIT_1){
                    if(tcb->pkt_retransmit == NULL){
                        _translate_to(tcb, TIME_WAIT, notify_owner);
                    }else{
                        _translate_to(tcb, CLOSING, notify_owner);
                    }
                }else if(tcb->state == FIN_WAIT_2){
                    _translate_to(tcb, TIME_WAIT, notify_owner);
                }else if(tcb->state == TIME_WAIT){
                    _restart_timewait_timer(tcb);
                }
            }
        }
        return 0;
    }

    if(event == TIMEOUT_TIMEWAIT){
        DEBUG("tcp_fsm: TIMEOUT_TIMEWAIT\n");
        _translate_to(tcb, CLOSED, notify_owner);
        return 0;
    }

    if(event == TIMEOUT_RETRANSMIT){
        DEBUG("tcp_fsm: TIMEOUT_RETRANSMIT\n");
        _pkt_setup_retransmit(tcb, tcb->pkt_retransmit);
        _pkt_send(tcb, tcb->pkt_retransmit, 0);
        return 0;
    }

    if(event == TIMEOUT_USER){
        DEBUG("tcp_fsm: TIMEOUT_USER\n");
        _translate_to(tcb, CLOSED, notify_owner );
        return 0;
    }
    return 0;
}

ssize_t  _fsm(gnrc_tcp_tcb_t *tcb, gnrc_tcp_fsm_event_t event, gnrc_pktsnip_t *in_pkt, void* buf, size_t nByte)
{
    msg_t msg;
    ssize_t result;
    bool notify_owner;

    /* Lock FSM */
    mutex_lock(&tcb->mtx);
    notify_owner = false;
    result = _fsm_unprotected(tcb, event, in_pkt, buf, nByte, &notify_owner);
    
    /* Notify owner if something interesting happend */
    if(notify_owner){
        msg.type = MSG_TYPE_NOTIFY_USER;
        msg_send(&msg, tcb->owner);
    }
    /* Unlock FSM */
    mutex_unlock(&tcb->mtx);
    return result;
}
