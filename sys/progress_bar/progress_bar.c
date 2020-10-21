/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_progress_bar
 * @{
 *
 * @file
 * @brief       Progress bar implementation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "progress_bar.h"

void progress_bar_print(char *prefix, char *suffix, uint8_t value)
{
    if (value > 100) {
        value = 100;
    }

    /* Hide cursor */
    printf("\033[?25l");

    /* Hack for pyterm: prepare space for the progress bar */
    putchar('\n');
    printf("\033[1A");

    /* Move cursor at the beginning of the line */
    putchar('\r');

    /* Display progress bar prefix if any */
    if (prefix) {
        printf("%s", prefix);
    }

    printf(PROGRESS_BAR_PREFIX_CHARACTER);

    /* Fully reprint the progress bar */
    for (unsigned i = 0; i < PROGRESS_BAR_LENGTH; ++i) {
        if (100 * i < (uint16_t)(value * PROGRESS_BAR_LENGTH)) {
            printf(PROGRESS_BAR_FULL_CHARACTER);
        }
        else {
            printf(PROGRESS_BAR_EMPTY_CHARACTER);
        }
    }

    printf(PROGRESS_BAR_SUFFIX_CHARACTER);

    /* Display progress bar suffix if any */
    if (suffix) {
        printf("%s", suffix);
    }

    /* Hack for pyterm */
    printf("\033[s");
    putchar('\n');
    printf("\033[u");

    /* show cursor */
    printf("\033[?25h");

#if defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)
    fflush(stdout);
#endif
}

void progress_bar_update(progress_bar_t *progress_bar)
{
    progress_bar_print(progress_bar->prefix, progress_bar->suffix,
                       progress_bar->value);
}

void progress_bar_prepare_multi(uint8_t len)
{
    /* Give enough space to print all progress bars. */
    for (uint8_t i = 0; i < len; ++i) {
        putchar('\n');
    }
}

void progress_bar_update_multi(progress_bar_t *progress_bar_list, uint8_t len)
{
    /* Move cursor to the line of the first progress bar. */
    printf("\033[%dA", len);

    for (uint8_t i = 0; i < len; ++i) {
        /* Display each progress bar as usual */
        progress_bar_update(&progress_bar_list[i]);

        /* Move cursor to next progress bar line. */
        putchar('\n');
    }
}
