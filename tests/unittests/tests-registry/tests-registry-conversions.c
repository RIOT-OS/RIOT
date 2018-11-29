/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   Unit tests for the data conversion helper functions of the RIOT
 *          registry.
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "embUnit/embUnit.h"
#include "embUnit.h"
#include "registry/registry.h"
#include "tests-registry.h"

#define TEST_INT8_VAL_STR   "-127"
#define TEST_INT8_VAL       -127

#define TEST_INT16_VAL_STR  "32767"
#define TEST_INT16_VAL      32767

#define TEST_INT32_VAL_STR  "-2147483647"
#define TEST_INT32_VAL      -2147483647

#define TEST_STRING         "this is the test value"

#define TEST_BYTES_VAL_STR  "VGhpcyBpcyBSSU9UIQ=="
#define TEST_BYTES_VAL      {0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, \
                             0x52, 0x49, 0x4f, 0x54, 0x21}

#define TEST_BOOL_VAL_STR  "1"
#define TEST_BOOL_VAL      1

#if defined(CONFIG_REGISTRY_USE_INT64)
#define TEST_INT64_VAL_STR "9223372036854775807"
#define TEST_INT64_VAL     9223372036854775807
#endif

static void test_value_from_str(void)
{
    /* test int8 */
    int8_t int8_res = 0;
    int res = registry_value_from_str(TEST_INT8_VAL_STR, REGISTRY_TYPE_INT8,
                                      (void *)&int8_res, 0);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(TEST_INT8_VAL, int8_res);

    /* test int16 */
    int16_t int16_res = 0;
    res = registry_value_from_str(TEST_INT16_VAL_STR, REGISTRY_TYPE_INT16,
                                  (void *)&int16_res, 0);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(TEST_INT16_VAL, int16_res);

    /* test int32 */
    int32_t int32_res = 0;
    res = registry_value_from_str(TEST_INT32_VAL_STR, REGISTRY_TYPE_INT32,
                                  (void *)&int32_res, 0);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(TEST_INT32_VAL, int32_res);

    /* test string */
    char str_res[sizeof(TEST_STRING)];
    /* this should fail because of not enough space on the buffer */
    res = registry_value_from_str(TEST_STRING,REGISTRY_TYPE_STRING,
                                      (void *)str_res, 0);
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);

    /* this should succeed */
    res = registry_value_from_str(TEST_STRING,REGISTRY_TYPE_STRING,
                                  (void *)str_res, sizeof(str_res));
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING, str_res);

    /* test bytes */
    /* this should fail as for bytes `registry_bytes_from_str` should be used */
    res = registry_value_from_str(TEST_BYTES_VAL_STR, REGISTRY_TYPE_BYTES,
                                  (void *)str_res, 0);
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);

    /* test bool */
    int bool_res = 0;
    res = registry_value_from_str(TEST_BOOL_VAL_STR, REGISTRY_TYPE_BOOL,
                                  (void *)&bool_res, 0);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(TEST_BOOL_VAL, bool_res);

#if defined(CONFIG_REGISTRY_USE_INT64)
    /* test int64 */
    int64_t int64_res = 0;
    res = registry_value_from_str(TEST_INT64_VAL_STR, REGISTRY_TYPE_INT64,
                                  (void *)&int64_res, 0);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(TEST_INT64_VAL, int64_res);
#endif /* CONFIG_REGISTRY_USE_INT64 */
}

static void test_bytes_from_str(void)
{
    uint8_t bytes_res[sizeof(TEST_BYTES_VAL_STR)];
    uint8_t bytes_actual[] = TEST_BYTES_VAL;
    int size = sizeof(bytes_res);

    int res = registry_bytes_from_str(TEST_BYTES_VAL_STR, (void *)bytes_res,
                                      &size);
    TEST_ASSERT_EQUAL_INT(0, res);

    for (unsigned i = 0; i < sizeof(bytes_actual); i++) {
        TEST_ASSERT_EQUAL_INT(bytes_actual[i], bytes_res[i]);
    }

    /* this should fail due to not enough space in the buffer */
    size = 0;
    res = registry_bytes_from_str(TEST_BYTES_VAL_STR, (void *)bytes_res,
                                  &size);
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
}

#define TEST_OUT_BUF_SIZE   24
static void test_str_from_value(void)
{
    char out_buf[TEST_OUT_BUF_SIZE];
    /* test int8 */
    int8_t int8_val = TEST_INT8_VAL;
    char *res = registry_str_from_value(REGISTRY_TYPE_INT8, (void *)&int8_val,
                                      out_buf, TEST_OUT_BUF_SIZE);
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_STRING(TEST_INT8_VAL_STR, out_buf);

    /* test int16 */
    int16_t int16_val = TEST_INT16_VAL;
    res = registry_str_from_value(REGISTRY_TYPE_INT16, (void *)&int16_val,
                                  out_buf, TEST_OUT_BUF_SIZE);
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_STRING(TEST_INT16_VAL_STR, out_buf);

    /* test int32 */
    int32_t int32_val = TEST_INT32_VAL;
    res = registry_str_from_value(REGISTRY_TYPE_INT32, (void *)&int32_val,
                                  out_buf, TEST_OUT_BUF_SIZE);
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_STRING(TEST_INT32_VAL_STR, out_buf);

    /* test string */
    char str_val[] = TEST_STRING;
    res = registry_str_from_value(REGISTRY_TYPE_STRING, (void *)&str_val,
                                  out_buf, TEST_OUT_BUF_SIZE);
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING, out_buf);

    /* this should fail due to not enough space in the buffer */
    res = registry_str_from_value(REGISTRY_TYPE_STRING, (void *)&str_val,
                                  out_buf, 0);
    TEST_ASSERT_NULL(res);

     /* this should fail as for bytes `registry_str_from_bytes` should be
      * used */
    res = registry_str_from_value(REGISTRY_TYPE_BYTES, (void *)&int32_val,
                                  out_buf, TEST_OUT_BUF_SIZE);
    TEST_ASSERT_NULL(res);

    /* test bool */
    int bool_val = TEST_BOOL_VAL;
    res = registry_str_from_value(REGISTRY_TYPE_BOOL, (void *)&bool_val,
                                  out_buf, TEST_OUT_BUF_SIZE);
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_STRING(TEST_BOOL_VAL_STR, out_buf);

#if defined(CONFIG_REGISTRY_USE_INT64)
    int64_t int64_val = TEST_INT64_VAL;
    res = registry_str_from_value(REGISTRY_TYPE_INT64, (void *)&int64_val,
                                  out_buf, TEST_OUT_BUF_SIZE);
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_STRING(TEST_INT64_VAL_STR, out_buf);
#endif /* CONFIG_REGISTRY_USE_INT64 */
}

static void test_str_from_bytes(void)
{
    char bytes_out[sizeof(TEST_BYTES_VAL_STR)];
    char bytes_val[] = TEST_BYTES_VAL;
    char *res;

    res = registry_str_from_bytes((void *)bytes_val, sizeof(bytes_val),
                                  bytes_out, sizeof(bytes_out));
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_STRING(TEST_BYTES_VAL_STR, bytes_out);

    /* this should fail due to not enough space in the buffer */
    res = registry_str_from_bytes((void *)bytes_val, sizeof(bytes_val),
                                  bytes_out, 0);
    TEST_ASSERT_NULL(res);
}

Test *tests_registry_conversions_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_value_from_str),
        new_TestFixture(test_bytes_from_str),
        new_TestFixture(test_str_from_value),
        new_TestFixture(test_str_from_bytes)
    };

    EMB_UNIT_TESTCALLER(registry_conversions_tests, NULL, NULL, fixtures);

    return (Test *)&registry_conversions_tests;
}
