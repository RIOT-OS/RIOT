/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for registry_export
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
#include "registry.h"

#include "tests-registry.h"
#include "namespace/tests.h"
#include "namespace/tests/nested.h"

static bool successful = false;

static registry_tests_nested_instance_t test_nested_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static registry_instance_t test_nested_instance_1 = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME)
    .name = "test-nested-parameter-test",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .apply_cb = NULL,
};

static registry_error_t export_parameter_cb(const registry_node_t *node,
                                            const void *context)
{
    (void)context;

    if (node->type == REGISTRY_NODE_PARAMETER && node->value.parameter.parameter != NULL &&
        node->value.parameter.parameter->id == *(registry_parameter_id_t *)context &&
        node->value.parameter.instance == &test_nested_instance_1) {
        successful = true;
    }

    return REGISTRY_ERROR_NONE;
}

static registry_error_t export_group_cb(const registry_node_t *node,
                                        const void *context)
{
    (void)context;

    if (node->type == REGISTRY_NODE_GROUP && node->value.group.group != NULL &&
        node->value.group.group->id == *(registry_group_id_t *)context) {
        successful = true;
    }

    return REGISTRY_ERROR_NONE;
}

static registry_error_t export_instance_cb(const registry_node_t *node,
                                           const void *context)
{
    (void)context;

    if (node->type == REGISTRY_NODE_INSTANCE && node->value.instance == &test_nested_instance_1) {
        successful = true;
    }

    return REGISTRY_ERROR_NONE;
}

static registry_error_t export_schema_cb(const registry_node_t *node,
                                         const void *context)
{
    (void)context;

    if (node->type == REGISTRY_NODE_SCHEMA && node->value.schema != NULL &&
        node->value.schema == &registry_tests_nested) {
        successful = true;
    }

    return REGISTRY_ERROR_NONE;
}

static registry_error_t export_namespace_cb(const registry_node_t *node,
                                            const void *context)
{
    (void)context;

    if (node->type == REGISTRY_NODE_NAMESPACE && node->value.namespace != NULL &&
        node->value.namespace == &registry_tests) {
        successful = true;
    }

    return REGISTRY_ERROR_NONE;
}

static void test_registry_setup(void)
{
    /* init registry */
    registry_init();

    /* add schema instances */
    registry_add_schema_instance(&registry_tests_nested, &test_nested_instance_1);
}

static void test_registry_teardown(void)
{
}

static void tests_registry_export_parameter(void)
{
    const registry_parameter_id_t parameter_id = REGISTRY_TESTS_NESTED_PARAMETER;

    successful = false;

    const registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
        .value.parameter = {
            .instance = &test_nested_instance_1,
            .parameter = &registry_tests_nested_parameter,
        },
    };

    registry_export(&node, &export_parameter_cb, 0, &parameter_id);

    TEST_ASSERT(successful);
}

static void tests_registry_export_group(void)
{
    const registry_node_t node = {
        .type = REGISTRY_NODE_GROUP,
        .value.group = {
            .instance = &test_nested_instance_1,
            .group = &registry_tests_nested_group,
        },
    };

    /* check if group gets exported */
    const registry_group_id_t group_id = REGISTRY_TESTS_NESTED_GROUP;

    successful = false;
    registry_export(&node, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);

    /* check that siblings get NOT exported */
    const registry_parameter_id_t sibling_parameter_id = REGISTRY_TESTS_NESTED_PARAMETER;
    successful = false;
    registry_export(&node, &export_parameter_cb, 0, &sibling_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);

    /* check if children get exported */
    const registry_parameter_id_t child_parameter_id = REGISTRY_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);

    /* tree_traversal_depth 1 => only group => parameter gets NOT exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 1, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);

    /* tree_traversal_depth 2 => group + 1 level more => parameter gets exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 2, &child_parameter_id);
    TEST_ASSERT(successful);
}

static void tests_registry_export_instance(void)
{
    const registry_node_t node = {
        .type = REGISTRY_NODE_INSTANCE,
        .value.instance = &test_nested_instance_1,
    };

    /* check if instance gets exported */
    successful = false;
    registry_export(&node, &export_instance_cb, 0, NULL);
    TEST_ASSERT(successful);

    /* check if group gets exported */
    const registry_group_id_t group_id = REGISTRY_TESTS_NESTED_GROUP;

    successful = false;
    registry_export(&node, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);

    /* check if parameter get exported */
    const registry_parameter_id_t child_parameter_id = REGISTRY_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);

    /* tree_traversal_depth 2 => only instance and group => parameter gets NOT exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 2, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);

    /* tree_traversal_depth 3 => instance, group and parameter => parameter gets exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 3, &child_parameter_id);
    TEST_ASSERT(successful);
}

static void tests_registry_export_schema(void)
{
    const registry_node_t node = {
        .type = REGISTRY_NODE_SCHEMA,
        .value.schema = &registry_tests_nested,
    };

    /* check if schema gets exported */
    successful = false;
    registry_export(&node, &export_schema_cb, 0, NULL);
    TEST_ASSERT(successful);

    /* check if instance gets exported */
    successful = false;
    registry_export(&node, &export_instance_cb, 0, NULL);
    TEST_ASSERT(successful);

    /* check if group gets exported */
    const registry_group_id_t group_id = REGISTRY_TESTS_NESTED_GROUP;

    successful = false;
    registry_export(&node, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);

    /* check if parameter get exported */
    const registry_parameter_id_t child_parameter_id = REGISTRY_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);

    /* tree_traversal_depth 3 => only schema, instance and group => parameter gets NOT exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 3, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);

    /* tree_traversal_depth 4 => schema, instance, group and parameter => parameter gets exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 4, &child_parameter_id);
    TEST_ASSERT(successful);
}

static void tests_registry_export_namespace(void)
{
    const registry_node_t node = {
        .type = REGISTRY_NODE_NAMESPACE,
        .value.namespace = &registry_tests,
    };

    /* check if namespace gets exported */
    successful = false;
    registry_export(&node, &export_namespace_cb, 0, NULL);
    TEST_ASSERT(successful);

    /* check if schema gets exported */
    successful = false;
    registry_export(&node, &export_schema_cb, 0, NULL);
    TEST_ASSERT(successful);

    /* check if instance gets exported */
    successful = false;
    registry_export(&node, &export_instance_cb, 0, NULL);
    TEST_ASSERT(successful);

    /* check if group gets exported */
    const registry_group_id_t group_id = REGISTRY_TESTS_NESTED_GROUP;

    successful = false;
    registry_export(&node, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);

    /* check if parameter get exported */
    const registry_parameter_id_t child_parameter_id = REGISTRY_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);

    /* tree_traversal_depth 4 => only namespace, schema, instance and group => parameter gets NOT exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 4, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);

    /* tree_traversal_depth 5 => namespace, schema, instance, group and parameter => parameter gets exported */
    successful = false;
    registry_export(&node, &export_parameter_cb, 5, &child_parameter_id);
    TEST_ASSERT(successful);
}

static void tests_registry_export_all(void)
{
    /* check if namespace gets exported */
    successful = false;
    registry_export(NULL, &export_namespace_cb, 0, NULL);
    TEST_ASSERT(successful);

    /* tree_traversal_depth 1 => nothing gets exported because node == NULL */
    /* => no namespace was specified and 1 means only export the exact match (NULL) */
    successful = false;
    registry_export(NULL, &export_namespace_cb, 1, NULL);
    TEST_ASSERT_EQUAL_INT(false, successful);

    /* check if schema gets exported */
    successful = false;
    registry_export(NULL, &export_schema_cb, 0, NULL);
    TEST_ASSERT(successful);

    /* check if instance gets exported */
    successful = false;
    registry_export(NULL, &export_instance_cb, 0, NULL);
    TEST_ASSERT(successful);

    /* check if group gets exported */
    const registry_group_id_t group_id = REGISTRY_TESTS_NESTED_GROUP;

    successful = false;
    registry_export(NULL, &export_group_cb, 0, &group_id);
    TEST_ASSERT(successful);

    /* check if parameter get exported */
    const registry_parameter_id_t child_parameter_id = REGISTRY_TESTS_NESTED_GROUP_PARAMETER;

    /* tree_traversal_depth 0 => infinite => parameter gets exported */
    successful = false;
    registry_export(NULL, &export_parameter_cb, 0, &child_parameter_id);
    TEST_ASSERT(successful);

    /* tree_traversal_depth 1 => nothing gets exported because node == NULL */
    /* => not even a namespace was specified and 1 means only export the exact match (NULL) */
    successful = false;
    registry_export(NULL, &export_parameter_cb, 1, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);

    /* tree_traversal_depth 5 => root, namespace, schema, instance, group */
    /* => parameter gets NOT exported */
    successful = false;
    registry_export(NULL, &export_parameter_cb, 5, &child_parameter_id);
    TEST_ASSERT_EQUAL_INT(false, successful);

    /* tree_traversal_depth 6 => root, namespace, schema, instance, group and parameter */
    /* => parameter gets exported */
    successful = false;
    registry_export(NULL, &export_parameter_cb, 6, &child_parameter_id);
    TEST_ASSERT(successful);
}

Test *tests_registry_export_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(tests_registry_export_parameter),
        new_TestFixture(tests_registry_export_group),
        new_TestFixture(tests_registry_export_instance),
        new_TestFixture(tests_registry_export_schema),
        new_TestFixture(tests_registry_export_namespace),
        new_TestFixture(tests_registry_export_all),
    };

    EMB_UNIT_TESTCALLER(registry_tests, test_registry_setup, test_registry_teardown, fixtures);

    return (Test *)&registry_tests;
}
