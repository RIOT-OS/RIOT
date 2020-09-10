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

#include "openwsn.h"
#include "net/mac/sap.h"
#include "net/ieee802154.h"
#include "errno.h"

#include "02b-MAChigh/schedule.h"
#include "02b-MAChigh/sixtop.h"
#include "02b-MAChigh/msf.h"
#include "cross-layers/idmanager.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/packetfunctions.h"

extern mac_t openwsn_mac;

void ieee154e_indicate_sync(bool isSync)
{
    mlme_indication_t indication;
    if (isSync) {
        indication.type = MLME_SYNC;
    }
    else {
        indication.type = MLME_SYNC_LOSS;
    }
    mac_mlme_indication(&openwsn_mac, &indication);
}

void sixtop_indicate_recv(uint8_t code)
{
    mlme_confirm_t confirm;
    switch (code)
    {
    case IANA_6TOP_RC_SUCCESS:
        confirm.status = MAC_REQ_STATUS_SUCCESS;
        break;
    case IANA_6TOP_RC_EOL:
        confirm.status = -EINVAL;
        break;
    case IANA_6TOP_RC_BUSY:
        confirm.status = -EBUSY;
        break;
    case IANA_6TOP_RC_LOCKED:
        confirm.status = -EBUSY;
        break;
    default:
        confirm.status = -EINVAL;
        break;
    }
    confirm.type = MLME_SIXTOP_REQ;
    mac_mlme_confirm(&openwsn_mac, &confirm);
}

static void _mlme_set(mac_t *mac, const mlme_request_t *mlme_request,
                               mlme_confirm_t *mlme_confirm)
{
    (void) mac;
    switch(mlme_request->mib.type) {
        case MIB_ROLE:
            idmanager_setRole(mlme_request->mib.content);
            mlme_confirm->status = MAC_REQ_STATUS_SUCCESS;
            break;
        case MIB_SLOTFRAME_LENGTH:
            schedule_setFrameLength(mlme_request->mib.content);
            mlme_confirm->status = MAC_REQ_STATUS_SUCCESS;
            break;
        case MIB_SLOTFRAME_HANDLE:
            schedule_setFrameHandle(mlme_request->mib.content);
            mlme_confirm->status = MAC_REQ_STATUS_SUCCESS;
            break;
        default:
            mlme_confirm->status = -EINVAL;
            break;
    }
}

static void _mlme_get(mac_t *mac, const mlme_request_t *mlme_request,
                               mlme_confirm_t *mlme_confirm)
{
    (void) mac;

    switch(mlme_request->mib.type) {
        case MIB_ROLE:
            mlme_confirm->mib.content = IEEE802154E_ROLE_LEAF;
            if (idmanager_isCoordinator()) {
                mlme_confirm->mib.content = IEEE802154E_ROLE_COORDINATOR;
            }
            else if (idmanager_isPanCoordinator()) {
                mlme_confirm->mib.content = IEEE802154E_ROLE_PAN_COORDINATOR;
            }
            mlme_confirm->status = MAC_REQ_STATUS_SUCCESS;
            break;
        case MIB_SLOTFRAME_LENGTH:
            mlme_confirm->mib.content = schedule_getFrameLength();
            mlme_confirm->status = MAC_REQ_STATUS_SUCCESS;
            break;
        default:
            mlme_confirm->status = -EINVAL;
            break;
    }
}

void _link_request(mac_t *mac, const mlme_request_t *mlme_request,
                               mlme_confirm_t *mlme_confirm)
{
    (void) mac;
    int res;

    cellType_t cell_type = CELLTYPE_OFF;

    switch (mlme_request->link.type)
    {
    case TX_LINK:
        cell_type = CELLTYPE_TX;
        break;
    case RX_LINK:
        cell_type = CELLTYPE_RX;
        break;
    case TXRX_LINK:
        cell_type = CELLTYPE_TXRX;
        break;
    case ADVERTISING_LINK:
        cell_type = CELLTYPE_TXRX;
        break;
    default:
        break;
    }

    open_addr_t open_addr;
    memset(&open_addr,0,sizeof(open_addr));

    open_addr.type = ADDR_ANYCAST;

    /* Advertising links must be multicast in OpenWSN */
    if (!(mlme_request->link.type & ADVERTISING_LINK)) {
        if (mlme_request->link.addr != NULL) {
            open_addr.type = ADDR_64B;
            memcpy(open_addr.addr_64b, mlme_request->link.addr,
                IEEE802154_LONG_ADDRESS_LEN);
        }
    }

    if (mlme_request->link.operation == ADD_LINK) {
        res = schedule_addActiveSlot(mlme_request->link.slot_offset,
                                     cell_type, mlme_request->link.shared,
                                     false, mlme_request->link.channel_offset,
                                     &open_addr);
    }
    else if (mlme_request->link.operation == DELETE_LINK) {
        res = schedule_removeActiveSlot(mlme_request->link.slot_offset,
                                        cell_type, mlme_request->link.shared,
                                        &open_addr);
    }
    else {
        /* *MODIFY_LINK is unsupported */
        mlme_confirm->status = -EINVAL;
        return;
    }

    if (!res) {
        mlme_confirm->status = MAC_REQ_STATUS_SUCCESS;
    }
    else {
        mlme_confirm->status = -EINVAL;
    }
}

void _sixtop_request(mac_t *mac, const mlme_request_t *mlme_request,
                               mlme_confirm_t *mlme_confirm)
{
    (void) mac;
    open_addr_t open_addr;
    cellInfo_ht cells_add[CELLLIST_MAX_LEN];
    cellInfo_ht cells_rmv[CELLLIST_MAX_LEN];

    if (mlme_request->sixtop.addr != NULL) {
        open_addr.type = ADDR_64B;
        memcpy(open_addr.addr_64b, mlme_request->sixtop.addr, IEEE802154_LONG_ADDRESS_LEN);
    } else {
        mlme_confirm->status = -EINVAL;
        goto out;
    }

    /* sixtop unsupported option combinations */
    if (mlme_request->sixtop.opts == 0 ||
        mlme_request->sixtop.opts == SHARED_CELL) {
        mlme_confirm->status = -EINVAL;
        goto out;
    }
    /* openwsn TXRX cells must be shared */
    if (mlme_request->sixtop.opts == (TX_CELL | RX_CELL)) {
        mlme_confirm->status = -EINVAL;
        goto out;
    }

    switch (mlme_request->sixtop.cmd)
    {
    case SIXTOP_ADD:
        if (!msf_candidateAddCellList(cells_add, mlme_request->sixtop.num)) {
            mlme_confirm->status = -EINVAL;
            goto out;
        }
        break;
    case SIXTOP_REMOVE:
        if (!msf_candidateRemoveCellList(cells_rmv, &open_addr,
                                         mlme_request->sixtop.num,
                                         (uint8_t) mlme_request->sixtop.opts)){
            mlme_confirm->status = -EINVAL;
            goto out;
        }
        break;
    case SIXTOP_RELOCATE:
        if (!schedule_getCellsToBeRelocated(&open_addr, cells_rmv) ||
            !msf_candidateAddCellList(cells_add, mlme_request->sixtop.num)) {
            mlme_confirm->status = -EINVAL;
            goto out;
        }
        break;
    case SIXTOP_CLEAR:
        break;
    default:
        break;
    }

    int ret = sixtop_request(mlme_request->sixtop.cmd, &open_addr,
                             mlme_request->sixtop.num, (uint8_t) mlme_request->sixtop.opts,
                             cells_add, cells_rmv, IANA_6TISCH_SFID_MSF, 0, 0);

    if (!ret) {
        mlme_confirm->status = MAC_REQ_STATUS_DEFERRED;
    }
    else {
        mlme_confirm->status = -EINVAL;
    }

out:
    if (mlme_confirm->status != MAC_REQ_STATUS_DEFERRED) {
        /* not going to acquire and release for now */
    }
}

void mac_mlme_request(mac_t *mac, const mlme_request_t *mlme_request,
                               mlme_confirm_t *mlme_confirm)
{
    switch (mlme_request->type) {
        case MLME_START:
            /* unsupported */
            break;
        case MLME_RESET:
            /* unsupported */
            break;
        case MLME_SET:
            _mlme_set(mac, mlme_request, mlme_confirm);
            break;
        case MLME_GET:
            _mlme_get(mac, mlme_request, mlme_confirm);
            break;
        case MLME_SET_LINK:
            _link_request(mac, mlme_request, mlme_confirm);
            break;
        case MLME_SIXTOP_REQ:
            _sixtop_request(mac, mlme_request, mlme_confirm);
            break;
        default:
            mlme_confirm->status = -EINVAL;
            break;
    }
}
