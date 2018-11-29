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
 * @brief   Unit tests for the export functionality of the RIOT Registry.
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

static char _name[REGISTRY_MAX_NAME_LEN];
static char _val[REGISTRY_MAX_VAL_LEN];
static int _export_func(const char *name, char *val)
{
    (void)name;
    (void)val;
    strcpy(_name, name);
    strcpy(_val, val);
    return 0;
}

static int _export_handler(int (*export_func)(const char *name, char *val),
                               int argc, char **argv, void *context)
{
    (void)export_func;
    (void)argc;
    (void)argv;
    (*(int *)context)++;
    char name[] = TEST_APP_TREE_L1 NAME_SEPARATOR TEST_APP_TREE_L2 \
                  NAME_SEPARATOR TEST_APP_TREE_L3;
    export_func(name, TEST_SET_VALUE_VAL);
    return 0;
}

static void test_export(void)
{
    int res;
    int num_calls = 0;
    char name[] = TEST_APP_TREE_L1 NAME_SEPARATOR TEST_APP_TREE_L2 \
                  NAME_SEPARATOR TEST_APP_TREE_L3;
    registry_handler_t h1 = {
        .name = TEST_APP_TREE_L1,
        .hndlr_export = _export_handler,
        .context = (void *)&num_calls
    };
    registry_handler_t h2 = {
        .name = TEST_SENSORS_TREE_L1,
        .hndlr_export = _export_handler,
        .context = (void *)&num_calls
    };

    registry_init();
    registry_register(&h1);
    registry_register(&h2);

    /* test non existant handler */
    res = registry_export(_export_func, "nonexistant");
    /* should not find a handler */
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
    /* should not call any callback function */
    TEST_ASSERT_EQUAL_INT(0, num_calls);

    num_calls = 0;
    res = registry_export(_export_func, TEST_APP_TREE_L1);
    TEST_ASSERT_EQUAL_INT(0, res);
    /* should only call export function for 'app' handler */
    TEST_ASSERT_EQUAL_INT(1, num_calls);
    TEST_ASSERT_EQUAL_STRING(name, _name);
    TEST_ASSERT_EQUAL_STRING(TEST_SET_VALUE_VAL, _val);

    num_calls = 0;
    res = registry_export(_export_func, NULL);
    TEST_ASSERT_EQUAL_INT(0, res);
    /* should call export function for every handler */
    TEST_ASSERT_EQUAL_INT(2, num_calls);
}

Test *tests_registry_export_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_export)
    };

    EMB_UNIT_TESTCALLER(registry_export_tests, NULL, NULL, fixtures);

    return (Test *)&registry_export_tests;
}
