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

static bool successful = false;

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

static runtime_config_error_t export_parameter_cb(const runtime_config_node_t *node,
                                                  const void *context)
{
    (void)context;

    if (node->type == RUNTIME_CONFIG_NODE_TYPE_PARAMETER && node->as_parameter.parameter != NULL &&
        node->as_parameter.parameter->id == *(runtime_config_parameter_id_t *)context &&
        node->as_parameter.schema_instance == &test_nested_instance_1) {
        successful = true;
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_error_t export_group_cb(const runtime_config_node_t *node,
                                              const void *context)
{
    (void)context;

    if (node->type == RUNTIME_CONFIG_NODE_TYPE_GROUP && node->as_group.group != NULL &&
        node->as_group.group->id == *(runtime_config_group_id_t *)context) {
        successful = true;
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_error_t export_instance_cb(const runtime_config_node_t *node,
                                                 const void *context)
{
    (void)context;

    if (node->type == RUNTIME_CONFIG_NODE_TYPE_SCHEMA_INSTANCE && node->as_schema_instance == &test_nested_instance_1) {
        successful = true;
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_error_t export_schema_cb(const runtime_config_node_t *node,
                                               const void *context)
{
    (void)context;

    if (node->type == RUNTIME_CONFIG_NODE_TYPE_SCHEMA && node->as_schema != NULL &&
        node->as_schema == &runtime_config_tests_nested) {
        successful = true;
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_error_t export_namespace_cb(const runtime_config_node_t *node,
                                                  const void *context)
{
    (void)context;

    if (node->type == RUNTIME_CONFIG_NODE_TYPE_NAMESPACE && node->as_namespace != NULL &&
        node->as_namespace == &runtime_config_tests) {
        successful = true;
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
    const runtime_config_parameter_id_t parameter_id = RUNTIME_CONFIG_TESTS_NESTED_PARAMETER;

    successful = false;

    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_PARAMETER(
        &test_nested_instance_1,
        &runtime_config_tests_nested_parameter);

    const runtime_config_error_t res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 0, &parameter_id);

    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_group(void)
{
    runtime_config_error_t res;
    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_GROUP(
        &test_nested_instance_1,
        &runtime_config_tests_nested_group);

    /* check if group gets exported */
    const runtime_config_group_id_t group_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP;

    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check that siblings get NOT exported */
    const runtime_config_parameter_id_t sibling_parameter_id = RUNTIME_CONFIG_TESTS_NESTED_PARAMETER;
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 0, &sibling_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if children get exported */
    const runtime_config_parameter_id_t child_parameter_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 1 => only group => parameter gets NOT exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 1, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 2 => group + 1 level more => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 2, &child_parameter_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_instance(void)
{
    runtime_config_error_t res;
    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_SCHEMA_INSTANCE(
        &test_nested_instance_1);

    /* check if instance gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_instance_cb, 0, NULL);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if group gets exported */
    const runtime_config_group_id_t group_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP;

    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if parameter get exported */
    const runtime_config_parameter_id_t child_parameter_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 2 => only instance and group => parameter gets NOT exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 2, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 3 => instance, group and parameter => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 3, &child_parameter_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_schema(void)
{
    runtime_config_error_t res;
    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_SCHEMA(
        &runtime_config_tests_nested);

    /* check if schema gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_schema_cb, 0, NULL);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if instance gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_instance_cb, 0, NULL);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if group gets exported */
    const runtime_config_group_id_t group_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP;

    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if parameter get exported */
    const runtime_config_parameter_id_t child_parameter_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 3 => only schema, instance and group => parameter gets NOT exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 3, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 4 => schema, instance, group and parameter => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 4, &child_parameter_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_namespace(void)
{
    runtime_config_error_t res;
    const runtime_config_node_t node = RUNTIME_CONFIG_NODE_NAMESPACE(
        &runtime_config_tests);

    /* check if namespace gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_namespace_cb, 0, NULL);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if schema gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_schema_cb, 0, NULL);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if instance gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_instance_cb, 0, NULL);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if group gets exported */
    const runtime_config_group_id_t group_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP;

    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if parameter get exported */
    const runtime_config_parameter_id_t child_parameter_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 4 => only namespace, schema, instance and group => parameter gets NOT exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 4, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 5 => namespace, schema, instance, group and parameter => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(&node, &export_parameter_cb, 5, &child_parameter_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);
}

static void tests_runtime_config_traverse_all(void)
{
    runtime_config_error_t res;

    /* check if namespace gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(NULL, &export_namespace_cb, 0, NULL);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 1 => nothing gets exported because node == NULL */
    /* => no namespace was specified and 1 means only export the exact match (NULL) */
    successful = false;
    res = runtime_config_traverse_config_tree(NULL, &export_namespace_cb, 1, NULL);
    TEST_ASSERT_EQUAL_INT(false, successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if schema gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(NULL, &export_schema_cb, 0, NULL);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if instance gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(NULL, &export_instance_cb, 0, NULL);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if group gets exported */
    const runtime_config_group_id_t group_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP;

    successful = false;
    res = runtime_config_traverse_config_tree(NULL, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* check if parameter get exported */
    const runtime_config_parameter_id_t child_parameter_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(NULL, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 1 => nothing gets exported because node == NULL */
    /* => not even a namespace was specified and 1 means only export the exact match (NULL) */
    successful = false;
    res = runtime_config_traverse_config_tree(NULL, &export_parameter_cb, 1, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 5 => root, namespace, schema, instance, group */
    /* => parameter gets NOT exported */
    successful = false;
    res = runtime_config_traverse_config_tree(NULL, &export_parameter_cb, 5, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    /* tree_traversal_depth 6 => root, namespace, schema, instance, group and parameter */
    /* => parameter gets exported */
    successful = false;
    res = runtime_config_traverse_config_tree(NULL, &export_parameter_cb, 6, &child_parameter_id);
    TEST_ASSERT(successful);
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
