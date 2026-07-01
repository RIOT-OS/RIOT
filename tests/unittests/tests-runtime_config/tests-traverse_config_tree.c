/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Unittests for runtime_config_traverse_config_tree
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
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
#include "runtime_config.h"

#include "tests-runtime_config.h"
#include "namespace/tests.h"
#include "namespace/tests/nested.h"

static runtime_config_error_t apply_schema_instance_cb(
    const runtime_config_schema_instance_t *const schema_instance)
{
    (void)schema_instance;

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_error_t apply_group_cb(
    const runtime_config_schema_instance_t *const schema_instance,
    const runtime_config_group_or_parameter_id_t group_or_parameter_id)
{
    (void)schema_instance;
    (void)group_or_parameter_id;

    return RUNTIME_CONFIG_ERROR_NONE;
}

typedef struct {
    uint32_t namespace;
    uint32_t schema;
    uint32_t instance;
    uint32_t group;
    uint32_t group_parameter;
    uint32_t parameter;
} node_counter_t;

static node_counter_t node_counter;

static void reset_node_counter(void)
{
    memset(&node_counter, 0, sizeof(node_counter));
}

static runtime_config_tests_nested_instance_t test_nested_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static runtime_config_schema_instance_t test_nested_instance_1 = {
#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME)
    .name = "test-nested-parameter-test",
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .apply_schema_instance_cb = apply_schema_instance_cb,
    .apply_group_or_parameter_cb = apply_group_cb,
};

static runtime_config_error_t traverse_cb(
    const runtime_config_node_t *node,
    const void *context)
{
    (void)context;

    switch (node->type) {
    case RUNTIME_CONFIG_NODE_TYPE_PARAMETER:
        if (node->as_parameter.parameter->schema->id != RUNTIME_CONFIG_TESTS_NESTED) {
            /* We only consider the nested schema for this test */
            return RUNTIME_CONFIG_ERROR_NONE;
        }
        if (node->as_parameter.parameter->id == RUNTIME_CONFIG_TESTS_NESTED_PARAMETER) {
            node_counter.parameter++;
        }
        if (node->as_parameter.parameter->id == RUNTIME_CONFIG_TESTS_NESTED_GROUP_PARAMETER) {
            node_counter.group_parameter++;
        }
        break;

    case RUNTIME_CONFIG_NODE_TYPE_GROUP:
        if (node->as_group.group->schema->id != RUNTIME_CONFIG_TESTS_NESTED) {
            /* We only consider the nested schema for this test */
            return RUNTIME_CONFIG_ERROR_NONE;
        }
        if (node->as_group.group->id == RUNTIME_CONFIG_TESTS_NESTED_GROUP) {
            node_counter.group++;
        }

        break;

    case RUNTIME_CONFIG_NODE_TYPE_SCHEMA_INSTANCE:
        if (node->as_schema_instance->schema->id != RUNTIME_CONFIG_TESTS_NESTED) {
            /* We only consider the nested schema for this test */
            return RUNTIME_CONFIG_ERROR_NONE;
        }
        node_counter.instance++;
        break;

    case RUNTIME_CONFIG_NODE_TYPE_SCHEMA:
        if (node->as_schema->id != RUNTIME_CONFIG_TESTS_NESTED) {
            /* We only consider the nested schema for this test */
            return RUNTIME_CONFIG_ERROR_NONE;
        }
        node_counter.schema++;
        break;

    case RUNTIME_CONFIG_NODE_TYPE_NAMESPACE:
        node_counter.namespace++;
        break;
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

static void test_runtime_config_setup(void)
{
    /* init runtime configuration module */
    runtime_config_init();

    /* add schema instances */
    runtime_config_add_schema_instance(&runtime_config_tests_nested, &test_nested_instance_1);
}

static void test_runtime_config_teardown(void)
{
}

static void tests_runtime_config_traverse_parameter(void)
{
    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_PARAMETER(
        &test_nested_instance_1,
        &runtime_config_tests_nested_parameter);

    reset_node_counter();
    const runtime_config_error_t res = runtime_config_traverse_config_tree(&node, &traverse_cb, 0, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_group(void)
{
    runtime_config_error_t res;
    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_GROUP(
        &test_nested_instance_1,
        &runtime_config_tests_nested_group);

    /* tree_traversal_depth 0 => group + everything under it gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 0, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 1 => only group gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 1, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 2 => group + 1 level => nested parameter is called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 2, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_instance(void)
{
    runtime_config_error_t res;
    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_SCHEMA_INSTANCE(
        &test_nested_instance_1);

    /* tree_traversal_depth 0 => instance + everything below gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 0, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 1 => only instance gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 1, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 2 => instance + 1 level (nested parameter is not called) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 2, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 3 => instance + everything below in this case */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 3, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_schema(void)
{
    runtime_config_error_t res;
    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_SCHEMA(
        &runtime_config_tests_nested);

    /* tree_traversal_depth 0 => schema + everything below gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 0, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 1 => only schema gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 1, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 2 => schema + 1 level (instance is called) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 2, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 3 => schema + 2 level (nested parameter is not called) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 3, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 4 => schema + everything below in this case */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 4, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_namespace(void)
{
    runtime_config_error_t res;
    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_NAMESPACE(
        &runtime_config_tests);

    /* tree_traversal_depth 0 => namespace + everything below gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 0, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 1 => only namespace gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 1, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 2 => namespace + 1 level (schema is called) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 2, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 3 => namespace + 2 level (schema and instance) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 3, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 4 => namespace + 3 level (nested parameter is not called) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 4, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 5 => namespace + everything below in this case */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(&node, &traverse_cb, 5, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_all(void)
{
    runtime_config_error_t res;

    /* tree_traversal_depth 0 => root + everything below gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(NULL, &traverse_cb, 0, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 1 => nothing gets called */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(NULL, &traverse_cb, 1, NULL);
    TEST_ASSERT_EQUAL_INT(0, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 2 => root + 1 level (namespace is called) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(NULL, &traverse_cb, 2, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(0, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 3 => root + 2 level (namespace and schema) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(NULL, &traverse_cb, 3, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(0, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 4 => root + 3 level (namespace, schema and instance) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(NULL, &traverse_cb, 4, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(0, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 5 => root + 4 level (nested parameter is not called) */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(NULL, &traverse_cb, 5, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(0, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 6 => root + everything below in this case */
    reset_node_counter();
    res = runtime_config_traverse_config_tree(NULL, &traverse_cb, 6, NULL);
    TEST_ASSERT_EQUAL_INT(1, node_counter.namespace);
    TEST_ASSERT_EQUAL_INT(1, node_counter.schema);
    TEST_ASSERT_EQUAL_INT(1, node_counter.instance);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group);
    TEST_ASSERT_EQUAL_INT(1, node_counter.group_parameter);
    TEST_ASSERT_EQUAL_INT(1, node_counter.parameter);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

Test *tests_runtime_config_traverse_config_tree_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(tests_runtime_config_traverse_parameter),
        new_TestFixture(tests_runtime_config_traverse_group),
        new_TestFixture(tests_runtime_config_traverse_instance),
        new_TestFixture(tests_runtime_config_traverse_schema),
        new_TestFixture(tests_runtime_config_traverse_namespace),
        new_TestFixture(tests_runtime_config_traverse_all),
    };

    EMB_UNIT_TESTCALLER(runtime_config_tests, test_runtime_config_setup, test_runtime_config_teardown, fixtures);

    return (Test *)&runtime_config_tests;
}
