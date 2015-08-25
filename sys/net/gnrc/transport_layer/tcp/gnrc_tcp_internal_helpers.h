/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* TODO: Currently Unused: Maybe removable in final version */
//#if ENABLE_DEBUG

typedef union {
    struct __attribute__((packed)) {
        uint16_t fin         : 1;
        uint16_t syn         : 1;
        uint16_t rst         : 1;
        uint16_t psh         : 1;
        uint16_t ack         : 1;
        uint16_t urg         : 1;
        uint16_t reserved    : 6;
        uint16_t data_offset : 4;
    }as_bits;
    uint16_t as_uint;
} off_ctl_t;

/*
static void _dump_tcb (const GNRC_tcp_tcb_t *tcb)
{
    char bufAddr[tcb->peer_addr_len];

    printf("+--------------------------------\n");
    printf("| Dump Transmission Control Block\n");
    printf("+--------------------------------\n");
    switch(tcb->state){
    case CLOSED:      printf("| state = CLOSED\n");      break;
    case LISTEN:      printf("| state = LISTEN\n");      break;
    case SYN_SENT:    printf("| state = SYN_SENT\n");    break;
    case SYN_RCVD:    printf("| state = SYN_RCVD\n");    break;
    case ESTABLISHED: printf("| state = ESTABLISHED\n"); break;
    case CLOSE_WAIT:  printf("| state = CLOSE_WAIT\n");  break;
    case LAST_ACK:    printf("| state = LAST_ACK\n");    break;
    case FIN_WAIT_1:  printf("| state = FIN_WAIT_1\n");  break;
    case FIN_WAIT_2:  printf("| state = FIN_WAIT_2\n");  break;
    case CLOSING:     printf("| state = CLOSING\n");     break;
    case TIME_WAIT:   printf("| state = TIME_WAIT\n");   break;
    }
#if MODULE_GNRC_IPV6
    GNRC_ipv6_addr_to_str(bufAddr, (GNRC_ipv6_addr_t *)tcb->peer_addr, tcb->peer_addr_len);
    printf("| peer_addr(as string) = %s\n", bufAddr);
    printf("| peer_addr(as pointer) = %p\n", tcb->peer_addr);
    printf("| peer_addr_len = %u\n", tcb->peer_addr_len);
#endif
    printf("| peer_port = %u\n",tcb->peer_port);
    printf("| local_port = %u\n", tcb->local_port);
    printf("| snd_una = %lu\n", tcb->snd_una);
    printf("| snd_nxt = %lu\n", tcb->snd_nxt);
    printf("| snd_wnd = %u\n", tcb->snd_wnd);
    printf("| snd_ur = %u\n", tcb->snd_ur);
    printf("| snd_wl1 = %lu\n", tcb->snd_wl1);
    printf("| snd_wl2 = %lu\n", tcb->snd_wl2);
    printf("| iss = %lu\n", tcb->iss);
    printf("| rcv_nxt = %lu\n", tcb->rcv_nxt);
    printf("| rcv_wnd = %u\n", tcb->rcv_wnd);
    printf("| irs = %lu\n", tcb->irs);
    printf("| mss = %u\n", tcb->mss);
    printf("| netreg_entry.pid = %u\n", tcb->netreg_entry.pid);
    printf("| netreg_entry.demux_ctx = %lu\n", tcb->netreg_entry.demux_ctx);
    printf("| netreg_entry.next = %p\n", (void *)tcb->netreg_entry.next);
    printf("+--------------------------------\n\n");
}
*/

/**
 * TODO: Make Optional, should not be in working release except for Debug reasons.
 */
void gnrc_tcp_hdr_print (const gnrc_tcp_hdr_t *hdr)
{

    off_ctl_t ctl;
    ctl.as_uint = byteorder_ntohs(hdr->off_ctl);
    int nopts = ctl.as_bits.data_offset - OFFSET_BASE;

    printf("+--------------------------------\n");
    printf("| Dump NG TCP Header\n");
    printf("+--------------------------------\n");
    printf("| src_port = %u\n", byteorder_ntohs(hdr->src_port));
    printf("| dst_port = %u\n", byteorder_ntohs(hdr->dst_port));
//    printf("| seq_num = %lu\n", byteorder_ntohl(hdr->seq_num));
//    printf("| ack_num = %lu\n", byteorder_ntohl(hdr->ack_num));
    printf("| off_ctl = %u\n", byteorder_ntohs(hdr->off_ctl));
    printf("|\n");
    printf("| Offset and Control Bits detailed\n");
    printf("|     off.ctl.data_offset = %u\n", ctl.as_bits.data_offset);
    printf("|     off.ctl.reserved = %u\n", ctl.as_bits.reserved);
    printf("|     off.ctl.urg = %u\n", ctl.as_bits.urg);
    printf("|     off.ctl.ack = %u\n", ctl.as_bits.ack);
    printf("|     off.ctl.psh = %u\n", ctl.as_bits.psh);
    printf("|     off.ctl.rst = %u\n", ctl.as_bits.rst);
    printf("|     off.ctl.syn = %u\n", ctl.as_bits.syn);
    printf("|     off.ctl.fin = %u\n", ctl.as_bits.fin);
    printf("|\n");
    printf("| window = %u\n", byteorder_ntohs(hdr->window));
    printf("| checksum = %u\n", byteorder_ntohs(hdr->checksum));
    printf("| urgent_ptr = %u\n", byteorder_ntohs(hdr->urgent_ptr));

    for(int i=0; i<nopts; i++){
//        printf("| options[%0d] = %lu\n", i, byteorder_ntohl(hdr->options[i]));
    }
    printf("+--------------------------------\n\n");
}
//#endif

static gnrc_tcp_hdr_t* _get_tcp_hdr(gnrc_pktsnip_t* pkt)
{
    gnrc_pktsnip_t* snp = NULL;
    LL_SEARCH_SCALAR(pkt, snp, type, GNRC_NETTYPE_TCP);
    return (gnrc_tcp_hdr_t*) snp->data;
}

/* TODO: Rewrite */
/**
 * @brief Build and allocate a tcp paket in paketbuffer, tcb stores pointer to new paket.
 *
 * @param[in/out] tcb       transmission control block that contains segment information
 * @param[in] ctl           control bits to set in pkt
 * @param[in] seq_num       sequence number to use in new paket
 * @param[in] ack_num       acknowledgment number to use in new paket
 * @param[in] payload       pointer to payload buffer
 * @param[in] payload_len   payload size
 *
 * @return                  Zero on success
 * @return                  -ENOMEM if pktbuf is full.
 */
static int8_t _build_pkt(gnrc_tcp_tcb_t *tcb,
                         const uint16_t ctl,
                         const uint32_t seq_num,
                         const uint32_t ack_num,
                         uint8_t *payload,
                         const size_t payload_len)
{
    gnrc_pktsnip_t *pay_snp = NULL;
    gnrc_pktsnip_t *tcp_snp = NULL;
    gnrc_pktsnip_t *ip6_snp = NULL;
    gnrc_tcp_hdr_t  tcp_hdr;
    uint8_t nopts = 0;

    /* Add payload, if supplied */
    if(payload != NULL && payload_len > 0){
        pay_snp = gnrc_pktbuf_add(pay_snp, payload, payload_len, GNRC_NETTYPE_UNDEF);
        if(pay_snp == NULL){
            DEBUG("tcp: Can't allocate buffer for Payload Header\n.");
            return -ENOMEM;
        }
    }

    /* Build tcp-header */
    tcp_hdr.src_port = byteorder_htons(tcb->local_port);
    tcp_hdr.dst_port = byteorder_htons(tcb->peer_port);
    tcp_hdr.checksum = byteorder_htons(0);
    tcp_hdr.seq_num = byteorder_htonl(seq_num);
    tcp_hdr.ack_num = byteorder_htonl(ack_num);
    tcp_hdr.window = byteorder_htons(tcb->rcv_wnd);
    tcp_hdr.urgent_ptr = byteorder_htons(0);

    /* tcp option handling */
    /* If this is a syn-message, send mss option */
    if(ctl & MSK_SYN){
        /* NOTE: "path MTU Discorvery" usable in the future? MSS Depends on Link-Layer */
        /* TODO: lookup via IP-Layer */
        tcp_hdr.options[nopts] = byteorder_htonl(OPT_MSS(GNRC_TCP_DEFAULT_MSS));
        nopts += 1;
    }
    tcp_hdr.off_ctl = byteorder_htons( ((OFFSET_BASE + nopts) << 12) | ctl);

    /* allocate tcp header */
    tcp_snp = gnrc_pktbuf_add(pay_snp, &tcp_hdr, (OFFSET_BASE + nopts) * 4, GNRC_NETTYPE_TCP);
    if(tcp_snp == NULL){
        DEBUG("tcp: Can't allocate buffer for TCP Header\n.");
        gnrc_pktbuf_release(pay_snp);
        return -ENOMEM;
    }

    /* Build network layer header */
#ifdef MODULE_GNRC_IPV6
    ip6_snp = gnrc_ipv6_hdr_build(tcp_snp, NULL, 0, tcb->peer_addr, sizeof(ipv6_addr_t)); // TODO don't send the tcb->peer_addr?
    if(ip6_snp == NULL){
        DEBUG("tcp: Can't allocate buffer for IPv6 Header.\n");
        gnrc_pktbuf_release(tcp_snp);
        return -ENOMEM;
    }
#endif

    /* Assign Pointers to tcb */
    tcb->cur_pkt = ip6_snp;
    tcb->cur_tcp_hdr = (gnrc_tcp_hdr_t *) tcp_snp->data;
    tcb->cur_seg_len = payload_len;
    return 0;
}

/**
 * @brief Builds a context for multiplexing(combines peer and local port to a context)
 *
 * @param[in] peer_port    peer portnumber
 * @param[in] local_port   local portnumber
 *
 * @return    context generated from parameters
 */
static inline uint32_t _build_context(const uint16_t peer_port, const uint16_t local_port)
{
    return (peer_port << 16) | local_port;
}

static inline bool _is_context_unique(const uint16_t peer_port, const uint16_t local_port)
{
    if( gnrc_netreg_num(GNRC_NETTYPE_TCP, _build_context(peer_port, local_port)) == 0 ){
        return true;
    }
    return false;
}

/**
 * @brief Registers a thread on in a multiplexing context.
 *
 * The context is evaluated by the portnumbers in a given tcb
 *
 * @param[in] tcb   tcb used to evaluate context.
 *
 * @return          zero on success
 */
static int8_t _demux_register(gnrc_tcp_tcb_t *tcb)
{
    tcb->netreg_entry.demux_ctx = _build_context(tcb->peer_port, tcb->local_port);
    tcb->netreg_entry.pid = tcb->owner;
    gnrc_netreg_register(GNRC_NETTYPE_TCP, &tcb->netreg_entry);
    return 0;
}

/**
 * @brief Removes thread from a multiplexing context.
 *
 * The context is evaluated by the portnumbers in a given tcb
 *
 * @param[in] tcb   tcb used to evaluate context.
 *
 * @return          zero on success
 */
static int8_t _demux_remove(gnrc_tcp_tcb_t *tcb)
{
    tcb->netreg_entry.demux_ctx = _build_context(tcb->peer_port, tcb->local_port);
    tcb->netreg_entry.pid = tcb->owner;
    gnrc_netreg_unregister(GNRC_NETTYPE_TCP, &tcb->netreg_entry);
    return 0;
}

/**
 * @brief Changes multiplexing context to new context.
 *
 * The old context is specified by a given tcb. The new context is stored in tcb
 * after this call.
 *
 * @param[in/out] tcb              tcb used to evaluate current context
 * @param[in]     new_peer_port    new_peer_port to store in tcb
 * @param[in]     new_local_port   new_local_port to store in tcb
 *
 * @return                         zero on success
 */
static inline int8_t _demux_change_context(gnrc_tcp_tcb_t *tcb,
                                    uint16_t new_peer_port,
                                    uint16_t new_local_port)
{
    _demux_remove(tcb);
    tcb->peer_port = new_peer_port;
    tcb->local_port = new_local_port;
    _demux_register(tcb);
    return 0;
}

/**
 * @brief Search for an unused Portnumber between Port 1024 and Port 65535.
 *        Those Ports are not controlled by the IANA. They are free to use.
 *
 * @param[in] dst_port   TODO
 *
 * @return    currently unused Port Number in [1024, 65535]
 */
static uint16_t _get_free_port(const uint8_t dst_port)
{
        uint16_t rnd_val = (genrand_uint32() >> 16);
        while(  rnd_val < BEG_REG_PORTS
             || gnrc_netreg_num(GNRC_NETTYPE_TCP, _build_context(rnd_val, dst_port)) != 0
             )
        {
            rnd_val = (genrand_uint32() >> 16);
        }
        return rnd_val;
}

/**
 * @brief Calculates checksum over payload, tcp-header and network layer header
 *
 * @param[in] hdr          gnrc_pktsnip_t to tcp-header
 * @param[in] pseudo_hdr   gnrc_pktsnip_t to network layer header
 * @param[in] payload      gnrc_pktsnip_t to payload
 *
 * @return                 non zero checksum if given network layer is supported
 * @return                 zero if given network layer is not supported
 */
static uint16_t _calc_csum(const gnrc_pktsnip_t *hdr,
                           const gnrc_pktsnip_t *pseudo_hdr,
                           const gnrc_pktsnip_t *payload)
{
    uint16_t csum = 0;
    uint16_t len = (uint16_t) hdr->size;

    /* Process payload */
    while(payload && payload != hdr){
        csum = inet_csum(csum, (uint8_t *)payload->data, payload->size);
        len += (uint16_t)payload->size;
        payload = payload->next;
    }

    /* Process tcp-header, before checksum field(Byte 16 to 18) */
    csum = inet_csum(csum, (uint8_t *)hdr->data, 16);
    /* Process tcp-header, after checksum field */
    csum = inet_csum(csum, ((uint8_t *)hdr->data) + 18, hdr->size - 18);

    /* Process Network layer Header */
    switch (pseudo_hdr->type) {
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            csum = ipv6_hdr_inet_csum(csum, pseudo_hdr->data, PROTNUM_TCP, len);
            break;
#endif
        default:
            return 0;
    }
    return ~csum;
}

/**
 * @brief Parses options of a given tcp-header pktsnip.
 *
 * @param[out] tcb   transmission control block to memorize options.
 * @param[in]  hdr   tcp header to be checked
 *
.* @return           zero on success
 * @return           a negative value on error
 */
static int8_t _parse_options(gnrc_tcp_tcb_t *tcb, gnrc_tcp_hdr_t *hdr)
{
    uint8_t word_idx = 0;
    uint8_t byte_idx = 0;
    uint8_t word_end = 0;
    uint32_t word = 0;
    uint16_t off_ctl = byteorder_ntohs(hdr->off_ctl);

    word_end = GET_OFFSET(off_ctl) - OFFSET_BASE;

    /* There are existing options */
    while(word_idx < word_end){
        /* Read current word from options */
        word = byteorder_ntohl(hdr->options[word_idx]);

        /* If byte index is not aligned to word index. Fill word with bytes from next word. */
        if(byte_idx){
            word >>= (byte_idx * 8);
            word |= (byteorder_ntohl(hdr->options[word_idx+1]) << ((sizeof(word)-byte_idx)*8));
        }

        /* Option handling */
        switch(OPT_GET_KIND(word)){
            case OPT_KIND_EOL:
                /* Option: End of List */
                DEBUG("tcp: Option eol\n");
                return 0;

            case OPT_KIND_NOP:
                /* Option: No operation */
                byte_idx += 1;
                DEBUG("tcp: Option nop\n");
                break;

            case OPT_KIND_MSS:
                /* Option: Maximum Segment Size */
                DEBUG("tcp: Option mss\n");
                if(OPT_GET_LENGTH(word) == OPT_LENGTH_MSS){
                    tcb->mss = OPT_GET_VAL_2B(word);
                    byte_idx += 4;
                }else{
                    DEBUG("tcp: invalid MSS Option length.\n");
                    return -1;
                }
                break;

                /* Add options support HERE */
            default:
                DEBUG("tcp: Unsupported option received\n");
                byte_idx += 1;
        }

        /* Update index */
        if(byte_idx >= 4){
            word_idx += 1;
            byte_idx -= 4;
        }
    }
    return 0;
}

/* TODO: Make this overflow-safe */
static bool _is_seq_num_acceptable(gnrc_tcp_tcb_t *tcb, uint32_t snum, uint32_t slen)
{
    uint32_t rn = tcb->rcv_nxt;
    uint32_t rw = tcb->rcv_wnd;
    uint32_t sl = snum + slen -1;

    /* Possible case 1 */
    if(slen == 0 && rw == 0 && rn == snum){
        return true;
    }

    /* Possible case 2 */
    if(slen == 0 && rw > 0 && rn <= snum && snum < (rn + rw)){
        return true;
    }

    /* Possible case 3 */
    if(slen > 0 && rw > 0 && ((rn <= snum && snum < rn + rw) || (rn <= sl && sl < (rn + rw)))){
        return true;
    }

    /* Everthing else is not acceptable */
    return false;
}

static uint32_t _get_seg_len(gnrc_pktsnip_t *pkt)
{
    uint32_t seg_len = 0;
    gnrc_pktsnip_t *snp = NULL;
    LL_SEARCH_SCALAR(pkt, snp, type, GNRC_NETTYPE_UNDEF);
    while(snp && snp->type == GNRC_NETTYPE_UNDEF){
        seg_len += snp->size;
        snp = snp->next;
    }
    return seg_len;
}

/* TCB-Linked-List Handling Functions */

static void _add_tcb_to_list(gnrc_tcp_tcb_t *tcb)
{
    gnrc_tcp_tcb_t** ptr = &_head_tcb_list;
    while(*ptr && *ptr != tcb){
        ptr = (gnrc_tcp_tcb_t **) &((*ptr)->next);
    }
    *ptr = tcb;
}

static void _rem_tcb_from_list(gnrc_tcp_tcb_t *tcb)
{
    gnrc_tcp_tcb_t** ptr = &_head_tcb_list;
    while(*ptr && *ptr != tcb){
        ptr = (gnrc_tcp_tcb_t **) &((*ptr)->next);
    }
    if(*ptr != NULL){
        *ptr = (gnrc_tcp_tcb_t *) tcb->next;
        tcb->next = NULL;
    }
}

static gnrc_tcp_tcb_t* _search_tcb_of_owner(kernel_pid_t owner)
{
    gnrc_tcp_tcb_t* ptr = _head_tcb_list;
    while(ptr && ptr->owner != owner){
        ptr = (gnrc_tcp_tcb_t *) ptr->next;
    }
    return ptr;
}

/*
static void _dump_ret_queue(GNRC_tcp_tcb_t* tcb)
{
    printf("Current Buffer Size: %d\n", tcb->ret_size);
    for(int i=0; i < GNRC_TCP_RETRANSMIT_QUEUE_SIZE; ++i){
        printf("%d: pkt: %lu, no: %d\n",i ,(uint32_t) tcb->ret_queue[i].pkt, tcb->ret_queue[i].no_of_retries);
    }
}
*/

static void _clear_retransmit_queue(gnrc_tcp_tcb_t* tcb){
    for(uint8_t i=0; i < tcb->ret_size; i++){
        gnrc_pktbuf_release(tcb->ret_queue[i].pkt);
    }
    tcb->ret_size = 0;
}

static void _send_pkt(gnrc_tcp_tcb_t* tcb, bool retransmit)
{
    msg_t msg;

    /* If this is a retransmit, transmit queue head */
    if(retransmit){
        tcb->cur_pkt = tcb->ret_queue[0].pkt;
        tcb->ret_queue[0].no_of_retries += 1;

        /* Calculate new timer: Todo RTT */
        tcb->ret_tout.seconds = 2;
        gnrc_pktbuf_hold(tcb->cur_pkt, gnrc_netreg_num(tcb->cur_pkt->type, GNRC_NETREG_DEMUX_CTX_ALL));

        /* signal user */
        msg_send(&msg, tcb->owner);

    /* Add Paket to Queue if it contains data, or a SYN Flag*/
    }else if(tcb->cur_seg_len > 0 || byteorder_ntohs(tcb->cur_tcp_hdr->off_ctl) & MSK_SYN){
        /* If queue is full -> Release pkt*/
        if(tcb->ret_size >= GNRC_TCP_RETRANSMIT_QUEUE_SIZE){
        	gnrc_pktbuf_release(tcb->cur_pkt);
            DEBUG("_send_pkt: Retransmit Queue is full, release pkt");
            return;
        }
        /* Increase users to make pkt persistent, add it to queue, update size */
        tcb->ret_queue[tcb->ret_size].pkt = tcb->cur_pkt;
        tcb->ret_queue[tcb->ret_size].no_of_retries = 0;
        gnrc_pktbuf_hold(tcb->cur_pkt, gnrc_netreg_num(tcb->cur_pkt->type, GNRC_NETREG_DEMUX_CTX_ALL));
        tcb->ret_size += 1;

        /* Calculate new timer */
        tcb->ret_tout.seconds = 2;

        /* signal user */
        msg_send(&msg, tcb->owner);
    }

    /* Send pkt */
    gnrc_netapi_send(_tcp_pid, tcb->cur_pkt);
}

static void _ack_pkt(gnrc_tcp_tcb_t* tcb, uint32_t ack)
{
    msg_t msg;
    gnrc_pktsnip_t* pkt = NULL;
    gnrc_tcp_queue_entry_t tmp;
    uint32_t seg = 0;

    /* If queue is empty there is nothing to ack */
    if(tcb->ret_size <= 0){
        DEBUG("Queue is empty, this could be an Error");
        return;
    }

    /* For queue size, check if pakets are acked */
    for(uint8_t i=0; i<tcb->ret_size; i++){
        pkt = tcb->ret_queue[i].pkt;
        seg = byteorder_ntohl(_get_tcp_hdr(pkt)->seq_num) + _get_seg_len(pkt);

        /*if seq_no + seg_size < ack -> pkt is acked, remove it from queue*/
        if(seg < ack){
        	gnrc_pktbuf_release(pkt);

            // Swap entries
            for(uint8_t j=i; j<tcb->ret_size-1; ++j){
                tmp = tcb->ret_queue[j];
                tcb->ret_queue[j] = tcb->ret_queue[j+1];
                tcb->ret_queue[j+1] = tmp;
            }

            tcb->ret_size -= 1;
        }
    }

    /* Remove timeout, if retransmit queue is empty */
    if(tcb->ret_size <= 0){
        tcb->ret_tout.seconds = 0;
        tcb->ret_tout.microseconds = 0;
    }

    /* Signal User */
    msg_send(&msg, tcb->owner);
}
