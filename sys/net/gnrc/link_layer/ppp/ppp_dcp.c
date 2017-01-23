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
 * @brief       Implementation of the Control Protocol for Driver interface.
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @}
 */
#include "net/gnrc/ppp/ppp.h"

#define ENABLE_DEBUG    (0)
#define ENABLE_MONITOR (1)
#include "debug.h"

#define MONITOR_TIMEOUT (5000000) /**< timeout of PPP monitor */

int dcp_handler(gnrc_ppp_protocol_t *protocol, uint8_t ppp_event, void *args)
{
    msg_t *msg = &protocol->msg;
    msg_t *timer_msg = &((gnrc_ppp_dcp_t *) protocol)->timer_msg;
    xtimer_t *xtimer = &((gnrc_ppp_dcp_t *) protocol)->xtimer;
    gnrc_ppp_dcp_t *dcp = (gnrc_ppp_dcp_t *) protocol;
    netdev2_t *pppdev = (netdev2_t*) protocol->pppdev->dev;

    netopt_enable_t en;
    switch (ppp_event) {
        case PPP_UL_STARTED:
            break;

        case PPP_UL_FINISHED:
            /*Remove timer*/
            xtimer_remove(xtimer);
            dcp->dead_counter = GNRC_PPP_DCP_DEAD_COUNTER;
            en = NETOPT_DISABLE;
            pppdev->driver->set(pppdev, NETOPT_DIAL_UP, &en, sizeof(netopt_enable_t));
            break;

        case PPP_LINKUP:
            protocol->state = PROTOCOL_UP;
            dcp->dead_counter = GNRC_PPP_DCP_DEAD_COUNTER;
            send_ppp_event(msg, ppp_msg_set(PROT_LCP, PPP_LINKUP));
#if ENABLE_MONITOR
            /*Start monitor*/
            send_ppp_event_xtimer(timer_msg, xtimer, ppp_msg_set(PROT_DCP, PPP_MONITOR), MONITOR_TIMEOUT);
#endif
            break;

        case PPP_MONITOR:
            if (dcp->dead_counter) {
                send_ppp_event(msg, ppp_msg_set(PROT_LCP, PPP_MONITOR));
                send_ppp_event_xtimer(timer_msg, xtimer, ppp_msg_set(PROT_DCP, PPP_MONITOR), MONITOR_TIMEOUT);
                if (dcp->dead_counter-- != GNRC_PPP_DCP_DEAD_COUNTER) {
                    DEBUG("gnrc_ppp: dcp: No response from modem. Send echo request.\n");
                }
            }
            else {
                send_ppp_event(msg, ppp_msg_set(PROT_DCP, PPP_UL_FINISHED));
            }
            break;

        case PPP_LINK_ALIVE:
            DEBUG("gnrc_ppp: received echo reply. Link is working.\n");
            dcp->dead_counter = GNRC_PPP_DCP_DEAD_COUNTER;
            break;

        default:
            DEBUG("gnrc_ppp: dcp: Receive unknown message\n");
            break;
    }
    return 0;
}
int dcp_init(gnrc_netdev2_t *ppp_dev)
{
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) ppp_dev->dev;
    ppp_protocol_init((gnrc_ppp_protocol_t*) &pppdev->dcp, ppp_dev, dcp_handler, PROT_DCP);
    pppdev->dcp.dead_counter = GNRC_PPP_DCP_DEAD_COUNTER;
    return 0;
}
