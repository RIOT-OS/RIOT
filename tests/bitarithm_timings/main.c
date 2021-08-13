/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup   tests
 * @{
 *
 * @file
 * @brief     Measure the speed of the function in bitarithm.c
 *
 * @author    René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>
#include <limits.h>

#include "bitarithm.h"
#include "xtimer.h"

#define TIMEOUT_S (5ul)
#define TIMEOUT (TIMEOUT_S * US_PER_SEC)
#define PER_ITERATION (4)

#if ARCH_32_BIT
#define TEST_AND_CLEAR_TEST_MASK_0  (0xA2101045UL)
#define TEST_AND_CLEAR_TEST_MASK_1  (0x22101044UL)
#else
#define TEST_AND_CLEAR_TEST_MASK_0  (0x9045UL)
#define TEST_AND_CLEAR_TEST_MASK_1  (0x1244UL)

#endif

static atomic_bool done;

static void callback(void *unused)
{
    (void)unused;
    atomic_store(&done, true);
}

static void run_test(const char *name, unsigned (*test)(unsigned))
{
    unsigned i = 0;
    unsigned long count = 0;
    atomic_store(&done, false);

    xtimer_t xtimer = { .callback = callback };
    xtimer_set(&xtimer, TIMEOUT);

    do {
        if (++i == UINT_MAX) {
            /* bitarithm_lsb must not be called with 0, but if all bits of i are
             * ones, that ~i will be zero. Therefore, we jump back to start
             * when all bits of i are ones
             */
            i = 1;
        }

        for (unsigned j = 0; j < PER_ITERATION; ++j) {
            volatile unsigned r;
            r = test(i);
            (void) r;
            r = test(-1u - i);
            (void) r;
            r = test(~i);
            (void) r;
            r = test(~(-1u - i));
            (void) r;
        }

        ++count;
    } while (atomic_load(&done) == false);

    printf("+ %s: %lu iterations per second\r\n", name, (4*PER_ITERATION) * count / TIMEOUT_S);
}

static unsigned do_test_and_clear(unsigned state)
{
    uint8_t index  = 0;
    unsigned found = 0;

    while (state) {
        state = bitarithm_test_and_clear(state, &index);
        found |= 1 << index;
    }

    return found;
}

static void run_test_test_and_clear(void)
{
    unsigned long count = 0;
    atomic_store(&done, false);

    xtimer_t xtimer = { .callback = callback };
    xtimer_set(&xtimer, TIMEOUT);

    do {
        assert(do_test_and_clear(TEST_AND_CLEAR_TEST_MASK_0) == TEST_AND_CLEAR_TEST_MASK_0);
        assert(do_test_and_clear(TEST_AND_CLEAR_TEST_MASK_1) == TEST_AND_CLEAR_TEST_MASK_1);
        ++count;
    } while (atomic_load(&done) == false);

    printf("+ %s: %lu iterations per second\r\n", "bitarithm_test_and_clear", 2 * count / TIMEOUT_S);
}

#define run_test(test) run_test(#test, test)

int main(void)
{
    printf("Start.\r\n");

    run_test(bitarithm_msb);
    run_test(bitarithm_lsb);
    run_test(bitarithm_bits_set);
    run_test_test_and_clear();

    printf("Done.\r\n");
    return 0;
}
