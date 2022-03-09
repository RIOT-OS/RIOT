/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <string.h>

#include "embUnit.h"

#include "net/netopt.h"

#include "unittests-constants.h"
#include "tests-netopt.h"

void test_netopt2str(void)
{
    /* here we just test if netopt2str has a value defined for any given
     * NETOPT option code */
    for (int i = 0; i < NETOPT_NUMOF; i++) {
        TEST_ASSERT(strcmp(netopt2str(i), "unknown"));
    }
}

void test_netopt2str_unknown(void)
{
    TEST_ASSERT_EQUAL_INT(0, strcmp(netopt2str(NETOPT_NUMOF), "unknown"));
}

Test *tests_netopt_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_netopt2str),
        new_TestFixture(test_netopt2str_unknown),
    };

    EMB_UNIT_TESTCALLER(netopt_tests, NULL, NULL, fixtures);

    return (Test *)&netopt_tests;
}

void tests_netopt(void)
{
    TESTS_RUN(tests_netopt_tests());
}
/** @} */
