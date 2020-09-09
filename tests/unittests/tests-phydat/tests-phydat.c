/*
 * Copyright (C) 2018 Eistec AB
 *               2018 Otto-von-Guericke-Universität Magdeburg
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for the phydat module
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "embUnit.h"
#include "tests-phydat.h"

#include "phydat.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Default is smaller implementation */
#ifndef PHYDAT_FIT_TRADE_PRECISION_FOR_ROM
#define PHYDAT_FIT_TRADE_PRECISION_FOR_ROM 1
#endif

#if (PHYDAT_DIM != 3)
#error "PHYDAT unittests are only applicable if PHYDAT_DIM is 3"
#endif

#define BUFSIZE         (128U)

static char test[BUFSIZE];

typedef struct {
    int dim;
    phydat_t dat;
    const char *json;
} tdat_t;

/* define test data */
static tdat_t data[] = {
    {
        .dim  = 1,
        .dat  = { { 2345, 0, 0 }, UNIT_TEMP_C, -2 },
        .json = "{\"d\":23.45,\"u\":\"°C\"}",
    },
    {
        .dim  = 3,
        .dat  = { { 1032, 10, -509 }, UNIT_G, -3 },
        .json = "{\"d\":[1.032,0.010,-0.509],\"u\":\"g\"}",
    },
    {
        .dim  = 3,
        .dat  = { { 1200, 38, 98 }, UNIT_M, -3 },
        .json = "{\"d\":[1.200,0.038,0.098],\"u\":\"m\"}",
    },
    {
        .dim  = 2,
        .dat  = { { 19, 23, 0 }, UNIT_NONE, 0 },
        .json = "{\"d\":[19,23],\"u\":\"none\"}",
    },
    {
        .dim  = 1,
        .dat  = { { 1, 0, 0 }, UNIT_BOOL, 0 },
        .json = "{\"d\":true,\"u\":\"none\"}",
    },
    {
        .dim  = 3,
        .dat  = { { 1, 0, 1 }, UNIT_BOOL, 0 },
        .json = "{\"d\":[true,false,true],\"u\":\"none\"}",
    },
    {
        .dim = 3,
        .dat = { { 1, 1, 1970 }, UNIT_DATE, 0 },
        .json = "{\"d\":[1,1,1970],\"u\":\"date\"}",
    },
    {
        .dim = 3,
        .dat = { { 23, 59, 14}, UNIT_TIME, 0 },
        .json = "\"d\":[23,59,14],\"u\":\"time\"}",
    }

};

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

    for (unsigned int i = 0; i < ARRAY_SIZE(dims); i++) {
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

static void test_unitstr__success(void)
{
    /* test the verbose cases */
    TEST_ASSERT_EQUAL_STRING("undefined", phydat_unit_to_str_verbose(UNIT_UNDEF));
    TEST_ASSERT_EQUAL_STRING("none", phydat_unit_to_str_verbose(UNIT_NONE));
    TEST_ASSERT_EQUAL_STRING("none", phydat_unit_to_str_verbose(UNIT_BOOL));
    TEST_ASSERT_EQUAL_STRING("time", phydat_unit_to_str_verbose(UNIT_TIME));
    TEST_ASSERT_EQUAL_STRING("date", phydat_unit_to_str_verbose(UNIT_DATE));

    TEST_ASSERT_EQUAL_STRING("°C", phydat_unit_to_str_verbose(UNIT_TEMP_C));
    TEST_ASSERT_EQUAL_STRING("°F", phydat_unit_to_str_verbose(UNIT_TEMP_F));
    TEST_ASSERT_EQUAL_STRING("K", phydat_unit_to_str_verbose(UNIT_TEMP_K));
    TEST_ASSERT_EQUAL_STRING("lx", phydat_unit_to_str_verbose(UNIT_LUX));
    TEST_ASSERT_EQUAL_STRING("m", phydat_unit_to_str_verbose(UNIT_M));
    TEST_ASSERT_EQUAL_STRING("m^2", phydat_unit_to_str_verbose(UNIT_M2));
    TEST_ASSERT_EQUAL_STRING("m^3", phydat_unit_to_str_verbose(UNIT_M3));
    TEST_ASSERT_EQUAL_STRING("g", phydat_unit_to_str_verbose(UNIT_G));
    TEST_ASSERT_EQUAL_STRING("dps", phydat_unit_to_str_verbose(UNIT_DPS));
    TEST_ASSERT_EQUAL_STRING("G", phydat_unit_to_str_verbose(UNIT_GR));
    TEST_ASSERT_EQUAL_STRING("A", phydat_unit_to_str_verbose(UNIT_A));
    TEST_ASSERT_EQUAL_STRING("V", phydat_unit_to_str_verbose(UNIT_V));
    TEST_ASSERT_EQUAL_STRING("W", phydat_unit_to_str_verbose(UNIT_W));
    TEST_ASSERT_EQUAL_STRING("dBm", phydat_unit_to_str_verbose(UNIT_DBM));
    TEST_ASSERT_EQUAL_STRING("Gs", phydat_unit_to_str_verbose(UNIT_GS));
    TEST_ASSERT_EQUAL_STRING("Bar", phydat_unit_to_str_verbose(UNIT_BAR));
    TEST_ASSERT_EQUAL_STRING("Pa", phydat_unit_to_str_verbose(UNIT_PA));
    TEST_ASSERT_EQUAL_STRING("permille", phydat_unit_to_str_verbose(UNIT_PERMILL));
    TEST_ASSERT_EQUAL_STRING("ppm", phydat_unit_to_str_verbose(UNIT_PPM));
    TEST_ASSERT_EQUAL_STRING("ppb", phydat_unit_to_str_verbose(UNIT_PPB));
    TEST_ASSERT_EQUAL_STRING("cd", phydat_unit_to_str_verbose(UNIT_CD));
    TEST_ASSERT_EQUAL_STRING("%", phydat_unit_to_str_verbose(UNIT_PERCENT));
    TEST_ASSERT_EQUAL_STRING("cts", phydat_unit_to_str_verbose(UNIT_CTS));
    TEST_ASSERT_EQUAL_STRING("C", phydat_unit_to_str_verbose(UNIT_COULOMB));
    TEST_ASSERT_EQUAL_STRING("g/m^3", phydat_unit_to_str_verbose(UNIT_GPM3));
    TEST_ASSERT_EQUAL_STRING("F", phydat_unit_to_str_verbose(UNIT_F));
    TEST_ASSERT_EQUAL_STRING("pH", phydat_unit_to_str_verbose(UNIT_PH));
    TEST_ASSERT_EQUAL_STRING("#/m^3", phydat_unit_to_str_verbose(UNIT_CPM3));
    TEST_ASSERT_EQUAL_STRING("ohm", phydat_unit_to_str_verbose(UNIT_OHM));

}

static void test_json__success(void)
{
    size_t len;

    len = phydat_to_json(&data[0].dat, data[0].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[0].json) + 1), len);
    len = phydat_to_json(&data[0].dat, data[0].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[0].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[0].json, (const char *)test);

    len = phydat_to_json(&data[1].dat, data[1].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[1].json) + 1), len);
    len = phydat_to_json(&data[1].dat, data[1].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[1].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[1].json, (const char *)test);

    len = phydat_to_json(&data[2].dat, data[2].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[2].json) + 1), len);
    len = phydat_to_json(&data[2].dat, data[2].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[2].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[2].json, (const char *)test);

    len = phydat_to_json(&data[3].dat, data[3].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[3].json) + 1), len);
    len = phydat_to_json(&data[3].dat, data[3].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[3].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[3].json, (const char *)test);

    len = phydat_to_json(&data[4].dat, data[4].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[4].json) + 1), len);
    len = phydat_to_json(&data[4].dat, data[4].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[4].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[4].json, (const char *)test);

    len = phydat_to_json(&data[5].dat, data[5].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[5].json) + 1), len);
    len = phydat_to_json(&data[5].dat, data[5].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[5].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[5].json, (const char *)test);
}

Test *tests_phydat_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_phydat_fit),
        new_TestFixture(test_unitstr__success),
        new_TestFixture(test_json__success),
    };

    EMB_UNIT_TESTCALLER(phydat_tests, NULL, NULL, fixtures);

    return (Test *)&phydat_tests;
}

void tests_phydat(void)
{
    TESTS_RUN(tests_phydat_tests());
}
