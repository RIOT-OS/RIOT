#include <stdio.h>
#include "net/gnrc/pktbuf.h"
#include "tcp_internal/option.h"
#include "tcp_internal/helper.h"

void _clear_retransmit(gnrc_tcp_tcb_t* tcb){
    if(tcb->pkt_retransmit != NULL){
        gnrc_pktbuf_release(tcb->pkt_retransmit);
        xtimer_remove(&(tcb->timer_timeout));
        tcb->pkt_retransmit = NULL;
    }
}

void _gnrc_tcp_tcb_print (const gnrc_tcp_tcb_t *tcb){

    printf("+--------------------------------\n");
    printf("| Dump Transmission Conrol Block \n");
    printf("+--------------------------------\n");

    switch(tcb->state){
    case CLOSED: printf("| state: CLOSED\n");  break;
    default:     printf("| state: UNKNOWN\n"); break;
    }
    printf("| peer_addr: \n");
    printf("| peer_addr_len: \n");
    printf("| peer_port: \n");
    printf("| local_port: \n");
    printf("| snd_una: %"PRIu32"\n",tcb->snd_una);
    printf("| snd_nxt: %"PRIu32"\n",tcb->snd_nxt);
    printf("| snd_wnd: %"PRIu16"\n",tcb->snd_wnd);
    printf("+--------------------------------\n\n");
}

void _gnrc_tcp_hdr_print (const tcp_hdr_t *hdr)
{
    uint16_t control = byteorder_ntohs(hdr->off_ctl);
    uint16_t nopts = ((control & MSK_OFFSET) >> 12) - OPTION_OFFSET_BASE;

    printf("+--------------------------------\n");
    printf("| Dump TCP Header\n");
    printf("+--------------------------------\n");
    printf("| src_port:   %"PRIu16"\n", byteorder_ntohs(hdr->src_port));
    printf("| dst_port:   %"PRIu16"\n", byteorder_ntohs(hdr->dst_port));
    printf("| seq_num:    %"PRIu32"\n", byteorder_ntohl(hdr->seq_num));
    printf("| ack_num:    %"PRIu32"\n", byteorder_ntohl(hdr->ack_num));
    printf("| off_ctl:    %"PRIu16"\n", control);
    printf("| window:     %"PRIu16"\n", byteorder_ntohs(hdr->window));
    printf("| checksum:   %"PRIu16"\n", byteorder_ntohs(hdr->checksum));
    printf("| urgent:     %"PRIu16"\n", byteorder_ntohs(hdr->urgent_ptr));
    for(int i=0; i<nopts; i++){
        printf("| options[%1d]: 0x%"PRIx32"\n", i, byteorder_ntohl(hdr->options[i]));
    }
    printf("|\n");
    printf("| Offset and Control Bits detailed:\n");
    printf("|     offset: %"PRIu8"\n", (control & MSK_OFFSET) >> 12);
    printf("|     urg:    %"PRIu8"\n", (control & MSK_URG) >> 5);
    printf("|     ack:    %"PRIu8"\n", (control & MSK_ACK) >> 4);
    printf("|     psh:    %"PRIu8"\n", (control & MSK_PSH) >> 3);
    printf("|     rst:    %"PRIu8"\n", (control & MSK_RST) >> 2);
    printf("|     syn:    %"PRIu8"\n", (control & MSK_SYN) >> 1);
    printf("|     fin:    %"PRIu8"\n", (control & MSK_FIN));
    printf("+--------------------------------\n\n");

}
