/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Unit tests for the commit functionality of the RIOT Registry.
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "embUnit/embUnit.h"
#include "embUnit.h"
#include "registry/registry.h"
#include "tests-registry.h"

static int _commit_cb(void *context)
{
    (*(int *)context)++;
    return 0;
}

static void test_commit(void)
{
    int _commit_cb_calls = 0;
    registry_handler_t h1 = {
        .name = TEST_APP_TREE_L1,
        .hndlr_commit = _commit_cb,
        .context = &_commit_cb_calls
    }, h2 = {
        .name = TEST_SENSORS_TREE_L1,
        .hndlr_commit = _commit_cb,
        .context = &_commit_cb_calls
    };

    /* Re initialize module and register two handlers */
    registry_init();
    registry_register(&h1);
    registry_register(&h2);

    /* Commit callback is returning 0, this call should too */
    TEST_ASSERT_EQUAL_INT(0, registry_commit(TEST_APP_TREE_L1));
    /* The commit callback should have only been called once */
    TEST_ASSERT_EQUAL_INT(1, _commit_cb_calls);
    _commit_cb_calls = 0;

    /* Commit callback is returning 0, this call should too */
    TEST_ASSERT_EQUAL_INT(0, registry_commit(NULL));
    /* The commit callback should have only been called twice */
    TEST_ASSERT_EQUAL_INT(2, _commit_cb_calls);
}

Test *tests_registry_commit_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_commit)
    };

    EMB_UNIT_TESTCALLER(registry_commit_tests, NULL, NULL, fixtures);

    return (Test *)&registry_commit_tests;
}
