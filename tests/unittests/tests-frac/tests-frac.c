/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <inttypes.h>
#include <string.h>
#include "embUnit.h"
#include "tests-frac.h"

#include "frac.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static const uint32_t u32_fraction_operands[] = {
    1ul,
    2ul,
    5ul,
    10ul,
    100ul,
    1000ul,
    1000000ul,
    2000000ul,
    4000000ul,
    8000000ul,
    16000000ul,
    32000000ul,
    641ul,
    274177ul,
    32768ul,
    9600ul,
    38400ul,
    115200ul,
    230400ul,
    460800ul,
    921600ul,
    4096ul,
    15625ul,
    125ul,
    1048576ul,
    0x10000000ul,
    0x1000000ul,
    1000000000ul,
    999999733ul,  /* <- prime */
    512000000ul,
    1024000000ul,
    0x40000000ul,
    0x80000000ul,
    0xc0000000ul,
    0xe0000000ul,
    0xf0000000ul,
    0xfffffff0ul,
    0xfffffff8ul,
    0xfffffffcul,
    0xfffffffeul,
    0xfffffffful,
};

static const uint32_t u32_test_values[] = {
    0ul,
    1ul,
    10ul,
    32ul,
    15625ul,
    15625ul*5,
    (15625ul*5)+1,
    0xfffful,
    0xfffful<<10,
    1234567890ul,
    99999999ul,
    1000000ul,
    115200ul,
    38400ul,
    57600ul,
    921600ul,
    32768ul,
    16000000ul,
    15999999ul,
    32767ul,
    327679999ul,
    100000000ul,
    2100012683ul,            /* <- prime */
    0x7ffffffful,
    0x80000000ul,
    0xc0000000ul,
    0xe0000000ul,
    0xf0000000ul,
    0xfffffff0ul,
    0xfffffff8ul,
    0xfffffffcul,
    0xfffffffeul,
    0xfffffffful,
};

#define N_U32_OPERANDS ARRAY_SIZE(u32_fraction_operands)
#define N_U32_VALS ARRAY_SIZE(u32_test_values)

static void test_frac_scale32(void)
{
    for (unsigned k = 0; k < N_U32_OPERANDS; ++k) {
        for (unsigned j = 0; j < N_U32_OPERANDS; ++j) {
            uint32_t num = u32_fraction_operands[j];
            uint32_t den = u32_fraction_operands[k];
            frac_t frac;
            frac_init(&frac, num, den);
            for (unsigned i = 0; i < N_U32_VALS; i++) {
                DEBUG("Scaling %" PRIu32 " by (%" PRIu32 " / %" PRIu32 "), ",
                    u32_test_values[i], num, den);
                /* intermediate result */
                volatile uint64_t tmp = (uint64_t)u32_test_values[i] * num;
                volatile uint64_t expected = tmp / (uint64_t)den;
                if (expected > 0xfffffffful) {
                    expected &= 0xfffffffful;
                }
                uint32_t actual = frac_scale(&frac, u32_test_values[i]);
                if ((uint32_t)expected != actual) {
                    int32_t diff = actual - expected;
#ifdef PRIu64
                    DEBUG("%" PRIu32 " * (%" PRIu32 " / %" PRIu32 ")"
                        " tmp %" PRIu64 " expect %" PRIu32 ", actual %" PRIu32
                        ", diff = %" PRId32 " shift=%u\n",
                        u32_test_values[i], num, den, tmp, (uint32_t)expected,
                        actual, diff, frac.shift);
#else
                    DEBUG("%" PRIu32 " * (%" PRIu32 " / %" PRIu32 ")"
                        " tmp %" PRIu32"%s expect %" PRIu32 ", actual %" PRIu32
                        ", diff = %" PRId32 " shift=%u\n",
                        u32_test_values[i], num, den,
                        (uint32_t)tmp, ((uint32_t)tmp != tmp) ? "!trunc" : "",
                        (uint32_t)expected,
                        actual, diff, frac.shift);
#endif

                    /* The frac algorithm sacrifices accuracy for speed,
                     * some large numbers will be incorrectly rounded,
                     * resulting in small differences here.. */
                    uint32_t max_error = frac_scale(&frac, 2);
                    max_error = max_error ? max_error : 1;
                    TEST_ASSERT_EQUAL_INT(1, diff >= 0);
                    if ((uint32_t)diff <= max_error) {
                        continue;
                    }
                }
                TEST_ASSERT_EQUAL_INT((uint32_t)expected, (uint32_t)actual);
            }
        }
    }
}

Test *tests_frac_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_frac_scale32),
    };

    EMB_UNIT_TESTCALLER(frac_tests, NULL, NULL, fixtures);

    return (Test *)&frac_tests;
}

void tests_frac(void)
{
    TESTS_RUN(tests_frac_tests());
}
