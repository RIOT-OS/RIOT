/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for registry_commit
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

#include "tests-registry.h"
#include "registry/namespace/tests.h"
#include "registry/namespace/tests/nested.h"

#if IS_USED(MODULE_REGISTRY_NAMESPACE_TESTS_NESTED) || IS_ACTIVE(DOXYGEN)

static bool successful = false;
static registry_group_or_parameter_id_t parameter_id;
static registry_group_or_parameter_id_t group_id;

static int commit_parameter_cb(const registry_commit_cb_scope_t scope,
                               const registry_group_or_parameter_id_t *group_or_parameter_id,
                               const void *context)
{
    (void)context;

    if (scope == REGISTRY_COMMIT_PARAMETER && group_or_parameter_id != NULL &&
        *group_or_parameter_id == parameter_id) {
        successful = true;
    }

    return 0;
}

static int commit_group_cb(const registry_commit_cb_scope_t scope,
                           const registry_group_or_parameter_id_t *group_or_parameter_id,
                           const void *context)
{
    (void)context;

    if (scope == REGISTRY_COMMIT_GROUP && group_or_parameter_id != NULL &&
        *group_or_parameter_id == group_id) {
        successful = true;
    }

    return 0;
}

static int commit_instance_cb(const registry_commit_cb_scope_t scope,
                              const registry_group_or_parameter_id_t *group_or_parameter_id,
                              const void *context)
{
    (void)context;

    if (scope == REGISTRY_COMMIT_INSTANCE && group_or_parameter_id == NULL) {
        successful = true;
    }

    return 0;
}

static registry_tests_nested_instance_t test_nested_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static registry_instance_t test_nested_instance_parameter_test = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME)
    .name = "test-nested-parameter-test",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .commit_cb = &commit_parameter_cb,
};

static registry_instance_t test_nested_instance_group_test = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME)
    .name = "test-nested-group-test",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .commit_cb = &commit_group_cb,
};

static registry_instance_t test_nested_instance_instance_test = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME)
    .name = "test-nested-instance-test",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .commit_cb = &commit_instance_cb,
};

static void test_registry_setup(void)
{
    /* init registry */
    registry_init();

    /* add schema instances */
    registry_add_schema_instance(&registry_tests_nested, &test_nested_instance_parameter_test);
    registry_add_schema_instance(&registry_tests_nested, &test_nested_instance_group_test);
    registry_add_schema_instance(&registry_tests_nested, &test_nested_instance_instance_test);
}

static void test_registry_teardown(void)
{
}

static void tests_registry_commit_parameter(void)
{
    successful = false;
    parameter_id = REGISTRY_TESTS_NESTED_PARAMETER;

    const registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
        .location.parameter = &registry_tests_nested_parameter,
        .instance = &test_nested_instance_parameter_test,
    };

    registry_commit(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_commit_group(void)
{
    successful = false;
    group_id = REGISTRY_TESTS_NESTED_GROUP;

    const registry_node_t node = {
        .type = REGISTRY_NODE_GROUP,
        .location.group = &registry_tests_nested_group,
        .instance = &test_nested_instance_group_test,
    };

    registry_commit(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_commit_instance(void)
{
    successful = false;

    const registry_node_t node = {
        .type = REGISTRY_NODE_INSTANCE,
        .instance = &test_nested_instance_instance_test,
    };

    registry_commit(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_commit_schema(void)
{
    successful = false;

    const registry_node_t node = {
        .type = REGISTRY_NODE_SCHEMA,
        .location.schema = &registry_tests_nested,
    };

    registry_commit(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_commit_namespace(void)
{
    successful = false;

    const registry_node_t node = {
        .type = REGISTRY_NODE_NAMESPACE,
        .location.namespace = &registry_tests,
    };

    registry_commit(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_commit_all(void)
{
    successful = false;
    registry_commit(NULL);
    TEST_ASSERT(successful);
}

Test *tests_registry_commit_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_registry_commit_parameter),
        new_TestFixture(tests_registry_commit_group),
        new_TestFixture(tests_registry_commit_instance),
        new_TestFixture(tests_registry_commit_schema),
        new_TestFixture(tests_registry_commit_namespace),
        new_TestFixture(tests_registry_commit_all),
    };

    EMB_UNIT_TESTCALLER(registry_tests, test_registry_setup, test_registry_teardown, fixtures);

    return (Test *)&registry_tests;
}

#endif

/** @} */
