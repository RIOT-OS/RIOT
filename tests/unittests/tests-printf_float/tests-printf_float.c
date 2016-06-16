/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Implementations of unit tests for the SAUL registry
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "tests-printf_float.h"

#define BUFSIZE             (10)

static const float in0 = 2016.0349f;
static const float in1 = 123.4567f;
static const float in2 = 0.0f;

static void sfprintf_float(void)
{
    char tmp[BUFSIZE];
    char *str = tmp;

    snprintf(str, BUFSIZE, "%f", in0);
    TEST_ASSERT_EQUAL_STRING("2016.0349", str);

    snprintf(str, BUFSIZE, "%.2f", in0);
    TEST_ASSERT_EQUAL_STRING("2016.03", str);

    snprintf(str, BUFSIZE, "%.f", in0);
    TEST_ASSERT_EQUAL_STRING("2016", str);

    snprintf(str, BUFSIZE, "%.4f", in1);
    TEST_ASSERT_EQUAL_STRING("123.4567", str);

    snprintf(str, BUFSIZE, "%.1f", in1);
    TEST_ASSERT_EQUAL_STRING("123.5", str);

    snprintf(str, BUFSIZE, "%.2f", in1);
    TEST_ASSERT_EQUAL_STRING("123.46", str);

    snprintf(str, BUFSIZE, "%.f", in2);
    TEST_ASSERT_EQUAL_STRING("0", str);

    snprintf(str, BUFSIZE, "%.3f", in2);
    TEST_ASSERT_EQUAL_STRING("0.000", str);

    snprintf(str, BUFSIZE, "%2.04f", in2);
    TEST_ASSERT_EQUAL_STRING("0.0000", str);
}

Test *tests_printf_float_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(sfprintf_float)
    };

    EMB_UNIT_TESTCALLER(pkt_tests, NULL, NULL, fixtures);

    return (Test *)&pkt_tests;
}

void tests_printf_float(void)
{
    TESTS_RUN(tests_printf_float_tests());
}
