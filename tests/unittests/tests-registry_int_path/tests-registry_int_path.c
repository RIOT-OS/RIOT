/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    unittests
 * @brief       Unittests for the ``registry_int_path`` module
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#include "embUnit/embUnit.h"
#include "tests-registry_int_path.h"

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
#include "registry/int_path.h"
#include "registry/namespace/tests.h"
#include "registry/namespace/tests/nested.h"

#if IS_USED(MODULE_REGISTRY_NAMESPACE_TESTS_NESTED) || IS_ACTIVE(DOXYGEN)

static registry_tests_nested_instance_t test_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static registry_instance_t test_instance = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME)
    .name = "test-nested-parameter-test",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
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

/* to int_path */
static void tests_registry_to_parameter_int_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
        .instance = &test_instance,
        .location.parameter = &registry_tests_nested_parameter,
    };
    registry_int_path_t path;
    int res = registry_node_to_int_path(&node, &path);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(registry_tests.id, path.value.parameter_path.namespace_id);
    TEST_ASSERT_EQUAL_INT(registry_tests_nested.id, path.value.parameter_path.schema_id);
    TEST_ASSERT_EQUAL_INT(test_instance.id, path.value.parameter_path.instance_id);
    TEST_ASSERT_EQUAL_INT(registry_tests_nested_parameter.id, path.value.parameter_path.parameter_id);
}

static void tests_registry_to_group_int_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_GROUP,
        .instance = &test_instance,
        .location.group = &registry_tests_nested_group,
    };
    registry_int_path_t path;
    int res = registry_node_to_int_path(&node, &path);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(registry_tests.id, path.value.group_path.namespace_id);
    TEST_ASSERT_EQUAL_INT(registry_tests_nested.id, path.value.group_path.schema_id);
    TEST_ASSERT_EQUAL_INT(test_instance.id, path.value.group_path.instance_id);
    TEST_ASSERT_EQUAL_INT(registry_tests_nested_group.id, path.value.group_path.group_id);
}

static void tests_registry_to_instance_int_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_INSTANCE,
        .instance = &test_instance,
    };
    registry_int_path_t path;
    int res = registry_node_to_int_path(&node, &path);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(registry_tests.id, path.value.instance_path.namespace_id);
    TEST_ASSERT_EQUAL_INT(registry_tests_nested.id, path.value.instance_path.schema_id);
    TEST_ASSERT_EQUAL_INT(test_instance.id, path.value.instance_path.instance_id);
}

static void tests_registry_to_schema_int_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_SCHEMA,
        .location.schema = &registry_tests_nested,
    };
    registry_int_path_t path;
    int res = registry_node_to_int_path(&node, &path);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(registry_tests.id, path.value.schema_path.namespace_id);
    TEST_ASSERT_EQUAL_INT(registry_tests_nested.id, path.value.schema_path.schema_id);
}

static void tests_registry_to_namespace_int_path(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_NAMESPACE,
        .location.namespace = &registry_tests,
    };
    registry_int_path_t path;
    int res = registry_node_to_int_path(&node, &path);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(registry_tests.id, path.value.namespace_path.namespace_id);
}

/* from int_path */
static void tests_registry_from_group_or_parameter_int_path(void)
{
    registry_node_t node;

    /* parameter */
    const registry_int_path_t parameter_path = {
        .type = REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER,
        .value.group_or_parameter_path = {
            .namespace_id = registry_tests.id,
            .schema_id = registry_tests_nested.id,
            .instance_id = test_instance.id,
            .group_or_parameter_id = registry_tests_nested_group_parameter.id,
        },
    };

    int res = registry_node_from_int_path(&parameter_path, &node);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_PARAMETER, node.type);
    TEST_ASSERT_EQUAL_INT((int)&test_instance, (int)node.instance);
    TEST_ASSERT_EQUAL_INT((int)&registry_tests_nested_group_parameter, (int)node.location.parameter);


    /* group */
    const registry_int_path_t group_path = {
        .type = REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER,
        .value.group_or_parameter_path = {
            .namespace_id = registry_tests.id,
            .schema_id = registry_tests_nested.id,
            .instance_id = test_instance.id,
            .group_or_parameter_id = registry_tests_nested_group.id,
        },
    };

    res = registry_node_from_int_path(&group_path, &node);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_GROUP, node.type);
    TEST_ASSERT_EQUAL_INT((int)&test_instance, (int)node.instance);
    TEST_ASSERT_EQUAL_INT((int)&registry_tests_nested_group, (int)node.location.group);
}

static void tests_registry_from_parameter_int_path(void)
{
    registry_node_t node;

    const registry_int_path_t path = {
        .type = REGISTRY_INT_PATH_TYPE_PARAMETER,
        .value.parameter_path = {
            .namespace_id = registry_tests.id,
            .schema_id = registry_tests_nested.id,
            .instance_id = test_instance.id,
            .parameter_id = registry_tests_nested_group_parameter.id,
        },
    };

    int res = registry_node_from_int_path(&path, &node);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_PARAMETER, node.type);
    TEST_ASSERT_EQUAL_INT((int)&test_instance, (int)node.instance);
    TEST_ASSERT_EQUAL_INT((int)&registry_tests_nested_group_parameter, (int)node.location.parameter);
}

static void tests_registry_from_group_int_path(void)
{
    registry_node_t node;

    const registry_int_path_t path = {
        .type = REGISTRY_INT_PATH_TYPE_GROUP,
        .value.group_path = {
            .namespace_id = registry_tests.id,
            .schema_id = registry_tests_nested.id,
            .instance_id = test_instance.id,
            .group_id = registry_tests_nested_group.id,
        },
    };

    int res = registry_node_from_int_path(&path, &node);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_GROUP, node.type);
    TEST_ASSERT_EQUAL_INT((int)&test_instance, (int)node.instance);
    TEST_ASSERT_EQUAL_INT((int)&registry_tests_nested_group, (int)node.location.group);
}

static void tests_registry_from_instance_int_path(void)
{
    registry_node_t node;

    const registry_int_path_t path = {
        .type = REGISTRY_INT_PATH_TYPE_INSTANCE,
        .value.instance_path = {
            .namespace_id = registry_tests.id,
            .schema_id = registry_tests_nested.id,
            .instance_id = test_instance.id,
        },
    };

    int res = registry_node_from_int_path(&path, &node);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_INSTANCE, node.type);
    TEST_ASSERT_EQUAL_INT((int)&test_instance, (int)node.instance);
}

static void tests_registry_from_schema_int_path(void)
{
    registry_node_t node;

    const registry_int_path_t path = {
        .type = REGISTRY_INT_PATH_TYPE_SCHEMA,
        .value.schema_path = {
            .namespace_id = registry_tests.id,
            .schema_id = registry_tests_nested.id,
        },
    };

    int res = registry_node_from_int_path(&path, &node);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_SCHEMA, node.type);
    TEST_ASSERT_EQUAL_INT((int)&registry_tests_nested, (int)node.location.schema);
}

static void tests_registry_from_namespace_int_path(void)
{
    registry_node_t node;

    const registry_int_path_t path = {
        .type = REGISTRY_INT_PATH_TYPE_NAMESPACE,
        .value.namespace_path = {
            .namespace_id = registry_tests.id,
        },
    };

    int res = registry_node_from_int_path(&path, &node);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(REGISTRY_NODE_NAMESPACE, node.type);
    TEST_ASSERT_EQUAL_INT((int)&registry_tests, (int)node.location.namespace);
}

Test *tests_registry_int_path_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* to int_path */
        new_TestFixture(tests_registry_to_parameter_int_path),
        new_TestFixture(tests_registry_to_group_int_path),
        new_TestFixture(tests_registry_to_instance_int_path),
        new_TestFixture(tests_registry_to_schema_int_path),
        new_TestFixture(tests_registry_to_namespace_int_path),
        /* from int_path */
        new_TestFixture(tests_registry_from_group_or_parameter_int_path),
        new_TestFixture(tests_registry_from_parameter_int_path),
        new_TestFixture(tests_registry_from_group_int_path),
        new_TestFixture(tests_registry_from_instance_int_path),
        new_TestFixture(tests_registry_from_schema_int_path),
        new_TestFixture(tests_registry_from_namespace_int_path),
    };

    EMB_UNIT_TESTCALLER(registry_int_path_tests, test_setup, NULL, fixtures);

    return (Test *)&registry_int_path_tests;
}

void tests_registry_int_path(void)
{
    TESTS_RUN(tests_registry_int_path_tests());
}

#endif

/** @} */
