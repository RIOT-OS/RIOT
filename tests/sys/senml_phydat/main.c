/*
 * SPDX-FileCopyrightText: 2021 Silke Hofstra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       SenML Phydat tests
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "embUnit.h"
#include "flash_utils.h"
#include "senml/phydat.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef PRIi64
#define PRIi64 "lli"
#endif

typedef struct {
    phydat_t phydat;
    senml_value_t senml;
} time_test_t;

typedef struct {
    phydat_t phydat;
    senml_value_t senml1;
    senml_value_t senml2;
    uint8_t dim;
} value_test_t;

#define senml_s(s) { .attr = { .unit = SENML_UNIT_SECOND }, \
                     .value = { SENML_TYPE_NUMERIC_INT, { .i = s } } }

#define senml_f(v, u) { .attr = { .unit = u }, \
                        .value = { SENML_TYPE_NUMERIC_FLOAT, { .f = v } } }

#define senml_df(m, e, u) { .attr = { .unit = u }, \
                            .value = { SENML_TYPE_NUMERIC_DECFRAC, { .df = { e, m } } } }

static value_test_t value_tests[] = {
    {
        .phydat = { { 360, 0, 0 }, UNIT_M, 6 },
        .senml1 = senml_f(360e6, SENML_UNIT_METER),
        .senml2 = senml_df(360, 6, SENML_UNIT_METER),
        .dim = 0
    },
    {
        .phydat = { { 864, 0, 0 }, UNIT_TIME, 2 },
        .senml1 = senml_f(86400, SENML_UNIT_SECOND),
        .senml2 = senml_df(864, 2, SENML_UNIT_SECOND),
        .dim = 0
    },
    {
        .phydat = { { 0, 144, 0 }, UNIT_TIME, 1 },
        .senml1 = senml_f(1440, SENML_UNIT_MINUTE),
        .senml2 = senml_df(144, 1, SENML_UNIT_MINUTE),
        .dim = 1
    },
    {
        .phydat = { { 0, 0, 24 }, UNIT_TIME, 0 },
        .senml1 = senml_f(24, SENML_UNIT_HOUR),
        .senml2 = senml_df(24, 0, SENML_UNIT_HOUR),
        .dim = 2
    },
    {
        .phydat = { { 0, 0, 0 }, UNIT_TEMP_F, 3 },
        .senml1 = senml_f(255.37, SENML_UNIT_KELVIN),
        .senml2 = senml_df(0, 3, SENML_UNIT_NONE),
    },
    {
        .phydat = { { 314, 0, 0 }, UNIT_G_FORCE, -2 },
        .senml1 = senml_f(30.792881, SENML_UNIT_METER_PER_SQUARE_SECOND),
        .senml2 = senml_df(314, -2, SENML_UNIT_NONE),
    },
    {
        .phydat = { { 988, 0, 0 }, UNIT_BAR, -3 },
        .senml1 = senml_f(98.8e3, SENML_UNIT_PASCAL),
        .senml2 = senml_df(988, 2, SENML_UNIT_PASCAL),
    },
    {
        .phydat = { { 193, 0, 0 }, UNIT_GPM3, 5 },
        .senml1 = senml_f(19.3e3, SENML_UNIT_KILOGRAM_PER_CUBIC_METER),
        .senml2 = senml_df(193, 2, SENML_UNIT_KILOGRAM_PER_CUBIC_METER),
    },
    {
        .phydat = { { 220, 0, 0 }, UNIT_GAUSS, 3 },
        .senml1 = senml_f(22, SENML_UNIT_TESLA),
        .senml2 = senml_df(220, -1, SENML_UNIT_TESLA),
    }
};

void test_phydat_to_senml_float(void)
{
    senml_value_t res;
    char unit_buf[10];
    size_t size = 0;

    for (size_t i = 0; i < ARRAY_SIZE(value_tests); i++) {
        senml_value_t *exp = &(value_tests[i].senml1);

        phydat_to_senml_float(&res, &(value_tests[i].phydat), value_tests[i].dim);

        size = phydat_unit_write(unit_buf, ARRAY_SIZE(unit_buf), value_tests[i].phydat.unit);
        unit_buf[size] = 0;
        DEBUG("Float: %" PRIi16 "e%" PRIi16 " %" PRIsflash " -> %.f %s\n",
              value_tests[i].phydat.val[value_tests[i].dim], value_tests[i].phydat.scale,
              unit_buf,
              res.value.value.f,
              senml_unit_to_str(res.attr.unit));

        TEST_ASSERT_EQUAL_STRING(senml_unit_to_str(exp->attr.unit),
                                 senml_unit_to_str(res.attr.unit));
        TEST_ASSERT_EQUAL_INT(exp->value.type, res.value.type);
        TEST_ASSERT_EQUAL_INT((int)roundf(exp->value.value.f * 100),
                              (int)roundf(res.value.value.f * 100));
    }
}

void test_phydat_to_senml_decimal(void)
{
    senml_value_t res;
    char unit_buf[10];
    size_t size = 0;

    for (size_t i = 0; i < ARRAY_SIZE(value_tests); i++) {
        senml_value_t *exp = &(value_tests[i].senml2);

        phydat_to_senml_decimal(&res, &(value_tests[i].phydat), value_tests[i].dim);

        size = phydat_unit_write(unit_buf, ARRAY_SIZE(unit_buf), value_tests[i].phydat.unit);
        unit_buf[size] = 0;

        DEBUG("Decimal: %" PRIi16 "e%" PRIi16 " %s -> %" PRIi32 "e%" PRIi32 " %" PRIsflash"\n",
              value_tests[i].phydat.val[value_tests[i].dim], value_tests[i].phydat.scale,
              unit_buf,
              res.value.value.df.m, res.value.value.df.e,
              senml_unit_to_str(res.attr.unit));

        TEST_ASSERT_EQUAL_STRING(senml_unit_to_str(exp->attr.unit),
                                 senml_unit_to_str(res.attr.unit));
        TEST_ASSERT_EQUAL_INT(exp->value.type, res.value.type);
        TEST_ASSERT_EQUAL_INT(exp->value.value.df.m, res.value.value.df.m);
        TEST_ASSERT_EQUAL_INT(exp->value.value.df.e, res.value.value.df.e);
    }
}

Test *tests_senml(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
/* Don't run this test on CPUs with unpredictable rounding */
#if !defined(__AVR__) && !defined(__MSP430__)
        new_TestFixture(test_phydat_to_senml_float),
#endif
        new_TestFixture(test_phydat_to_senml_decimal),
    };
    EMB_UNIT_TESTCALLER(senml_tests, NULL, NULL, fixtures);
    return (Test *)&senml_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_senml());
    TESTS_END();
    return 0;
}
