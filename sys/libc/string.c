/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <errno.h>
#include "string_utils.h"

ssize_t strscpy(char *dest, const char *src, size_t count)
{
    const char *start = dest;

    if (!count) {
        return -E2BIG;
    }

    while (--count && *src) {
        *dest++ = *src++;
    }

    *dest = 0;

    if (*src == 0) {
        return dest - start;
    } else {
        return -E2BIG;
    }
}

const void *memchk(const void *data, uint8_t c, size_t len)
{
    const uint8_t *end = (uint8_t *)data + len;
    for (const uint8_t *d = data; d != end; ++d) {
        if (c != *d) {
            return d;
        }
    }

    return NULL;
}

/** @} */
