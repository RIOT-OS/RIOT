/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file        hwrng.c
 * @brief       Copy random values to buffer
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "periph_cpu.h"
#include "periph/hwrng.h"

#if defined(MODULE_PERIPH_HWRNG) && HWRNG_HAS_GENERIC_READ

#define ROUND_UP_PTR(n) (void*)(4 + (~3 & ((intptr_t)(n) - 1)))

static inline void copy_bytewise(void *dest, const void *src, size_t n)
{
    uint8_t *d = dest;
    const uint8_t *s = src;

    switch (n) {
    case 3:
        *d++ = *s++;
        /* fall-through */
    case 2:
        *d++ = *s++;
        /* fall-through */
    case 1:
        *d++ = *s++;
        /* fall-through */
    }
}

void hwrng_read(void *buf, unsigned int num)
{
    uint32_t tmp;
    uint32_t *b32;
    size_t unaligned;

#if HWRNG_HAS_POWERONOFF
    hwrng_poweron();
#endif

    /* directly jump to fast path if destination buffer
       is properly aligned */
    if (!((intptr_t)buf & 0x3)) {
        b32 = buf;
        goto fast;
    }

    b32 = ROUND_UP_PTR(buf);
    unaligned = (uintptr_t)b32 - (uintptr_t)buf;

    tmp = hwrng_uint32();
    if (num < unaligned) {
        unaligned = num;
    }

    copy_bytewise(buf, &tmp, unaligned);
    num -= unaligned;

fast:
    while (num >= sizeof(uint32_t)) {
        *b32++ = hwrng_uint32();
        num -= sizeof(uint32_t);
    }

    if (!num) {
        goto out;
    }

    tmp = hwrng_uint32();
    copy_bytewise(b32, &tmp, num);

out:
#if HWRNG_HAS_POWERONOFF
    hwrng_poweroff();
#else
    ;
#endif
}

#endif
