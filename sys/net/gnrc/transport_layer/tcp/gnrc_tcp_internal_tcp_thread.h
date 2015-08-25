/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
static gnrc_pktsnip_t* _tcp_build_reset(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *snp = NULL;
    gnrc_tcp_hdr_t tcp_hdr;
    uint16_t ctl = 0;
    size_t seg_len = _get_seg_len(pkt);

    /* Get received tcp header */
    LL_SEARCH_SCALAR(pkt, snp, type, GNRC_NETTYPE_TCP);
    gnrc_tcp_hdr_t *tcp_hdr_old = (gnrc_tcp_hdr_t *)snp->data;

    /* Get received network layer header */
#ifdef MODULE_GNRC_IPV6
    ipv6_hdr_t *ip6_hdr = (ipv6_hdr_t *)snp->next->data;
#endif

    ctl = byteorder_ntohs(tcp_hdr_old->off_ctl);

    /* TODO, work on it: Fill new tcp header */
    tcp_hdr.src_port = tcp_hdr_old->dst_port;
    tcp_hdr.dst_port = tcp_hdr_old->src_port;

    if(ctl & MSK_ACK){
        tcp_hdr.seq_num = tcp_hdr_old->ack_num;
        tcp_hdr.ack_num = byteorder_htonl(0);
        tcp_hdr.off_ctl = byteorder_htons(OFFSET_BASE | MSK_RST);

    }else {
        tcp_hdr.seq_num = byteorder_htonl(0);
        tcp_hdr.ack_num = byteorder_htonl(byteorder_ntohl(tcp_hdr_old->seq_num) + seg_len + 1);
        tcp_hdr.off_ctl = byteorder_htons(OFFSET_BASE | MSK_RST_ACK);
    }

    tcp_hdr.checksum = byteorder_htons(0);
    tcp_hdr.window = byteorder_htons(0);
    tcp_hdr.urgent_ptr = byteorder_htons(0);

    /* Build new tcp header */
    snp = gnrc_pktbuf_add(NULL, &tcp_hdr, OFFSET_BASE * 4, GNRC_NETTYPE_TCP);
    if(snp == NULL){
        DEBUG("tcp: Can't allocate buffer for TCP Header\n.");
        return NULL;
    }

    /* Build new network layer header */
#ifdef MODULE_GNRC_IPV6
    snp = gnrc_ipv6_hdr_build(snp, (uint8_t *) &(ip6_hdr->dst), sizeof(ip6_hdr->dst),
                            (uint8_t *) &(ip6_hdr->src), sizeof(ip6_hdr->src));
    if(snp == NULL){
        DEBUG("tcp: Can't allocate buffer for IPv6 Header.\n");
        return NULL;
    }
#endif

    return snp;
}

static void _tcp_send(gnrc_pktsnip_t *pkt)
{
    /* NOTE: Sending Direction: pkt = nw, nw->next = tcp, tcp->next = payload */
    gnrc_pktsnip_t* tcp_snp = NULL;
    gnrc_pktsnip_t* ip6_snp = NULL;
    gnrc_netreg_entry_t* sendto = NULL;

    /* Search for tcp header */
    LL_SEARCH_SCALAR(pkt, tcp_snp, type, GNRC_NETTYPE_TCP);
    if(tcp_snp == NULL ){
        DEBUG("tcp: unable to send packet without tcp header, discarding it.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* Print TCP Header */
    gnrc_tcp_hdr_print((gnrc_tcp_hdr_t *) tcp_snp->data);
    LL_SEARCH_SCALAR(pkt, ip6_snp, type, GNRC_NETTYPE_IPV6);
    ipv6_hdr_print((ipv6_hdr_t *) ip6_snp->data);

    /* Send Message down the network stack */
    sendto = gnrc_netreg_lookup(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL);
    if(sendto == NULL){
        DEBUG("tcp: unable to forward packet, network layer not found\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* Increase users for this pkt. */
    gnrc_pktbuf_hold(pkt, gnrc_netreg_num(GNRC_NETTYPE_TCP, GNRC_NETREG_DEMUX_CTX_ALL));
    while(sendto){
        gnrc_netapi_send(sendto->pid, pkt);
        sendto = gnrc_netreg_getnext(sendto);
    }
}

static void _tcp_receive(gnrc_pktsnip_t *pkt)
{
    /* NOTE: Receiving direction: pkt = payload, payload->next = tcp, tcp->next = nw */
    uint16_t ctl = 0;
    uint32_t context = 0;
    gnrc_pktsnip_t* tcp_snp = NULL;
    gnrc_tcp_hdr_t* tcp_hdr = NULL;
    gnrc_netreg_entry_t* sendto = NULL;

    /* Search for tcp header */
    LL_SEARCH_SCALAR(pkt, tcp_snp, type, GNRC_NETTYPE_TCP);
    if(tcp_snp == NULL ){
        DEBUG("tcp: received packet without tcp header, discarding it.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* We found a tcp header */
    tcp_hdr = (gnrc_tcp_hdr_t*) tcp_snp->data;
    ctl = byteorder_ntohs(tcp_hdr->off_ctl);

    /* Validate Checksum */
    if( byteorder_ntohs(tcp_hdr->checksum) != _calc_csum(tcp_snp, tcp_snp->next, pkt)){
        DEBUG("tcp: received packet with invalid checksum, discarding it.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* Validate Offset */
    if(GET_OFFSET(ctl) < OFFSET_BASE ){
        DEBUG("tcp: received packet with offset < 5, discarding it\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* If only SYN is set, move it to contexts that wait for a initial communication */
    /* If not, send it to fitting context(context = srcport << 16 & dstport) */
    if((ctl & MSK_SYN_ACK) == MSK_SYN){
        context = _build_context(PORT_UNSPEC, byteorder_ntohs(tcp_hdr->dst_port));
    } else {
        context = _build_context(byteorder_ntohs(tcp_hdr->src_port), byteorder_ntohs(tcp_hdr->dst_port));
    }

    /* Lookup pids associated with this context */
    sendto = gnrc_netreg_lookup(GNRC_NETTYPE_TCP, context);
    if(sendto){
        /* Call every affected FSM with RCVD_PKT event.*/
        while(sendto){
            /* NOTE: Assumption, one thread holds only one tcb. Might be problematic!*/
            _fsm(_search_tcb_of_owner(sendto->pid), pkt, RCVD_PKT);
            sendto = gnrc_netreg_getnext(sendto);
        }
    }else{
        /* Nobody cares about this context. Reply with RST*/
        DEBUG("tcp: unable to forward packet as no one is interested in it.\n");
        if((ctl & MSK_RST) != MSK_RST){
            gnrc_netapi_send(_tcp_pid, _tcp_build_reset(pkt));
        }
        
    }
    /* Release received Paket */
    gnrc_pktbuf_release(pkt);
}

static void* _tcp_event_loop(__attribute__((unused))void *arg)
{
    msg_t msg;
    msg_t reply;
    msg_t msg_queue[GNRC_TCP_MSG_QUEUE_SIZE];

    /* setup reply message */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t)-ENOTSUP;

    /* Save own PID */
    _tcp_pid = thread_getpid();

    /* Init message queue*/
    msg_init_queue(msg_queue, GNRC_TCP_MSG_QUEUE_SIZE);

    /* Register GNRC_tcp in netreg */
    gnrc_netreg_entry_t entry;
    entry.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    entry.pid = _tcp_pid;
    gnrc_netreg_register(GNRC_NETTYPE_TCP, &entry);

    /* dispatch NETAPI Messages */
    while(1){
        msg_receive(&msg);

        switch (msg.type) {
            /* Pass Message up the network stack */
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("_tcp_thread(): GNRC_NETAPI_MSG_TYPE_RCV\n");
                _tcp_receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Pass Message down the network stack */
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("_tcp_thread(): GNRC_NETAPI_MSG_TYPE_SND\n");
                _tcp_send((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Option Set and Get Messages*/
            case GNRC_NETAPI_MSG_TYPE_SET:
            case GNRC_NETAPI_MSG_TYPE_GET:
                DEBUG("_tcp_thread(): GNRC_NETAPI_MSG_TYPE_SET or _GET\n");
                msg_reply(&msg, &reply);
                break;

            default:
                DEBUG("_tcp_thread(): received unidentified message\n");
        }
    }

    /* Never reached */
    return NULL;
}
