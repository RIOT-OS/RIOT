/*
 * Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "test.h"

#include "fmt.h"
#include "random.h"
#include "xtimer.h"

#ifdef MODULE_PERIPH_HWRNG
#include "periph/hwrng.h"
#endif

/**
 * @brief   Seed for initializing random module.
 */
uint32_t seed = 0;

/**
 * @brief   Source of randomness.
 */
rng_source_t source = 0;

/**
 * @brief   Initialize the RNG, is needed.
 *
 * @param[in] name  The test name.
 */
static void test_init(char *name)
{
    /* prepare RNG source */
    if (source == RNG_PRNG) {
        random_init(seed);
    }
#ifdef MODULE_PERIPH_HWRNG
    else if (source == RNG_HWRNG) {
        hwrng_init();
    }
#endif

    /* print test overview */
    printf("Running %s test, with seed %" PRIu32 " using ", name, seed);

    if (source == RNG_PRNG) {
#if MODULE_PRNG_FORTUNA
        puts("Fortuna PRNG.\n");
#elif MODULE_PRNG_MERSENNE
        puts("Mersenne Twister PRNG.\n");
#elif MODULE_PRNG_MINSTD
        puts("Park & Miller Minimal Standard PRNG.\n");
#elif MODULE_PRNG_MUSL_LCG
        puts("Musl C PRNG.\n");
#elif MODULE_PRNG_SHA1PRNG
        puts("SHA1 PRNG.\n");
#elif MODULE_PRNG_TINYMT32
        puts("Tiny Mersenne Twister PRNG.\n");
#elif MODULE_PRNG_XORSHIFT
        puts("XOR Shift PRNG.\n");
#else
        puts("unknown PRNG.\n");
#endif
    }
#ifdef MODULE_PERIPH_HWRNG
    else if (source == RNG_HWRNG) {
        puts("HW RNG.\n");
    }
#endif
    else if (source == RNG_CONSTANT) {
        puts("constant value.\n");
    }
}

/**
 * @brief   Retrieve a 32-bit number of the source RNG.
 */
static inline uint32_t test_get_uint32(void)
{
    if (source == RNG_PRNG) {
        return random_uint32();
    }
#ifdef MODULE_PERIPH_HWRNG
    else if (source == RNG_HWRNG) {
        uint32_t result;
        hwrng_read(&result, 4);

        return result;
    }
#endif
    else if (source == RNG_CONSTANT) {
        /* use the seed as the constant value */
        return seed;
    }

    return 0;
}

/**
 * @brief   Retrieve a 32-bit number of the source RNG on [a,b)-interval
 */
static inline uint32_t test_get_uint32_range(uint32_t a, uint32_t b)
{
    if (source == RNG_PRNG) {
        return random_uint32_range(a, b);
    }
#ifdef MODULE_PERIPH_HWRNG
    else if (source == RNG_HWRNG) {
        puts("Range feature not supported by HWRNG");
    }
#endif
    else if (source == RNG_CONSTANT) {
        /* use the seed as the constant value */
        return seed;
    }

    return 0;
}

/**
 * @brief   Helper for printing `passed` or `failed` depending on condition
 *
 * @param[in] condition     The test condition.
 */
void test_pass_fail(bool condition)
{
    if (condition) {
        puts("passed");
    }
    else {
        puts("failed");
    }
}

void test_distributions(uint32_t samples)
{
    char tmp[16] = { 0 };

    uint32_t distributions[32] = { 0 };

    /* initialize test */
    test_init("distributions");

    /* take random samples */
    while (samples--) {
        uint32_t value = test_get_uint32();

        /* count bits */
        for (int i = 0; i < 32; i++) {
            if (value & (1 << i)) {
                distributions[i]++;
            }
        }
    }

    /* sum the total number of ones */
    uint64_t total = 0;
    uint32_t min = UINT32_MAX;
    uint32_t max = 0;

    for (int i = 0; i < 32; i++) {
        total += distributions[i];

        if (distributions[i] < min) {
            min = distributions[i];
        }

        if (distributions[i] > max) {
            max = distributions[i];
        }
    }

    /* if total is zero, it would yield a division by zero */
    if ((total / 100) == 0) {
        puts("Total ones is zero.\n");
        return;
    }

    /* print the distribution to screen */
    fmt_u64_dec(tmp, total / 32);
    printf("For 32-bit samples (min = %" PRIu32 ", max = %" PRIu32 ", avg = %s):\n", min, max, tmp);

    /* print a bar for each bin, scaling max to 100% */
    for (int i = 0; i < 32; i++) {
        printf("%02d: ", i);

        /* calculate the width of the bar (max 4 + 75 chars) */
        uint8_t bars = (distributions[i] / (max / 75));

        for (unsigned int j = 0; j < bars; j++) {
            printf("#");
        }

        puts("");
    }

    puts("");
}

void test_dump(uint32_t samples)
{
    test_init("dump");

    while (samples--) {
        printf("%" PRIu32 "\n", test_get_uint32());
    }
}

void test_dump_range(uint32_t samples, uint32_t low_thresh, uint32_t high_thresh)
{
    test_init("dump range");

    while (samples--) {
        printf("%" PRIu32 "\n", test_get_uint32_range(low_thresh, high_thresh));
    }
}

void test_fips(void)
{
    uint8_t last_bit = UINT8_MAX;

    uint32_t ones = 0;
    uint32_t poker[16] = { 0 };

    uint32_t runs = 1;
    uint32_t runs_ones[6] = { 0 };
    uint32_t runs_zeroes[6] = { 0 };

    uint32_t longruns = 1;
    uint32_t longruns_max = 0;

    /* initialize test */
    test_init("FIPS 140-2");

    /* FIPS 140-2 needs 20.000 bits, which are 625 32-bit random numbers */
    for (int i = 0; i < 625; i++) {
        uint32_t value = test_get_uint32();

        for (int j = 0; j < 4; j++) {
            uint8_t byte = ((uint8_t *) &value)[j];

            /* poker */
            poker[byte >> 4]++;
            poker[byte & 0x0f]++;

            for (int k = 0; k < 8; k++) {
                uint8_t bit = (byte >> k) & 0x01;

                /* monobit */
                if (bit) {
                    ones++;
                }

                /* run length */
                if (bit == last_bit) {
                    runs++;
                }
                else {
                    if (runs > 6) {
                        runs = 6;
                    }

                    if (last_bit) {
                        runs_ones[runs - 1]++;
                    }
                    else {
                        runs_zeroes[runs - 1]++;
                    }

                    runs = 1;
                }

                /* longruns */
                if (bit == last_bit) {
                    longruns++;
                }
                else {
                    if (longruns > longruns_max) {
                        longruns_max = longruns;
                    }

                    longruns = 1;
                }

                last_bit = bit;
            }
        }
    }

    /* for a constant stream of bits, the last (long)run must be added */
    if (runs > 6) {
        runs = 6;
    }

    if (last_bit) {
        runs_ones[runs - 1]++;
    }
    else {
        runs_zeroes[runs - 1]++;
    }

    if (longruns > longruns_max) {
        longruns_max = longruns;
    }

    /* monobit test result */
    printf("- Monobit test: ");

    test_pass_fail(!((ones >= 10275) || (ones <= 9725)));

    /* poker test result */
    uint32_t result = 0;

    for (int i = 0; i < 16; i++) {
        result += poker[i] * poker[i];
    }

    printf("- Poker test: ");

    test_pass_fail(!((result > 1576928) || (result < 1563176)));

    /* runs test result */
    bool passed = true;
    uint32_t min[6] = { 2343, 1135, 542, 251, 111, 111 };
    uint32_t max[6] = { 2657, 1365, 708, 373, 201, 201 };

    for (int i = 0; i < 6; i++) {
        if (runs_ones[i] < min[i] || runs_ones[i] > max[i]) {
            passed = false;
        }

        if (runs_zeroes[i] < min[i] || runs_zeroes[i] > max[i]) {
            passed = false;
        }
    }

    printf("- Run test: ");

    test_pass_fail(passed);

    /* longruns test result */
    printf("- Longrun test: ");

    test_pass_fail(longruns_max < 26);
}

void test_entropy(uint32_t samples)
{
    uint8_t buffer[256] = { 0 };
    uint32_t length = 0;

    /* initialize test */
    test_init("entropy");

    /* take samples */
    for (uint32_t i = 0; i < samples; i++) {
        uint32_t value = test_get_uint32();

        buffer[((uint8_t *) &value)[0]]++;
        buffer[((uint8_t *) &value)[1]]++;
        buffer[((uint8_t *) &value)[2]]++;
        buffer[((uint8_t *) &value)[3]]++;

        length += 4;
    }

    /* calculate entropy */
    float entropy = 0.0;

    for (int i = 0; i < 256; i++) {
        if (buffer[i] != 0) {
            float count = (float) buffer[i] / (float) length;
            entropy += (-count * log2f(count));
        }
    }

    /* print results */
    printf("Calculated %02f bits of entropy from %" PRIu32 " samples.\n", (double) entropy, samples);
}

void cb_speed_timeout(void *arg)
{
    unsigned *running = arg;
    *running = 0;
}

void test_speed(uint32_t duration)
{
    char tmp1[16] = { 0 }, tmp2[16] = { 0 }, tmp3[16] = { 0 };

    uint64_t samples = 0;

    /* initialize test */
    test_init("speed");
    printf("Running speed test for %" PRIu32 " seconds\n", duration);

    /* collect samples as long as timer has not expired */
    unsigned running = 1;
    xtimer_t xt = {
        .target = 0,
        .long_target = 0,
        .callback = cb_speed_timeout,
        .arg = &running,
    };
    uint32_t start_usec = xtimer_now_usec();
    xtimer_set(&xt, duration * US_PER_SEC);
    while (running) {
        test_get_uint32();
        samples++;
    }
    uint32_t actual_duration_usec = xtimer_now_usec() - start_usec;

    /* print results */
    fmt_u64_dec(tmp1, samples);
    fmt_u64_dec(tmp2, (samples * 4000000 / 1024) / actual_duration_usec);
    fmt_s32_dfp(tmp3, actual_duration_usec, -6);
    printf("Collected %s samples in %s seconds (%s KiB/s).\n", tmp1, tmp3, tmp2);
}

void test_speed_range(uint32_t duration, uint32_t low_thresh, uint32_t high_thresh)
{
    char tmp1[16] = { 0 }, tmp2[16] = { 0 }, tmp3[16] = { 0 };

    uint64_t samples = 0;

    /* initialize test */
    test_init("speed range");

    /* collect samples as long as timer has not expired */
    unsigned running = 1;
    xtimer_t xt = {
        .target = 0,
        .long_target = 0,
        .callback = cb_speed_timeout,
        .arg = &running,
    };
    uint32_t start_usec = xtimer_now_usec();
    xtimer_set(&xt, duration * US_PER_SEC);
    while (running) {
        test_get_uint32_range(low_thresh, high_thresh);
        samples++;
    }
    uint32_t actual_duration_usec = xtimer_now_usec() - start_usec;

    /* print results */
    fmt_u64_dec(tmp1, samples);
    fmt_u64_dec(tmp2, (samples * 4000000 / 1024) / actual_duration_usec);
    fmt_s32_dfp(tmp3, actual_duration_usec, -6);
    printf("Collected %s samples in %s seconds (%s KiB/s).\n", tmp1, tmp3, tmp2);
}
