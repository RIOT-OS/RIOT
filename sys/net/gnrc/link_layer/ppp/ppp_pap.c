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
 * @brief       Implementation of the PAP authentication protocol
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @}
 */
#include "net/gnrc/ppp/pap.h"
#include "net/gnrc/ppp/ppp.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define PAP_TIMEOUT (5000000)       /**< timeout of PAP configure request */
#define DEFAULT_PAP_COUNTER (3)     /**< max attempts of PAP configuration */
#define DEFAULT_APN_USER_SIZE (0)   /**< default APN user size (no user) */
#define DEFAULT_APN_PASS_SIZE (0)   /**< default APN password size (no password) */

gnrc_pktsnip_t *_pap_payload(gnrc_ppp_pap_t *pap)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, 2 + pap->user_size + pap->pass_size, GNRC_NETTYPE_UNDEF);

    *((uint8_t *) pkt->data) = pap->user_size;
    *(((uint8_t *) pkt->data) + 1 + pap->user_size) = pap->pass_size;

    memcpy(((uint8_t *) pkt->data) + 1, pap->username, pap->user_size);
    memcpy(((uint8_t *) pkt->data) + 2 + pap->user_size, pap->password, pap->pass_size);
    return pkt;
}

void pap_recv(gnrc_ppp_protocol_t *protocol, gnrc_pktsnip_t *pkt)
{
    msg_t *msg = &protocol->msg;
    gnrc_ppp_pap_t *pap = (gnrc_ppp_pap_t *) protocol;
    ppp_hdr_t *hdr = (ppp_hdr_t *) pkt->data;
    xtimer_t *xtimer = &pap->xtimer;
    xtimer_remove(xtimer);
    if (hdr->code != GNRC_PPP_CONF_ACK) {
        DEBUG("gnrc_ppp: pap: Wrong APN auth. Closing link.\n");
        send_ppp_event(msg, ppp_msg_set(PROT_LCP, PPP_LINKDOWN));
    }
    else {
        protocol->state = PROTOCOL_UP;
        send_ppp_event(msg, ppp_msg_set(GNRC_PPP_BROADCAST_NCP, PPP_LINKUP));
    }
}
int pap_handler(gnrc_ppp_protocol_t *protocol, uint8_t ppp_event, void *args)
{
    gnrc_ppp_pap_t *pap = (gnrc_ppp_pap_t *) protocol;
    msg_t *msg = &protocol->msg;
    gnrc_pktsnip_t *pkt;
    xtimer_t *xtimer = &pap->xtimer;
    msg_t *timer_msg = &pap->timer_msg;
    gnrc_netdev2_t *gnrc_pppdev = protocol->pppdev;
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) gnrc_pppdev->dev;
    gnrc_ppp_lcp_t *lcp = (gnrc_ppp_lcp_t *) &pppdev->lcp;
    uint8_t local_auth = lcp->local_auth;

    switch (ppp_event) {
        case PPP_LINKUP:
            if (local_auth == GNRC_PPP_AUTH_PAP) {
                pkt = _pap_payload(pap);
                protocol->state = PROTOCOL_STARTING;
                send_pap_request(protocol->pppdev, ++pap->id, pkt);
                send_ppp_event_xtimer(timer_msg, xtimer, ppp_msg_set(PROT_AUTH, PPP_TIMEOUT), PAP_TIMEOUT);
            }
            else {
                protocol->state = PROTOCOL_UP;
                send_ppp_event(msg, ppp_msg_set(GNRC_PPP_BROADCAST_NCP, PPP_LINKUP));
            }
            break;
        case PPP_LINKDOWN:
            protocol->state = PROTOCOL_DOWN;
            send_ppp_event(msg, ppp_msg_set(GNRC_PPP_BROADCAST_NCP, PPP_LINKDOWN));
            break;
        case PPP_TIMEOUT:
            pkt = _pap_payload(pap);
            send_pap_request(protocol->pppdev, ++pap->id, pkt);
            send_ppp_event_xtimer(timer_msg, xtimer, ppp_msg_set(PROT_AUTH, PPP_TIMEOUT), PAP_TIMEOUT);
            break;
        default:
            DEBUG("gnrc_ppp: pap: Received unknown msg\n");
    }
    return 0;
}

int pap_init(gnrc_netdev2_t *ppp_dev)
{
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) ppp_dev->dev;
    gnrc_ppp_pap_t *pap = (gnrc_ppp_pap_t *) &pppdev->pap;

    pap->user_size = DEFAULT_APN_USER_SIZE;
    pap->pass_size = DEFAULT_APN_PASS_SIZE;
    ppp_protocol_init((gnrc_ppp_protocol_t*) pap, ppp_dev, pap_handler, PROT_AUTH);
    pap->counter = DEFAULT_PAP_COUNTER;
    return 0;
}
