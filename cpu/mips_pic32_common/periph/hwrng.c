/*
 * Copyright(C) 2017 Francois Berder <fberder@outlook.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include <string.h>
#include "board.h"
#include "periph/hwrng.h"

#ifdef _RNG

static void wait_plen_cycles(void)
{
    unsigned int i;
    for (i = 0; i < (RNGCON & _RNGCON_PLEN_MASK); ++i)
        __asm volatile ("nop");
}

void hwrng_init(void)
{
    RNGCON = _RNGCON_TRNGEN_MASK;

    /*
    * Wait to have at least 64 bits before setting the 64-bit seed
    * of the pseudo random generator.
    */
    while (RNGCNT < 64) {}

    /* Load seed from the TRNG */
    RNGCON |= _RNGCON_LOAD_MASK;
    while (RNGCON & _RNGCON_LOAD_MASK) {}

    RNGCON &= ~_RNGCON_TRNGEN_MASK;

    RNGPOLY1 = 0x00C00003;
    RNGPOLY2 = 0x00000000;

    RNGCON |= 42;   /* Set PLEN to 42 */
    RNGCON |= _RNGCON_CONT_MASK;
}

void hwrng_read(void *buf, unsigned int num)
{
    unsigned int i = 0;
    uint8_t *buffer = (uint8_t *)buf;

    RNGCON |= _RNGCON_PRNGEN_MASK;

    for (i = 0; i < (num >> 3); ++i) {
        uint32_t rng1, rng2;

        wait_plen_cycles();
        rng1 = RNGNUMGEN1;
        rng2 = RNGNUMGEN2;
        memcpy(buffer, &rng1, sizeof(rng1));
        memcpy(buffer + 4, &rng2, sizeof(rng2));
        buffer += 8;
    }

    num &= 0x7;
    if (num) {
        uint32_t rng1, n = num & 0x3;

        wait_plen_cycles();
        rng1 = RNGNUMGEN1;
        memcpy(buffer, &rng1, n);
        num -= n;
        buffer += n;

        if (num) {
            uint32_t rng2 = RNGNUMGEN2;
            memcpy(buffer, &rng2, num);
        }
    }

    RNGCON &= ~_RNGCON_PRNGEN_MASK;
}

#endif /* _RNG */
