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
    gnrc_pktsnip_t *tcp = NULL;

    /* Search for tcp header */
    LL_SEARCH_SCALAR(pkt, tcp, type, GNRC_NETTYPE_TCP);
    if(tcp == NULL ){
        DEBUG("tcp: cannot send packet: tcp header missing.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* Dispatch to network layer */
    if(!gnrc_netapi_dispatch_send(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)){
        DEBUG("tcp: cannot send packet: network layer not found\n");
        gnrc_pktbuf_release(pkt);
    }
}

void _receive(gnrc_pktsnip_t *pkt)
{
    /* NOTE: Receiving direction: pkt = payload, payload->next = tcp, tcp->next = nw */
    uint16_t ctl = 0;
    uint32_t context = 0;
    uint8_t hdr_size = 0;

    gnrc_pktsnip_t *tcp, *ipv6;
    tcp_hdr_t *hdr;

    gnrc_tcp_tcb_t *tcb = NULL;
    gnrc_netreg_entry_t* sendto = NULL;

    /* mark TCP header */
    tcp = gnrc_pktbuf_start_write(pkt);
    if(tcp == NULL){
        DEBUG("tcp: unable to get write access to packet\n");
        gnrc_pktbuf_release(pkt);
    }
    pkt = tcp;

    /* Get IP Header */
    LL_SEARCH_SCALAR(pkt, ipv6, type, GNRC_NETTYPE_IPV6);
    assert(ipv6 != NULL);

    /* Get TCP Header */
    LL_SEARCH_SCALAR(pkt, tcp, type, GNRC_NETTYPE_TCP);
    assert(tcp != NULL);

    /* Extract control bits */
    ctl = byteorder_ntohs(((tcp_hdr_t*) tcp->data)->off_ctl);

    /* Validate Offset */
    if(GET_OFFSET(ctl) < OPTION_OFFSET_BASE ){
        DEBUG("tcp: received packet with offset < 5, discarding it\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* Calculate tcp header size */
    hdr_size = GET_OFFSET(ctl) * 4;

    /* Mark TCP-Header, if it contains any payload */
    if((pkt->type == GNRC_NETTYPE_TCP) && (pkt->size != hdr_size)){
        tcp = gnrc_pktbuf_mark(pkt, hdr_size, GNRC_NETTYPE_TCP);
        if(tcp == NULL){
            DEBUG("tcp: error marking TCP header, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt->type = GNRC_NETTYPE_UNDEF;
    }

    hdr = (tcp_hdr_t *)tcp->data;

    /* Validate Checksum */
    if( byteorder_ntohs(hdr->checksum) != _pkt_calc_csum(tcp, ipv6, pkt)){
        DEBUG("tcp: received packet with invalid checksum, dropping it.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    /* If only SYN is set, move it to contexts that wait for a initial communication */
    /* If not, send it to fitting context(context = srcport << 16 & dstport) */
    if((ctl & MSK_SYN_ACK) == MSK_SYN){
        context = _demux_build_context(PORT_UNSPEC, byteorder_ntohs(hdr->dst_port));
    } else {
        context = _demux_build_context(byteorder_ntohs(hdr->src_port), byteorder_ntohs(hdr->dst_port));
    }

    /* Lookup pids associated with this context */
    sendto = gnrc_netreg_lookup(GNRC_NETTYPE_TCP, context);
    if(sendto){
        /* Call received packet affects a FSM with RCVD_PKT event.*/
        while(sendto){
            /* NOTE: Search for associated tcb, call FSM with received packet event. */
            LL_SEARCH_SCALAR(_head_list_tcb, tcb, owner, sendto->pid);
            _fsm(tcb, RCVD_PKT, pkt, NULL, 0);
            sendto = gnrc_netreg_getnext(sendto);

        }
    }else{
        /* Nobody cares about this context. Reply with Reset Packet*/
        DEBUG("tcp: unable to forward packet as no one is interested in it.\n");
        if((ctl & MSK_RST) != MSK_RST){
            _pkt_build_reset_from_pkt(&tcp, pkt);
            gnrc_netapi_send(_tcp_pid, tcp);
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
                _fsm((gnrc_tcp_tcb_t *)msg.content.ptr, TIMEOUT_RETRANSMIT, NULL, NULL, 0);
                break;

            /* Time Wait Timer expired -> Call FSM with timewait event */
            case MSG_TYPE_TIMEWAIT:
                DEBUG("tcp: MSG_TYPE_TIMEWAIT\n");
                _fsm((gnrc_tcp_tcb_t *)msg.content.ptr, TIMEOUT_TIMEWAIT, NULL, NULL, 0);
                break;

            default:
                DEBUG("tcp: received unidentified message\n");
        }
    }
    /* never reached */
    return NULL;
}
