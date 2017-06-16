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

#include "trace.h"

void trace_print(void)
{
    void *array[TRACE_SIZE + 1];
    size_t size;

    size = backtrace(array, TRACE_SIZE + 1);

    /* skip above line's return address and start with 1 */
    for (size_t i = 1; i < size; i++) {
        printf("%p\n", array[i]);
    }
}

/** @} */
