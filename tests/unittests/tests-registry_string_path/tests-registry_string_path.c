/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    unittests
 * @brief       Unittests for the ``registry_string_path`` module
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include "embUnit/embUnit.h"
#include "tests-registry_string_path.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <inttypes.h>
#include <errno.h>
#include "embUnit.h"
#include "kernel_defines.h"
#include "fmt.h"
#include "assert.h"
#include "vfs.h"
#include "board.h"
#include "mtd.h"
#include "registry.h"
#include "registry/string_path.h"
#include "namespace/tests.h"
#include "namespace/tests/nested.h"

#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)

static registry_tests_nested_instance_t test_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static registry_instance_t test_instance = {
    .name = "instance-1",
    .data = &test_instance_data,
    .apply_cb = NULL,
};

static void test_setup(void)
{
    /* init registry */
    registry_init();

    /* add schema instances */
    registry_add_schema_instance(&registry_tests_nested, &test_instance);
}

/* to string_path */
static void tests_registry_to_parameter_string_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
        .value.parameter = {
            .instance = &test_instance,
            .parameter = &registry_tests_nested_group_parameter,
        },
    };

    int size = registry_node_to_string_path(&node, NULL);
    char path[size + 1];

    registry_node_to_string_path(&node, path);

    TEST_ASSERT_EQUAL_STRING("/tests/nested/instance-1/group/parameter", path);
}

static void tests_registry_to_group_string_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_GROUP,
        .value.group = {
            .instance = &test_instance,
            .group = &registry_tests_nested_group,
        },
    };

    int size = registry_node_to_string_path(&node, NULL);
    char path[size + 1];

    registry_node_to_string_path(&node, path);

    TEST_ASSERT_EQUAL_STRING("/tests/nested/instance-1/group", path);
}

static void tests_registry_to_instance_string_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_INSTANCE,
        .value.instance = &test_instance,
    };

    int size = registry_node_to_string_path(&node, NULL);
    char path[size + 1];

    registry_node_to_string_path(&node, path);

    TEST_ASSERT_EQUAL_STRING("/tests/nested/instance-1", path);
}

static void tests_registry_to_schema_string_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_SCHEMA,
        .value.schema = &registry_tests_nested,
    };

    int size = registry_node_to_string_path(&node, NULL);
    char path[size + 1];

    registry_node_to_string_path(&node, path);

    TEST_ASSERT_EQUAL_STRING("/tests/nested", path);
}

static void tests_registry_to_namespace_string_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_NAMESPACE,
        .value.namespace = &registry_tests,
    };

    int size = registry_node_to_string_path(&node, NULL);
    char path[size + 1];

    registry_node_to_string_path(&node, path);

    TEST_ASSERT_EQUAL_STRING("/tests", path);
}

/* from string_path */
static void tests_registry_from_parameter_string_path(void)
{
    registry_node_t node;

    const char *str[] = { "tests", "nested", "instance-1", "group", "parameter" };

    registry_node_from_string_path(str, ARRAY_SIZE(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_PARAMETER, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.value.parameter.parameter->schema->namespace->name);
    TEST_ASSERT_EQUAL_STRING("nested", node.value.parameter.parameter->schema->name);
    TEST_ASSERT_EQUAL_STRING("instance-1", node.value.parameter.instance->name);
    TEST_ASSERT_EQUAL_STRING("parameter", node.value.parameter.parameter->name);
}

static void tests_registry_from_group_string_path(void)
{
    registry_node_t node;

    const char *str[] = { "tests", "nested", "instance-1", "group" };

    registry_node_from_string_path(str, ARRAY_SIZE(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_GROUP, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.value.group.group->schema->namespace->name);
    TEST_ASSERT_EQUAL_STRING("nested", node.value.group.group->schema->name);
    TEST_ASSERT_EQUAL_STRING("instance-1", node.value.group.instance->name);
    TEST_ASSERT_EQUAL_STRING("group", node.value.group.group->name);
}

static void tests_registry_from_instance_string_path(void)
{
    registry_node_t node;

    const char *str[] = { "tests", "nested", "instance-1" };

    registry_node_from_string_path(str, ARRAY_SIZE(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_INSTANCE, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.value.instance->schema->namespace->name);
    TEST_ASSERT_EQUAL_STRING("nested", node.value.instance->schema->name);
    TEST_ASSERT_EQUAL_STRING("instance-1", node.value.instance->name);
}

static void tests_registry_from_schema_string_path(void)
{
    registry_node_t node;

    const char *str[] = { "tests", "nested" };

    registry_node_from_string_path(str, ARRAY_SIZE(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_SCHEMA, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.value.schema->namespace->name);
    TEST_ASSERT_EQUAL_STRING("nested", node.value.schema->name);
}

static void tests_registry_from_namespace_string_path(void)
{
    registry_node_t node;

    const char *str[] = { "tests" };

    registry_node_from_string_path(str, ARRAY_SIZE(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_NAMESPACE, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.value.namespace->name);
}

Test *tests_registry_string_path_tests(void)
{
    (void)tests_registry_to_parameter_string_path;
    (void)tests_registry_to_group_string_path;
    (void)tests_registry_to_instance_string_path;
    (void)tests_registry_to_schema_string_path;
    (void)tests_registry_to_namespace_string_path;
    (void)tests_registry_from_parameter_string_path;
    (void)tests_registry_from_group_string_path;
    (void)tests_registry_from_instance_string_path;
    (void)tests_registry_from_schema_string_path;
    (void)tests_registry_from_namespace_string_path;

    EMB_UNIT_TESTFIXTURES(fixtures){
        /* to string_path */
        new_TestFixture(tests_registry_to_parameter_string_path),
        new_TestFixture(tests_registry_to_group_string_path),
        new_TestFixture(tests_registry_to_instance_string_path),
        new_TestFixture(tests_registry_to_schema_string_path),
        new_TestFixture(tests_registry_to_namespace_string_path),
        // /* from string_path */
        new_TestFixture(tests_registry_from_parameter_string_path),
        new_TestFixture(tests_registry_from_group_string_path),
        new_TestFixture(tests_registry_from_instance_string_path),
        new_TestFixture(tests_registry_from_schema_string_path),
        new_TestFixture(tests_registry_from_namespace_string_path),
    };

    EMB_UNIT_TESTCALLER(registry_string_path_tests, test_setup, NULL, fixtures);

    return (Test *)&registry_string_path_tests;
}

void tests_registry_string_path(void)
{
    TESTS_RUN(tests_registry_string_path_tests());
}

#endif
