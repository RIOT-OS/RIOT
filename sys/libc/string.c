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

#include <stdarg.h>
#include <stdio.h>
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

int __swprintf(string_writer_t *sw, FLASH_ATTR const char *restrict format, ...)
{
    va_list args;
    int res;

    va_start(args, format);
#ifdef __clang__
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wformat-nonliteral\"") \
    res = flash_vsnprintf(sw->position, sw->capacity, format, args);
    _Pragma("clang diagnostic pop")
#else
    res = flash_vsnprintf(sw->position, sw->capacity, format, args);
#endif
    va_end(args);

    if (res < (int)sw->capacity) {
        sw->capacity -= res;
        sw->position += res;
    } else {
        sw->position += sw->capacity;
        sw->capacity = 0;
        res = -E2BIG;
    }

    return res;
}

void reverse_buf(void *buf, size_t len)
{
    uint8_t *cur = buf;
    uint8_t *end = cur + len - 1;
    while (cur < end) {
        uint8_t tmp = *cur;
        *cur++ = *end;
        *end-- = tmp;
    }
}

static inline uint8_t swap_nibbles(uint8_t b)
{
    return (b << 4) | (b >> 4);
}

int dec_as_hex(uint32_t val, void *dst, size_t len)
{
    uint8_t *tgt = dst;
    uint8_t hex = 0;
    uint8_t idx = 0;

    memset(dst, 0, len);
    len *= 2;

    do {
        hex <<= 4;
        hex += val % 10;
        val /= 10;
        if (++idx % 2 == 0) {
            *tgt++ = swap_nibbles(hex);
            hex = 0;
        }
    } while (val && idx <= len);

    if (idx > len) {
        return -ENOBUFS;
    }

    if (idx % 2) {
        *tgt++ = hex;
    }

    return (uintptr_t)tgt - (uintptr_t)dst;
}

/** @} */
