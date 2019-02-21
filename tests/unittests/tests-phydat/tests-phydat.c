/*
 * Copyright (C) 2018 Eistec AB
 *               2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"
#include "tests-phydat.h"

#include "phydat.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Default is smaller implementation */
#ifndef PHYDAT_FIT_TRADE_PRECISION_FOR_ROM
#define PHYDAT_FIT_TRADE_PRECISION_FOR_ROM 1
#endif

static void test_phydat_fit(void)
{
    /* Input values for each test: */
    static const int32_t values[][3] = {
        { 100445, -1, -1 },
        { -5, 2000954, 3 },
        { 30000449, -30000450, 30000500 },
        { -30000449, -30000499, -30000500 },
        { 0, 0, 1234567 },
        { 32768, 32768, 32768 },
        { 32767, 32767, 32767 },
        { 32766, 32766, 32766 },
        { -32769, -32769, -32769 },
        { -32768, -32768, -32768 },
        { -32767, -32767, -32767 },
        { -32766, -32766, -32766 },
    };
    static const int8_t scales[] = {
        -6,
        42,
        0,
        -1,
        5,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
    };
    static const unsigned int dims[] = {
        1,
        2,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
    };
    static const uint8_t units[] = {
        UNIT_V,
        UNIT_A,
        UNIT_NONE,
        UNIT_LUX,
        UNIT_M,
        UNIT_NONE,
        UNIT_NONE,
        UNIT_NONE,
        UNIT_NONE,
        UNIT_NONE,
        UNIT_NONE,
        UNIT_NONE,
    };
    /* Expected output values for each test: */
    static const phydat_t expected[] = {
        { .val = {  10045,     -1,     -1 }, .unit = UNIT_V,    .scale = -5 },
        { .val = {      0,  20010,     -1 }, .unit = UNIT_A,    .scale = 44 },
        { .val = {  30000, -30000,  30001 }, .unit = UNIT_NONE, .scale =  3 },
        { .val = { -30000, -30000, -30001 }, .unit = UNIT_LUX,  .scale =  2 },
        { .val = {      0,      0,  12346 }, .unit = UNIT_M,    .scale =  7 },
        { .val = {   3277,   3277,   3277 }, .unit = UNIT_NONE, .scale =  1 },
        { .val = {  32767,  32767,  32767 }, .unit = UNIT_NONE, .scale =  0 },
        { .val = {  32766,  32766,  32766 }, .unit = UNIT_NONE, .scale =  0 },
        { .val = {  -3277,  -3277,  -3277 }, .unit = UNIT_NONE, .scale =  1 },
#if PHYDAT_FIT_TRADE_PRECISION_FOR_ROM
        { .val = {  -3277,  -3277,  -3277 }, .unit = UNIT_NONE, .scale =  1 },
#else
        { .val = { -32768, -32768, -32768 }, .unit = UNIT_NONE, .scale =  0 },
#endif
        { .val = { -32767, -32767, -32767 }, .unit = UNIT_NONE, .scale =  0 },
        { .val = { -32766, -32766, -32766 }, .unit = UNIT_NONE, .scale =  0 },
    };

    for (unsigned int i = 0; i < sizeof(dims) / sizeof(dims[0]); i++) {
        phydat_t dat = {
            .val = { -1, -1, -1 },
            .scale = scales[i],
            .unit = units[i]
        };
        phydat_fit(&dat, values[i], dims[i]);
        TEST_ASSERT_EQUAL_INT(expected[i].val[0], dat.val[0]);
        TEST_ASSERT_EQUAL_INT(expected[i].val[1], dat.val[1]);
        TEST_ASSERT_EQUAL_INT(expected[i].val[2], dat.val[2]);
        TEST_ASSERT_EQUAL_INT(expected[i].scale, dat.scale);
        TEST_ASSERT_EQUAL_INT(expected[i].unit, dat.unit);
    }
}

Test *tests_phydat_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_phydat_fit),
    };

    EMB_UNIT_TESTCALLER(phydat_tests, NULL, NULL, fixtures);

    return (Test *)&phydat_tests;
}

void tests_phydat(void)
{
    TESTS_RUN(tests_phydat_tests());
}
