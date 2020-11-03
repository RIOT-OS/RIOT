/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <stdbool.h>
#include <inttypes.h>

#include "mutex.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static gnrc_sixlowpan_ctx_t _ctxs[GNRC_SIXLOWPAN_CTX_SIZE];
static uint32_t _ctx_inval_times[GNRC_SIXLOWPAN_CTX_SIZE];
static mutex_t _ctx_mutex = MUTEX_INIT;

static uint32_t _current_minute(void);
static void _update_lifetime(uint8_t id);

static char ipv6str[IPV6_ADDR_MAX_STR_LEN];

static inline bool _valid(uint8_t id)
{
    _update_lifetime(id);
    return (_ctxs[id].prefix_len > 0);
}

gnrc_sixlowpan_ctx_t *gnrc_sixlowpan_ctx_lookup_addr(const ipv6_addr_t *addr)
{
    uint8_t best = 0;
    gnrc_sixlowpan_ctx_t *res = NULL;

    mutex_lock(&_ctx_mutex);

    for (unsigned int id = 0; id < GNRC_SIXLOWPAN_CTX_SIZE; id++) {
        if (_valid(id)) {
            uint8_t match = ipv6_addr_match_prefix(&_ctxs[id].prefix, addr);

            if ((_ctxs[id].prefix_len <= match) && (match > best)) {
                best = match;
                res = &(_ctxs[id]);
            }
        }
    }

    mutex_unlock(&_ctx_mutex);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (res != NULL) {
            DEBUG("6lo ctx: found context (%u, %s/%" PRIu8 ") ",
                (res->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK),
                ipv6_addr_to_str(ipv6str, &res->prefix, sizeof(ipv6str)),
                res->prefix_len);
            DEBUG("for address %s\n", ipv6_addr_to_str(ipv6str, addr, sizeof(ipv6str)));
        }
    }

    return res;
}

gnrc_sixlowpan_ctx_t *gnrc_sixlowpan_ctx_lookup_id(uint8_t id)
{
    if (id >= GNRC_SIXLOWPAN_CTX_SIZE) {
        return NULL;
    }

    mutex_lock(&_ctx_mutex);

    if (_valid(id)) {
        DEBUG("6lo ctx: found context (%u, %s/%" PRIu8 ")\n", id,
              ipv6_addr_to_str(ipv6str, &_ctxs[id].prefix, sizeof(ipv6str)),
              _ctxs[id].prefix_len);
        mutex_unlock(&_ctx_mutex);
        return &(_ctxs[id]);
    }

    mutex_unlock(&_ctx_mutex);

    return NULL;
}

gnrc_sixlowpan_ctx_t *gnrc_sixlowpan_ctx_update(uint8_t id, const ipv6_addr_t *prefix,
                                                uint8_t prefix_len, uint16_t ltime,
                                                bool comp)
{
    if ((id >= GNRC_SIXLOWPAN_CTX_SIZE) || (prefix_len == 0)) {
        return NULL;
    }

    mutex_lock(&_ctx_mutex);

    _ctxs[id].ltime = ltime;

    if (ltime == 0) {
        comp = false;
    }

    if (prefix_len > IPV6_ADDR_BIT_LEN) {
        _ctxs[id].prefix_len = IPV6_ADDR_BIT_LEN;
    }
    else {
        _ctxs[id].prefix_len = prefix_len;
    }

    _ctxs[id].flags_id = (comp) ? (GNRC_SIXLOWPAN_CTX_FLAGS_COMP | id) : id;

    if (!ipv6_addr_equal(&(_ctxs[id].prefix), prefix)) {
        ipv6_addr_set_unspecified(&(_ctxs[id].prefix));
        ipv6_addr_init_prefix(&(_ctxs[id].prefix), prefix, _ctxs[id].prefix_len);
    }
    DEBUG("6lo ctx: update context (%u, %s/%" PRIu8 "), lifetime: %" PRIu16 " min\n",
          id, ipv6_addr_to_str(ipv6str, &_ctxs[id].prefix, sizeof(ipv6str)),
          _ctxs[id].prefix_len, _ctxs[id].ltime);
    _ctx_inval_times[id] = ltime + _current_minute();

    mutex_unlock(&_ctx_mutex);
    return &(_ctxs[id]);
}

static uint32_t _current_minute(void)
{
    return xtimer_now_usec() / (US_PER_SEC * 60);
}

static void _update_lifetime(uint8_t id)
{
    uint32_t now;

    if (_ctxs[id].ltime == 0) {
        _ctxs[id].flags_id &= ~GNRC_SIXLOWPAN_CTX_FLAGS_COMP;
        return;
    }

    now = _current_minute();

    if (now >= _ctx_inval_times[id]) {
        DEBUG("6lo ctx: context %u was invalidated for compression\n", id);
        _ctxs[id].ltime = 0;
        _ctxs[id].flags_id &= ~GNRC_SIXLOWPAN_CTX_FLAGS_COMP;
    }
    else {
        _ctxs[id].ltime = (uint16_t)(_ctx_inval_times[id] - now);
    }
}

#ifdef TEST_SUITES
#include <string.h>

void gnrc_sixlowpan_ctx_reset(void)
{
    memset(_ctxs, 0, sizeof(_ctxs));
}
#endif

/** @} */
