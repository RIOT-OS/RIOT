/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <execinfo.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "backtrace.h"

void backtrace_print(void)
{
    void *array[BACKTRACE_SIZE + 1];
    int size;

    size = backtrace(array, BACKTRACE_SIZE + 1);

    /* skip above line's return address and start with 1 */
    for (int i = 1; i < size; i++) {
        printf("%p\n", array[i]);
    }
}

void backtrace_print_symbols(void)
{
    void *array[BACKTRACE_SIZE + 1];
    int size;

    size = backtrace(array, BACKTRACE_SIZE + 1);

    char ** symbols = backtrace_symbols(array, size);

    /* skip above line's return address and start with 1 */
    for (int i = 1; i < size; i++) {
        printf("%s\n", symbols[i]);
    }
    free(symbols);
}

int backtrace_len(void)
{
    void *array[BACKTRACE_SIZE + 1];

    return backtrace(array, BACKTRACE_SIZE + 1) - 1;
}

/** @} */
