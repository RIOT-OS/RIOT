/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    unittests
 * @brief       Unittests for the ``registry_string_path`` module
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
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
#include "fmt.h"
#include "assert.h"
#include "vfs.h"
#include "board.h"
#include "mtd.h"
#include "registry.h"
#include "registry/string_path.h"
#include "registry/namespace/tests.h"
#include "registry/namespace/tests/nested.h"

#if IS_USED(MODULE_REGISTRY_NAMESPACE_TESTS_NESTED) || IS_ACTIVE(DOXYGEN)

static registry_tests_nested_instance_t test_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static registry_instance_t test_instance = {
    .name = "instance-1",
    .data = &test_instance_data,
    .commit_cb = NULL,
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
        .location.parameter = &registry_tests_nested_group_parameter,
        .instance = &test_instance,
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
        .location.group = &registry_tests_nested_group,
        .instance = &test_instance,
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
        .instance = &test_instance,
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
        .location.schema = &registry_tests_nested,
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
        .location.namespace = &registry_tests,
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

    char str[] = "/tests/nested/instance-1/group/parameter";
    registry_node_from_string_path(str, sizeof(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_PARAMETER, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.location.parameter->schema->namespace->name);
    TEST_ASSERT_EQUAL_STRING("nested", node.location.parameter->schema->name);
    TEST_ASSERT_EQUAL_STRING("instance-1", node.instance->name);
    TEST_ASSERT_EQUAL_STRING("parameter", node.location.parameter->name);
}

static void tests_registry_from_group_string_path(void)
{
    registry_node_t node;

    char str[] = "/tests/nested/instance-1/group";
    registry_node_from_string_path(str, sizeof(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_GROUP, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.location.group->schema->namespace->name);
    TEST_ASSERT_EQUAL_STRING("nested", node.location.group->schema->name);
    TEST_ASSERT_EQUAL_STRING("instance-1", node.instance->name);
    TEST_ASSERT_EQUAL_STRING("group", node.location.group->name);
}

static void tests_registry_from_instance_string_path(void)
{
    registry_node_t node;

    char str[] = "/tests/nested/instance-1";
    registry_node_from_string_path(str, sizeof(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_INSTANCE, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.instance->schema->namespace->name);
    TEST_ASSERT_EQUAL_STRING("nested", node.instance->schema->name);
    TEST_ASSERT_EQUAL_STRING("instance-1", node.instance->name);
}

static void tests_registry_from_schema_string_path(void)
{
    registry_node_t node;

    char str[] = "/tests/nested";
    registry_node_from_string_path(str, sizeof(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_SCHEMA, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.location.schema->namespace->name);
    TEST_ASSERT_EQUAL_STRING("nested", node.location.schema->name);
}

static void tests_registry_from_namespace_string_path(void)
{
    registry_node_t node;

    char str[] = "/tests";
    registry_node_from_string_path(str, sizeof(str), &node);

    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_NAMESPACE, node.type);
    TEST_ASSERT_EQUAL_STRING("tests", node.location.namespace->name);
}

Test *tests_registry_string_path_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* to string_path */
        new_TestFixture(tests_registry_to_parameter_string_path),
        new_TestFixture(tests_registry_to_group_string_path),
        new_TestFixture(tests_registry_to_instance_string_path),
        new_TestFixture(tests_registry_to_schema_string_path),
        new_TestFixture(tests_registry_to_namespace_string_path),
        /* from string_path */
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

/** @} */
