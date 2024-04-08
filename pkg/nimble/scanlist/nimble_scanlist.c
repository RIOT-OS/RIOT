/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_scanlist
 * @{
 *
 * @file
 * @brief       Implementation of a list for NimBLE scan results
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <limits.h>

#include "ztimer.h"
#include "net/bluetil/ad.h"

#include "nimble_scanlist.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static nimble_scanlist_entry_t _mem[NIMBLE_SCANLIST_SIZE];

static clist_node_t _pool;
static clist_node_t _list;

static int _finder(clist_node_t *node, void *arg)
{
    const ble_addr_t *addr = (const ble_addr_t *)arg;
    nimble_scanlist_entry_t *e = (nimble_scanlist_entry_t *)node;
    if (ble_addr_cmp(&e->addr, addr) == 0) {
        return 1;
    }
    return 0;
}

static nimble_scanlist_entry_t *_find(const ble_addr_t *addr)
{
    return (nimble_scanlist_entry_t *)clist_foreach(&_list, _finder,
                                                    (void *)addr);
}

void nimble_scanlist_init(void)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_mem); i++) {
        clist_rpush(&_pool, &_mem[i].node);
    }
}

nimble_scanlist_entry_t *nimble_scanlist_get_by_pos(unsigned pos)
{
    nimble_scanlist_entry_t *e = nimble_scanlist_get_next(NULL);
    for (unsigned i = 0; (i < pos) && e; i++) {
        e = nimble_scanlist_get_next(e);
    }
    return e;
}

void nimble_scanlist_update(uint8_t type, const ble_addr_t *addr,
                            const nimble_scanner_info_t *info,
                            const uint8_t *ad, size_t len)
{
    assert(addr);

    /* Ignore bogus advertisements */
    if (len > BLE_ADV_PDU_LEN) {
        assert(0);
        return;
    }

    uint32_t now = (uint32_t)ztimer_now(ZTIMER_USEC);
    nimble_scanlist_entry_t *e = _find(addr);

    if (!e) {
        e = (nimble_scanlist_entry_t *)clist_lpop(&_pool);
        if (!e) {
            /* no space available, dropping newly discovered node */
            return;
        }
        memcpy(&e->addr, addr, sizeof(ble_addr_t));
        if (ad) {
            memcpy(e->ad, ad, len);
        }
        e->ad_len = len;
        e->last_rssi = info->rssi;
        e->first_update = now;
        e->adv_msg_cnt = 1;
        e->type = type;
        e->phy_pri = info->phy_pri;
        e->phy_sec = info->phy_sec;
        clist_rpush(&_list, (clist_node_t *)e);
    }
    else {
        e->adv_msg_cnt++;
    }

    e->last_update = now;
}

nimble_scanlist_entry_t *nimble_scanlist_get_next(nimble_scanlist_entry_t *e)
{
    if (e == NULL) {
        return (nimble_scanlist_entry_t *)_list.next;
    }
    else {
        e = (nimble_scanlist_entry_t *)e->node.next;
        if (e == (nimble_scanlist_entry_t *)_list.next) {
            /* end of list */
            return NULL;
        }
        return e;
    }
}

nimble_scanlist_entry_t *nimble_scanlist_get(unsigned pos)
{
    nimble_scanlist_entry_t *cur = nimble_scanlist_get_next(NULL);
    for (unsigned i = 0; i < pos; i++) {
        cur = nimble_scanlist_get_next(cur);
    }
    return cur;
}

void nimble_scanlist_clear(void)
{
    clist_node_t *node = clist_lpop(&_list);
    while (node) {
        clist_rpush(&_pool, node);
        node = clist_lpop(&_list);
    }
}
