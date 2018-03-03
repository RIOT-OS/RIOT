/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_random
 * @{
 * @file
 *
 * @brief Glue-code for tinymt32
 *
 * See https://github.com/MersenneTwister-Lab/TinyMT for details.
 *
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "tinymt32/tinymt32.h"

#define _ALIGNMENT_MASK (sizeof(uint32_t) - 1)

/* fits size to byte alignment */
static inline uint8_t *_align(uint8_t *buf)
{
    return (uint8_t *)(((size_t) buf + _ALIGNMENT_MASK) & ~(_ALIGNMENT_MASK));
}

static tinymt32_t _random;

void random_init(uint32_t seed)
{
    tinymt32_init(&_random, seed);
}

uint32_t random_uint32(void)
{
    return tinymt32_generate_uint32(&_random);
}

void random_bytes(uint8_t *buf, size_t size)
{
    size_t iter = size;
    size_t diff = _align(buf) - buf;
    uint32_t tmp;

    /* Fill first <4 unaligned bytes */
    if (diff) {
        tmp = tinymt32_generate_uint32(&_random);
        if (diff > size) {
            diff = size;
        }
        memcpy(buf, &tmp, diff);
        iter -= diff;
    }

    /* Fill aligned bytes */
    while (iter >= sizeof(uint32_t)) {
        *((uint32_t *) buf) = tinymt32_generate_uint32(&_random);
        buf += sizeof(uint32_t);
        iter -= sizeof(uint32_t);
    }

    /* Fill last bytes */
    if (iter) {
        tmp = tinymt32_generate_uint32(&_random);
        memcpy(buf, &tmp, iter);
    }
}

void random_init_by_array(uint32_t init_key[], int key_length)
{
    tinymt32_init_by_array(&_random, init_key, key_length);
}
