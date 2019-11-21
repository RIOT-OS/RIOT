/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_cb_mux
 * @{
 *
 * @file
 * @brief   cb_mux implementation
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include "cb_mux.h"
#include "utlist.h"

void cb_mux_add(cb_mux_t **head, cb_mux_t *entry)
{
    LL_APPEND(*head, entry);
}

void cb_mux_del(cb_mux_t **head, cb_mux_t *entry)
{
    LL_DELETE(*head, entry);
}

cb_mux_t *cb_mux_find_cbid(cb_mux_t *head, cb_mux_cbid_t cbid_val)
{
    cb_mux_t *entry;

    LL_SEARCH_SCALAR(head, entry, cbid, cbid_val);

    return entry;
}

cb_mux_t *cb_mux_find_low(cb_mux_t *head)
{
    cb_mux_t *entry = NULL;
    cb_mux_t *entry_low = NULL;
    cb_mux_cbid_t id = (cb_mux_cbid_t)(-1);

    LL_FOREACH(head, entry) {
        /* Entry does not have lower ID */
        if (entry->cbid >= id) {
            continue;
        }

        id = entry->cbid;
        entry_low = entry;
    }

    return entry_low;
}

cb_mux_t *cb_mux_find_high(cb_mux_t *head)
{
    cb_mux_t *entry = NULL;
    cb_mux_t *entry_high = NULL;
    cb_mux_cbid_t id = 0;

    LL_FOREACH(head, entry) {
        /* Entry does not have higher ID */
        if (entry->cbid <= id) {
            continue;
        }

        id = entry->cbid;
        entry_high = entry;
    }

    return entry_high;
}

cb_mux_cbid_t cb_mux_find_free_id(cb_mux_t *head)
{
    uint32_t free;
    cb_mux_cbid_t block;
    cb_mux_t *entry = NULL;
    uint8_t num;

    /* Search for free IDs in blocks of 32 IDs */
    for (block = 0; block + 31 < (cb_mux_cbid_t)(-1); block += 32) {
        /* Set all IDs in block to free */
        free = 0;

        LL_FOREACH(head, entry) {
            /* cbid falls within this block */
            if ((entry->cbid >= block) && (entry->cbid < block + 32)) {
                /* Set cbid to taken */
                free |= 1 << (entry->cbid & 0x1F);
            }
        }

        /* At least one ID in block free */
        if (~free) {
            break;
        }
    }

    /* Find which ID in block was free */
    for (num = 0; num < 32; num++) {
        if (~free & ((uint32_t)1 << num)) {
            return block | num;
        }
    }

    /* No free IDs */
    return (cb_mux_cbid_t)(-1);
}

void cb_mux_iter(cb_mux_t *head, cb_mux_iter_t func, void *arg)
{
    cb_mux_t *entry = NULL;

    LL_FOREACH(head, entry) {
        func(entry, arg);
    }
}
