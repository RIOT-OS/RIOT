/*
 * Copyright (C) 2020 Inria
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
 * @brief       OpenWSN required icmpv6rpl when using only MAC layer
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "idmanager.h"
#include "neighbors.h"
#include "IEEE802154E.h"
#include "packetfunctions.h"

static uint8_t _index = 0;

void icmpv6rpl_writeDODAGid(uint8_t *dodagid)
{
    (void) dodagid;
}

void icmpv6rpl_updateMyDAGrankAndParentSelection(void)
{
    if (neighbors_isStableNeighborByIndex(_index)) {
        return;
    }

    return;
}

bool icmpv6rpl_isReachable(void)
{
    /* consider node reachable if it is synched */
    if (idmanager_isPanCoordinator() == true) {
        return true;
    }
    if (ieee154e_isSynch()) {
        return true;
    }
    else {
        return false;
    }
}

dagrank_t icmpv6rpl_getMyDAGrank(void)
{
    return 0;
}

bool icmpv6rpl_getPreferredParentIndex(uint8_t *indexptr)
{
    *indexptr = _index;

    /* if we have a neighbor we have a parent */
    if (ieee154e_isSynch() == true && neighbors_getNumNeighbors()) {
        return true;
    }

    return false;
}

bool icmpv6rpl_getPreferredParentEui64(open_addr_t *addressToWrite)
{
    if (idmanager_isPanCoordinator() == true) {
        return false;
    }

    /* first parent is preferred parent without RPL */
    if (ieee154e_isSynch() == true && neighbors_getNumNeighbors()) {
        neighbors_getNeighborEui64(addressToWrite, ADDR_64B, _index);
    }

    return true;
}

bool icmpv6rpl_isPreferredParent(open_addr_t *address)
{
    open_addr_t temp;

    if (idmanager_isPanCoordinator() == true) {
        return false;
    }

    /* first parent is preferred parent without RPL */
    if (address->type == ADDR_64B) {
        neighbors_getNeighborEui64(&temp, ADDR_64B, _index);
        return packetfunctions_sameAddress(address, &temp);
    }
    else {
        return false;
    }
}
