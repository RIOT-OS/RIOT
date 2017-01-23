/*
 * Copyright (C) 2015 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_gnrc_ppp
 * @file
 * @brief       Functions for building PPP packets.
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @}
 */
#include "net/gnrc/ppp/ppp.h"

static void _send_ppp_pkt(gnrc_netdev2_t *dev, gnrc_nettype_t protocol, uint8_t code, uint8_t id, gnrc_pktsnip_t *payload)
{
    gnrc_pktsnip_t *pkt = pkt_build(protocol, code, id, payload);

    gnrc_ppp_send(dev, pkt);
}
void send_configure_request(gnrc_netdev2_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *opts)
{
    _send_ppp_pkt(dev, protocol, GNRC_PPP_CONF_REQ, id, opts);
}

void send_configure_ack(gnrc_netdev2_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *opts)
{
    _send_ppp_pkt(dev, protocol, GNRC_PPP_CONF_ACK, id, opts);
}

void send_configure_nak(gnrc_netdev2_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *opts)
{
    _send_ppp_pkt(dev, protocol, GNRC_PPP_CONF_NAK, id, opts);
}

void send_configure_rej(gnrc_netdev2_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *opts)
{
    _send_ppp_pkt(dev, protocol, GNRC_PPP_CONF_REJ, id, opts);
}
void send_terminate_req(gnrc_netdev2_t *dev, gnrc_nettype_t protocol, uint8_t id)
{
    _send_ppp_pkt(dev, protocol, GNRC_PPP_TERM_REQ, id, NULL);
}
void send_terminate_ack(gnrc_netdev2_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *response)
{
    _send_ppp_pkt(dev, protocol, GNRC_PPP_TERM_ACK, id, response);
}
void send_code_rej(gnrc_netdev2_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *rejected)
{
    _send_ppp_pkt(dev, protocol, GNRC_PPP_CODE_REJ, id, rejected);
}
void send_echo_reply(gnrc_netdev2_t *dev, gnrc_nettype_t protocol, uint8_t id, gnrc_pktsnip_t *data)
{
    _send_ppp_pkt(dev, protocol, GNRC_PPP_ECHO_REP, id, data);
}
void send_protocol_reject(gnrc_netdev2_t *dev, uint8_t id, gnrc_pktsnip_t *pkt)
{
    _send_ppp_pkt(dev, GNRC_NETTYPE_LCP, GNRC_PPP_PROT_REJ, id, pkt);
}
void send_pap_request(gnrc_netdev2_t *dev, uint8_t id, gnrc_pktsnip_t *credentials)
{
    _send_ppp_pkt(dev, GNRC_NETTYPE_PAP, GNRC_PPP_CONF_REQ, id, credentials);
}
