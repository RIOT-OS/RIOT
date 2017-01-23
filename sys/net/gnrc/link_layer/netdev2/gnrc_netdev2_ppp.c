/*
 * Copyright (C) Fundación Inria Chile 2017
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc
 * @{
 *
 * @file
 * @brief       GNRC to netdev adapter for PPP devices
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 *
 * @}
 */

#include <errno.h>
#include "assert.h"
#include "net/gnrc/netdev2.h"
#include "net/gnrc/ppp/ppp.h"
#include "net/netdev2/ppp.h"
#include "net/gnrc.h"
#include "net/hdlc/hdr.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define BCAST   (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)

static gnrc_pktsnip_t *_recv(gnrc_netdev2_t *gnrc_netdev2)
{
    return ppp_recv(gnrc_netdev2);
}

static int _send(gnrc_netdev2_t *dev, gnrc_pktsnip_t *pkt)
{
    ppp_ipv4_send(dev, (gnrc_pktsnip_t *) pkt);
    return 0;
}

static int _link_up(gnrc_netdev2_t *dev)
{
    dispatch_ppp_msg(dev, (0xFF00) | (PPP_LINKDOWN&0xFF));
    dispatch_ppp_msg(dev, (0xFF00) | (PPP_LINKUP&0xFF));
    return 0;
}

static int _link_down(gnrc_netdev2_t *dev)
{
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) dev->dev;
    gnrc_ppp_protocol_t *dcp = (gnrc_ppp_protocol_t*) &pppdev->dcp;
    dcp->state = PROTOCOL_DOWN;
    dispatch_ppp_msg(dev, ((PROT_LCP<<8)&0xFF00) | (PPP_LINKDOWN & 0xFF));
    return 0;
}

static int msg_handler(gnrc_netdev2_t *dev, msg_t *msg)
{
    int event = msg->content.value;
    switch(msg->type)
    {
        case GNRC_PPP_MSG_TYPE_EVENT:
            dispatch_ppp_msg(dev, event);
            break;
    }
    return 0;
}
void gnrc_netdev2_ppp_init(gnrc_netdev2_t *gnrc_netdev2, netdev2_ppp_t *dev)
{
    assert(gnrc_netdev2 && dev);

    gnrc_netdev2->send = _send;
    gnrc_netdev2->recv = _recv;
    gnrc_netdev2->link_up = _link_up;
    gnrc_netdev2->link_down = _link_down;
    gnrc_netdev2->msg_handler = msg_handler;
    gnrc_netdev2->dev = (netdev2_t *) dev;
}
