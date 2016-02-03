#include <utlist.h>
#include "net/gnrc/pkt.h"
#include "net/gnrc/tcp.h"
#include "net/gnrc/tcp/hdr.h"
#include "net/gnrc/tcp/tcb.h"
#include "tcp_internal/pkt.h"
#include "tcp_internal/fsm.h"
#include "tcp_internal/helper.h"
#include "tcp_internal/demux.h"
#include "tcp_internal/option.h"
#include "tcp_internal/eventloop.h"

#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

void _send(gnrc_pktsnip_t *pkt)
{
    /* NOTE: Sending Direction: pkt = nw, nw->next = tcp, tcp->next = payload */
    gnrc_pktsnip_t* tcp_snp = NULL;
    gnrc_netreg_entry_t* sendto = NULL;

    /* Search for tcp header */
    LL_SEARCH_SCALAR(pkt, tcp_snp, type, GNRC_NETTYPE_TCP);
    if(tcp_snp == NULL ){
        DEBUG("tcp: unable to send packet without tcp header, discarding it.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

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

void _receive(gnrc_pktsnip_t *pkt)
{
    /* NOTE: Receiving direction: pkt = payload, payload->next = tcp, tcp->next = nw */
    uint16_t ctl = 0;
    uint32_t context = 0;

    gnrc_pktsnip_t* tcp_snp = NULL;
    tcp_hdr_t* tcp_hdr = NULL;
    gnrc_tcp_tcb_t *tcb = NULL;
    gnrc_netreg_entry_t* sendto = NULL;

    /* Search for tcp header */
    LL_SEARCH_SCALAR(pkt, tcp_snp, type, GNRC_NETTYPE_TCP);
    if(tcp_snp == NULL ){
        DEBUG("tcp: received packet without tcp header, discarding it.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }else{
        tcp_hdr = (tcp_hdr_t*) tcp_snp->data;
    }

    /* Validate Checksum */
    if( byteorder_ntohs(tcp_hdr->checksum) != _pkt_calc_csum(tcp_snp, tcp_snp->next, pkt)){
        DEBUG("tcp: received packet with invalid checksum, discarding it.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* Exctract Control Bits and Offset */
    ctl = byteorder_ntohs(tcp_hdr->off_ctl);

    /* Validate Offset */
    if(GET_OFFSET(ctl) < OPTION_OFFSET_BASE ){
        DEBUG("tcp: received packet with offset < 5, discarding it\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* If only SYN is set, move it to contexts that wait for a initial communication */
    /* If not, send it to fitting context(context = srcport << 16 & dstport) */
    if((ctl & MSK_SYN_ACK) == MSK_SYN){
        context = _demux_build_context(PORT_UNSPEC, byteorder_ntohs(tcp_hdr->dst_port));
    } else {
        context = _demux_build_context(byteorder_ntohs(tcp_hdr->src_port), byteorder_ntohs(tcp_hdr->dst_port));
    }

    /* Lookup pids associated with this context */
    sendto = gnrc_netreg_lookup(GNRC_NETTYPE_TCP, context);
    if(sendto){
        /* Call received packet affects a FSM with RCVD_PKT event.*/
        while(sendto){
            /* NOTE: Search for tcb owner, call FSM with received packet event. */
            LL_SEARCH_SCALAR(_head_list_tcb, tcb, owner, sendto->pid);
            _fsm(tcb, pkt, RCVD_PKT);
            sendto = gnrc_netreg_getnext(sendto);
        }
    }else{
        /* Nobody cares about this context. Reply with Reset*/
        DEBUG("tcp: unable to forward packet as no one is interested in it.\n");
        if((ctl & MSK_RST) != MSK_RST){
            _pkt_build_reset_from_pkt(&tcp_snp, pkt);
            gnrc_netapi_send(_tcp_pid, tcp_snp);
        }
    }
    /* Release received Paket */
    gnrc_pktbuf_release(pkt);
}

void *_event_loop(__attribute__((unused))void *arg)
{
    msg_t msg;
    msg_t reply;
    msg_t msg_queue[GNRC_TCP_MSG_QUEUE_SIZE];

    /* Store pid */
    _tcp_pid = thread_getpid();

    /* setup reply message */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t)-ENOTSUP;

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
                DEBUG("tcp: GNRC_NETAPI_MSG_TYPE_RCV\n");
                _receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Pass Message down the network stack */
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("tcp: GNRC_NETAPI_MSG_TYPE_SND\n");
                _send((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            /* Option Set and Get Messages*/
            case GNRC_NETAPI_MSG_TYPE_SET:
            case GNRC_NETAPI_MSG_TYPE_GET:
                msg_reply(&msg, &reply);
                break;

            /* Retransmission Timer expired -> Call FSM with retransmission event */
            case MSG_TYPE_RETRANSMISSION:
                DEBUG("tcp: MSG_TYPE_RETRANSMISSION\n");
                _fsm((gnrc_tcp_tcb_t *)msg.content.ptr, NULL, TIMEOUT_RETRANSMIT);
                break;

            /* Time Wait Timer expired -> Call FSM with timewait event */
            case MSG_TYPE_TIMEWAIT:
                DEBUG("tcp: MSG_TYPE_TIMEWAIT\n");
                _fsm((gnrc_tcp_tcb_t *)msg.content.ptr, NULL, TIMEOUT_TIMEWAIT);
                break;

            default:
                DEBUG("tcp: received unidentified message\n");
        }
    }
    /* never reached */
    return NULL;
}
