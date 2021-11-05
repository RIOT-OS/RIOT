/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 *
 * @brief   Generic binary search for tables containing strings.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 *
 */

#include <string.h>

#include "binsearch.h"

int binsearch_str(const void *start, size_t offset, size_t stride, size_t nmemb,
                  const char *str, size_t n)
{
    const uint8_t *cstart = (((const uint8_t *)start) + offset);
    size_t lo = 0, hi = nmemb;

    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        const char *target = *((const char *const *)(uintptr_t)(cstart + mid * stride));
        int cmp = strncmp(str, target, n);

        if (cmp == 0) {
            return mid;
        }
        else if (cmp < 0) {
            hi = mid;
        }
        else {   /* (cmp > 0) */
            lo = mid + 1;
        }
    }
    return (-ENOENT);
}

const void *binsearch_str_p(const void *start, size_t offset, size_t stride,
                            size_t nmemb, const char *str, size_t n)
{
    int ix = binsearch_str(start, offset, stride, nmemb, str, n);

    return (ix == (-ENOENT)) ? NULL : (const uint8_t *)start + ix * stride;
}
