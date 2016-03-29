/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "fmt.h"
#include "tests-fmt.h"

static void test_fmt_byte_hex(void)
{
    char out[3] = "--";

    TEST_ASSERT_EQUAL_INT(2, fmt_byte_hex(out, 0));
    TEST_ASSERT_EQUAL_STRING("00", (char *) out);

    TEST_ASSERT_EQUAL_INT(2, fmt_byte_hex(out, 128));
    TEST_ASSERT_EQUAL_STRING("80", (char *) out);

    TEST_ASSERT_EQUAL_INT(2, fmt_byte_hex(out, 255));
    TEST_ASSERT_EQUAL_STRING("FF", (char *) out);
}

static void test_fmt_bytes_hex_reverse(void)
{
    char out[10];
    uint8_t val[4] = { 9, 8, 7, 6 };
    uint8_t bytes = 0;

    bytes = fmt_bytes_hex_reverse(out, val, 1);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(2, bytes);
    TEST_ASSERT_EQUAL_STRING("09", (char *) out);

    bytes = fmt_bytes_hex_reverse(out, val, 2);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(4, bytes);
    TEST_ASSERT_EQUAL_STRING("0809", (char *) out);

    bytes = fmt_bytes_hex_reverse(out, val, 3);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(6, bytes);
    TEST_ASSERT_EQUAL_STRING("070809", (char *) out);

    bytes = fmt_bytes_hex_reverse(out, val, 4);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(8, bytes);
    TEST_ASSERT_EQUAL_STRING("06070809", (char *) out);
}

static void test_fmt_u32_hex(void)
{
    char out[9] = "--------";
    uint32_t val = 3735928559;

    TEST_ASSERT_EQUAL_INT(8, fmt_u32_hex(out, val));
    TEST_ASSERT_EQUAL_STRING("DEADBEEF", (char *) out);
}

static void test_fmt_u64_hex(void)
{
    char out[17] = "----------------";
    uint64_t val = 1002843385516306400;

    TEST_ASSERT_EQUAL_INT(16, fmt_u64_hex(out, val));
    TEST_ASSERT_EQUAL_STRING("0DEAD0BEEF0CAFE0", (char *) out);
}

static void test_fmt_u32_dec(void)
{
    char out[9] = "--------";
    uint32_t val = 12345678;
    uint8_t chars = 0;

    chars = fmt_u32_dec(out, val);
    TEST_ASSERT_EQUAL_INT(8, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("12345678", (char *) out);
}

static void test_fmt_u16_dec(void)
{
    char out[5] = "----";
    uint16_t val = 6556;
    uint8_t chars = 0;

    chars = fmt_u16_dec(out, val);
    TEST_ASSERT_EQUAL_INT(4, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("6556", (char *) out);
}

static void test_fmt_s32_dec(void)
{
    char out[8] = "--------";
    int32_t val = 9876;
    uint8_t chars = 0;

    chars = fmt_s32_dec(out, val);
    TEST_ASSERT_EQUAL_INT(4, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("9876", (char *) out);

    val = -9876;
    chars = fmt_s32_dec(out, val);
    TEST_ASSERT_EQUAL_INT(5, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("-9876", (char *) out);
}

static void test_rmt_s16_dec(void)
{
    char out[7] = "-------";
    int16_t val;
    size_t len;

    val = 0;
    len = fmt_s16_dec(out, val);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(1, len);
    TEST_ASSERT_EQUAL_STRING("0", (char *)out);

    val = -32000;
    len = fmt_s16_dec(out, val);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(6, len);
    TEST_ASSERT_EQUAL_STRING("-32000", (char *)out);

    val = 12345;
    len = fmt_s16_dec(out, val);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("12345", (char *)out);
}

static void test_rmt_s16_dfp(void)
{
    char out[8] = "--------";
    int16_t val;
    unsigned fpp;
    size_t len;

    val = 0;
    fpp = 3;
    len = fmt_s16_dfp(out, val, fpp);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("0.000", (char *)out);

    val = 12345;
    fpp = 4;
    len = fmt_s16_dfp(out, val, fpp);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(6, len);
    TEST_ASSERT_EQUAL_STRING("1.2345", (char *)out);

    val = 12030;
    fpp = 3;
    len = fmt_s16_dfp(out, val, fpp);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(6, len);
    TEST_ASSERT_EQUAL_STRING("12.030", (char *)out);

    val = -3548;
    fpp = 2;
    len = fmt_s16_dfp(out, val, fpp);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(6, len);
    TEST_ASSERT_EQUAL_STRING("-35.48", (char *)out);

    val = -23;
    fpp = 4;
    len = fmt_s16_dfp(out, val, fpp);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(7, len);
    TEST_ASSERT_EQUAL_STRING("-0.0023", (char *)out);

    val = 50;
    fpp = 3;
    len = fmt_s16_dfp(out, val, fpp);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(5, len);
    TEST_ASSERT_EQUAL_STRING("0.050", (char *)out);

    val = -12345;
    fpp = 0;
    len = fmt_s16_dfp(out, val, fpp);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(6, len);
    TEST_ASSERT_EQUAL_STRING("-12345", (char *)out);

    val = 31987;
    fpp = 5;
    len = fmt_s16_dfp(out, val, fpp);
    out[len] = '\0';
    TEST_ASSERT_EQUAL_INT(0, len);
    TEST_ASSERT_EQUAL_STRING("", (char *)out);
}

static void test_fmt_strlen(void)
{
    const char *empty_str = "";
    const char *short_str = "short";
    const char *long_str = "this is a long string";

    TEST_ASSERT_EQUAL_INT(0, fmt_strlen(empty_str));
    TEST_ASSERT_EQUAL_INT(5, fmt_strlen(short_str));
    TEST_ASSERT_EQUAL_INT(21, fmt_strlen(long_str));
}

static void test_fmt_str(void)
{
    const char *string1 = "string1";
    char string2[]      = "StRiNg2";

    TEST_ASSERT_EQUAL_INT(fmt_strlen(string1), fmt_str(&string2[0], string1));
    TEST_ASSERT_EQUAL_STRING(string1, &string2[0]);
}

static void test_scn_u32_dec(void)
{
    const char *string1 = "123456789";
    uint32_t val1 = 123456789;
    uint32_t val2 = 12345;

    TEST_ASSERT_EQUAL_INT(val1, scn_u32_dec(string1, 9));
    TEST_ASSERT_EQUAL_INT(val2, scn_u32_dec(string1, 5));
}

Test *tests_fmt_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_fmt_byte_hex),
        new_TestFixture(test_fmt_bytes_hex_reverse),
        new_TestFixture(test_fmt_u32_hex),
        new_TestFixture(test_fmt_u64_hex),
        new_TestFixture(test_fmt_u32_dec),
        new_TestFixture(test_fmt_u16_dec),
        new_TestFixture(test_fmt_s32_dec),
        new_TestFixture(test_rmt_s16_dec),
        new_TestFixture(test_rmt_s16_dfp),
        new_TestFixture(test_fmt_strlen),
        new_TestFixture(test_fmt_str),
        new_TestFixture(test_scn_u32_dec),
    };

    EMB_UNIT_TESTCALLER(fmt_tests, NULL, NULL, fixtures);

    return (Test *)&fmt_tests;
}

void tests_fmt(void)
{
    TESTS_RUN(tests_fmt_tests());
}
/** @} */
