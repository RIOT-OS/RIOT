/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "embUnit.h"

const unsigned internal_test_was_linked_in = 1;

static void test_external_test_was_linked_in(void)
{
    extern const unsigned external_test_was_linked_in;
    TEST_ASSERT_EQUAL_INT(1, external_test_was_linked_in);
}

static Test *in_tree_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_external_test_was_linked_in),
    };

    EMB_UNIT_TESTCALLER(_in_tree_tests, NULL, NULL, fixtures);

    return (Test *)&_in_tree_tests;
}

void tests_in_tree(void)
{
    TESTS_RUN(in_tree_tests());
}
