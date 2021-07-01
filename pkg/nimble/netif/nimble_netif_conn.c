/*
 * Copyright (C) 2018-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_netif_conn
 * @{
 *
 * @file
 * @brief       Connection context handling for NimBLE netif
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>

#include "nimble_netif_conn.h"
#include "random.h"

#define ENABLE_DEBUG            0
#include "debug.h"

#define CONN_CNT                (NIMBLE_NETIF_MAX_CONN)

static mutex_t _lock = MUTEX_INIT;
static nimble_netif_conn_t _conn[CONN_CNT];

static int _find_by_state(uint16_t filter)
{
    for (unsigned i = 0; i < CONN_CNT; i++) {
        if (_conn[i].state & filter) {
            return (int)i;
        }
    }
    return NIMBLE_NETIF_CONN_INVALID;
}

void nimble_netif_conn_init(void)
{
    DEBUG("conn_init\n");
    memset(_conn, 0, sizeof(_conn));
    for (unsigned i = 0; i < CONN_CNT; i++) {
        _conn[i].state = NIMBLE_NETIF_UNUSED;
    }
}

nimble_netif_conn_t *nimble_netif_conn_get(int handle)
{
    if ((handle < 0) || (handle >= CONN_CNT)) {
        return NULL;
    }
    return &_conn[handle];
}

int nimble_netif_conn_get_adv(void)
{
    int handle;
    DEBUG("nimble_netif_conn_get_adv\n");
    mutex_lock(&_lock);
    handle = _find_by_state(NIMBLE_NETIF_ADV);
    mutex_unlock(&_lock);
    return handle;
}

int nimble_netif_conn_get_connecting(void)
{
    int handle;
    DEBUG("nimble_netif_conn_get_connecting\n");
    mutex_lock(&_lock);
    handle = _find_by_state(NIMBLE_NETIF_CONNECTING);
    mutex_unlock(&_lock);
    DEBUG("nimble_netif_conn_get_connecting - handle %i\n", handle);
    return handle;
}

int nimble_netif_conn_get_by_addr(const uint8_t *addr)
{
    assert(addr);
    int handle = NIMBLE_NETIF_CONN_INVALID;

    DEBUG("nimble_netif_conn_get_by_addr %02x\n", (int)addr[0]);
    mutex_lock(&_lock);
    for (unsigned i = 0; i < CONN_CNT; i++) {
        if ((_conn[i].state & NIMBLE_NETIF_L2CAP_CONNECTED) &&
            memcmp(_conn[i].addr, addr, BLE_ADDR_LEN) == 0) {
            handle = (int)i;
            break;
        }
    }
    mutex_unlock(&_lock);
    DEBUG("nimble_netif_conn_get_by_addr - found handle: %i\n", handle);

    return handle;
}

int nimble_netif_conn_get_by_gaphandle(uint16_t gaphandle)
{
    int handle = NIMBLE_NETIF_CONN_INVALID;

    DEBUG("nimble_netif_conn_get_by_gaphandle %i\n", (int)gaphandle);
    mutex_lock(&_lock);
    for (unsigned i = 0; i < CONN_CNT; i++) {
        if (_conn[i].gaphandle == gaphandle) {
            handle = (int)i;
            break;
        }
    }
    mutex_unlock(&_lock);
    DEBUG("nimble_netif_conn_get_by_gaphandle - found %i\n", handle);

    return handle;
}

int nimble_netif_conn_start_connection(const uint8_t *addr)
{
    int handle;

    DEBUG("nimble_netif_conn_start_connection, addr %02x\n", (int)addr[0]);
    mutex_lock(&_lock);
    handle = _find_by_state(NIMBLE_NETIF_UNUSED);
    if (handle != NIMBLE_NETIF_CONN_INVALID) {
        _conn[handle].state = NIMBLE_NETIF_CONNECTING;
        memcpy(_conn[handle].addr, addr, BLE_ADDR_LEN);
    }

    mutex_unlock(&_lock);

    return handle;
}

int nimble_netif_conn_start_adv(void)
{
    int handle;

    DEBUG("nimble_netif_conn_start_adv\n");
    mutex_lock(&_lock);
    handle = _find_by_state(NIMBLE_NETIF_ADV);
    if (handle != NIMBLE_NETIF_CONN_INVALID) {
        handle = NIMBLE_NETIF_BUSY;
    }
    else {
        handle = _find_by_state(NIMBLE_NETIF_UNUSED);
        if (handle != NIMBLE_NETIF_CONN_INVALID) {
            _conn[handle].state = NIMBLE_NETIF_ADV;
        }
        else {
            handle = NIMBLE_NETIF_NOMEM;
        }

    }
    mutex_unlock(&_lock);

    return handle;
}

void nimble_netif_conn_free(int handle, uint8_t *addr)
{
    assert((handle >= 0) && (handle < CONN_CNT));

    DEBUG("nimble_netif_conn_free, handle %i\n", handle);
    mutex_lock(&_lock);
    if (addr) {
        memcpy(addr, _conn[handle].addr, BLE_ADDR_LEN);
    }
    memset(&_conn[handle], 0, sizeof(nimble_netif_conn_t));
    _conn[handle].state = NIMBLE_NETIF_UNUSED;
    mutex_unlock(&_lock);
}

void nimble_netif_conn_foreach(uint16_t filter,
                               nimble_netif_conn_iter_t cb, void *arg)
{
    assert(cb);

    DEBUG("nimble_netif_conn_foreach 0x%04x\n", (int)filter);
    mutex_lock(&_lock);
    for (unsigned i = 0; i < CONN_CNT; i++) {
        if (_conn[i].state & filter) {
            int res = cb(&_conn[i], (int)i, arg);
            if (res != 0) {
                break;
            }
        }
    }
    mutex_unlock(&_lock);
}

int nimble_netif_conn_get_next(int handle, uint16_t filter)
{
    /* if handle is undefined, start from the beginning */
    if (handle < 0) {
        handle = -1;
    }
    for (int i = (handle + 1); i < (int)CONN_CNT; i++) {
        if (_conn[i].state & filter) {
            return i;
        }
    }

    return NIMBLE_NETIF_CONN_INVALID;
}

unsigned nimble_netif_conn_count(uint16_t filter)
{
    unsigned cnt = 0;

    DEBUG("nimble_netif_conn_count, filter 0x%04x\n", (int)filter);
    mutex_lock(&_lock);
    for (unsigned i = 0; i < CONN_CNT; i++) {
        if (_conn[i].state & filter) {
            ++cnt;
        }
    }
    mutex_unlock(&_lock);

    return cnt;
}

uint16_t nimble_netif_conn_get_itvl_ms(int handle)
{
    if ((handle == 0) || (handle >= CONN_CNT)) {
        return 0;
    }

    return ((_conn[handle].itvl * BLE_HCI_CONN_ITVL) / 1000);
}

bool nimble_netif_conn_itvl_used(uint16_t itvl, int skip_handle)
{
    for (int handle = 0; handle < CONN_CNT; handle++) {
        if ((handle != skip_handle) && (_conn[handle].itvl != 0)) {
            uint16_t diff = (_conn[handle].itvl < itvl)
                                                    ? itvl - _conn[handle].itvl
                                                    : _conn[handle].itvl - itvl;
            if (diff < NIMBLE_NETIF_CONN_ITVL_SPACING) {
                return true;
            }
        }
    }

    return false;
}

uint16_t nimble_netif_conn_gen_itvl(uint16_t min, uint16_t max)
{
    assert(min <= max);

    uint16_t start = random_uint32_range(min, max);

    if (NIMBLE_NETIF_CONN_ITVL_SPACING == 0) {
        return start;
    }

    for (uint16_t itvl = start;
         itvl <= max;
         itvl += NIMBLE_NETIF_CONN_ITVL_SPACING) {
        if (!nimble_netif_conn_itvl_used(itvl, NIMBLE_NETIF_CONN_INVALID)) {
            return itvl;
        }
    }
    for (uint16_t itvl = start - NIMBLE_NETIF_CONN_ITVL_SPACING;
         itvl >= min;
         itvl -= NIMBLE_NETIF_CONN_ITVL_SPACING) {
        if (!nimble_netif_conn_itvl_used(itvl, NIMBLE_NETIF_CONN_INVALID)) {
            return itvl;
        }
    }
    return 0;
}
