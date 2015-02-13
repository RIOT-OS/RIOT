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
 * @file    ng_netreg.c
 */

#include <errno.h>
#include <string.h>

#include "clist.h"
#include "net/ng_netreg.h"
#include "net/ng_nettype.h"
#include "utlist.h"

#define _INVALID_TYPE(type) (((type) <= NG_NETTYPE_UNDEF) || ((type) >= NG_NETTYPE_NUMOF))

/* The registry as lookup table by ng_nettype_t */
static ng_netreg_entry_t *netreg[NG_NETTYPE_NUMOF - 1]; /* leave out NG_NETTYPE_UNDEF */

void ng_netreg_init(void)
{
    /* set all pointers in registry to NULL */
    memset(netreg, 0, NG_NETTYPE_NUMOF * sizeof(ng_netreg_entry_t *));
}

int ng_netreg_register(ng_nettype_t type, ng_netreg_entry_t *entry)
{
    if (_INVALID_TYPE(type)) {
        return -EINVAL;
    }

    LL_PREPEND(netreg[type - 1], entry);

    return 0;
}

void ng_netreg_unregister(ng_nettype_t type, ng_netreg_entry_t *entry)
{
    if (_INVALID_TYPE(type)) {
        return;
    }

    LL_DELETE(netreg[type - 1], entry);
}

ng_netreg_entry_t *ng_netreg_lookup(ng_nettype_t type, uint32_t demux_ctx)
{
    ng_netreg_entry_t *res;

    if (_INVALID_TYPE(type)) {
        return NULL;
    }

    LL_SEARCH_SCALAR(netreg[type - 1], res, demux_ctx, demux_ctx);

    return res;
}

int ng_netreg_num(ng_nettype_t type, uint32_t demux_ctx)
{
    int num = 0;
    ng_netreg_entry_t *entry;

    if (_INVALID_TYPE(type)) {
        return 0;
    }

    entry = netreg[type - 1];

    while (entry != NULL) {
        if (entry->demux_ctx == demux_ctx) {
            num++;
        }

        entry = entry->next;
    }

    return num;
}

ng_netreg_entry_t *ng_netreg_getnext(ng_netreg_entry_t *entry)
{
    uint32_t demux_ctx;

    if (entry == NULL) {
        return NULL;
    }

    demux_ctx = entry->demux_ctx;

    LL_SEARCH_SCALAR(entry->next, entry, demux_ctx, demux_ctx);

    return entry;
}

/** @} */
