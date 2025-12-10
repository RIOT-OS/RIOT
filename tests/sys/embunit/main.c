/*
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
  * @ingroup    tests
  * @{
  *
  * @file
  * @brief      Unit tests for embUnit
  *
  * @author     Sebastian Meiling <s@mlng.net>
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "embUnit.h"

#define TEST_STRING_7   ("7")

static const char *num = "7";

static void test_string_macro(void)
{
    TEST_ASSERT_EQUAL_STRING(TEST_STRING_7, num);
}

static void test_string_array(void)
{
    int digit = 7;
    char should_be[2] = { '0' + digit, 0 };
    TEST_ASSERT_EQUAL_STRING(should_be, num);
}

TestRef test_embunit(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_string_macro),
        new_TestFixture(test_string_array)
    };

    EMB_UNIT_TESTCALLER(tests_embunit, NULL, NULL, fixtures);
    return (TestRef) & tests_embunit;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(test_embunit());
    TESTS_END();
}
