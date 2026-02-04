/*
 * SPDX-FileCopyrightText: 2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <assert.h>
#include <stdalign.h>
#include <stdint.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "checksum/fletcher32.h"

#include "tests-checksum.h"

/* String is longer than 359 so it checks the wrap-around property of
 * fletcher32 */
static const alignas(uint16_t) unsigned char wrap_around_data[] =
        "AD3Awn4kb6FtcsyE0RU25U7f55Yncn3LP3oEx9Gl4qr7iDW7I8L6Pbw9jNnh0sE4DmCKuc"
        "d1J8I34vn31W924y5GMS74vUrZQc08805aj4Tf66HgL1cO94os10V2s2GDQ825yNh9Yuq3"
        "QHcA60xl31rdA7WskVtCXI7ruH1A4qaR6Uk454hm401lLmv2cGWt5KTJmr93d3JsGaRRPs"
        "4HqYi4mFGowo8fWv48IcA3N89Z99nf0A0H2R6P0uI4Tir682Of3Rk78DUB2dIGQRRpdqVT"
        "tLhgfET2gUGU65V3edSwADMqRttI9JPVz8JS37g5QZj4Ax56rU1u0m0K8YUs57UYG5645n"
        "byNy4yqxu7";

static uint32_t _fletcher32(const uint8_t *buf, size_t len)
{
    return fletcher32((const uint16_t *)(uintptr_t)buf, len / 2);
}

static int calc_and_compare_checksum(const unsigned char *buf, size_t len,
                                     uint32_t expected)
{
    return _fletcher32(buf, len) == expected;
}

static void test_checksum_fletcher32_empty(void)
{
    /* the initial checksum value is 0xFFFFFFFF */
    alignas(uint16_t) unsigned char buf[] = "";
    uint32_t expect = 0xFFFFFFFF;

    TEST_ASSERT(calc_and_compare_checksum(buf, sizeof(buf) - 1, expect));
}

static void test_checksum_fletcher32_0to1_undetected(void)
{
    /* fletcher cannot distinguish between all 0 and all 1 segments */
    alignas(uint16_t) unsigned char buf0[16] = {
        0xA1, 0xA1, 0xA1, 0xA1,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x1A, 0x1A, 0x1A, 0x1A,
    };
    uint32_t expect = _fletcher32(buf0, sizeof(buf0));
    alignas(uint16_t) unsigned char buf1[16] = {
        0xA1, 0xA1, 0xA1, 0xA1,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0x1A, 0x1A, 0x1A, 0x1A,
    };

    TEST_ASSERT(calc_and_compare_checksum(buf1, sizeof(buf1), expect));
}

static void test_checksum_fletcher32_atof(void)
{
    /* XXX: not verified with external implementation yet */
    alignas(uint16_t) unsigned char buf[] = "abcdef";
    uint32_t expect = 0x56502d2a;

    TEST_ASSERT(calc_and_compare_checksum(buf, sizeof(buf) - 1, expect));
}

static void test_checksum_fletcher32_wrap_around(void)
{
    /* XXX: not verified with external implementation yet */
    uint32_t expect = 0x5bac8c3d;

    TEST_ASSERT(calc_and_compare_checksum(wrap_around_data,
                                          sizeof(wrap_around_data) - 1, expect));
}

static void test_checksum_fletcher32_wrap_around_piecewise(void)
{
    /* XXX: not verified with external implementation yet */
    uint32_t expect = 0x5bac8c3d;
    fletcher32_ctx_t ctx;
    fletcher32_init(&ctx);
    size_t full_len = sizeof(wrap_around_data) - 1;
    size_t initial_len = full_len / 2;
    fletcher32_update(&ctx, wrap_around_data, initial_len / 2);
    fletcher32_update(&ctx, (wrap_around_data + initial_len), (full_len - initial_len) / 2);
    uint32_t result = fletcher32_finish(&ctx);
    TEST_ASSERT_EQUAL_INT(result, expect);
}

Test *tests_checksum_fletcher32_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_checksum_fletcher32_empty),
        new_TestFixture(test_checksum_fletcher32_0to1_undetected),
        new_TestFixture(test_checksum_fletcher32_atof),
        new_TestFixture(test_checksum_fletcher32_wrap_around),
        new_TestFixture(test_checksum_fletcher32_wrap_around_piecewise),
    };

    EMB_UNIT_TESTCALLER(checksum_fletcher32_tests, NULL, NULL, fixtures);

    return (Test *)&checksum_fletcher32_tests;
}
