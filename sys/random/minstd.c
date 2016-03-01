/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * Code taken from C FAQ (http://c-faq.com/lib/rand.html).
 */

 /**
 * @ingroup sys_random
 * @{
 * @file
 *
 * @brief Simple Park & Miller "minimal standard" PRNG
 *
 * This file contains a simple Park-Miller pseudo random number generator.
 *
 * While not very random when considering crypto requirements, this is probably
 * random enough anywhere where pseudo-randomness is sufficient, e.g., when
 * provided with a sensible seed source, for MAC algorithms.
 *
 * The implementation is taken from the C FAQ, but modified to use magic number
 * division and adapted to RIOT's coding conventions..
 *
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdint.h>

#include "div.h"

#define a 48271
#define m 2147483647
#define q (m / a) /* 44488 */
#define r (m % a) /* 3399 */

static uint32_t _seed = 1;

int rand_minstd(void)
{
    uint32_t hi = div_u32_by_44488(_seed);
    uint32_t lo = div_u32_mod_44488(_seed);
    uint32_t test = (a * lo) - (r * hi);

    if(test > 0) {
        _seed = test;
    }
    else {
        _seed = test + m;
    }

    return _seed;
}

uint32_t random_uint32(void)
{
    /* minstd as implemented returns only values from 1 to 2147483647,
     * so run it two times to get 32bits */
    uint16_t A = (rand_minstd() >> 15);
    uint16_t B = (rand_minstd() >> 15);
    return  (((uint32_t)A) << 16) | B;
}

void random_init(uint32_t val)
{
    if (!val) {
        val = 1;
    }
    _seed = val;
}
