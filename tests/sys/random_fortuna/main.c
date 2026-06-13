/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for Fortuna PRNG
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "embUnit.h"
#include "fortuna.h"

static fortuna_state_t state;
static fortuna_state_t state2;

#ifdef CPU_NATIVE
/**
 * @brief   Large buffer for testing reads larger than FORTUNA_RESEED_LIMIT
 *          (1 MiB).
 */
static uint8_t _buf_reseed_limit[FORTUNA_RESEED_LIMIT + 1];
#endif

/**
 * @brief   Add `count` events of `length` bytes to the given entropy pool.
 */
static void _fill_pool(uint8_t pool, size_t count, uint8_t length)
{
    uint8_t data[32];

    memset(data, 0xAB, sizeof(data));

    for (size_t i = 0; i < count; i++) {
        fortuna_add_random_event(&state, data, length, 0, pool);
    }
}

/**
 * @brief   Seed the PRNG state with minimal entropy to allow testing of the
 *          generator and reseed logic.
 */
static void _seed_state(void)
{
    uint8_t dummy[4];

    _fill_pool(0, 2, 32);

    fortuna_random_data(&state, dummy, sizeof(dummy));
}

/**
 * @brief   Check if the scratchpad is all zeroes. This is used to verify that
 *          the scratchpad is properly wiped after use.
 */
static int _scratchpad_is_zero(void)
{
    const uint8_t *p = (const uint8_t *)&state.scratchpad;

    for (size_t i = 0; i < sizeof(state.scratchpad); i++) {
        if (p[i] != 0) {
            return 0;
        }
    }

    return 1;
}

static void setUp(void)
{
    fortuna_init(&state);
}

static void test_fortuna_init(void)
{
    TEST_ASSERT_EQUAL_INT(0, fortuna_init(&state));
    TEST_ASSERT_EQUAL_INT(0, state.gen.counter.split.l);
    TEST_ASSERT_EQUAL_INT(0, state.gen.counter.split.h);
}

static void test_fortuna_seed_size(void)
{
    TEST_ASSERT_EQUAL_INT(FORTUNA_SEED_SIZE, (int)sizeof(fortuna_seed_t));
}

static void test_fortuna_add_random_event__valid(void)
{
    uint8_t data[32];

    memset(data, 0, sizeof(data));

    TEST_ASSERT_EQUAL_INT(0, fortuna_add_random_event(&state, data, 1, 0, 0));
    TEST_ASSERT_EQUAL_INT(1, (int)state.pools[0].len);

    TEST_ASSERT_EQUAL_INT(0, fortuna_add_random_event(&state, data, 32, 0, 0));
    TEST_ASSERT_EQUAL_INT(33, (int)state.pools[0].len);

    TEST_ASSERT_EQUAL_INT(0, fortuna_add_random_event(&state, data, 1, 0, FORTUNA_POOLS - 1));
    TEST_ASSERT_EQUAL_INT(1, (int)state.pools[FORTUNA_POOLS - 1].len);
}

static void test_fortuna_add_random_event__invalid_length(void)
{
    uint8_t data[33];

    memset(data, 0, sizeof(data));

    TEST_ASSERT_EQUAL_INT(-1, fortuna_add_random_event(&state, data, 0, 0, 0));
    TEST_ASSERT_EQUAL_INT(-1, fortuna_add_random_event(&state, data, 33, 0, 0));
}

static void test_fortuna_add_random_event__invalid_pool(void)
{
    uint8_t data[1] = { 0 };

    TEST_ASSERT_EQUAL_INT(-2, fortuna_add_random_event(&state, data, 1, 0, FORTUNA_POOLS));
}

static void test_fortuna_random_data__seeded(void)
{
    uint8_t out[32];
    int all_zero;

    memset(out, 0, sizeof(out));
    _seed_state();

    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state, out, sizeof(out)));

    all_zero = 1;

    for (size_t i = 0; i < sizeof(out); i++) {
        if (out[i] != 0) {
            all_zero = 0;
            break;
        }
    }

    TEST_ASSERT(!all_zero);
}

static void test_fortuna_random_data__unseeded(void)
{
    uint8_t out[4];

    TEST_ASSERT_EQUAL_INT(-2, fortuna_random_data(&state, out, sizeof(out)));
}

static void test_fortuna_random_data__pool_drain_schedule(void)
{
    uint8_t out[4];

    /* Prime every pool with exactly FORTUNA_MIN_POOL_SIZE bytes. */
    for (uint8_t p = 0; p < FORTUNA_POOLS; p++) {
        _fill_pool(p, 2, 32);
    }

    /* reseed #1: only pool 0 must be drained */
    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state, out, sizeof(out)));
    TEST_ASSERT_EQUAL_INT(1, (int)state.reseeds);
    TEST_ASSERT_EQUAL_INT(0, (int)state.pools[0].len);

    for (int i = 1; i < (int)FORTUNA_POOLS; i++) {
        TEST_ASSERT_EQUAL_INT(64, (int)state.pools[i].len);
    }

    _fill_pool(0, 2, 32);

    /* reseed #2: pools 0 and 1 must be drained */
    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state, out, sizeof(out)));
    TEST_ASSERT_EQUAL_INT(2, (int)state.reseeds);
    TEST_ASSERT_EQUAL_INT(0, (int)state.pools[0].len);
    TEST_ASSERT_EQUAL_INT(0, (int)state.pools[1].len);

    for (int i = 2; i < (int)FORTUNA_POOLS; i++) {
        TEST_ASSERT_EQUAL_INT(64, (int)state.pools[i].len);
    }

    _fill_pool(0, 2, 32);

    /* reseed #3: only pool 0 must be drained, pools 2-31 still at length 64
     * (the FORTUNA_MIN_POOL_SIZE) */
    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state, out, sizeof(out)));
    TEST_ASSERT_EQUAL_INT(3, (int)state.reseeds);
    TEST_ASSERT_EQUAL_INT(0, (int)state.pools[0].len);

    for (int i = 2; i < (int)FORTUNA_POOLS; i++) {
        TEST_ASSERT_EQUAL_INT(64, (int)state.pools[i].len);
    }

    _fill_pool(0, 2, 32);

    /* reseed #4: pools 0, 1, and 2 must be drained, pool 2 transitions from
     * length 64 to 0 */
    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state, out, sizeof(out)));
    TEST_ASSERT_EQUAL_INT(4, (int)state.reseeds);
    TEST_ASSERT_EQUAL_INT(0, (int)state.pools[0].len);
    TEST_ASSERT_EQUAL_INT(0, (int)state.pools[1].len);
    TEST_ASSERT_EQUAL_INT(0, (int)state.pools[2].len);

    for (int i = 3; i < (int)FORTUNA_POOLS; i++) {
        TEST_ASSERT_EQUAL_INT(64, (int)state.pools[i].len);
    }
}

static void test_fortuna_random_data__determinism(void)
{
    uint8_t data[32];
    uint8_t out1[32], out2[32], out3[32];

    memset(data, 0xAB, sizeof(data));

    fortuna_init(&state);
    fortuna_add_random_event(&state, data, 32, 0, 0);
    fortuna_add_random_event(&state, data, 32, 0, 0);

    fortuna_init(&state2);
    fortuna_add_random_event(&state2, data, 32, 0, 0);
    fortuna_add_random_event(&state2, data, 32, 0, 0);

    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state, out1, sizeof(out1)));
    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state2, out2, sizeof(out2)));
    TEST_ASSERT(memcmp(out1, out2, sizeof(out1)) == 0);

    /* a different seed must produce different output */
    memset(data, 0xCD, sizeof(data));

    fortuna_init(&state2);
    fortuna_add_random_event(&state2, data, 32, 0, 0);
    fortuna_add_random_event(&state2, data, 32, 0, 0);

    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state2, out3, sizeof(out3)));
    TEST_ASSERT(memcmp(out1, out3, sizeof(out1)) != 0);
}

static void test_fortuna_random_data__wipe(void)
{
    uint8_t out[32];

    _seed_state();

    /* taint the scratchpad so the wipe is observable */
    memset(&state.scratchpad, 0xFF, sizeof(state.scratchpad));

    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state, out, sizeof(out)));
    TEST_ASSERT(_scratchpad_is_zero());
}

#ifdef CPU_NATIVE
static void test_fortuna_random_data__reseed_limit_exceeded(void)
{
    _seed_state();

    /* _buf_reseed_limit is sized as FORTUNA_RESEED_LIMIT + 1 */
    TEST_ASSERT_EQUAL_INT(-1,
                          fortuna_random_data(&state, _buf_reseed_limit,
                                              sizeof(_buf_reseed_limit)));
}

static void test_fortuna_random_data__large_read(void)
{
    /* _buf_reseed_limit is sized as FORTUNA_RESEED_LIMIT + 1 */
    size_t bytes = sizeof(_buf_reseed_limit);
    uint8_t *out = _buf_reseed_limit;
    int chunks = 0;

    _seed_state();

    /* this copies the logic of _read() from sys/random/fortuna.c, because that
     * file implements the random interface and is not accessible */
    do {
        size_t chunk = (bytes < FORTUNA_RESEED_LIMIT) ?
                       bytes : FORTUNA_RESEED_LIMIT;

        TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state, out, chunk));

        bytes -= chunk;
        out += chunk;
        chunks += 1;
    } while (bytes > 0);

    TEST_ASSERT_EQUAL_INT(2, chunks);
}
#endif

static void test_fortuna_update_seed__update(void)
{
    fortuna_seed_t seed;
    uint8_t out[4];

    memset(seed, 0xAB, sizeof(seed));

    TEST_ASSERT_EQUAL_INT(0, fortuna_update_seed(&state, &seed));
    TEST_ASSERT_EQUAL_INT(0, fortuna_random_data(&state, out, sizeof(out)));
}

static void test_fortuna_update_seed__wipe(void)
{
    fortuna_seed_t seed;

    memset(seed, 0xAB, sizeof(seed));

    /* taint the scratchpad so the wipe is observable */
    memset(&state.scratchpad, 0xFF, sizeof(state.scratchpad));

    TEST_ASSERT_EQUAL_INT(0, fortuna_update_seed(&state, &seed));
    TEST_ASSERT(_scratchpad_is_zero());
}

static void test_fortuna_write_seed(void)
{
    fortuna_seed_t seed;
    int all_zero;

    _seed_state();

    memset(seed, 0, sizeof(seed));

    TEST_ASSERT_EQUAL_INT(0, fortuna_write_seed(&state, &seed));
    TEST_ASSERT_EQUAL_INT(FORTUNA_SEED_SIZE, (int)sizeof(seed));

    all_zero = 1;

    for (size_t i = 0; i < sizeof(seed); i++) {
        if (seed[i] != 0) {
            all_zero = 0;
            break;
        }
    }

    TEST_ASSERT(!all_zero);
}

Test *tests_prng_fortuna(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_fortuna_init),
        new_TestFixture(test_fortuna_seed_size),
        new_TestFixture(test_fortuna_add_random_event__valid),
        new_TestFixture(test_fortuna_add_random_event__invalid_length),
        new_TestFixture(test_fortuna_add_random_event__invalid_pool),
        new_TestFixture(test_fortuna_random_data__seeded),
        new_TestFixture(test_fortuna_random_data__unseeded),
        new_TestFixture(test_fortuna_random_data__wipe),
        new_TestFixture(test_fortuna_random_data__pool_drain_schedule),
        new_TestFixture(test_fortuna_random_data__determinism),
#ifdef CPU_NATIVE
        new_TestFixture(test_fortuna_random_data__reseed_limit_exceeded),
        new_TestFixture(test_fortuna_random_data__large_read),
#endif
        new_TestFixture(test_fortuna_update_seed__update),
        new_TestFixture(test_fortuna_update_seed__wipe),
        new_TestFixture(test_fortuna_write_seed),
    };

    EMB_UNIT_TESTCALLER(fortuna_tests, setUp, NULL, fixtures);
    return (Test *)&fortuna_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_prng_fortuna());
    TESTS_END();

    return 0;
}
