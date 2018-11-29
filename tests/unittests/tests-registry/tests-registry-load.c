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
 * @brief   Unit tests for the loading mechanism of configuration parameters
 *          via the RIOT Registry.
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "embUnit/embUnit.h"
#include "embUnit.h"
#include "registry/registry.h"
#include "tests-registry.h"

static int _test_load_itf_load(registry_store_t *store, load_cb_t cb,
                               void *cb_arg)
{
    (void)store;
    char name[] = TEST_APP_TREE_L1 NAME_SEPARATOR TEST_APP_TREE_L2 \
                  NAME_SEPARATOR TEST_APP_TREE_L3;
    cb(name, TEST_SET_VALUE_VAL, cb_arg);
    return 0;
}

static int _argc = 0;
static char *_argv[REGISTRY_MAX_DIR_DEPTH];
static char *_val;

static int _set_handler(int argc, char **argv, char *val, void *context)
{
    (void) context;
    _argc = argc;
    _val = val;
    memcpy(_argv, argv, REGISTRY_MAX_DIR_DEPTH);
    return 0;
}

static void test_load(void)
{
    registry_store_itf_t interface = {.load = _test_load_itf_load};
    registry_store_t store = {.itf = &interface};
    registry_handler_t handler = {
        .name = TEST_APP_TREE_L1,
        .hndlr_set = _set_handler
    };
    char *tree[] = {TEST_APP_TREE_L2, TEST_APP_TREE_L3};

    registry_init();
    registry_register(&handler);
    registry_src_register(&store);
    registry_load();

    /* '-1' because the configuration group name is not passed to the set
     * callback */
    TEST_ASSERT_EQUAL_INT(TEST_APP_TREE_LEN - 1, _argc);
    TEST_ASSERT_EQUAL_STRING(TEST_SET_VALUE_VAL, _val);
    for (int i = 0; i < TEST_APP_TREE_LEN - 1; i++) {
        TEST_ASSERT_EQUAL_STRING(tree[i], _argv[i]);
    }
}

Test *tests_registry_load_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_load)
    };

    EMB_UNIT_TESTCALLER(registry_load_tests, NULL, NULL, fixtures);

    return (Test *)&registry_load_tests;
}
