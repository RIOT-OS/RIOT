/*
 * Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef TEST_H
#define TEST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define log2f for AVR-based targets.
 */
#ifdef __AVR__
#define log2f(x) (logf (x) / (float) 0.693147180559945309417)
#endif

/**
 * @brief   Enum of possible RNG sources.
 */
typedef enum {
    RNG_PRNG,
    RNG_CONSTANT,
#ifdef MODULE_PERIPH_HWRNG
    RNG_HWRNG,
#endif
    RNG_NUMOF
} rng_source_t;

/**
 * @brief   Names of the RNG sources.
 */
static const char sources[][12] = {
    [RNG_PRNG] = "PRNG",
    [RNG_CONSTANT] = "Constant",
#ifdef MODULE_PERIPH_HWRNG
    [RNG_HWRNG] = "HWRNG",
#endif
};

/**
 * @brief   Seed for initializing random module.
 */
extern uint32_t seed;

/**
 * @brief   Source of randomness.
 */
extern rng_source_t source;

/**
 * @brief   Helper for running the bit distribution test.
 *
 * For each sample, the individual bits are counted. After all samples are
 * collected, a graph is printed to visualize the distribution of bits.
 *
 * @param[in] samples   Number of samples.
 */
void test_distributions(uint32_t samples);

/**
 * @brief   Test for number dumping (e.g. for offline testing). Each number
 *          is printed on a separate line as an unsigned 32-bit number.
 *
 * @param[in] samples   Number of samples to print.
 */
void test_dump(uint32_t samples);

/**
 * @brief   Test for dumping random number r with a <= r < b. Each number
 *          is printed on a separate line as an unsigned 32-bit number.
 *
 * @param[in] samples   Number of samples to print.
 * @param[in] a         Minimum for random number
 * @param[in] b         Upper bound for random number
 *
 * @pre     a < b
 */
void test_dump_range(uint32_t samples, uint32_t a, uint32_t b);

/**
 * @brief   Run the FIPS 140-2 battery of test.
 *
 * The FIPS 140-2 tests are four statistical tests that will test 20000 bits
 * of generated random numbers:
 *   - monobit
 *   - poker test
 *   - runs
 *   - longruns
 *
 * Check the following URl for more information:
 * https://crypto.stackexchange.com/q/15052
 */
void test_fips(void);

/**
 * @brief   Run the entropy test. It collects N samples, then calculates
 *          Shannon's entropy.
 *
 * @param[in] samples   Number of samples to take.
 */
void test_entropy(uint32_t samples);

/**
 * @brief   Run the speed test for a given duration. It utilizes xtimer for
 *          setting an alarm.
 *
 * @param[in] duration  Test duration (in seconds)
 */
void test_speed(uint32_t duration);

/**
 * @brief   Run the speed test for random numbers r with a <= r < b and a given duration.
 *          It utilizes xtimer for setting an alarm.
 *
 * @param[in] duration  Test duration (in seconds)
 * @param[in] a         Minimum for random number
 * @param[in] b         Upper bound for random number
 *
 * @pre     a < b
 */
void test_speed_range(uint32_t duration, uint32_t a, uint32_t b);

#ifdef __cplusplus
}
#endif

#endif /* TEST_H */
/** @} */
