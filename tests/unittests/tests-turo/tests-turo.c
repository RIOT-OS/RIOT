/*
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Unit tests for test_utils_result_output module
 *
 * @author     Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */
#include "embUnit.h"
#include "kernel_defines.h"
#include "test_utils/result_output.h"

void test_turo_impl(void)
{
    turo_t ctx;
    turo_init(&ctx);
    turo_container_open(&ctx);
    turo_s32(&ctx, -1);
    turo_u32(&ctx, 0xFFFFFFFF);
    turo_s64(&ctx, -1);
    turo_u64(&ctx, 0xFFFFFFFFFFFFFFFF);
    turo_float(&ctx, 0.00001);
    turo_string(&ctx, "foo");
    turo_bool(&ctx, true);
    turo_dict_open(&ctx);
    turo_dict_key(&ctx, "bar");
    turo_u32(&ctx, 0);
    turo_dict_close(&ctx);
    turo_array_open(&ctx);
    turo_u32(&ctx, 0);
    turo_array_close(&ctx);
    turo_container_close(&ctx, 0);
}

void test_turo_helpers(void)
{
    turo_t ctx;
    turo_init(&ctx);
    turo_container_open(&ctx);
    uint8_t buf8[] = {1, 2, 3};
    int32_t buf32[] = {-1, 1, 0x7FFFFFFF};
    turo_array_u8(&ctx, buf8, sizeof(buf8));
    turo_array_s32(&ctx, buf32, ARRAY_SIZE(buf32));
    turo_dict_string(&ctx, "foo", "bar");
    turo_dict_s32(&ctx, "baz", 42);
    turo_container_close(&ctx, 0);
}

void test_turo_simple(void)
{
    turo_t ctx;
    turo_init(&ctx);
    turo_simple_s32(&ctx, 42);
    uint8_t buf8[] = {1, 2, 3};
    int32_t buf32[] = {-1, 1, 0x7FFFFFFF};
    turo_simple_array_u8(&ctx, buf8, sizeof(buf8));
    turo_simple_array_s32(&ctx, buf32, ARRAY_SIZE(buf32));
    turo_simple_dict_string(&ctx, "foo", "bar");
    turo_simple_dict_s32(&ctx, "baz", 42);
    turo_simple_exit_status(&ctx, -1);
}

Test *tests_turo_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_turo_impl),
        new_TestFixture(test_turo_helpers),
        new_TestFixture(test_turo_simple),
    };

    EMB_UNIT_TESTCALLER(turo_tests, NULL, NULL, fixtures);
    return (Test *)&turo_tests;
}

void tests_turo(void)
{
    TESTS_RUN(tests_turo_all());
}
