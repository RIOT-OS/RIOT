/*
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 *
 * @file
 * @brief       Test cases for the SHA256PRNG pseudo random number generator
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 */

#include <stdio.h>
#include <string.h>

#include "kernel_defines.h"
#include "random.h"

/**
 * @brief expected sequence for seed=1. This is only a regression test. The
 *        expected output was generated while porting the SHA256PRNG to RIOT.
 */
static const uint32_t seq_seed1[] =
   {1106729202, 3855353741,  932558076,  213257261, 1935649068, 3223344939,
    3700960722, 3580154139, 3802991633, 3783537878, 3862557448, 3401019389,
    3269475530,  260491589, 2254706846, 3754733214, 1693392656, 3020931263,
    2202015546, 2031345158};
/**
 * @brief expected sequence for seed=11799121 (generated at random.org). This is
 *        only a regression test. The expected output was generated while porting
 *        the SHA256PRNG to RIOT.
 */
static const uint8_t seq_seed2[] =
   {0x04, 0x8b, 0xc0, 0x91, 0x7b, 0x08, 0x4a, 0x2f, 0x8f, 0x9a, 0xd0, 0xa6, 0x65, 0x18,
    0x7b, 0x89, 0x9b, 0x74, 0x52, 0x45, 0x44, 0x74, 0xbd, 0x4a, 0x0c, 0x74, 0x8a, 0x0e,
    0xee, 0xdc, 0x76, 0x50, 0x67, 0xe8, 0x50, 0xce, 0x26, 0xdb, 0x0d, 0xef, 0x33, 0x38,
    0xba, 0x6b, 0x68, 0x8d, 0x5d, 0x83, 0xfd, 0xe3, 0xef, 0x19, 0x20, 0x59, 0xd9, 0xc9,
    0x12, 0x6b, 0x5f, 0x8c, 0xf9, 0x05, 0x36, 0x8f};

static void test_prng_sha256prng_seed1_u32(void)
{
    uint32_t seed = 1;
    uint32_t test32[ARRAY_SIZE(seq_seed1)];

    random_init(seed);

    /* request random samples */
    for (unsigned i = 0; i < ARRAY_SIZE(seq_seed1); i++) {
        test32[i] = random_uint32();
    }

    /* compare generator output and reference */
    if (!(memcmp(test32, seq_seed1, sizeof(seq_seed1)))) {
        printf("%s:SUCCESS\n", __func__);
    }
    else {
        printf("%s:FAILURE\n", __func__);
    }
}

static void test_prng_sha256prng_seed2_u8(void)
{
    uint32_t seed = 11799121;
    uint8_t test8[sizeof(seq_seed2)];

    random_init(seed);

    /* request random bytes */
    random_bytes(test8, sizeof(seq_seed2));

    /* compare generator output and reference */
    if (!(memcmp(test8, seq_seed2, sizeof(seq_seed2)))) {
        printf("%s:SUCCESS\n", __func__);
    }
    else {
        printf("%s:FAILURE\n", __func__);
    }
}

int main(void)
{
    test_prng_sha256prng_seed1_u32();
    test_prng_sha256prng_seed2_u8();

    return 0;
}
