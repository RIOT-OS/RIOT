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

void hwrng_read(void *buf, unsigned int num)
{
    uint32_t *b32 = (uint32_t *)buf;

#if HWRNG_HAS_POWERONOFF
    hwrng_poweron();
#endif

    while (num >= sizeof(uint32_t)) {
        *b32++ = hwrng_uint32();
        num -= sizeof(uint32_t);
    }

    if (!num) {
        goto out;
    }

    uint32_t tmp = hwrng_uint32();
    uint8_t *b = (uint8_t *)b32;
    uint8_t *r = (uint8_t *)&tmp;

    switch (num) {
    case 3:
        *b++ = *r++;
        /* fall-through */
    case 2:
        *b++ = *r++;
        /* fall-through */
    case 1:
        *b++ = *r++;
        /* fall-through */
    }

out:
#if HWRNG_HAS_POWERONOFF
    hwrng_poweroff();
#else
    ;
#endif
}

#endif
