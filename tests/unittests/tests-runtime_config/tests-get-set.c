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

/* (FLT_MAX_10_EXP + 1) + sign + dot + 6 decimal places */
#define FLOAT_MAX_CHAR_COUNT  ((FLT_MAX_10_EXP + 1) + 1 + 1 + 6)
#define DOUBLE_MAX_CHAR_COUNT ((DBL_MAX_10_EXP + 1) + 1 + 1 + 6) // (DBL_MAX_10_EXP + 1) + sign + dot + 6 decimal places

static runtime_config_error_t apply_cb(
    const runtime_config_group_or_parameter_id_t *group_or_parameter_id,
    const runtime_config_schema_instance_t *instance)
{
    (void)group_or_parameter_id;
    (void)instance;

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
    .apply_cb = &apply_cb,
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
    runtime_config_value_t output;

    runtime_config_node_t node = {
        .type = RUNTIME_CONFIG_NODE_PARAMETER,
        .value.parameter.instance = &test_full_instance_1,
    };

    /* bytes */
    typedef struct {
        uint8_t foo;
    } custom_struct_one_field_t;

    custom_struct_one_field_t custom_data_short = { .foo = UINT8_MAX };

    node.value.parameter.parameter = &runtime_config_tests_full_bytes;

    runtime_config_set(&node, &custom_data_short, sizeof(custom_data_short));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(custom_data_short.foo,
                          ((custom_struct_one_field_t *)output.buf)->foo);

    /* string */
    const char input_string[] = "";
    node.value.parameter.parameter = &runtime_config_tests_full_string;

    runtime_config_set(&node, input_string, sizeof(input_string));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_STRING("", (char *)output.buf);

    /* bool */
    const bool input_bool = false;
    node.value.parameter.parameter = &runtime_config_tests_full_boolean;

    runtime_config_set(&node, &input_bool, sizeof(input_bool));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_bool, *(bool *)output.buf);

    /* u8 */
    const uint8_t input_u8 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_u8;

    runtime_config_set(&node, &input_u8, sizeof(input_u8));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u8, *(uint8_t *)output.buf);

    /* u16 */
    const uint16_t input_u16 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_u16;

    runtime_config_set(&node, &input_u16, sizeof(input_u16));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u16, *(uint16_t *)output.buf);

    /* u32 */
    const uint32_t input_u32 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_u32;

    runtime_config_set(&node, &input_u32, sizeof(input_u32));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u32, *(uint32_t *)output.buf);

    /* u64 */
    const uint64_t input_u64 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_u64;

    runtime_config_set(&node, &input_u64, sizeof(input_u64));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u64, *(uint64_t *)output.buf);

    /* i8 */
    const int8_t input_i8 = INT8_MIN;
    node.value.parameter.parameter = &runtime_config_tests_full_i8;

    runtime_config_set(&node, &input_i8, sizeof(input_i8));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i8, *(int8_t *)output.buf);

    /* i16 */
    const int16_t input_i16 = INT16_MIN;
    node.value.parameter.parameter = &runtime_config_tests_full_i16;

    runtime_config_set(&node, &input_i16, sizeof(input_i16));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i16, *(int16_t *)output.buf);

    /* i32 */
    const int32_t input_i32 = INT32_MIN;
    node.value.parameter.parameter = &runtime_config_tests_full_i32;

    runtime_config_set(&node, &input_i32, sizeof(input_i32));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i32, *(int32_t *)output.buf);

    /* i64 */
    const int64_t input_i64 = INT64_MIN;
    node.value.parameter.parameter = &runtime_config_tests_full_i64;

    runtime_config_set(&node, &input_i64, sizeof(input_i64));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i64, *(int64_t *)output.buf);

    /* f32 */
    const float input_f32 = FLT_MIN;
    node.value.parameter.parameter = &runtime_config_tests_full_f32;

    runtime_config_set(&node, &input_f32, sizeof(input_f32));
    runtime_config_get(&node, &output);

    char input_f32_string[FLOAT_MAX_CHAR_COUNT + 1] = { 0 };
    char output_f32_string[FLOAT_MAX_CHAR_COUNT + 1] = { 0 };
    sprintf(input_f32_string, "%f", input_f32);
    sprintf(output_f32_string, "%f", *(float *)output.buf);

    TEST_ASSERT_EQUAL_STRING(input_f32_string, output_f32_string);

    /* f64 */
    const double input_f64 = DBL_MIN;
    node.value.parameter.parameter = &runtime_config_tests_full_f64;

    runtime_config_set(&node, &input_f64, sizeof(input_f64));
    runtime_config_get(&node, &output);

    char input_f64_string[DOUBLE_MAX_CHAR_COUNT + 1] = { 0 };
    char output_f64_string[DOUBLE_MAX_CHAR_COUNT + 1] = { 0 };
    sprintf(input_f64_string, "%f", input_f64);
    sprintf(output_f64_string, "%f", *(double *)output.buf);

    TEST_ASSERT_EQUAL_STRING(input_f64_string, output_f64_string);
}

static void tests_runtime_config_zero_values(void)
{
    runtime_config_value_t output;

    runtime_config_node_t node = {
        .type = RUNTIME_CONFIG_NODE_PARAMETER,
        .value.parameter.instance = &test_full_instance_1,
    };

    /* bytes */
    typedef struct {
        uint8_t foo;
    } custom_struct_one_field_t;

    custom_struct_one_field_t custom_data_short = { .foo = 0 };

    node.value.parameter.parameter = &runtime_config_tests_full_bytes;

    runtime_config_set(&node, &custom_data_short, sizeof(custom_data_short));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(0, ((custom_struct_one_field_t *)output.buf)->foo);

    /* string */
    const char input_string[] = "";
    node.value.parameter.parameter = &runtime_config_tests_full_string;

    runtime_config_set(&node, input_string, sizeof(input_string));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_STRING("", (char *)output.buf);

    /* bool */
    const bool input_bool = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_boolean;

    runtime_config_set(&node, &input_bool, sizeof(input_bool));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_bool, *(bool *)output.buf);

    /* u8 */
    const uint8_t input_u8 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_u8;

    runtime_config_set(&node, &input_u8, sizeof(input_u8));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u8, *(uint8_t *)output.buf);

    /* u16 */
    const uint16_t input_u16 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_u16;

    runtime_config_set(&node, &input_u16, sizeof(input_u16));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u16, *(uint16_t *)output.buf);

    /* u32 */
    const uint32_t input_u32 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_u32;

    runtime_config_set(&node, &input_u32, sizeof(input_u32));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u32, *(uint32_t *)output.buf);

    /* u64 */
    const uint64_t input_u64 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_u64;

    runtime_config_set(&node, &input_u64, sizeof(input_u64));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u64, *(uint64_t *)output.buf);

    /* i8 */
    const int8_t input_i8 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_i8;

    runtime_config_set(&node, &input_i8, sizeof(input_i8));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i8, *(int8_t *)output.buf);

    /* i16 */
    const int16_t input_i16 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_i16;

    runtime_config_set(&node, &input_i16, sizeof(input_i16));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i16, *(int16_t *)output.buf);

    /* i32 */
    const int32_t input_i32 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_i32;

    runtime_config_set(&node, &input_i32, sizeof(input_i32));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i32, *(int32_t *)output.buf);

    /* i64 */
    const int64_t input_i64 = 0;
    node.value.parameter.parameter = &runtime_config_tests_full_i64;

    runtime_config_set(&node, &input_i64, sizeof(input_i64));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i64, *(int64_t *)output.buf);

    /* f32 */
    const float input_f32 = 0.0;
    node.value.parameter.parameter = &runtime_config_tests_full_f32;

    runtime_config_set(&node, &input_f32, sizeof(input_f32));
    runtime_config_get(&node, &output);

    char input_f32_string[FLOAT_MAX_CHAR_COUNT + 1] = { 0 };
    char output_f32_string[FLOAT_MAX_CHAR_COUNT + 1] = { 0 };
    sprintf(input_f32_string, "%f", input_f32);
    sprintf(output_f32_string, "%f", *(float *)output.buf);

    TEST_ASSERT_EQUAL_STRING(input_f32_string, output_f32_string);

    /* f64 */
    const double input_f64 = 0.0;
    node.value.parameter.parameter = &runtime_config_tests_full_f64;

    runtime_config_set(&node, &input_f64, sizeof(input_f64));
    runtime_config_get(&node, &output);

    char input_f64_string[DOUBLE_MAX_CHAR_COUNT + 1] = { 0 };
    char output_f64_string[DOUBLE_MAX_CHAR_COUNT + 1] = { 0 };
    sprintf(input_f64_string, "%f", input_f64);
    sprintf(output_f64_string, "%f", *(double *)output.buf);

    TEST_ASSERT_EQUAL_STRING(input_f64_string, output_f64_string);
}

static void tests_runtime_config_ensure_that_max_values_can_be_set_and_get(void)
{
    runtime_config_value_t output;

    runtime_config_node_t node = {
        .type = RUNTIME_CONFIG_NODE_PARAMETER,
        .value.parameter.instance = &test_full_instance_1,
    };

    /* bytes */
    typedef struct {
        uint8_t foo;
    } custom_struct_one_field_t;

    custom_struct_one_field_t custom_data_short = { .foo = UINT8_MAX };

    node.value.parameter.parameter = &runtime_config_tests_full_bytes;

    runtime_config_set(&node, &custom_data_short, sizeof(custom_data_short));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(UINT8_MAX,
                          ((custom_struct_one_field_t *)output.buf)->foo);

    /* string */
    char input_string[sizeof(test_full_instance_1_data.string)] = { 0 };

    for (size_t i = 0; i < sizeof(input_string) - 1; i++) {
        input_string[i] = 'a';
    }

    node.value.parameter.parameter = &runtime_config_tests_full_string;

    runtime_config_set(&node, input_string, sizeof(input_string));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_STRING(input_string, (char *)output.buf);

    /* bool */
    const bool input_bool = true;
    node.value.parameter.parameter = &runtime_config_tests_full_boolean;

    runtime_config_set(&node, &input_bool, sizeof(input_bool));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_bool, *(bool *)output.buf);

    /* u8 */
    const uint8_t input_u8 = UINT8_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_u8;

    runtime_config_set(&node, &input_u8, sizeof(input_u8));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u8, *(uint8_t *)output.buf);

    /* u16 */
    const uint16_t input_u16 = UINT16_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_u16;

    runtime_config_set(&node, &input_u16, sizeof(input_u16));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u16, *(uint16_t *)output.buf);

    /* u32 */
    const uint32_t input_u32 = UINT32_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_u32;

    runtime_config_set(&node, &input_u32, sizeof(input_u32));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u32, *(uint32_t *)output.buf);

    /* u64 */
    const uint64_t input_u64 = UINT64_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_u64;

    runtime_config_set(&node, &input_u64, sizeof(input_u64));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_u64, *(uint64_t *)output.buf);

    /* i8 */
    const int8_t input_i8 = INT8_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_i8;

    runtime_config_set(&node, &input_i8, sizeof(input_i8));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i8, *(int8_t *)output.buf);

    /* i16 */
    const int16_t input_i16 = INT16_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_i16;

    runtime_config_set(&node, &input_i16, sizeof(input_i16));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i16, *(int16_t *)output.buf);

    /* i32 */
    const int32_t input_i32 = INT32_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_i32;

    runtime_config_set(&node, &input_i32, sizeof(input_i32));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i32, *(int32_t *)output.buf);

    /* i64 */
    const int64_t input_i64 = INT64_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_i64;

    runtime_config_set(&node, &input_i64, sizeof(input_i64));
    runtime_config_get(&node, &output);

    TEST_ASSERT_EQUAL_INT(input_i64, *(int64_t *)output.buf);

    /* f32 */
    const float input_f32 = FLT_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_f32;

    runtime_config_set(&node, &input_f32, sizeof(input_f32));
    runtime_config_get(&node, &output);

    char input_f32_string[FLOAT_MAX_CHAR_COUNT + 1] = { 0 };
    char output_f32_string[FLOAT_MAX_CHAR_COUNT + 1] = { 0 };
    sprintf(input_f32_string, "%f", input_f32);
    sprintf(output_f32_string, "%f", *(float *)output.buf);

    TEST_ASSERT_EQUAL_STRING(input_f32_string, output_f32_string);

    /* f64 */
    const double input_f64 = DBL_MAX;
    node.value.parameter.parameter = &runtime_config_tests_full_f64;

    runtime_config_set(&node, &input_f64, sizeof(input_f64));
    runtime_config_get(&node, &output);

    char input_f64_string[DOUBLE_MAX_CHAR_COUNT + 1] = { 0 };
    char output_f64_string[DOUBLE_MAX_CHAR_COUNT + 1] = { 0 };
    sprintf(input_f64_string, "%f", input_f64);
    sprintf(output_f64_string, "%f", *(double *)output.buf);

    TEST_ASSERT_EQUAL_STRING(input_f64_string, output_f64_string);
}

Test *tests_runtime_config_get_set_tests(void)
{
    (void)tests_runtime_config_min_values;
    (void)tests_runtime_config_zero_values;
    (void)tests_runtime_config_ensure_that_max_values_can_be_set_and_get;

    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(tests_runtime_config_min_values),
        new_TestFixture(tests_runtime_config_zero_values),
        new_TestFixture(tests_runtime_config_ensure_that_max_values_can_be_set_and_get),
    };

    EMB_UNIT_TESTCALLER(runtime_config_tests, test_runtime_config_setup, test_runtime_config_teardown, fixtures);

    return (Test *)&runtime_config_tests;
}
