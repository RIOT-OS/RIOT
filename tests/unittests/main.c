/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "unittests.h"
#include "map.h"

#include "lpm.h"

#define UNCURRY(FUN, ARGS) FUN(ARGS)
#define RUN_TEST_SUITES(...) MAP(RUN_TEST_SUITE, __VA_ARGS__)
#define RUN_TEST_SUITE(TEST_SUITE) \
    do { \
        extern void tests_##TEST_SUITE(void); \
        tests_##TEST_SUITE(); \
    } while (0);

int main(void)
{
#ifdef OUTPUT
    TextUIRunner_setOutputter(OUTPUTTER);
#endif

    TESTS_START();
#ifndef NO_TEST_SUITES
    UNCURRY(RUN_TEST_SUITES, TEST_SUITES)
#endif
    TESTS_END();

    lpm_set(LPM_POWERDOWN);
    return 0;
}
