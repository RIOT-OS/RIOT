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
 * @brief       Unittests for runtime_config_get, runtime_config_set
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
#include "namespace/tests/full.h"

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

static runtime_config_tests_full_instance_t test_full_instance_1_data = {
    .bytes = { 7 },
    .string = "hello world",
    .boolean = true,
    .u8 = 9,
    .u16 = 17,
    .u32 = 33,
    .u64 = 65,
    .i8 = 8,
    .i16 = 16,
    .i32 = 32,
    .i64 = 64,
    .f32 = 3.2,
    .f64 = 6.4,
};

static runtime_config_schema_instance_t test_full_instance_1 = {
#if IS_ACTIVE(CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME)
    .name = "test-full-1",
#endif /* CONFIG_RUNTIME_CONFIG_ENABLE_META_NAME */
    .data = &test_full_instance_1_data,
    .apply_schema_instance_cb = apply_schema_instance_cb,
    .apply_group_or_parameter_cb = apply_group_cb,
};

static void test_runtime_config_setup(void)
{
    /* init runtime configuration module */
    runtime_config_init();

    /* add schema instances */
    runtime_config_add_schema_instance(&runtime_config_tests_full, &test_full_instance_1);
}

static void test_runtime_config_teardown(void)
{
}

static void tests_runtime_config_min_values(void)
{
    runtime_config_error_t res;
    void *output;
    size_t output_len;

    /* The parameter is NULL, because we will set it specifically for each test case below */
    runtime_config_node_t node = RUNTIME_CONFIG_NODE_PARAMETER(&test_full_instance_1, NULL);

    /* bytes */
    typedef struct {
        uint8_t foo;
    } custom_struct_one_field_t;

    custom_struct_one_field_t custom_data_short = { .foo = UINT8_MAX };

    node.as_parameter.parameter = &runtime_config_tests_full_bytes;

    res = runtime_config_set(&node, &custom_data_short, sizeof(custom_data_short));
    res = runtime_config_get(&node, &output, &output_len);

    TEST_ASSERT_EQUAL_INT(custom_data_short.foo,
                          ((custom_struct_one_field_t *)output)->foo);

    /* string */
    const char input_string[] = "";
    node.as_parameter.parameter = &runtime_config_tests_full_string;

    res = runtime_config_set(&node, input_string, sizeof(input_string));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_STRING("", (char *)output);

    /* bool */
    const bool input_bool = false;
    node.as_parameter.parameter = &runtime_config_tests_full_boolean;

    res = runtime_config_set(&node, &input_bool, sizeof(input_bool));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_bool, *(bool *)output);

    /* u8 */
    const uint8_t input_u8 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_u8;

    res = runtime_config_set(&node, &input_u8, sizeof(input_u8));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u8, *(uint8_t *)output);

    /* u16 */
    const uint16_t input_u16 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_u16;

    res = runtime_config_set(&node, &input_u16, sizeof(input_u16));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u16, *(uint16_t *)output);

    /* u32 */
    const uint32_t input_u32 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_u32;

    res = runtime_config_set(&node, &input_u32, sizeof(input_u32));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u32, *(uint32_t *)output);

    /* u64 */
    const uint64_t input_u64 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_u64;

    res = runtime_config_set(&node, &input_u64, sizeof(input_u64));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u64, *(uint64_t *)output);

    /* i8 */
    const int8_t input_i8 = INT8_MIN;
    node.as_parameter.parameter = &runtime_config_tests_full_i8;

    res = runtime_config_set(&node, &input_i8, sizeof(input_i8));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i8, *(int8_t *)output);

    /* i16 */
    const int16_t input_i16 = INT16_MIN;
    node.as_parameter.parameter = &runtime_config_tests_full_i16;

    res = runtime_config_set(&node, &input_i16, sizeof(input_i16));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i16, *(int16_t *)output);

    /* i32 */
    const int32_t input_i32 = INT32_MIN;
    node.as_parameter.parameter = &runtime_config_tests_full_i32;

    res = runtime_config_set(&node, &input_i32, sizeof(input_i32));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i32, *(int32_t *)output);

    /* i64 */
    const int64_t input_i64 = INT64_MIN;
    node.as_parameter.parameter = &runtime_config_tests_full_i64;

    res = runtime_config_set(&node, &input_i64, sizeof(input_i64));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i64, *(int64_t *)output);

    /* f32 */
    const float input_f32 = FLT_MIN;
    node.as_parameter.parameter = &runtime_config_tests_full_f32;

    res = runtime_config_set(&node, &input_f32, sizeof(input_f32));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT(input_f32 == *(float *)output);

    /* f64 */
    const double input_f64 = DBL_MIN;
    node.as_parameter.parameter = &runtime_config_tests_full_f64;

    res = runtime_config_set(&node, &input_f64, sizeof(input_f64));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT(input_f64 == *(double *)output);
}

static void tests_runtime_config_zero_values(void)
{
    runtime_config_error_t res;
    void *output;
    size_t output_len;

    /* The parameter is NULL, because we will set it specifically for each test case below */
    runtime_config_node_t node = RUNTIME_CONFIG_NODE_PARAMETER(&test_full_instance_1, NULL);

    /* bytes */
    typedef struct {
        uint8_t foo;
    } custom_struct_one_field_t;

    custom_struct_one_field_t custom_data_short = { .foo = 0 };

    node.as_parameter.parameter = &runtime_config_tests_full_bytes;

    res = runtime_config_set(&node, &custom_data_short, sizeof(custom_data_short));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(0, ((custom_struct_one_field_t *)output)->foo);

    /* string */
    const char input_string[] = "";
    node.as_parameter.parameter = &runtime_config_tests_full_string;

    res = runtime_config_set(&node, input_string, sizeof(input_string));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_STRING("", (char *)output);

    /* bool */
    const bool input_bool = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_boolean;

    res = runtime_config_set(&node, &input_bool, sizeof(input_bool));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_bool, *(bool *)output);

    /* u8 */
    const uint8_t input_u8 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_u8;

    res = runtime_config_set(&node, &input_u8, sizeof(input_u8));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u8, *(uint8_t *)output);

    /* u16 */
    const uint16_t input_u16 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_u16;

    res = runtime_config_set(&node, &input_u16, sizeof(input_u16));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u16, *(uint16_t *)output);

    /* u32 */
    const uint32_t input_u32 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_u32;

    res = runtime_config_set(&node, &input_u32, sizeof(input_u32));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u32, *(uint32_t *)output);

    /* u64 */
    const uint64_t input_u64 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_u64;

    res = runtime_config_set(&node, &input_u64, sizeof(input_u64));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u64, *(uint64_t *)output);

    /* i8 */
    const int8_t input_i8 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_i8;

    res = runtime_config_set(&node, &input_i8, sizeof(input_i8));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i8, *(int8_t *)output);

    /* i16 */
    const int16_t input_i16 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_i16;

    res = runtime_config_set(&node, &input_i16, sizeof(input_i16));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i16, *(int16_t *)output);

    /* i32 */
    const int32_t input_i32 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_i32;

    res = runtime_config_set(&node, &input_i32, sizeof(input_i32));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i32, *(int32_t *)output);

    /* i64 */
    const int64_t input_i64 = 0;
    node.as_parameter.parameter = &runtime_config_tests_full_i64;

    res = runtime_config_set(&node, &input_i64, sizeof(input_i64));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i64, *(int64_t *)output);

    /* f32 */
    const float input_f32 = 0.0;
    node.as_parameter.parameter = &runtime_config_tests_full_f32;

    res = runtime_config_set(&node, &input_f32, sizeof(input_f32));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT(input_f32 == *(float *)output);

    /* f64 */
    const double input_f64 = 0.0;
    node.as_parameter.parameter = &runtime_config_tests_full_f64;

    res = runtime_config_set(&node, &input_f64, sizeof(input_f64));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT(input_f64 == *(double *)output);
}

static void tests_runtime_config_ensure_that_max_values_can_be_set_and_get(void)
{
    runtime_config_error_t res;
    void *output;
    size_t output_len;

    /* The parameter is NULL, because we will set it specifically for each test case below */
    runtime_config_node_t node = RUNTIME_CONFIG_NODE_PARAMETER(&test_full_instance_1, NULL);

    /* bytes */
    typedef struct {
        uint8_t foo;
    } custom_struct_one_field_t;

    custom_struct_one_field_t custom_data_short = { .foo = UINT8_MAX };

    node.as_parameter.parameter = &runtime_config_tests_full_bytes;

    res = runtime_config_set(&node, &custom_data_short, sizeof(custom_data_short));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(UINT8_MAX,
                          ((custom_struct_one_field_t *)output)->foo);

    /* string */
    char input_string[sizeof(test_full_instance_1_data.string)] = { 0 };

    for (size_t i = 0; i < sizeof(input_string) - 1; i++) {
        input_string[i] = 'a';
    }

    node.as_parameter.parameter = &runtime_config_tests_full_string;

    res = runtime_config_set(&node, input_string, sizeof(input_string));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_STRING(input_string, (char *)output);

    /* bool */
    const bool input_bool = true;
    node.as_parameter.parameter = &runtime_config_tests_full_boolean;

    res = runtime_config_set(&node, &input_bool, sizeof(input_bool));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_bool, *(bool *)output);

    /* u8 */
    const uint8_t input_u8 = UINT8_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_u8;

    res = runtime_config_set(&node, &input_u8, sizeof(input_u8));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u8, *(uint8_t *)output);

    /* u16 */
    const uint16_t input_u16 = UINT16_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_u16;

    res = runtime_config_set(&node, &input_u16, sizeof(input_u16));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u16, *(uint16_t *)output);

    /* u32 */
    const uint32_t input_u32 = UINT32_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_u32;

    res = runtime_config_set(&node, &input_u32, sizeof(input_u32));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u32, *(uint32_t *)output);

    /* u64 */
    const uint64_t input_u64 = UINT64_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_u64;

    res = runtime_config_set(&node, &input_u64, sizeof(input_u64));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_u64, *(uint64_t *)output);

    /* i8 */
    const int8_t input_i8 = INT8_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_i8;

    res = runtime_config_set(&node, &input_i8, sizeof(input_i8));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i8, *(int8_t *)output);

    /* i16 */
    const int16_t input_i16 = INT16_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_i16;

    res = runtime_config_set(&node, &input_i16, sizeof(input_i16));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i16, *(int16_t *)output);

    /* i32 */
    const int32_t input_i32 = INT32_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_i32;

    res = runtime_config_set(&node, &input_i32, sizeof(input_i32));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i32, *(int32_t *)output);

    /* i64 */
    const int64_t input_i64 = INT64_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_i64;

    res = runtime_config_set(&node, &input_i64, sizeof(input_i64));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT_EQUAL_INT(input_i64, *(int64_t *)output);

    /* f32 */
    const float input_f32 = FLT_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_f32;

    res = runtime_config_set(&node, &input_f32, sizeof(input_f32));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT(input_f32 == *(float *)output);

    /* f64 */
    const double input_f64 = DBL_MAX;
    node.as_parameter.parameter = &runtime_config_tests_full_f64;

    res = runtime_config_set(&node, &input_f64, sizeof(input_f64));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NONE, res);

    TEST_ASSERT(input_f64 == *(double *)output);
}

static void tests_runtime_config_cannot_get_set_invalid_node_type(void)
{
    runtime_config_error_t res;
    void *output;
    size_t output_len;

    /* A node targeting something other than a parameter */
    runtime_config_node_t node = RUNTIME_CONFIG_NODE_GROUP(&test_full_instance_1, NULL);

    /* Ensure get fails with invalid node error */
    res = runtime_config_get(&node, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NODE_INVALID, res);

    /* Ensure set fails with invalid node error */
    uint32_t val = 123;
    res = runtime_config_set(&node, &val, sizeof(val));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_NODE_INVALID, res);
}

static void tests_runtime_config_cannot_use_too_small_buffer(void)
{
    runtime_config_error_t res;

    runtime_config_node_t node = RUNTIME_CONFIG_NODE_PARAMETER(&test_full_instance_1, &runtime_config_tests_full_u32);

    uint16_t too_small_buffer = 0;

    /* Ensure passing a strictly smaller buffer trips the bounds check */
    res = runtime_config_set(&node, &too_small_buffer, sizeof(too_small_buffer));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_BUF_LEN_TOO_SMALL, res);
}

static void tests_runtime_config_cannot_use_too_large_buffer(void)
{
    runtime_config_error_t res;

    runtime_config_node_t node = RUNTIME_CONFIG_NODE_PARAMETER(&test_full_instance_1, &runtime_config_tests_full_u8);

    uint32_t too_large_buffer = 0;

    /* Ensure passing a strictly larger buffer trips the bounds check */
    res = runtime_config_set(&node, &too_large_buffer, sizeof(too_large_buffer));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_BUF_LEN_TOO_LARGE, res);
}

static void tests_runtime_config_cannot_use_null_arguments(void)
{
    runtime_config_error_t res;
    void *output;
    size_t output_len;
    uint32_t val = 123;

    runtime_config_node_t node = RUNTIME_CONFIG_NODE_PARAMETER(&test_full_instance_1, &runtime_config_tests_full_u32);

    /* Test runtime_config_get NULL checks */
    res = runtime_config_get(NULL, &output, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_INVALID_ARGUMENT, res);

    res = runtime_config_get(&node, NULL, &output_len);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_INVALID_ARGUMENT, res);

    res = runtime_config_get(&node, &output, NULL);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_INVALID_ARGUMENT, res);

    /* Test runtime_config_set NULL checks */
    res = runtime_config_set(NULL, &val, sizeof(val));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_INVALID_ARGUMENT, res);

    res = runtime_config_set(&node, NULL, sizeof(val));
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_INVALID_ARGUMENT, res);

    /* Also check for buf_len 0, since that also is INVALID */
    res = runtime_config_set(&node, &val, 0);
    TEST_ASSERT_EQUAL_INT(RUNTIME_CONFIG_ERROR_INVALID_ARGUMENT, res);
}

Test *tests_runtime_config_get_set_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(tests_runtime_config_min_values),
        new_TestFixture(tests_runtime_config_zero_values),
        new_TestFixture(tests_runtime_config_ensure_that_max_values_can_be_set_and_get),
        new_TestFixture(tests_runtime_config_cannot_get_set_invalid_node_type),
        new_TestFixture(tests_runtime_config_cannot_use_too_small_buffer),
        new_TestFixture(tests_runtime_config_cannot_use_too_large_buffer),
        new_TestFixture(tests_runtime_config_cannot_use_null_arguments),
    };

    EMB_UNIT_TESTCALLER(runtime_config_tests, test_runtime_config_setup, test_runtime_config_teardown, fixtures);

    return (Test *)&runtime_config_tests;
}
