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
 * @brief   Unit tests for the saving mechanism of configuration parameters
 *          via the RIOT Registry.
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

char _test_save_name[REGISTRY_MAX_NAME_LEN];
char _test_save_value[REGISTRY_MAX_VAL_LEN];
int _test_save_itf_save(registry_store_t *store, const char *name,
                    const char *value)
{
    (void)store;
    strcpy(_test_save_name, name);
    strcpy(_test_save_value, value);
    return 0;
}

int _test_save_cb_export(int (*export_func)(const char *name, char *val),
                         int argc, char **argv, void *context)
{
    (void)context;
    (void)argc;
    (void)argv;
    char name[] = TEST_APP_TREE_L1 NAME_SEPARATOR TEST_APP_TREE_L2 \
                  NAME_SEPARATOR TEST_APP_TREE_L3;
    return export_func(name, TEST_SET_VALUE_VAL);
}

int _test_save_itf_load_non_existant(registry_store_t *store, load_cb_t cb,
                                     void *cb_arg)
{
    (void)store;
    char name[] = "non/existant/parameter";
    char value[] = "value";
    cb(name, value, cb_arg);
    return 0;
}

int _test_save_itf_load_existant(registry_store_t *store, load_cb_t cb,
                                 void *cb_arg)
{
    (void)store;
    char name[] = TEST_APP_TREE_L1 NAME_SEPARATOR TEST_APP_TREE_L2 \
                  NAME_SEPARATOR TEST_APP_TREE_L3;
    char value[] = TEST_SET_VALUE_VAL;
    cb(name, value, cb_arg);
    return 0;
}

static void test_save(void)
{
    /* the registry checks for duplicates on the selected storage, so the load
     * function of the interface will be called. To test this two load functions
     * are created, one that simulates not having the value already stored and
     * a second one that simulates having it. This tests the duplicate check. */
    registry_store_itf_t interface = {
        .save = _test_save_itf_save,
        .load = _test_save_itf_load_non_existant
    };
    registry_store_t store = {.itf = &interface};
    registry_handler_t handler = {
        .name = TEST_APP_TREE_L1,
        .hndlr_export = _test_save_cb_export
    };

    char name[] = TEST_APP_TREE_L1 NAME_SEPARATOR TEST_APP_TREE_L2 \
                  NAME_SEPARATOR TEST_APP_TREE_L3;

    registry_init();
    registry_register(&handler);
    registry_dst_register(&store);
    int res = registry_save();

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING(TEST_SET_VALUE_VAL, _test_save_value);
    TEST_ASSERT_EQUAL_STRING(name, _test_save_name);

    interface.load = _test_save_itf_load_existant;
    res = registry_save();
    TEST_ASSERT_EQUAL_INT(-EEXIST, res);
}

Test *tests_registry_save_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_save)
    };

    EMB_UNIT_TESTCALLER(registry_save_tests, NULL, NULL, fixtures);

    return (Test *)&registry_save_tests;
}
