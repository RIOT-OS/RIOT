/*
 * Copyright (C) 2016 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_gnrc_ppp
 * @file
 * @brief       Implementation of the PPP protocol
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include <errno.h>

#include "msg.h"
#include "thread.h"
#include "net/gnrc.h"
#include "net/gnrc/netdev2.h"
#include "net/ppptype.h"
#include "net/gnrc/ppp/ppp.h"
#include "net/gnrc/ppp/lcp.h"
#include "net/gnrc/ppp/ipcp.h"
#include "net/gnrc/ppp/fsm.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/ppp/pap.h"
#include "net/hdlc/hdr.h"
#include "net/ppp/hdr.h"
#include <errno.h>
#include <string.h>
#include "byteorder.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif


void send_ppp_event(msg_t *msg, gnrc_ppp_msg_t ppp_msg)
{
    msg->type = GNRC_PPP_MSG_TYPE_EVENT;
    msg->content.value = ppp_msg;
    msg_send(msg, thread_getpid());
}

void send_ppp_event_xtimer(msg_t *msg, xtimer_t *xtimer, gnrc_ppp_msg_t ppp_msg, int timeout)
{
    msg->type = GNRC_PPP_MSG_TYPE_EVENT;
    msg->content.value = ppp_msg;
    xtimer_remove(xtimer);
    xtimer_set_msg(xtimer, timeout, msg, thread_getpid());
}

/* Generate PPP pkt */
gnrc_pktsnip_t *pkt_build(gnrc_nettype_t pkt_type, uint8_t code, uint8_t id, gnrc_pktsnip_t *payload)
{
    ppp_hdr_t ppp_hdr;

    ppp_hdr.code = code;
    ppp_hdr.id = id;

    int payload_length = payload ? payload->size : 0;
    ppp_hdr.length = byteorder_htons(payload_length + sizeof(ppp_hdr_t));

    gnrc_pktsnip_t *ppp_pkt = gnrc_pktbuf_add(payload, (void *) &ppp_hdr, sizeof(ppp_hdr_t), pkt_type);
    return ppp_pkt;
}

gnrc_ppp_target_t _get_target_from_protocol(uint16_t protocol)
{
    switch (protocol) {
        case PPPTYPE_LCP:
            return PROT_LCP;
            break;
        case PPPTYPE_NCP_IPV4:
            return PROT_IPCP;
            break;
        case PPPTYPE_IPV4:
            return PROT_IPV4;
        case PPPTYPE_PAP:
            return PROT_AUTH;
        default:
            DEBUG("gnrc_ppp: Received unknown PPP protocol. Discard.\n");
    }
    return PROT_UNDEF;
}
gnrc_pktsnip_t *retrieve_pkt(netdev2_t *dev)
{
    int nbytes;

    nbytes = dev->driver->recv(dev, NULL, 0, NULL);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, nbytes, GNRC_NETTYPE_UNDEF);
    dev->driver->recv(dev, pkt->data, nbytes, NULL);
    return pkt;
}

int gnrc_ppp_setup(gnrc_netdev2_t *dev, netdev2_t *netdev)
{
    dev->dev = (netdev2_t*) netdev;

	netdev2_ppp_t *pppdev = (netdev2_ppp_t*) netdev;

    dcp_init(dev);
    lcp_init(dev);
    ipcp_init(dev);
    ppp_ipv4_init(dev);
    pap_init(dev);

    trigger_fsm_event((gnrc_ppp_fsm_t *) &pppdev->lcp, PPP_E_OPEN, NULL);
    trigger_fsm_event((gnrc_ppp_fsm_t *) &pppdev->ipcp, PPP_E_OPEN, NULL);
    return 0;
}


int gnrc_ppp_send(gnrc_netdev2_t *dev, gnrc_pktsnip_t *pkt)
{
    hdlc_hdr_t hdlc_hdr;
    netdev2_t *netdev = (netdev2_t*) dev->dev;
	netdev2_ppp_t *pppdev = (netdev2_ppp_t*) netdev;

    hdlc_hdr.address = GNRC_PPP_HDLC_ADDRESS;
    hdlc_hdr.control =GNRC_PPP_HDLC_CONTROL;
    hdlc_hdr.protocol = byteorder_htons(gnrc_nettype_to_ppp_protnum(pkt->type));

    gnrc_pktsnip_t *hdr = gnrc_pktbuf_add(pkt, (void *) &hdlc_hdr, sizeof(hdlc_hdr_t), GNRC_NETTYPE_HDLC);

    if (gnrc_pkt_len(hdr) > pppdev->lcp.peer_mru) {
        DEBUG("gnrc_ppp: Sending exceeds peer MRU. Dropping packet.\n");
        gnrc_pktbuf_release(hdr);
        return -EBADMSG;
    }
    /* Get iovec representation */
    size_t n;
    int res = -ENOBUFS;
    hdr = gnrc_pktbuf_get_iovec(hdr, &n);
    if (hdr != NULL) {
        struct iovec *vector = (struct iovec *) hdr->data;
        res = netdev->driver->send(netdev, vector, n);
    }
    gnrc_pktbuf_release(hdr);
    return res;
}

static int _pkt_is_ppp(gnrc_pktsnip_t *pkt)
{
    return (pkt->type == PPPTYPE_NCP_IPV4 || pkt->type == PPPTYPE_PAP || pkt->type == PPPTYPE_LCP);
}

static int _ppp_pkt_is_valid(gnrc_pktsnip_t *pkt)
{
    ppp_hdr_t *hdr = pkt->data;

    return byteorder_ntohs(hdr->length) < pkt->size;
}

int _prot_is_allowed(gnrc_netdev2_t *dev, uint16_t protocol)
{
	netdev2_ppp_t *pppdev = (netdev2_ppp_t*) dev->dev;
    switch (protocol) {
        case PPPTYPE_LCP:
            return ((gnrc_ppp_protocol_t*) &pppdev->lcp)->state == PROTOCOL_STARTING || ((gnrc_ppp_protocol_t*) &pppdev->lcp)->state == PROTOCOL_UP;
        case PPPTYPE_NCP_IPV4:
            return ((gnrc_ppp_protocol_t*) &pppdev->ipcp)->state == PROTOCOL_STARTING || ((gnrc_ppp_protocol_t*) &pppdev->ipcp)->state == PROTOCOL_UP;
        case PPPTYPE_PAP:
            return ((gnrc_ppp_protocol_t*) &pppdev->pap)->state == PROTOCOL_STARTING;
        case PPPTYPE_IPV4:
            return ((gnrc_ppp_protocol_t*) &pppdev->ipv4)->state == PROTOCOL_UP;
    }
    return 0;
}

gnrc_ppp_protocol_t *_get_prot_by_target(netdev2_ppp_t *pppdev, gnrc_ppp_target_t target)
{
    gnrc_ppp_protocol_t *target_prot;
    switch (target) {
        case PROT_LCP:
            target_prot = (gnrc_ppp_protocol_t *) &pppdev->lcp;
            break;
        case PROT_IPCP:
        case GNRC_PPP_BROADCAST_NCP:
            target_prot = (gnrc_ppp_protocol_t *) &pppdev->ipcp;
            break;
        case PROT_IPV4:
            target_prot = (gnrc_ppp_protocol_t *) &pppdev->ipv4;
            break;
        case PROT_DCP:
        case GNRC_PPP_BROADCAST_LCP:
            target_prot = (gnrc_ppp_protocol_t *) &pppdev->dcp;
            break;
        case PROT_AUTH:
            target_prot = (gnrc_ppp_protocol_t *) &pppdev->pap;
            break;
        default:
            target_prot = NULL;
    }
    return target_prot;
}

gnrc_pktsnip_t *ppp_recv(gnrc_netdev2_t *gnrc_netdev)
{
    gnrc_pktsnip_t *pkt = NULL;
    netdev2_t *netdev = (netdev2_t*) gnrc_netdev->dev;
	netdev2_ppp_t *pppdev = (netdev2_ppp_t*) netdev;

    gnrc_ppp_target_t target;

    pkt = retrieve_pkt(netdev);
    gnrc_pktsnip_t *result = gnrc_pktbuf_mark(pkt, sizeof(hdlc_hdr_t), GNRC_NETTYPE_HDLC);
    if (!result) {
        DEBUG("gnrc_ppp: no space left in packet buffer\n");
        return NULL;
    }

    /*Drop packet if exceeds MRU*/
    if (gnrc_pkt_len(pkt) > pppdev->lcp.mru) {
        DEBUG("gnrc_ppp: Exceeded MRU of device. Dropping packet.\n");
        goto safe_out;
    }

    hdlc_hdr_t *hdlc_hdr = (hdlc_hdr_t *) result->data;
    target = _get_target_from_protocol(byteorder_ntohs(hdlc_hdr->protocol));

    if (!target) {
        /* Remove hdlc header */
        network_uint16_t protocol = ((hdlc_hdr_t*) pkt->next->data)->protocol;
        gnrc_pktbuf_remove_snip(pkt, pkt->next);
        gnrc_pktsnip_t *rp = gnrc_pktbuf_add(pkt, &protocol, 2, GNRC_NETTYPE_UNDEF);
        send_protocol_reject(gnrc_netdev, pppdev->lcp.pr_id++, rp);
        return NULL;
    }

    if (!_prot_is_allowed(gnrc_netdev, byteorder_ntohs(hdlc_hdr->protocol))) {
        DEBUG("gnrc_ppp: Received a ppp packet that's not allowed in current ppp state. Discard packet\n");
        goto safe_out;
    }

    if (_pkt_is_ppp(pkt) && !_ppp_pkt_is_valid(pkt)) {
        DEBUG("gnrc_ppp: Invalid ppp packet. Discard.\n");
        goto safe_out;
    }

    gnrc_pktsnip_t *ret_pkt = NULL;
    switch(target)
    {
        case PROT_LCP:
            fsm_handle_ppp_msg((gnrc_ppp_protocol_t*) &pppdev->lcp, PPP_RECV, pkt);
            break;
        case PROT_IPCP:
            fsm_handle_ppp_msg((gnrc_ppp_protocol_t*) &pppdev->ipcp, PPP_RECV, pkt);
            break;
        case PROT_IPV4:
            ret_pkt = ppp_ipv4_recv(gnrc_netdev, pkt);
            break;
        case PROT_AUTH:
            pap_recv((gnrc_ppp_protocol_t*) &pppdev->pap, pkt);
            break;
        default:
            DEBUG("Unrecognized target\n");
        }

    return ret_pkt;
safe_out:
    gnrc_pktbuf_release(pkt);
    return NULL;
}
int dispatch_ppp_msg(gnrc_netdev2_t *dev, gnrc_ppp_msg_t ppp_msg)
{
    gnrc_ppp_target_t target = ppp_msg_get_target(ppp_msg);
    gnrc_ppp_event_t event = ppp_msg_get_event(ppp_msg);
    netdev2_t *netdev = (netdev2_t*) dev->dev;
	netdev2_ppp_t *pppdev = (netdev2_ppp_t*) netdev;

    gnrc_ppp_protocol_t *target_prot;

    if (event == PPP_RECV) {
        assert(false);
        return 0;
    }
    else
    {
        target_prot = _get_prot_by_target(pppdev, target);

        if(!target_prot)
        {
            DEBUG("Unrecognized target\n");
            return -1;
        }

        return target_prot->handler(target_prot, event, NULL);
    }


}

void gnrc_ppp_trigger_event(msg_t *msg, kernel_pid_t pid, uint8_t target, uint8_t event)
{
    msg->type = GNRC_PPP_MSG_TYPE_EVENT;
    msg->content.value = (target << 8) | (event & 0xffff);
    msg_send(msg, pid);
}

void ppp_protocol_init(gnrc_ppp_protocol_t *protocol, gnrc_netdev2_t *pppdev, int (*handler)(gnrc_ppp_protocol_t *, uint8_t, void *), uint8_t id)
{
    protocol->handler = handler;
    protocol->id = id;
    protocol->pppdev = pppdev;
    protocol->state = PROTOCOL_DOWN;
}
