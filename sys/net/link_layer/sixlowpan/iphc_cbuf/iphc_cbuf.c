/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @addtogroup  net_sixlowpan_iphc_cbuf
 * @{
 *
 * @file        iphc_cbuf.c
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <string.h>

#include "mutex.h"
#include "vtimer.h"

#include "ipv6/addr.h"
#include "sixlowpan/iphc_cbuf.h"

static sixlowpan_iphc_cbuf_t _cbuf[SIXLOWPAN_IPHC_CBUF_SIZE];
static uint32_t _cbuf_invalid_time[SIXLOWPAN_IPHC_CBUF_SIZE];
static mutex_t _cbuf_mutex = MUTEX_INIT;

static uint32_t _now_in_minutes(void)
{
    timex_t now;

    vtimer_now(&now);

    return now.seconds / 60;
}

static void _cid_update_lifetime(uint8_t cid)
{
    uint32_t now = _now_in_minutes();
#ifdef DEVELHELP

    if (cid >= SIXLOWPAN_IPHC_CBUF_SIZE) {
        return;
    }

#endif

    if (now >= _cbuf_invalid_time[cid]) {
        _cbuf[cid].lifetime = 0;
    }
    else {
        _cbuf[cid].lifetime = _cbuf_invalid_time[cid] - now;
    }
}

static int _cid_still_valid(uint8_t cid)
{
#ifdef DEVELHELP

    if (cid >= SIXLOWPAN_IPHC_CBUF_SIZE) {
        return 0;
    }

#endif
    _cid_update_lifetime(cid);

    return (_cbuf[cid].lifetime > 0);
}

void _iphc_cbuf_internal_remove(uint8_t cid)
{
    if (cid < SIXLOWPAN_IPHC_CBUF_SIZE) {
        _cbuf[cid].cid = 0;
        _cbuf[cid].prefix_len = 0;
        memset(&(_cbuf[cid].prefix), 0, sizeof(ipv6_addr_t));
        _cbuf[cid].lifetime = 0;
        _cbuf_invalid_time[cid] = 0;
    }
}

sixlowpan_iphc_cbuf_t *sixlowpan_iphc_cbuf_lookup_addr(const ipv6_addr_t *addr)
{
    sixlowpan_iphc_cbuf_t *c = NULL;
    uint8_t bytes;

    mutex_lock(&_cbuf_mutex);

    for (int i = 0; i < SIXLOWPAN_IPHC_CBUF_SIZE; i++) {
        bytes = _cbuf[i].prefix_len / 8;

        if (_cbuf[i].prefix_len > 0 && memcmp(addr, &(_cbuf[i].prefix), bytes) == 0) {
            if (_cbuf[i].prefix_len % 8) {
                uint8_t mask = 0xff << (8 - (_cbuf[i].prefix_len - (bytes * 8)));

                if ((addr->u8[bytes] & mask) != (_cbuf[i].prefix.u8[bytes] & mask)) {
                    continue;
                }
            }

            if (_cid_still_valid(i)) {
                if (c == NULL || c->prefix_len < _cbuf[i].prefix_len) {
                    c = &_cbuf[i];
                }
            }
            else {
                _iphc_cbuf_internal_remove(i);
            }
        }
    }

    mutex_unlock(&_cbuf_mutex);

    return c;
}

sixlowpan_iphc_cbuf_t *sixlowpan_iphc_cbuf_lookup_cid(uint8_t cid)
{
    sixlowpan_iphc_cbuf_t *c = NULL;

    if (cid >= SIXLOWPAN_IPHC_CBUF_SIZE) {
        return NULL;
    }

    mutex_lock(&_cbuf_mutex);

    if (_cid_still_valid(cid)) {
        c = &(_cbuf[cid]);
    }
    else {
        _iphc_cbuf_internal_remove(cid);
    }

    mutex_unlock(&_cbuf_mutex);

    return c;
}

uint8_t sixlowpan_iphc_cbuf_num(void)
{
    uint8_t num = 0;

    mutex_lock(&_cbuf_mutex);

    for (int i = 0; i < SIXLOWPAN_IPHC_CBUF_SIZE; i++) {
        if (_cid_still_valid(i) && _cbuf[i].prefix_len > 0) {
            num++;
        }
        else {
            _iphc_cbuf_internal_remove(i);
        }
    }

    mutex_unlock(&_cbuf_mutex);

    return num;
}

void sixlowpan_iphc_cbuf_remove_by_cid(uint8_t cid)
{
    mutex_lock(&_cbuf_mutex);
    _iphc_cbuf_internal_remove(cid);
    mutex_unlock(&_cbuf_mutex);
}

void sixlowpan_iphc_cbuf_remove_invalid(void)
{
    mutex_lock(&_cbuf_mutex);

    for (int i = 0; i < SIXLOWPAN_IPHC_CBUF_SIZE; i++) {
        if (!_cid_still_valid(i)) {
            _iphc_cbuf_internal_remove(i);
        }
    }

    mutex_unlock(&_cbuf_mutex);
}

sixlowpan_iphc_cbuf_t *sixlowpan_iphc_cbuf_update(uint8_t cid, const ipv6_addr_t *prefix,
        uint8_t prefix_len, uint16_t lifetime)
{
    sixlowpan_iphc_cbuf_t *c = NULL;

    mutex_lock(&_cbuf_mutex);

    if (cid < SIXLOWPAN_IPHC_CBUF_SIZE && prefix_len > 0 && lifetime > 0) {
        _cbuf[cid].cid = cid;
        _cbuf[cid].prefix_len = (prefix_len < IPV6_ADDR_BIT_LEN) ? prefix_len : IPV6_ADDR_BIT_LEN;
        _cbuf[cid].lifetime = lifetime;
        ipv6_addr_init_prefix(&(_cbuf[cid].prefix), prefix, prefix_len);
        _cbuf_invalid_time[cid] = _now_in_minutes() + lifetime;

        c = &(_cbuf[cid]);
    }

    mutex_unlock(&_cbuf_mutex);

    return c;
}

/**
 * @}
 */
