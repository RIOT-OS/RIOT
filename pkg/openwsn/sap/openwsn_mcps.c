/*
 * Copyright (C) 2019 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <stdio.h>

#include "errno.h"
#include "02b-MAChigh/schedule.h"
#include "02b-MAChigh/sixtop.h"
#include "cross-layers/idmanager.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/packetfunctions.h"
#include "openwsn.h"
#include "net/ieee802154.h"
#include "net/mac/sap.h"

extern mac_t openwsn_mac;

void upper_sendDone(OpenQueueEntry_t *msg, owerror_t error)
{
    mcps_confirm_t mcps_confirm;
    if (error == E_SUCCESS) {
        mcps_confirm.status = MAC_REQ_STATUS_SUCCESS;
    }
    else {
        mcps_confirm.status = -EINVAL;
    }
    mac_mcps_confirm(&openwsn_mac, &mcps_confirm);
    openqueue_freePacketBuffer(msg);
}

void upper_receive(OpenQueueEntry_t *msg)
{
    mcps_indication_t ind;
    ind.data.data = msg->payload;
    ind.data.len = msg->length;
    ind.data.addr = msg->l3_sourceAdd.addr_64b;
    mac_mcps_indication(&openwsn_mac, &ind);
    openqueue_freePacketBuffer(msg);
}

void mac_mcps_request(mac_t *mac, const mcps_request_t *mcps_request,
                               mcps_confirm_t *mcps_confirm)
{
    (void) mac;
    OpenQueueEntry_t *pkt;

    pkt = openqueue_getFreePacketBuffer(COMPONENT_SIXTOP);

    if (pkt == NULL) {
        mcps_confirm->status = -ENOBUFS;
        goto out;
    }

    pkt->owner = COMPONENT_SAP;
    pkt->creator = COMPONENT_SAP;

    if(packetfunctions_reserveHeader(&pkt, mcps_request->data.len)) {
        mcps_confirm->status = -EMSGSIZE;
        goto out;
    }
    memcpy(&pkt->payload[0], mcps_request->data.data,
           mcps_request->data.len);

    if(mcps_request->data.addr != NULL) {
        pkt->l2_nextORpreviousHop.type = ADDR_64B;
        memcpy(pkt->l2_nextORpreviousHop.addr_64b,
               mcps_request->data.addr, IEEE802154_LONG_ADDRESS_LEN);
    }
    else {
        pkt->l2_nextORpreviousHop.type = ADDR_16B;
        pkt->l2_nextORpreviousHop.addr_16b[0] = 0xff;
        pkt->l2_nextORpreviousHop.addr_16b[1] = 0xff;
    }

    if(!sixtop_send(pkt)) {
        mcps_confirm->status = MAC_REQ_STATUS_DEFERRED;
    }
    else {
        mcps_confirm->status = -EINVAL;
    }

out:
    if (mcps_confirm->status != MAC_REQ_STATUS_DEFERRED) {
        /* not going to acquire and release for now */
    }
}
