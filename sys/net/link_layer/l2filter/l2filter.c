/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_l2filter
 * @{
 *
 * @file
 * @brief       Link layer address filter implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "assert.h"
#include "net/l2filter.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static inline bool match(const l2filter_t *filter,
                         const void *addr, size_t addr_len)
{
    return ((filter->addr_len == addr_len) &&
            (memcmp(filter->addr, addr, addr_len) == 0));
}

void l2filter_init(l2filter_t *list)
{
    assert(list);

    for (unsigned i = 0; i < CONFIG_L2FILTER_LISTSIZE; i++) {
        list[i].addr_len = 0;
    }
}

int l2filter_add(l2filter_t *list, const void *addr, size_t addr_len)
{
    assert(list && addr && (addr_len <= CONFIG_L2FILTER_ADDR_MAXLEN));

    int res = -ENOMEM;

    for (unsigned i = 0; i < CONFIG_L2FILTER_LISTSIZE; i++) {
        if (list[i].addr_len == 0) {
            list[i].addr_len = addr_len;
            memcpy(list[i].addr, addr, addr_len);
            res = 0;
            break;
        }
    }

    return res;
}

int l2filter_rm(l2filter_t *list, const void *addr, size_t addr_len)
{
    assert(list && addr && (addr_len <= CONFIG_L2FILTER_ADDR_MAXLEN));

    int res = -ENOENT;

    for (unsigned i = 0; i < CONFIG_L2FILTER_LISTSIZE; i++) {
        if (match(&list[i], addr, addr_len)) {
            list[i].addr_len = 0;
            res = 0;
            break;
        }
    }

    return res;
}

bool l2filter_pass(const l2filter_t *list, const void *addr, size_t addr_len)
{
    assert(list && addr && (addr_len <= CONFIG_L2FILTER_ADDR_MAXLEN));

#ifdef MODULE_L2FILTER_WHITELIST
    bool res = false;
    for (unsigned i = 0; i < CONFIG_L2FILTER_LISTSIZE; i++) {
        if (match(&list[i], addr, addr_len)) {
            DEBUG("[l2filter] whitelist: address match -> packet passes\n");
            res = true;
            break;
        }
    }
    DEBUG("[l2filter] whitelist: no match -> packet dropped\n");
#else
    bool res = true;
    for (unsigned i = 0; i < CONFIG_L2FILTER_LISTSIZE; i++) {
        if (match(&list[i], addr, addr_len)) {
            DEBUG("[l2filter] blacklist: address match -> packet dropped\n");
            res = false;
            break;
        }
    }
    DEBUG("[l2fitler] blacklist: no match -> packet passes\n");
#endif

    return res;
}
