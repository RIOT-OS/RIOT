/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>

#include "embUnit.h"

const unsigned external_test_was_linked_in = 1;

static void test_internal_test_was_linked_in(void)
{
    extern const unsigned internal_test_was_linked_in;
    TEST_ASSERT_EQUAL_INT(1, internal_test_was_linked_in);
}

static Test *out_of_tree_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_internal_test_was_linked_in),
    };

    EMB_UNIT_TESTCALLER(_out_of_tree_tests, NULL, NULL, fixtures);

    return (Test *)&_out_of_tree_tests;
}

void tests_out_of_tree(void)
{
    TESTS_RUN(out_of_tree_tests());
}
