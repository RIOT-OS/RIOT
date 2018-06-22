/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * Code referring to Docjar
 * (http://www.docjar.net/html/api/gnu/java/security/provider/SHA1PRNG.java.html)
 */

 /**
 * @ingroup sys_random
 * @{
 * @file
 *
 * @brief   SHA1PRNG random number generator implementation
 *
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "hashes/sha1.h"

#define SEED_SIZE           (20)

static sha1_context ctx;
static uint32_t datapos = SEED_SIZE;
static int8_t digestdata[SEED_SIZE];
static int8_t prng_state[SEED_SIZE];

void _updatestate(int8_t *state)
{
    signed last = 1;
    char zf = 0;

    for (int i = 0; i < 20; i++) {
        signed v;
        char t;
        /* add two bytes */
        v = (int)state[i] + (int)digestdata[i] + last;

        /* result is lower 8 bits */
        t = (char)(v & 0xff);

        /* check for state collision */
        zf = zf | (state[i] != t);

        /* store result, */
        state[i] = t;

        /* high 8 bits are carry, store for next iteration */
        last = (v >> 8);
    }

    /* make sure at least one bit changes! */
    if (!zf) {
       state[0]++;
    }

    /* update SHA1 state */
    sha1_update(&ctx, (void *)state, sizeof(state));
}

void _random_bytes(uint8_t *bytes, size_t size) /* TODO: use with global API */
{
    uint32_t loc = 0;
    while (loc < size)
    {
        int copy;
        /* find min between remaining out bytes and so far unused seed bytes */
        if ( (size-loc) < (SEED_SIZE - datapos) )
        {
            copy = size-loc;
        }
        else
        {
            /* in first iteration this will be 0 */
            copy = SEED_SIZE - datapos;
        }
        if (copy > 0)
        {
            /* directly copy data to out buffer */
            memcpy(bytes + loc, digestdata + datapos, copy);
            datapos += copy;
            loc += copy;
        }
        /* no out data ready, (re)fill internal buffer */
        else
        {
            /* reset SHA1 internal state */
            sha1_init(&ctx);

            /* update SHA1 internal state with PRNG state */
            sha1_update(&ctx, (void *)prng_state, sizeof(prng_state));

            /* get the digest */
            sha1_final(&ctx, digestdata);

            /* update PRNG state for next round */
            _updatestate(prng_state);

            /* reset counter for buffer position */
            datapos = 0;
        }
    }
}

void random_init(uint32_t seed)
{
    sha1_init(&ctx);
    sha1_update(&ctx, (void *)&seed, sizeof(uint32_t));
    sha1_final(&ctx, digestdata);

    /* copy seeded SHA1 state to PRNG state */
    memcpy(prng_state, &ctx.state, 20);
}
void random_init_by_array(uint32_t init_key[], int key_length)
{
    sha1_init(&ctx);
    sha1_update(&ctx, (void *)init_key, key_length);
    sha1_final(&ctx, digestdata);

    /* copy seeded SHA1 state to PRNG state */
    memcpy(prng_state, &ctx.state, 20);
}

uint32_t random_uint32(void)
{
    uint32_t ret;
    int8_t bytes[sizeof(uint32_t)];
    _random_bytes((uint8_t *)bytes, sizeof(uint32_t));

    ret = ((bytes[0] & 0xff) << 24)
        | ((bytes[1] & 0xff) << 16)
        | ((bytes[2] & 0xff) <<  8)
        |  (bytes[3] & 0xff);

    return ret;
}
