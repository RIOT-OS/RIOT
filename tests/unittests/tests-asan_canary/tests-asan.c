/*
 * Copyright (C) 2024 Christian Amsüss
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Christian Amsüss <chrysn@fsfe.org>
 */

#include <stdint.h>
#include <stdio.h>

#include "tests-asan.h"

static void __attribute__ ((noinline)) read_10(uint8_t *source)
{
    printf("Read %d\n", source[10]);
}

static void tests_read_beyond_buf(void)
{
    uint8_t source[5] = {0, };
    read_10(&source[0]);
}

Test *tests_asan_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_read_beyond_buf),
    };

    EMB_UNIT_TESTCALLER(asan_tests, NULL, NULL, fixtures);

    return (Test *)&asan_tests;
}

void tests_asan_canary(void)
{
    TESTS_RUN(tests_asan_tests());
}
