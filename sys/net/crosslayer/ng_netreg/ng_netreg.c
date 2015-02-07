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

/* unstructured pool of entries, marked as unclaimed by
 * netreg_entries[i].pid == KERNEL_PID_UNDEF */
static ng_netreg_entry_t netreg_entries[NG_NETREG_SIZE];

void ng_netreg_init(void)
{
    /* set all pointers in registry to NULL */
    memset(netreg, 0, NG_NETTYPE_NUMOF * sizeof(ng_netreg_entry_t *));

    for (int i = 0; i < NG_NETREG_SIZE; i++) {
        netreg_entries[i].pid = KERNEL_PID_UNDEF;   /* mark entry pool unclaimed */
    }
}

static inline int _netreg_entry_equal(ng_netreg_entry_t *a, ng_netreg_entry_t *b)
{
    /* to be pluggable with utlist, so result must be 0 when match */
    return ((a->pid != b->pid) || (a->demux_ctx != b->demux_ctx));
}

int ng_netreg_register(ng_nettype_t type, uint16_t demux_ctx, kernel_pid_t pid)
{
    ng_netreg_entry_t *entry = NULL;

    if (_INVALID_TYPE(type)) {
        return -EINVAL;
    }

    for (int i = 0; i < NG_NETREG_SIZE; i++) {  /* Search unclaimed entry */
        if (netreg_entries[i].pid == KERNEL_PID_UNDEF) {
            entry = &(netreg_entries[i]);
            break;
        }
    }

    if (entry == NULL) {
        return -ENOMEM;
    }

    entry->demux_ctx = demux_ctx;
    entry->pid = pid;

    if (netreg[type - 1] == NULL) {
        netreg[type - 1] = entry;
    }
    else {
        ng_netreg_entry_t *dup;

        /* search for duplicates to new entry */
        LL_SEARCH(netreg[type - 1], dup, entry, _netreg_entry_equal);

        if (dup == NULL) {  /* new entry is not a duplicate, add to registry */
            LL_PREPEND(netreg[type - 1], entry);
        }
        else {  /* new entry is a duplicate, mark as unclaimed again */
            entry->pid = KERNEL_PID_UNDEF;
        }
    }

    return 0;
}

void ng_netreg_unregister(ng_nettype_t type, uint16_t demux_ctx, kernel_pid_t pid)
{
    ng_netreg_entry_t *entry = netreg[type - 1];

    if (_INVALID_TYPE(type)) {
        return;
    }

    while (entry != NULL) {
        /* Find entry with given parameters in registry */
        if ((entry->pid == pid) && (entry->demux_ctx == demux_ctx)) {
            LL_DELETE(netreg[type - 1], entry);
            entry->pid = KERNEL_PID_UNDEF;

            return;
        }

        entry = entry->next;
    }
}

ng_netreg_entry_t *ng_netreg_lookup(ng_nettype_t type, uint16_t demux_ctx)
{
    ng_netreg_entry_t *res;

    if (_INVALID_TYPE(type)) {
        return NULL;
    }

    LL_SEARCH_SCALAR(netreg[type - 1], res, demux_ctx, demux_ctx);

    return res;
}

int ng_netreg_num(ng_nettype_t type, uint16_t demux_ctx)
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
    uint16_t demux_ctx;

    if (entry == NULL) {
        return NULL;
    }

    demux_ctx = entry->demux_ctx;

    LL_SEARCH_SCALAR(entry->next, entry, demux_ctx, demux_ctx);

    return entry;
}

/** @} */
