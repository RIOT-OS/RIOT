/*
 * SPDX-FileCopyrightText: 2018 Acutam Automation, LLC
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       cb_mux benchmark application
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <stdio.h>

#include "cb_mux.h"
#include "xtimer.h"

/* Number of entries in the cb_mux list */
#define NUM_ENTRIES (20U)

/* Fail if us greater than threshold */
#define FAIL_THRESH (200UL)

/* Head of cb_mux list */
cb_mux_t *cb_mux_head;

/* cb_mux list entries */
cb_mux_t entries[NUM_ENTRIES];

/* Timing */
unsigned long time_prev, time_curr;

void cb(void *arg)
{
    (void)arg;
    time_curr = xtimer_now_usec();
}

int main(void)
{
    unsigned long xtimer_delay, time_diff;
    uint8_t num;
    cb_mux_t *entry;

    puts("cb_mux benchmark application");

    /* Delay due to fetching timer with xtimer */
    time_prev = xtimer_now_usec();
    xtimer_delay = time_prev - xtimer_now_usec();

    /* Test for worst case: finding last entry */
    entries[NUM_ENTRIES - 1].cbid = 1;

    printf("Populating cb_mux list with %u items\n", NUM_ENTRIES);

    for (num = 0; num < NUM_ENTRIES; num++) {
        entries[num].cb = cb;
        cb_mux_add(&cb_mux_head, &(entries[num]));
    }

    puts("Finding the last list entry");

    time_prev = xtimer_now_usec();

    entry = cb_mux_find_cbid(cb_mux_head, 1);
    entry->cb(entry->arg);

    time_diff = time_curr - time_prev - xtimer_delay;

    printf("List walk time: %lu us\n", time_diff);

    if (time_diff > FAIL_THRESH) {
        printf("Walk time greater than threshold of %lu us\n", FAIL_THRESH);
        puts("[FAILURE]");
        return 1;
    }
    else {
        printf("Walk time less than threshold of %lu us\n", FAIL_THRESH);
        puts("[SUCCESS]");
    }

    return 0;
}
