/*
 * Copyright (C) 2019 Inria
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
 * @brief       progress_bar test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "xtimer.h"

#include "progress_bar.h"

#define PROGRESS_BAR_LIST_NUMOF     (5U)

/* Test single progress bar */
static progress_bar_t progress_bar;

/* Test multiple progress bars */
static progress_bar_t progress_bar_list[PROGRESS_BAR_LIST_NUMOF];

int main(void)
{
    puts("Progress bar test application.");

    /* Test a single progress bar */
    sprintf(progress_bar.prefix, "%s ", "Progress bar 0");

    for (uint8_t i = 0; i < 101; ++i) {
        progress_bar.value = i;
        sprintf(progress_bar.suffix, " %3d%%", i);

        progress_bar_update(&progress_bar);

        xtimer_msleep(50);
    }

    puts("\nDone!");

    /* Prepare enough space for the progress bars */
    progress_bar_prepare_multi(PROGRESS_BAR_LIST_NUMOF);

    for (uint8_t i = 0; i < PROGRESS_BAR_LIST_NUMOF; ++i) {
        sprintf(progress_bar_list[i].prefix, "%s %d ", "Progress bar", i + 1);
    }

    for (uint8_t i = 0; i < 101; ++i) {
        for (uint8_t p = PROGRESS_BAR_LIST_NUMOF; p > 0 ; --p) {
            (p * i < 101) ? progress_bar_list[PROGRESS_BAR_LIST_NUMOF - p].value = i * p : 100;
        }

        progress_bar_update_multi(progress_bar_list, PROGRESS_BAR_LIST_NUMOF);
        xtimer_msleep(50);
    }

    puts("Done!");

    return 0;
}
