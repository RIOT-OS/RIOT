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
 * @brief   Unit tests for setting and getting configuration parameters via
 *          the RIOT Registry.
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

static int _argc = 0;
static char *_argv[REGISTRY_MAX_DIR_DEPTH];
static char *_val;

static int _set_cb(int argc, char **argv, char *val, void *context)
{
    (void) context;
    _argc = argc;
    _val = val;
    memcpy(_argv, argv, REGISTRY_MAX_DIR_DEPTH);
    return 0;
}

static void test_set_value(void)
{
    registry_handler_t h = {
        .name = TEST_APP_TREE_L1,
        .hndlr_set = _set_cb
    };

    char name[] = TEST_APP_TREE_L1 NAME_SEPARATOR TEST_APP_TREE_L2 \
                  NAME_SEPARATOR TEST_APP_TREE_L3;
    char *tree[] = {TEST_APP_TREE_L2, TEST_APP_TREE_L3};

    registry_init();
    registry_register(&h);
    registry_set_value(name, TEST_SET_VALUE_VAL);

    TEST_ASSERT_EQUAL_INT(TEST_APP_TREE_LEN - 1, _argc);
    TEST_ASSERT_EQUAL_STRING(TEST_SET_VALUE_VAL, _val);
    for (int i = 0; i < TEST_APP_TREE_LEN - 1; i++) {
        TEST_ASSERT_EQUAL_STRING(tree[i], _argv[i]);
    }
}

static char *_get_value_argv[REGISTRY_MAX_DIR_DEPTH];
static char _get_value_response[] = TEST_SET_VALUE_VAL;

static char *_get_cb(int argc, char **argv, char *val, int val_len_max,
                     void *context)
{
    (void)val;
    (void)val_len_max;
    *(int *)context = argc;
    for (int i = 0; i < argc; i++) {
        _get_value_argv[i] = argv[i];
    }
    return _get_value_response;
}

static void test_get_value(void)
{
    int _get_value_argc = 0;
    registry_handler_t h1 = {
        .name = TEST_APP_TREE_L1,
        .hndlr_get = _get_cb,
        .context = &_get_value_argc
    };

    registry_handler_t h2 = {
        .name = TEST_SENSORS_TREE_L1
    };

    /* Name of the 'app' parameter to get the value */
    char name1[] = TEST_APP_TREE_L1 NAME_SEPARATOR TEST_APP_TREE_L2 \
                   NAME_SEPARATOR TEST_APP_TREE_L3;

    /* Name of the 'sensors' parameter to get the value */
    char name2[] = TEST_SENSORS_TREE_L1 NAME_SEPARATOR TEST_SENSORS_TREE_L2;

    /* Sections of the 'app' tree that should be passed to the 'get'
     * callback.
     */
    char *tree[] = {TEST_APP_TREE_L2, TEST_APP_TREE_L3};

    char *res1, *res2;

    /* Re initialize module and register two handlers */
    registry_init();
    registry_register(&h1);
    registry_register(&h2);

    /* Try to get the parameters values. This should call the 'get' callback
     * function if any.
     */
    res1 = registry_get_value(name1, NULL, 0);
    res2 = registry_get_value(name2, NULL, 0);

    /* The 'get' callback should be called without the L1 level of the name
     * tree.
     */
    TEST_ASSERT_EQUAL_INT(TEST_APP_TREE_LEN - 1, _get_value_argc);
    TEST_ASSERT_EQUAL_STRING(TEST_SET_VALUE_VAL, res1);
    for (int i = 0; i < TEST_APP_TREE_LEN - 1; i++) {
        TEST_ASSERT_EQUAL_STRING(tree[i], _get_value_argv[i]);
    }
    /* If there is no 'get' callback function registered it should return
     * NULL.
     */
    TEST_ASSERT_NULL(res2);
}


Test *tests_registry_set_get_value_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_set_value),
        new_TestFixture(test_get_value),
    };

    EMB_UNIT_TESTCALLER(registry_set_get_value_tests, NULL, NULL, fixtures);

    return (Test *)&registry_set_get_value_tests;
}
