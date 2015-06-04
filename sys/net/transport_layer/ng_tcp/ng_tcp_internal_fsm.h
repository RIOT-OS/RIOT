/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* FSM */
/* FSM accroding to rfc. */
static int8_t _translate_to(ng_tcp_tcb_t *tcb, ng_tcp_states_t state){
    switch(state){
        case CLOSED:   _demux_remove(tcb);
                       break;
        case LISTEN:   _demux_change_context(tcb, PORT_UNSPEC, tcb->local_port);
                       break;
        default:       break;
    }
    tcb->state = state;
    return 0;
}

static int8_t _fsm(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    if(event == CALL_OPEN){
        /* Common actions */
        _demux_register(tcb);
        tcb->rcv_wnd = DEFAULT_WINDOW;

        if(tcb->options & AI_PASSIVE){
            /* Connection is passive, T: CLOSED -> LISTEN */
            _translate_to(tcb, LISTEN);
        }else{
            /* Connection is active, init tcb values, send SYN, T: CLOSED -> SYN_SENT */
            tcb->iss = genrand_uint32();
            tcb->snd_una = tcb->iss;
            tcb->snd_nxt = tcb->snd_una+1;
            _send_pkt(tcb, _build_pkt(tcb, MSK_SYN, tcb->iss, 0, NULL, 0));
            _translate_to(tcb, SYN_SENT);
        }
    }

    if(event == CALL_CLOSE){
        /* TODO */
    }

    if(event == CALL_SEND){
        /* TODO */
    }

    if(event == CALL_RECV){
        /* TODO */
    }

    if(event == RCVD_PKT){
        ng_pktsnip_t *snp = NULL;
        ng_tcp_hdr_t *tcp = NULL;
        uint16_t ctl = 0;
        uint32_t seq = 0;
        uint32_t ack = 0;
        uint32_t urg = 0;

        /* Search TCP header. */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp = (ng_tcp_hdr_t *) snp->data;

        /* Verify paket options, return if they were faulty */
        if(_parse_options(tcb, tcp)<0){
            return 0;
        }

        /* Extract needed values from header */
        ctl = byteorder_ntohs(tcp->off_ctl);
        seq = byteorder_ntohl(tcp->seq_num);
        ack = byteorder_ntohl(tcp->ack_num);
        urg = byteorder_ntohs(tcp->urgent_ptr);

        switch(tcb->state){
            /* Handle state LISTEN */
            case LISTEN:
                /* 1) Check RST: if set, do nothing */
                if(ctl & MSK_RST){
                    break;
                }
#ifdef MODULE_NG_IPV6
                /* Copy received address information into tcb*/
                LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_IPV6);
                ng_ipv6_hdr_t *ip6 = (ng_ipv6_hdr_t *)snp->data;
                memcpy(tcb->peer_addr, &(ip6->src), tcb->peer_addr_len);
#endif
                /* 2) Check ACK: if set, send reset, seq_no = ack_no */
                if(ctl & MSK_ACK){
                    _send_pkt(tcb, _build_pkt(tcb, MSK_RST, ack, 0, NULL, 0));
                    break;
                }
                /* 3) Check SYN: Setup incomming connection*/
                if(ctl & MSK_SYN){
                    uint16_t src = byteorder_ntohs(tcp->src_port);
                    uint16_t dst = byteorder_ntohs(tcp->dst_port);

                    /* Check if connection is handled by another tcb */
                    if(ng_netreg_num(NG_NETTYPE_TCP, _build_context(src, dst)) > 0){
                        break;
                    }
                    /* Move to new demux context, fill tcb */
                    _demux_change_context(tcb, src, dst);
                    tcb->irs = byteorder_ntohl(tcp->seq_num);
                    tcb->rcv_nxt = tcb->irs + 1;
                    tcb->iss = genrand_uint32();
                    tcb->snd_una = tcb->iss;
                    tcb->snd_nxt = tcb->snd_una+1;
                    tcb->snd_wnd = byteorder_ntohs(tcp->window);

                    /* Send SYN+ACK: seq_no = iss, ack_no = rcv_nxt, T: LISTEN -> SYN_RCVD */
                    _send_pkt(tcb, _build_pkt(tcb, MSK_SYN_ACK, tcb->iss, tcb->rcv_nxt, NULL, 0));
                    _translate_to(tcb, SYN_RCVD);
                }
                break;

            /* Handle state SYN_SENT */
            case SYN_SENT:
                /* 1) Check ACK: if set, validate it */
                if(ctl & MSK_ACK){
                    /* If ack invalid and RST not set: send reset, seq_no = ack_no */
                    if(ack <= tcb->iss || tcb->snd_nxt < ack){
                        if((ctl & MSK_RST) != MSK_RST){
                             _send_pkt(tcb, _build_pkt(tcb, MSK_RST, ack, 0, NULL, 0));
                        }
                        break;
                    }
                }
                /* 2) Check RST: If valid ack and rst set, T:SYN_SENT -> CLOSED, else return */
                if(ctl & MSK_RST){
                    if(ctl & MSK_ACK){
                        _translate_to(tcb, CLOSED);
                    }
                    break;
                }
                /* 3) Check SYN: If set and values valid, translate */
                if(ctl & MSK_SYN){
                    tcb->rcv_nxt = seq + 1;
                    tcb->irs = seq;
                    if(ctl & MSK_ACK){
                        tcb->snd_una = ack;
                        /* TODO: Remove acked pakets */
                    }

                    if(tcb->snd_una > tcb->iss){
                        /* Initial SYN has been acked send ack, T: SYN_SENT -> ESTABLISHED */
                        _send_pkt(tcb, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
                        _translate_to(tcb, ESTABLISHED);
                    }else{
                        /* Simultanious SYN received send SYN+ACK, T: SYN_SENT -> SYN_RCVD */
                        _send_pkt(tcb, _build_pkt(tcb, MSK_SYN_ACK, tcb->iss, tcb->rcv_nxt, NULL, 0));
                        _translate_to(tcb, SYN_RCVD);
                    }
                }
                break;

            /* Handle other states */
            default:
                /* 1) Check seq_no: If RST set: return, if seq_no invalid, send ACK*/
                if(!_is_seq_num_acceptable(tcb, seq, _get_seg_len(pkt))){
                    if((ctl & MSK_RST) != MSK_RST){
                        _send_pkt(tcb, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
                    }
                    break;
                }
                /* 2) Check RST */
                if(ctl & MSK_RST){
                    /* State = SYN_RCVD: if passive opened: T: SYN_RCVD -> LISTEN */
                    if(tcb->state == SYN_RCVD && (tcb->options & AI_PASSIVE)){
                        _translate_to(tcb, LISTEN);
                    }else{
                        _translate_to(tcb, CLOSED);
                    }
                    break;
                }
                /* 3) Check SYN: send RST, seq_no = snd_nxt, ack_no = rcv_nxt */
                if(ctl & MSK_SYN){
                    _send_pkt(tcb, _build_pkt(tcb, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0));
                    _translate_to(tcb, CLOSED);
                    break;
                }
                /* 4) Check ACK: return if ACK is not set */
                if(!(ctl & MSK_ACK)){
                    break;
                }else{
                    if(tcb->state == SYN_RCVD){
                        if(tcb->snd_una <= ack && ack <= tcb->snd_nxt){
                            _translate_to(tcb, ESTABLISHED);
                        }else{
                            _send_pkt(tcb, _build_pkt(tcb, MSK_RST, ack, 0, NULL, 0));
                        }
                    }else if(tcb->state == ESTABLISHED){
                        if(tcb->snd_una < ack && ack <= tcb->snd_nxt){
                            tcb->snd_una = ack;
                            /* TODO: Remove acked packets, from retrans queue */
                            /* TODO: Notify User */
                            if(tcb->snd_wl1 < seq || (tcb->snd_wl1 == seq && tcb->snd_wl2 <= ack)){
                                tcb->snd_wnd = byteorder_ntohs(tcp->window);
                                tcb->snd_wl1 = seq;
                                tcb->snd_wl2 = ack;
                            }
                        }else if(ack < tcb->snd_una){
                            /* Old Duplicate */
                            break;
                        }else if(tcb->snd_nxt > ack){
                            _send_pkt(tcb, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
                            break;
                        }/* TODO: Other State Processing */
                    }
                }
                /* 5) Check URG */
                if(ctl & MSK_URG){
                    if(tcb->state == ESTABLISHED || tcb->state == FIN_WAIT_1 || tcb->state == FIN_WAIT_2){
                        tcb->rcv_up = (tcb->rcv_up > urg) ? tcb->rcv_up : urg;
                        /* TODO: Signal Userspace */
                    }
                }
                /* 6) Process Payload */
                if(tcb->state == ESTABLISHED || tcb->state == FIN_WAIT_1 || tcb->state == FIN_WAIT_2){
                    /* Transfer Payload to user buffer */
                    /* Signal if Push was receivd */
                    /* Update Receive Windows */
                    /* Ack. Tranfer if possible with other segments */
                    //_send_pkt(tcb, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
                }

                /* 7) Check FIN */
                if(ctl & MSK_FIN){
                    if(tcb->state == CLOSED || tcb->state == LISTEN || tcb->state == SYN_SENT){
                        break;
                    }

                    /* Signal User: Connection closing */
                    tcb->rcv_nxt = seq;
                    _send_pkt(tcb, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt,NULL,0));

                    if(tcb->state == SYN_RCVD || tcb->state == ESTABLISHED){
                        _translate_to(tcb, CLOSE_WAIT);
                    }else if(tcb->state == FIN_WAIT_1){
                        /* TODO: Check if FIN has been ACKED */
                        if(1){
                            _translate_to(tcb, TIME_WAIT);
                        }else{
                            _translate_to(tcb, CLOSING);
                        }
                    }else if(tcb->state == FIN_WAIT_2){
                        _translate_to(tcb, TIME_WAIT);
                    }else if(tcb->state == TIME_WAIT){
                        /* Restart timer, 2MSL  */
                    }
                }
                break;
        }
    }

    if(event == TIME_RETRANSMIT){
        _send_pkt(tcb, tcb->ret_queue[tcb->ret_head].pkt);
    }

    /* Cleanup */
    /* remove received paket from ng_pktbuf */
    if(pkt){
        ng_pktbuf_release(pkt);
    }

    return 0;
}
