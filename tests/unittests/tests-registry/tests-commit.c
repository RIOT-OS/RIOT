/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for registry_apply
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
static registry_group_or_parameter_id_t parameter_id;
static registry_group_or_parameter_id_t group_id;

static registry_error_t apply_parameter_cb(const registry_apply_cb_scope_t scope,
                                            const registry_group_or_parameter_id_t *
                                                group_or_parameter_id,
                                            const void *context)
{
    (void)context;

    if (scope == REGISTRY_APPLY_PARAMETER && group_or_parameter_id != NULL &&
        *group_or_parameter_id == parameter_id) {
        successful = true;
    }

    return REGISTRY_ERROR_NONE;
}

static registry_error_t apply_group_cb(const registry_apply_cb_scope_t scope,
                                        const registry_group_or_parameter_id_t *
                                            group_or_parameter_id,
                                        const void *context)
{
    (void)context;

    if (scope == REGISTRY_APPLY_GROUP && group_or_parameter_id != NULL &&
        *group_or_parameter_id == group_id) {
        successful = true;
    }

    return REGISTRY_ERROR_NONE;
}

static registry_error_t apply_instance_cb(const registry_apply_cb_scope_t scope,
                                           const registry_group_or_parameter_id_t *
                                               group_or_parameter_id,
                                           const void *context)
{
    (void)context;

    if (scope == REGISTRY_APPLY_INSTANCE && group_or_parameter_id == NULL) {
        successful = true;
    }

    return REGISTRY_ERROR_NONE;
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
    .apply_cb = &apply_parameter_cb,
};

static registry_instance_t test_nested_instance_group_test = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME)
    .name = "test-nested-group-test",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .apply_cb = &apply_group_cb,
};

static registry_instance_t test_nested_instance_instance_test = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME)
    .name = "test-nested-instance-test",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .apply_cb = &apply_instance_cb,
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

static void tests_registry_apply_parameter(void)
{
    successful = false;
    parameter_id = REGISTRY_TESTS_NESTED_PARAMETER;

    const registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
        .value.parameter = {
            .instance = &test_nested_instance_parameter_test,
            .parameter = &registry_tests_nested_parameter,
        },
    };

    registry_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_apply_group(void)
{
    successful = false;
    group_id = REGISTRY_TESTS_NESTED_GROUP;

    const registry_node_t node = {
        .type = REGISTRY_NODE_GROUP,
        .value.group = {
            .instance = &test_nested_instance_group_test,
            .group = &registry_tests_nested_group,
        },
    };

    registry_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_apply_instance(void)
{
    successful = false;

    const registry_node_t node = {
        .type = REGISTRY_NODE_INSTANCE,
        .value.instance = &test_nested_instance_instance_test,
    };

    registry_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_apply_schema(void)
{
    successful = false;

    const registry_node_t node = {
        .type = REGISTRY_NODE_SCHEMA,
        .value.schema = &registry_tests_nested,
    };

    registry_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_apply_namespace(void)
{
    successful = false;

    const registry_node_t node = {
        .type = REGISTRY_NODE_NAMESPACE,
        .value.namespace = &registry_tests,
    };

    registry_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_registry_apply_all(void)
{
    successful = false;
    registry_apply(NULL);
    TEST_ASSERT(successful);
}

Test *tests_registry_apply_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(tests_registry_apply_parameter),
        new_TestFixture(tests_registry_apply_group),
        new_TestFixture(tests_registry_apply_instance),
        new_TestFixture(tests_registry_apply_schema),
        new_TestFixture(tests_registry_apply_namespace),
        new_TestFixture(tests_registry_apply_all),
    };

    EMB_UNIT_TESTCALLER(registry_tests, test_registry_setup, test_registry_teardown, fixtures);

    return (Test *)&registry_tests;
}
