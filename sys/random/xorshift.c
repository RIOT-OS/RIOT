/**
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * Code taken from Wikipedia (https://en.wikipedia.org/wiki/Xorshift)
 */

 /**
 * @ingroup sys_random
 * @{
 * @file
 *
 * @brief   Xorshift random number generator implementation
 *
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdint.h>

#include "random.h"

/* The state word must be initialized to non-zero */
uint32_t xorshift32(uint32_t *state)
{
    uint32_t x = *state;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;

    return x;
}

/* The state array must be initialized to not be all zero */
uint32_t xorshift128(uint32_t *state)
{
    uint32_t t = state[3];

    t ^= t << 11;
    t ^= t >> 8;
    state[3] = state[2];
    state[2] = state[1];
    state[1] = state[0];
    t ^= state[0];
    t ^= state[0] >> 19;
    state[0] = t;

    return t;
}

static uint32_t _state32;

uint32_t random_uint32(void)
{
    return xorshift32(&_state32);
}

void random_init(uint32_t val)
{
    if (!val) {
        val = 1;
    }
    _state32 = val;
}
