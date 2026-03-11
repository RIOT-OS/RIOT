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
 * @brief       Unittests for runtime_config_apply
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

static bool successful = false;
static runtime_config_group_or_parameter_id_t parameter_id;
static runtime_config_group_or_parameter_id_t group_id;

static runtime_config_error_t apply_parameter_cb(
    const runtime_config_group_or_parameter_id_t *group_or_parameter_id,
    const runtime_config_schema_instance_t *instance)
{
    (void)instance;

    if (group_or_parameter_id != NULL &&
        *group_or_parameter_id == parameter_id) {
        successful = true;
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_error_t apply_group_cb(
    const runtime_config_group_or_parameter_id_t *group_or_parameter_id,
    const runtime_config_schema_instance_t *instance)
{
    (void)instance;

    if (group_or_parameter_id != NULL &&
        *group_or_parameter_id == group_id) {
        successful = true;
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_error_t apply_instance_cb(
    const runtime_config_group_or_parameter_id_t *group_or_parameter_id,
    const runtime_config_schema_instance_t *instance)
{
    (void)instance;

    if (group_or_parameter_id == NULL) {
        successful = true;
    }

    return RUNTIME_CONFIG_ERROR_NONE;
}

static runtime_config_tests_nested_instance_t test_nested_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static runtime_config_schema_instance_t test_nested_instance_parameter_test = {
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME)
    .name = "test-nested-parameter-test",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .apply_cb = &apply_parameter_cb,
};

static runtime_config_schema_instance_t test_nested_instance_group_test = {
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME)
    .name = "test-nested-group-test",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .apply_cb = &apply_group_cb,
};

static runtime_config_schema_instance_t test_nested_instance_instance_test = {
#if IS_ACTIVE(RUNTIME_CONFIG_ENABLE_META_NAME)
    .name = "test-nested-instance-test",
#endif /* RUNTIME_CONFIG_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .apply_cb = &apply_instance_cb,
};

static void test_runtime_config_setup(void)
{
    /* init runtime config module */
    runtime_config_init();

    /* add schema instances */
    runtime_config_add_schema_instance(&runtime_config_tests_nested, &test_nested_instance_parameter_test);
    runtime_config_add_schema_instance(&runtime_config_tests_nested, &test_nested_instance_group_test);
    runtime_config_add_schema_instance(&runtime_config_tests_nested, &test_nested_instance_instance_test);
}

static void test_runtime_config_teardown(void)
{
}

static void tests_runtime_config_apply_parameter(void)
{
    successful = false;
    parameter_id = RUNTIME_CONFIG_TESTS_NESTED_PARAMETER;

    const runtime_config_node_t node = {
        .type = RUNTIME_CONFIG_NODE_PARAMETER,
        .value.parameter = {
            .instance = &test_nested_instance_parameter_test,
            .parameter = &runtime_config_tests_nested_parameter,
        },
    };

    runtime_config_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_runtime_config_apply_group(void)
{
    successful = false;
    group_id = RUNTIME_CONFIG_TESTS_NESTED_GROUP;

    const runtime_config_node_t node = {
        .type = RUNTIME_CONFIG_NODE_GROUP,
        .value.group = {
            .instance = &test_nested_instance_group_test,
            .group = &runtime_config_tests_nested_group,
        },
    };

    runtime_config_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_runtime_config_apply_instance(void)
{
    successful = false;

    const runtime_config_node_t node = {
        .type = RUNTIME_CONFIG_NODE_INSTANCE,
        .value.instance = &test_nested_instance_instance_test,
    };

    runtime_config_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_runtime_config_apply_schema(void)
{
    successful = false;

    const runtime_config_node_t node = {
        .type = RUNTIME_CONFIG_NODE_SCHEMA,
        .value.schema = &runtime_config_tests_nested,
    };

    runtime_config_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_runtime_config_apply_namespace(void)
{
    successful = false;

    const runtime_config_node_t node = {
        .type = RUNTIME_CONFIG_NODE_NAMESPACE,
        .value.namespace = &runtime_config_tests,
    };

    runtime_config_apply(&node);

    TEST_ASSERT(successful);
}

static void tests_runtime_config_apply_all(void)
{
    successful = false;
    runtime_config_apply(NULL);
    TEST_ASSERT(successful);
}

Test *tests_runtime_config_apply_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(tests_runtime_config_apply_parameter),
        new_TestFixture(tests_runtime_config_apply_group),
        new_TestFixture(tests_runtime_config_apply_instance),
        new_TestFixture(tests_runtime_config_apply_schema),
        new_TestFixture(tests_runtime_config_apply_namespace),
        new_TestFixture(tests_runtime_config_apply_all),
    };

    EMB_UNIT_TESTCALLER(runtime_config_tests, test_runtime_config_setup, test_runtime_config_teardown, fixtures);

    return (Test *)&runtime_config_tests;
}
