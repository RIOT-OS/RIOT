/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       cb_mux test application
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <stdio.h>

#include "cb_mux.h"

/* Head of cb_mux list */
cb_mux_t *cb_mux_head;

/* Flags for mux_iter */
enum {
    ITER_TEST = 1
};

/* Function to iterate over cb_mux list */
void mux_iter(cb_mux_t *entry, void *arg)
{
    (void)arg;

    entry->info = (void *)((uintptr_t)entry->info | (1 << ITER_TEST));
}

/* Test callback */
void cb(void *arg)
{
    printf("Callback %u executed\n", (uint8_t)(uintptr_t)arg);
}

int main(void)
{
    cb_mux_t entries[5];
    cb_mux_cbid_t num;
    cb_mux_t *entry;

    puts("cb_mux test routine");

    for (num = 0; num < 5; num++) {
        entries[num].cb = cb;
        entries[num].arg = (void *)num;
        entries[num].cbid = num;
    }

    puts("Test list addition, retrieval, execution of 5 CBs");

    for (num = 0; num < 5; num++) {
        cb_mux_add(&cb_mux_head, &(entries[num]));
    }

    for (num = 0; num < 5; num++) {
        entry = cb_mux_find_cbid(cb_mux_head, num);

        if (entry->cb == NULL) {
            puts("[FAILED] Unexpected NULL pointer");
            return 1;
        }

        entry->cb(entry->arg);
    }

    puts("Test list deletion of CB 0, 2, 4, execution of 1, 3");

    cb_mux_del(&cb_mux_head, &(entries[0]));
    cb_mux_del(&cb_mux_head, &(entries[2]));
    cb_mux_del(&cb_mux_head, &(entries[4]));

    for (num = 0; num < 5; num++) {
        entry = cb_mux_find_cbid(cb_mux_head, num);

        if (entry == NULL) {
            continue;
        }

        entry->cb(entry->arg);
    }

    puts("Test execution of CB with lowest ID (1)");

    entry = cb_mux_find_low(cb_mux_head);
    if (entry->cb == NULL) {
        puts("[FAILED] Unexpected NULL pointer");
        return 1;
    }
    entry->cb(entry->arg);

    puts("Test execution of CB with highest ID (3)");

    entry = cb_mux_find_high(cb_mux_head);
    if (entry->cb == NULL) {
        puts("[FAILED] Unexpected NULL pointer");
        return 1;
    }
    entry->cb(entry->arg);

    puts("Re-adding list entries (0, 2, 4) by finding next free ID");

    num = cb_mux_find_free_id(cb_mux_head);
    while (num < 5) {
        cb_mux_add(&cb_mux_head, &(entries[num]));

        printf("Added entry %i\n", num);

        num = cb_mux_find_free_id(cb_mux_head);
    }

    puts("Test iteration of a function over list");

    cb_mux_iter(cb_mux_head, mux_iter, NULL);

    for (num = 0; num < 5; num++) {
        if ((uintptr_t)entries[num].info & (1 << ITER_TEST)) {
            printf("Entry %i was updated correctly\n", num);
        }
    }

    puts("Tests complete!");

    return 0;
}
