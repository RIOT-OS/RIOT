/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Interactive Sync implementation
 *
 *
 * @author      Gaëtan Harter <gaetan.harter@fu-berlin.de>
 */

#include <stdio.h>
#include "test_utils/interactive_sync.h"

void test_utils_interactive_sync(void)
{
    char c = '\0'; /* Print help on first loop */
    do {
        if (c == 'r') {
            /* This one should have a different case than the help message
             * otherwise we match it when using 'expect' */
            puts("READY");
        }
        else if (c != '\n' && c != '\r') {
            puts("Help: Press s to start test, r to print it is ready");
        }
        c = getchar();
    } while (c != 's');

    puts("START");
}
