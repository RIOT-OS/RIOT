/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* FSM */
static int8_t _fsm_closed(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    (void) pkt;

    if(event == CALL_OPEN){
        tcb->rcv_wnd = DEFAULT_WINDOW;

        if(tcb->flags & AI_PASSIVE){
            /* Open was passive, Register on demux context, Translation: CLOSED -> LISTEN */
            _demux_register(tcb);
            _setup_timeout(tcb, TIMEOUT_UNSPEC);
            tcb->state = LISTEN;
        }else{
            /* Open was active, Check if given address is specified */
#ifdef MODULE_NG_IPV6
            if(ng_ipv6_addr_is_unspecified((ng_ipv6_addr_t*) tcb->peer_addr)){
                return -EDESTADDRREQ;
            }
#endif
            /* Setup initial values, register on demux context, send SYN to peer.*/
            tcb->iss = genrand_uint32();
            tcb->snd_una = tcb->iss;
            tcb->snd_nxt = tcb->snd_una+1;
            _demux_register(tcb);
            _setup_timeout(tcb, TIMEOUT_SYN_SENT);
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_SYN, tcb->iss, 0, NULL, 0));

            /* Translation : CLOSED -> SYN_SENT */
            tcb->state = SYN_SENT;
        }
    }

    return 0;
}

static uint8_t _fsm_listen(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t src_port = 0;
    uint16_t dst_port = 0;
    uint16_t ctl_bits = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *)snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* RST Set: Drop paket and return */
        if(ctl_bits & MSK_RST){
            return 0;
        }

#ifdef MODULE_NG_IPV6
        /* Get IPv6 Header, copy address info.*/
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_IPV6);
        ng_ipv6_hdr_t *ip6_hdr = (ng_ipv6_hdr_t *)snp->data;
        memcpy(tcb->peer_addr, &(ip6_hdr->src), tcb->peer_addr_len);
#endif

        /* ACK Set: Send reset to peer, drop paket and return. */
        if(ctl_bits & MSK_ACK){
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, byteorder_ntohl(tcp_hdr->ack_num), 0, NULL, 0));
            return 0;
        }

        /* SYN Set: Setup new connection*/
        if(ctl_bits & MSK_SYN){
            src_port = byteorder_ntohs(tcp_hdr->src_port);
            dst_port = byteorder_ntohs(tcp_hdr->dst_port);

            /* Check if connection is handled. Drop paket and return */
            if(ng_netreg_num(NG_NETTYPE_TCP, _build_context(src_port, dst_port)) > 0){
                DEBUG("tcp: context already handled.\n");
                return 0;
            }

            /* Move to new demux context */
            _demux_change_context(tcb, src_port, dst_port);

            /* Handle this connection. Fill tcb with received information */
            tcb->irs = byteorder_ntohl(tcp_hdr->seq_num);
            tcb->rcv_nxt = tcb->irs + 1;
            tcb->iss = genrand_uint32();
            tcb->snd_una = tcb->iss;
            tcb->snd_nxt = tcb->snd_una+1;
            tcb->snd_wnd = byteorder_ntohs(tcp_hdr->window);

            /* Drop paket, send SYN+ACK to peer, translate: LISTEN -> SYN_RCVD. */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_SYN_ACK, tcb->iss, tcb->rcv_nxt, NULL, 0));
            _setup_timeout(tcb, TIMEOUT_SYN_RCVD);
            tcb->state = SYN_RCVD;
        }
    }
    return 0;
}

static int8_t _fsm_syn_rcvd(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t ctl_bits = 0;
    uint32_t ack_num = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    /* Event was a timeout: Transition: SYN_RCVD -> CLOSED */
    if(event == TIME_TIMEOUT){
        // TODO: Setup Reset
        //ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, NULL, 0));
        _demux_remove(tcb);
        tcb->state = CLOSED;
        return 0;
    }

    /* Event was a received paket */
    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *)snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* Check if received seq_num is invalid*/
        if(!_is_seq_num_acceptable(tcb, byteorder_ntohl(tcp_hdr->seq_num), _get_seg_len(pkt))){
            /* RST Set and seq_num is not acceptable: return*/
            if(ctl_bits & MSK_RST){
                return 0;
            }
            /* if seq_num is not acceptable: send ack, return */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
            return 0;
        }

        /* RST Set and valid seq_num: drop paket, transition: SYN_RCVD -> LISTEN, return */
        if(ctl_bits & MSK_RST){
            _demux_change_context(tcb, PORT_UNASSIGNED, tcb->local_port);
            _setup_timeout(tcb, TIMEOUT_UNSPEC);
            tcb->state = LISTEN;
            return 0;
        }

        /* SYN Set and valid seq_num: send Reset, transition SYN_RCVD -> CLOSED, return */
        if(ctl_bits & MSK_SYN){
            /* Todo: Signal send and recv calls */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0));
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* Process ACK */
        if((ctl_bits & MSK_ACK) == 0 ){
            /* ACK not set: return */
            return 0;
        }else{
            /* ACK Set and valid: Transition: SYN_RCVD -> ESTABLISHED, if ACK invalid send reset. */
            ack_num = byteorder_ntohl(tcp_hdr->ack_num);
            if(tcb->snd_una <= ack_num && ack_num <= tcb->snd_nxt){
                tcb->state = ESTABLISHED;
            }else{
                ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, byteorder_ntohl(tcp_hdr->ack_num), 0, NULL, 0));
            }
        }

        /* FIN Set: Transition: SYN_RCVD -> CLOSE_WAIT */
        if(ctl_bits & MSK_FIN){
            /* TODO: FIN  hadling */
            tcb->state = CLOSE_WAIT;
        }
    }

    // TODO: close call
    return -1;
}

static int8_t _fsm_syn_sent(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t ctl_bits = 0;
    uint32_t ack_num = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    /* Event was a timeout: Transition: SYN_SENT -> CLOSED */
    if(event == TIME_TIMEOUT){
        _demux_remove(tcb);
        tcb->state = CLOSED;
        return 0;
    }

    /* Event was a received paket: Process it */
    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *) snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* If RST only is set: return*/
        if((ctl_bits & MSK_RST_ACK) == MSK_RST){
            return 0;
        }

        /* If ACK is set: validate it */
        if(ctl_bits & MSK_ACK){
            ack_num = byteorder_ntohl(tcp_hdr->ack_num);
            if( ack_num < tcb->snd_una || tcb->snd_nxt < ack_num){
                /* ack_num is invalid: send RST, if RST was not received */
                if((ctl_bits & MSK_RST) == 0){
                    ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, byteorder_ntohl(tcp_hdr->ack_num), 0, NULL, 0));
                }
                return 0;
            }
            /* RST and valid ACK: Transition: SYN_SENT -> CLOSED */
            if(ctl_bits & MSK_RST){
                _demux_remove(tcb);
                tcb->state = CLOSED;
                return 0;
            }
        }

        if(ctl_bits & MSK_SYN){
            /* SYN+ACK: "Normal Mode"  */
            if(ctl_bits & MSK_ACK){
                tcb->irs = byteorder_ntohl(tcp_hdr->seq_num);     // Get IRS from received seq_num
                tcb->rcv_nxt = tcb->irs + 1;                      // Init rcv_nxt
                tcb->snd_una = byteorder_ntohl(tcp_hdr->ack_num); // Increase snd_una
                tcb->snd_wnd = byteorder_ntohs(tcp_hdr->window);  // Update send window

                /* If snd_una acks iss: Transition: SYN_SENT-> ESTABLISHED. Send ACK to peer */
                if(tcb->snd_una == tcb->iss+1){
                    tcb->state = ESTABLISHED;
                    ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
                }
            }
        }
    }
    // TODO: Close call, recv syn call
    /* Transition : SYN_SENT -> CLOSED */
    // Close and signal possily waiting thread
    return 0;
}

static int8_t _fsm_established(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t ctl_bits = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    if(event == CALL_CLOSE){
        ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_FIN, 0,0, NULL ,0));
        tcb->state = FIN_WAIT_1;
    }

    /* Event was a received paket: Process it */
    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *)snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* 1) Validate seqno*/
        if(!_is_seq_num_acceptable(tcb, byteorder_ntohl(tcp_hdr->seq_num), _get_seg_len(pkt))){
            /* RST Set and seq_num is not acceptable: return*/
            if(ctl_bits & MSK_RST){
                return 0;
            }
            /* if seq_num is not acceptable: send ack, return */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
            return 0;
        }

        /* 2) Check for RST: ESTABLISHED -> CLOSED*/
        if(ctl_bits & MSK_RST){
            /* TODO: Signal send and recv calls */
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* 3) Check for SYN: send Reset, transition ESTABLISHED -> CLOSED, return */
        if(ctl_bits & MSK_SYN){
            /* Todo: Signal send and recv calls */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0));
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* 4) Check for ACK */
        if((ctl_bits & MSK_ACK) == 0 ){
            /* ACK not set: return */
            return 0;
        }else{
            /* Todo: Process incomming Data */
        }

        /* 5) Check for Urgent Bit*/
        if(ctl_bits & MSK_URG){
            /* TODO: Urgend Handling */
        }

        /* 6) Process Segemnt Text */
        // TODO

        /* 7) Check for FIN: Transition: ESTABLISHED -> CLOSE_WAIT */
        if(ctl_bits & MSK_FIN){
            /* TODO: FIN  hadling */
            tcb->state = CLOSE_WAIT;
        }
    }

    return 0;
}

static int8_t _fsm_close_wait(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t ctl_bits = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    if(event == CALL_CLOSE){
        ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_FIN, 0,0, NULL ,0));
        tcb->state = CLOSING;
    }

    /* Event was a received paket: Process it */
    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *)snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* Check if received seq_num is invalid*/
        if(!_is_seq_num_acceptable(tcb, byteorder_ntohl(tcp_hdr->seq_num), _get_seg_len(pkt))){
            /* RST Set and seq_num is not acceptable: return*/
            if(ctl_bits & MSK_RST){
                return 0;
            }
            /* if seq_num is not acceptable: send ack, return */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
            return 0;
        }

        /* RST set and valid seqno: CLOSE_WAIT -> CLOSED*/
        if(ctl_bits & MSK_RST){
            /* TODO: Signal send and recv calls */
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* SYN Set and valid seq_num: send Reset, transition CLOSE_WAIT -> CLOSED, return */
        if(ctl_bits & MSK_SYN){
            /* Todo: Signal send and recv calls */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0));
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* 4) Check for ACK */
        if((ctl_bits & MSK_ACK) == 0 ){
            /* ACK not set: return */
            return 0;
        }else{
            /* Todo: Process incomming Data */
        }

    }

    return 0;
}

static int8_t _fsm_last_ack(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t ctl_bits = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    /* Event was a received paket: Process it */
    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *)snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* Check if received seq_num is invalid*/
        if(!_is_seq_num_acceptable(tcb, byteorder_ntohl(tcp_hdr->seq_num), _get_seg_len(pkt))){
            /* RST Set and seq_num is not acceptable: return*/
            if(ctl_bits & MSK_RST){
                return 0;
            }
            /* if seq_num is not acceptable: send ack, return */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
            return 0;
        }

        /* RST set and valid seqno: LAST_ACK -> CLOSED*/
        if(ctl_bits & MSK_RST){
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* SYN Set and valid seq_num: send Reset, transition LAST_ACK -> CLOSED, return */
        if(ctl_bits & MSK_SYN){
            /* Todo: Signal send and recv calls */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0));
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* 4) Check for ACK */
        if((ctl_bits & MSK_ACK) == 0 ){
            /* ACK not set: return */
            return 0;
        }else{
            /* Todo: Process incomming Data */
        }

    }
    return 0;
}

static int8_t _fsm_fin_wait_1(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t ctl_bits = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    /* Event was a received paket: Process it */
    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *)snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* Check if received seq_num is invalid*/
        if(!_is_seq_num_acceptable(tcb, byteorder_ntohl(tcp_hdr->seq_num), _get_seg_len(pkt))){
            /* RST Set and seq_num is not acceptable: return*/
            if(ctl_bits & MSK_RST){
                return 0;
            }
            /* if seq_num is not acceptable: send ack, return */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
            return 0;
        }

        /* RST set and valid seqno: FIN_WAIT_1 -> CLOSED*/
        if(ctl_bits & MSK_RST){
            /* TODO: Signal send and recv calls */
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* SYN set and valid seq_num: send Reset, transition FIN_WAIT_1 -> CLOSED, return */
        if(ctl_bits & MSK_SYN){
            /* Todo: Signal send and recv calls */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0));
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* 4) Check for ACK */
        if((ctl_bits & MSK_ACK) == 0 ){
            /* ACK not set: return */
            return 0;
        }else{
            /* Todo: Process incomming Data */
        }

        /* 5) Check for Urgent Bit*/
        if(ctl_bits & MSK_URG){
            /* TODO: Urgend Handling */
        }

        /* 6) Process Segement Text */
        // TODO

        /* 7) Process FIN */
        if(ctl_bits & MSK_FIN){
            /* TODO: FIN  hadling */
            tcb->state = TIME_WAIT;
        }
    }

    return 0;
}

static int8_t _fsm_fin_wait_2(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t ctl_bits = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    /* Event was a received paket: Process it */
    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *)snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* Check if received seq_num is invalid*/
        if(!_is_seq_num_acceptable(tcb, byteorder_ntohl(tcp_hdr->seq_num), _get_seg_len(pkt))){
            /* RST Set and seq_num is not acceptable: return*/
            if(ctl_bits & MSK_RST){
                return 0;
            }
            /* if seq_num is not acceptable: send ack, return */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
            return 0;
        }

        /* RST set and valid seqno: FIN_WAIT_2 -> CLOSED*/
        if(ctl_bits & MSK_RST){
            /* TODO: Signal send and recv calls */
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* SYN Set and valid seq_num: send Reset, transition FIN_WAIT_2 -> CLOSED, return */
        if(ctl_bits & MSK_SYN){
            /* Todo: Signal send and recv calls */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0));
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* 4) Check for ACK */
        if((ctl_bits & MSK_ACK) == 0 ){
            /* ACK not set: return */
            return 0;
        }else{
            /* Todo: Process incomming Data */
        }

        /* 5) Check for Urgent Bit*/
        if(ctl_bits & MSK_URG){
            /* TODO: Urgend Handling */
        }

        /* 6) Process Segement Text */
        // TODO

        /* 7) Process FIN */
        if(ctl_bits & MSK_FIN){
            /* TODO: FIN  hadling */
            tcb->state = TIME_WAIT;
        }

    }

    return 0;
}

static int8_t _fsm_closing(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t ctl_bits = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    /* Event was a received paket: Process it */
    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *)snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* Check if received seq_num is invalid*/
        if(!_is_seq_num_acceptable(tcb, byteorder_ntohl(tcp_hdr->seq_num), _get_seg_len(pkt))){
            /* RST Set and seq_num is not acceptable: return*/
            if(ctl_bits & MSK_RST){
                return 0;
            }
            /* if seq_num is not acceptable: send ack, return */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
            return 0;
        }

        /* RST set and valid seqno: CLOSING -> CLOSED*/
        if(ctl_bits & MSK_RST){
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* SYN Set and valid seq_num: send Reset, transition CLOSING -> CLOSED, return */
        if(ctl_bits & MSK_SYN){
            /* Todo: Signal send and recv calls */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0));
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* 4) Check for ACK */
        if((ctl_bits & MSK_ACK) == 0 ){
            /* ACK not set: return */
            return 0;
        }else{
            /* Todo: Process incomming Data */
        }
    }
    return 0;
}

static int8_t _fsm_time_wait(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    uint16_t ctl_bits = 0;
    ng_pktsnip_t *snp = NULL;
    ng_tcp_hdr_t *tcp_hdr = NULL;

    /* Event was a received paket: Process it */
    if(event == RCVD_PKT){
        /* Get TCP Header and extract data */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        tcp_hdr = (ng_tcp_hdr_t *)snp->data;
        ctl_bits = byteorder_ntohs(tcp_hdr->off_ctl);

        /* Check if received seq_num is invalid*/
        if(!_is_seq_num_acceptable(tcb, byteorder_ntohl(tcp_hdr->seq_num), _get_seg_len(pkt))){
            /* RST Set and seq_num is not acceptable: return*/
            if(ctl_bits & MSK_RST){
                return 0;
            }
            /* if seq_num is not acceptable: send ack, return */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_ACK, tcb->snd_nxt, tcb->rcv_nxt, NULL, 0));
            return 0;
        }

        /* RST set and valid seqno: TIME_WAIT -> CLOSED*/
        if(ctl_bits & MSK_RST){
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* SYN Set and valid seq_num: send Reset, transition TIME_WAIT -> CLOSED, return */
        if(ctl_bits & MSK_SYN){
            /* Todo: Signal send and recv calls */
            ng_netapi_send(_tcp_pid, _build_pkt(tcb, MSK_RST, tcb->snd_nxt, tcb->rcv_nxt, NULL,0));
            _demux_remove(tcb);
            tcb->state = CLOSED;
            return 0;
        }

        /* 4) Check for ACK */
        if((ctl_bits & MSK_ACK) == 0 ){
            /* ACK not set: return */
            return 0;
        }else{
            /* Todo: Process incomming Data */
        }

        /* 5) Check for FIN: Restart timeout */
        if(ctl_bits & MSK_FIN){
            /* TODO: FIN  hadling */
        }
    }

    return 0;
}

static int8_t _fsm(ng_tcp_tcb_t *tcb, ng_pktsnip_t *pkt, ng_tcp_event_t event)
{
    int8_t res = 0;
    ng_pktsnip_t *snp = NULL;

    /* Syncronize Statemaschine */
    mutex_lock(&(tcb->mtx));

    /* If event was a received paket, parse options*/
    if(event == RCVD_PKT){
        /* If Options are faulty, drop pkt, return non critical error */
        LL_SEARCH_SCALAR(pkt, snp, type, NG_NETTYPE_TCP);
        if( _parse_options(tcb, ((ng_tcp_hdr_t *)snp->data)) < 0){
            DEBUG("_fsm(): Received faulty options\n");
            ng_pktbuf_release(pkt);
            mutex_unlock(&(tcb->mtx));
            return 0;
        }
    }

    switch(tcb->state){
        case CLOSED:        res = _fsm_closed(tcb, pkt, event);
                            break;

        case LISTEN:        res = _fsm_listen(tcb, pkt, event);
                            break;

        case SYN_RCVD:      res = _fsm_syn_rcvd(tcb, pkt, event);
                            break;

        case SYN_SENT:      res = _fsm_syn_sent(tcb, pkt, event);
                            break;

        case ESTABLISHED:   res = _fsm_established(tcb, pkt, event);
                            break;

        case CLOSE_WAIT:    res = _fsm_close_wait(tcb, pkt, event);
                            break;

        case LAST_ACK:      res = _fsm_last_ack(tcb, pkt, event);
                            break;

        case FIN_WAIT_1:    res = _fsm_fin_wait_1(tcb, pkt, event);
                            break;

        case FIN_WAIT_2:    res = _fsm_fin_wait_2(tcb, pkt, event);
                            break;

        case CLOSING:       res = _fsm_closing(tcb, pkt, event);
                            break;

        case TIME_WAIT:     res = _fsm_time_wait(tcb, pkt, event);
                            break;

        default:            DEBUG("_fsm(): Non-legal State\n");
    }

    /* If paket was received, drop it */
    if(event == RCVD_PKT){
        ng_pktbuf_release(pkt);
    }
    mutex_unlock(&(tcb->mtx));
    return res;
}
