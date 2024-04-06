/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    unittests
 * @brief       Unittests for the ``registry_storage`` module
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <inttypes.h>
#include <errno.h>
#include "embUnit.h"
#include "fmt.h"
#include "assert.h"
#include "vfs.h"
#include "board.h"
#include "mtd.h"
#include "registry.h"
#include "registry/storage.h"

#include "tests-registry_storage.h"
#include "registry/namespace/tests.h"
#include "registry/namespace/tests/nested.h"

#if IS_USED(MODULE_REGISTRY_NAMESPACE_TESTS_NESTED) || IS_ACTIVE(DOXYGEN)

#define _TESTS_REGISTRY_LOAD_STORED_VALUE 60

static registry_tests_nested_instance_t test_nested_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static registry_instance_t test_nested_instance = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME)
    .name = "test-nested-parameter-test",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .commit_cb = NULL,
};

static int load(const registry_storage_instance_t *storage,
                const load_cb_t load_cb);
static int save(const registry_storage_instance_t *storage,
                const registry_node_t *node,
                const registry_value_t *value);

static registry_storage_t storage_test = {
    .load = load,
    .save = save,
};

static uint8_t storage_test_instance_data = 7;

static registry_storage_instance_t storage_test_instance = {
    .storage = &storage_test,
    .data = &storage_test_instance_data,
};

static int load(const registry_storage_instance_t *storage,
                const load_cb_t load_cb)
{
    if (storage == &storage_test_instance) {
        registry_node_t node = {
            .type = REGISTRY_NODE_PARAMETER,
            .location.parameter = &registry_tests_nested_parameter,
            .instance = &test_nested_instance,
        };
        uint8_t buf = _TESTS_REGISTRY_LOAD_STORED_VALUE;
        return load_cb(&node, &buf, sizeof(buf));
    }

    return -EINVAL;
}

static int save(const registry_storage_instance_t *storage,
                const registry_node_t *node,
                const registry_value_t *value)
{
    if (storage == &storage_test_instance &&
        node->instance == &test_nested_instance &&
        node->location.parameter == &registry_tests_nested_group_parameter &&
        value->buf == &test_nested_instance_data.group_parameter &&
        value->buf_len == sizeof(uint8_t) &&
        value->type == REGISTRY_TYPE_UINT8) {

        return 0;
    }

    return -EINVAL;
}

REGISTRY_ADD_STORAGE_SOURCE(storage_test_instance);
REGISTRY_SET_STORAGE_DESTINATION(storage_test_instance);

static void test_setup(void)
{
    /* init registry */
    registry_init();

    /* add schema instances */
    registry_add_schema_instance(&registry_tests_nested, &test_nested_instance);
}

static void tests_registry_load(void)
{
    /* check if the registry_load function gets the correct input values internally */
    TEST_ASSERT(registry_load() == 0);

    /* check if the load_cb sets the value to the registry */
    registry_value_t output;

    registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
        .location.parameter = &registry_tests_nested_parameter,
        .instance = &test_nested_instance,
    };

    registry_get(&node, &output);
    TEST_ASSERT_EQUAL_INT(_TESTS_REGISTRY_LOAD_STORED_VALUE, *(uint8_t *)output.buf);
}

static void tests_registry_save_parameter(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
        .location.parameter = &registry_tests_nested_group_parameter,
        .instance = &test_nested_instance,
    };
    TEST_ASSERT_EQUAL_INT(0, registry_save(&node));
}

static void tests_registry_save_group(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_GROUP,
        .location.group = &registry_tests_nested_group,
        .instance = &test_nested_instance,
    };
    TEST_ASSERT_EQUAL_INT(0, registry_save(&node));
}

static void tests_registry_save_instance(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_INSTANCE,
        .instance = &test_nested_instance,
    };
    TEST_ASSERT_EQUAL_INT(0, registry_save(&node));
}

static void tests_registry_save_schema(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_SCHEMA,
        .location.schema = &registry_tests_nested,
    };
    TEST_ASSERT_EQUAL_INT(0, registry_save(&node));
}

static void tests_registry_save_namespace(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_NAMESPACE,
        .location.namespace = &registry_tests,
    };
    TEST_ASSERT_EQUAL_INT(0, registry_save(&node));
}

static void tests_registry_save_all(void)
{
    TEST_ASSERT_EQUAL_INT(0, registry_save(NULL));
}

Test *tests_registry_storage_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* load */
        new_TestFixture(tests_registry_load),
        /* save */
        new_TestFixture(tests_registry_save_parameter),
        new_TestFixture(tests_registry_save_group),
        new_TestFixture(tests_registry_save_instance),
        new_TestFixture(tests_registry_save_schema),
        new_TestFixture(tests_registry_save_namespace),
        new_TestFixture(tests_registry_save_all),
    };

    EMB_UNIT_TESTCALLER(registry_storage_tests, test_setup, NULL, fixtures);

    return (Test *)&registry_storage_tests;
}

void tests_registry_storage(void)
{
    TESTS_RUN(tests_registry_storage_tests());
}

#endif

/** @} */
