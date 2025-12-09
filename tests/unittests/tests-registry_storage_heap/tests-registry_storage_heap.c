/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    unittests
 * @brief       Unittests for the ``registry_storage_heap`` module
 * @{
 *
 * @file
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
#include "registry.h"
#include "registry/storage.h"

#include "tests-registry_storage_heap.h"
#include "namespace/tests.h"
#include "namespace/tests/nested.h"

#if IS_USED(MODULE_REGISTRY_STORAGE_HEAP) || IS_ACTIVE(DOXYGEN)

static registry_tests_nested_instance_t test_nested_instance_data = {
    .parameter = 9,
    .group_parameter = 5,
};

static registry_instance_t test_nested_instance = {
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME)
    .name = "test-nested-parameter-test",
#  endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
    .data = &test_nested_instance_data,
    .apply_cb = NULL,
};

static registry_storage_instance_t heap_instance = {
    .storage = &registry_storage_heap,
    .data = NULL,
};

static void test_setup(void)
{
    /* init registry */
    registry_init();

    /* add schema instances */
    registry_add_schema_instance(&registry_tests_nested, &test_nested_instance);
}

static void tests_load_and_save(void)
{
    registry_node_t node = {
        .type = REGISTRY_NODE_PARAMETER,
        .value.parameter = {
            .instance = &test_nested_instance,
            .parameter = &registry_tests_nested_group_parameter,
        },
    };

    /* set input to 8 */
    const uint8_t saved_input = 8;

    registry_set(&node, &saved_input, sizeof(saved_input));

    /* save input to storage */
    registry_storage_save(&heap_instance, NULL);

    /* override input with the value 20 */
    const uint8_t override_input = 20;

    registry_set(&node, &override_input, sizeof(override_input));

    /* load old value from storage */
    registry_storage_load(&heap_instance);

    /* check if the value is set back to 8 and not 20 anymore */
    registry_value_t output_value;

    registry_get(&node, &output_value);

    TEST_ASSERT_EQUAL_INT(saved_input, *(uint8_t *)output_value.buf);
}

Test *tests_registry_storage_heap_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(tests_load_and_save),
    };

    EMB_UNIT_TESTCALLER(registry_tests, test_setup, NULL, fixtures);

    return (Test *)&registry_tests;
}

void tests_registry_storage_heap(void)
{
    TESTS_RUN(tests_registry_storage_heap_tests());
}

#endif
