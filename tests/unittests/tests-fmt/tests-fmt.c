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
#include <string.h>

#include "embUnit/embUnit.h"

#include "fmt.h"
#include "tests-fmt.h"

static void test_fmt_byte_hex(void)
{
    char out[8] = "zzzzzzz";

    out[2] = '\0';
    TEST_ASSERT_EQUAL_INT(2, fmt_byte_hex(out, 0));
    TEST_ASSERT_EQUAL_STRING("00", (char *) out);

    TEST_ASSERT_EQUAL_INT(2, fmt_byte_hex(out, 128));
    TEST_ASSERT_EQUAL_STRING("80", (char *) out);

    TEST_ASSERT_EQUAL_INT(2, fmt_byte_hex(out, 255));
    TEST_ASSERT_EQUAL_STRING("FF", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zzzz", &out[3]);
}

static void test_fmt_bytes_hex(void)
{
    char out[20] = "zzzzzzzzzzzzzzzzzzz";
    uint8_t val[7] = { 0xAA, 9, 8, 7, 6, 0xA8, 0xEF};
    uint8_t bytes = 0;

    bytes = fmt_bytes_hex(out, val, 0);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(0, bytes);
    TEST_ASSERT_EQUAL_STRING("", (char *) out);

    bytes = fmt_bytes_hex(out, val, 1);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(2, bytes);
    TEST_ASSERT_EQUAL_STRING("AA", (char *) out);

    bytes = fmt_bytes_hex(out, val, 2);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(4, bytes);
    TEST_ASSERT_EQUAL_STRING("AA09", (char *) out);

    bytes = fmt_bytes_hex(out, val, 3);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(6, bytes);
    TEST_ASSERT_EQUAL_STRING("AA0908", (char *) out);

    bytes = fmt_bytes_hex(out, val, 4);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(8, bytes);
    TEST_ASSERT_EQUAL_STRING("AA090807", (char *) out);

    bytes = fmt_bytes_hex(out, val, 5);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(10, bytes);
    TEST_ASSERT_EQUAL_STRING("AA09080706", (char *) out);

    bytes = fmt_bytes_hex(out, val, 6);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(12, bytes);
    TEST_ASSERT_EQUAL_STRING("AA09080706A8", (char *) out);

    bytes = fmt_bytes_hex(out, val, 7);
    out[bytes] = '\0';
    TEST_ASSERT_EQUAL_INT(14, bytes);
    TEST_ASSERT_EQUAL_STRING("AA09080706A8EF", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zzzz", &out[15]);
}

static void test_fmt_bytes_hex_reverse(void)
{
    char out[12] = "zzzzzzzzzzz";
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

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zz", &out[9]);
}

static void test_fmt_hex_byte(void)
{
    char hex[3] = "00";
    uint8_t byte;

    byte = fmt_hex_byte(hex);
    TEST_ASSERT_EQUAL_INT(0x00, byte);

    memcpy(hex, "12", 2);
    byte = fmt_hex_byte(hex);
    TEST_ASSERT_EQUAL_INT(0x12, byte);

    memcpy(hex, "AB", 2);
    byte = fmt_hex_byte(hex);
    TEST_ASSERT_EQUAL_INT(0xAB, byte);

    memcpy(hex, "CD", 2);
    byte = fmt_hex_byte(hex);
    TEST_ASSERT_EQUAL_INT(0xCD, byte);

    memcpy(hex, "EF", 2);
    byte = fmt_hex_byte(hex);
    TEST_ASSERT_EQUAL_INT(0xEF, byte);
}

static void test_fmt_hex_bytes(void)
{
    uint8_t val = 0;
    uint8_t bytes = fmt_hex_bytes(&val, "");
    TEST_ASSERT_EQUAL_INT(0, bytes);
    TEST_ASSERT_EQUAL_INT(0, val);

    bytes = fmt_hex_bytes(&val, "A");
    TEST_ASSERT_EQUAL_INT(0, val);
    TEST_ASSERT_EQUAL_INT(0, bytes);

    char hex2[3] = "00";
    uint8_t val1[1] = { 0 };
    bytes = fmt_hex_bytes(val1, hex2);
    TEST_ASSERT_EQUAL_INT(1, bytes);
    TEST_ASSERT_EQUAL_INT(0, val1[0]);

    memcpy(hex2, "AB", 2);
    hex2[2] = '\0';
    val1[0] = 0;
    bytes = fmt_hex_bytes(val1, hex2);
    TEST_ASSERT_EQUAL_INT(1, bytes);
    TEST_ASSERT_EQUAL_INT(0xAB, val1[0]);

    memcpy(hex2, "CD", 2);
    hex2[2] = '\0';
    val1[0] = 0;
    bytes = fmt_hex_bytes(val1, hex2);
    TEST_ASSERT_EQUAL_INT(1, bytes);
    TEST_ASSERT_EQUAL_INT(0xCD, val1[0]);

    memcpy(hex2, "EF", 2);
    hex2[2] = '\0';
    val1[0] = 0;
    bytes = fmt_hex_bytes(val1, hex2);
    TEST_ASSERT_EQUAL_INT(1, bytes);
    TEST_ASSERT_EQUAL_INT(0xEF, val1[0]);

    char hex6[] = "0102aF";
    uint8_t val3[3];
    bytes = fmt_hex_bytes(val3, hex6);
    TEST_ASSERT_EQUAL_INT(3, bytes);
    TEST_ASSERT_EQUAL_INT(1, val3[0]);
    TEST_ASSERT_EQUAL_INT(2, val3[1]);
    TEST_ASSERT_EQUAL_INT(0xAF, val3[2]);
}

static void test_fmt_u32_hex(void)
{
    char out[12] = "zzzzzzzzzzz";
    uint32_t val = 3735928559;

    out[8] = '\0';
    TEST_ASSERT_EQUAL_INT(8, fmt_u32_hex(out, val));
    TEST_ASSERT_EQUAL_STRING("DEADBEEF", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zz", &out[9]);
}

static void test_fmt_u64_hex(void)
{
    char out[20] = "zzzzzzzzzzzzzzzzzzz";
    uint64_t val = 1002843385516306400;

    out[16] = '\0';
    TEST_ASSERT_EQUAL_INT(16, fmt_u64_hex(out, val));
    TEST_ASSERT_EQUAL_STRING("0DEAD0BEEF0CAFE0", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zz", &out[17]);
}

static void test_fmt_u32_dec(void)
{
    char out[16] = "zzzzzzzzzzzzzzz";
    uint32_t val = 12345678;
    uint8_t chars = 0;

    chars = fmt_u32_dec(out, val);
    TEST_ASSERT_EQUAL_INT(8, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("12345678", (char *) out);

    val = 1234567890;
    chars = fmt_u32_dec(out, val);
    TEST_ASSERT_EQUAL_INT(10, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("1234567890", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zzzz", &out[11]);
}

static void test_fmt_u16_dec(void)
{
    char out[8] = "zzzzzzz";
    uint16_t val = 6556;
    uint8_t chars = 0;

    chars = fmt_u16_dec(out, val);
    TEST_ASSERT_EQUAL_INT(4, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("6556", (char *) out);

    val = 65535;
    chars = fmt_u16_dec(out, val);
    TEST_ASSERT_EQUAL_INT(5, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("65535", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("z", &out[6]);
}

static void test_fmt_s32_dec_a(void)
{
    char out[16] = "zzzzzzzzzzzzzzz";
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

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zzzzzzzzz", &out[6]);
}

static void test_fmt_s32_dec_b(void)
{
    char out[16] = "zzzzzzzzzzzzzzz";
    int32_t val = 2147483647;
    uint8_t chars = 0;

    chars = fmt_s32_dec(out, val);
    TEST_ASSERT_EQUAL_INT(10, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("2147483647", (char *) out);

    val = -2147483648;
    chars = fmt_s32_dec(out, val);
    TEST_ASSERT_EQUAL_INT(11, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("-2147483648", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zzz", &out[12]);
}

static void test_fmt_s64_dec_a(void)
{
    char out[24] = "zzzzzzzzzzzzzzzzzzzzzzz";
    int64_t val = 9876;
    uint8_t chars = 0;

    chars = fmt_s64_dec(out, val);
    TEST_ASSERT_EQUAL_INT(4, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("9876", (char *) out);

    val = -9876;
    chars = fmt_s64_dec(out, val);
    TEST_ASSERT_EQUAL_INT(5, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("-9876", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zzzzzzzzzzzzzzzzz", &out[6]);
}

static void test_fmt_s64_dec_b(void)
{
    char out[24] = "zzzzzzzzzzzzzzzzzzzzzzz";
    int64_t val = 2147483647;
    uint8_t chars = 0;

    chars = fmt_s64_dec(out, val);
    TEST_ASSERT_EQUAL_INT(10, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("2147483647", (char *) out);

    val = -2147483648;
    chars = fmt_s64_dec(out, val);
    TEST_ASSERT_EQUAL_INT(11, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("-2147483648", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zzzzzzzzzzz", &out[12]);
}

static void test_fmt_s64_dec_c(void)
{
    char out[24] = "zzzzzzzzzzzzzzzzzzzzzzz";
    int64_t val = 9223372036854775807ll;
    uint8_t chars = 0;

    chars = fmt_s64_dec(out, val);
    TEST_ASSERT_EQUAL_INT(19, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("9223372036854775807", (char *) out);

    /* typing -9223372036854775808 as a decimal literal causes a compiler warning
     * "integer constant is so large that it is unsigned"
     * because of compiler internal workings and the C standard, using
     * -9223372036854775807-1 works around this peculiarity.
     * See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52661 */
    val = -9223372036854775807ll - 1;
    chars = fmt_s64_dec(out, val);
    TEST_ASSERT_EQUAL_INT(20, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("-9223372036854775808", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zz", &out[21]);
}

static void test_fmt_u64_dec_a(void)
{
    char out[24] = "zzzzzzzzzzzzzzzzzzzzzzz";
    uint64_t val = 0;
    uint8_t chars = 0;

    chars = fmt_u64_dec(out, val);
    TEST_ASSERT_EQUAL_INT(1, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("0", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zzzzzzzzzzzzzzzzzzzzz", &out[2]);
}

static void test_fmt_u64_dec_b(void)
{
    char out[24] = "zzzzzzzzzzzzzzzzzzzzzzz";
    uint64_t val = 18446744073709551615LLU;
    uint8_t chars = 0;

    chars = fmt_u64_dec(out, val);
    TEST_ASSERT_EQUAL_INT(20, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("18446744073709551615", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zz", &out[21]);
}

static void test_fmt_u64_dec_c(void)
{
    char out[24] = "zzzzzzzzzzzzzzzzzzzzzzz";
    uint64_t val = 1234567890123456789LLU;
    uint8_t chars = 0;

    chars = fmt_u64_dec(out, val);
    TEST_ASSERT_EQUAL_INT(19, chars);
    out[chars] = '\0';
    TEST_ASSERT_EQUAL_STRING("1234567890123456789", (char *) out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zzz", &out[20]);
}

static void test_fmt_s16_dec(void)
{
    char out[10] = "zzzzzzzzz";
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

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("zz", &out[7]);
}

static void test_fmt_s16_dfp(void)
{
    char out[14] = "zzzzzzzzzzzzz";
    int16_t val;
    int fpp;
    size_t len, act_len;

    val = 0;
    fpp = -3;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(5, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(5, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("0.000", (char *)out);

    val = 12345;
    fpp = -4;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(6, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(6, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("1.2345", (char *)out);

    val = 12030;
    fpp = -3;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(6, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(6, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("12.030", (char *)out);

    val = -3548;
    fpp = -2;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(6, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(6, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-35.48", (char *)out);

    val = -23;
    fpp = -4;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-0.0023", (char *)out);

    val = 50;
    fpp = -3;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(5, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(5, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("0.050", (char *)out);

    val = -12345;
    fpp = -0;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(6, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(6, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-12345", (char *)out);

    val = 31987;
    fpp = -5;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("0.31987", (char *)out);


    val = -32768;
    fpp = -2;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-327.68", (char *)out);

    /* test also for positive fp digits */
    val = 32767;
    fpp = 0;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(5, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(5, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("32767", (char *)out);

    val = 31987;
    fpp = 3;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(8, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(8, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("31987000", (char *)out);

    val = -23;
    fpp = 4;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-230000", (char *)out);

    val = -1023;
    fpp = 6;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(11, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(11, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-1023000000", (char *)out);

    val = -32768;
    fpp = -7;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-0.0032768", (char *)out);

    val = 17;
    fpp = 9;
    len = fmt_s16_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(11, len);
    act_len = fmt_s16_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(11, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("17000000000", (char *)out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("z", &out[12]);
}

static void test_fmt_s32_dfp(void)
{
    char out[30] = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
    int32_t val;
    unsigned fpp;
    size_t len;
    size_t act_len;

    val = 0;
    fpp = -7;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(9, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(9, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("0.0000000", (char *)out);

    val = 123456789;
    fpp = -7;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("12.3456789", (char *)out);

    val = 120030;
    fpp = -3;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(7, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("120.030", (char *)out);

    val = -314159;
    fpp = -5;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(8, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(8, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-3.14159", (char *)out);

    val = -23;
    fpp = -7;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-0.0000023", (char *)out);

    val = 50;
    fpp = -6;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(8, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(8, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("0.000050", (char *)out);

    val = -123456789;
    fpp = 0;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-123456789", (char *)out);

    val = 2147483647; /* INT32_MAX */
    fpp = 2;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(12, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(12, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("214748364700", (char *)out);

    val = 2147483647; /* INT32_MAX */
    fpp = -5;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(11, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(11, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("21474.83647", (char *)out);

    val = -2147483648; /* INT32_MIN */
    fpp = 2;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(13, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(13, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-214748364800", (char *)out);

    val = -2147483648; /* INT32_MIN */
    fpp = -5;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(12, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(12, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-21474.83648", (char *)out);

    val = -1;
    fpp = 25;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(27, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(27, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-10000000000000000000000000", (char *)out);

    val = -1;
    fpp = -7;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(10, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-0.0000001", (char *)out);

    /* check that the buffer was not overflowed */
    TEST_ASSERT_EQUAL_STRING("z", &out[28]);
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

static void test_fmt_strnlen(void)
{
    const char *empty_str = "";
    const char *short_str = "short";
    const char *long_str = "this is a long string";

    TEST_ASSERT_EQUAL_INT(0, fmt_strnlen(empty_str, 16));
    TEST_ASSERT_EQUAL_INT(5, fmt_strnlen(short_str, 16));
    TEST_ASSERT_EQUAL_INT(16, fmt_strnlen(long_str, 16));
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

static void test_fmt_lpad(void)
{
    const char base[] = "abcd";
    char string[9] = {0};

    strcpy(string, base);

    fmt_lpad(string, 4, 8, ' ');

    TEST_ASSERT_EQUAL_STRING("    abcd", (char*)string);

    fmt_lpad(string, 0, 0, '1');

    TEST_ASSERT_EQUAL_STRING("    abcd", (char*)string);

    fmt_lpad(string, 4, 0, '2');

    TEST_ASSERT_EQUAL_STRING("    abcd", (char*)string);

    fmt_lpad(string, 0, 4, '3');

    TEST_ASSERT_EQUAL_STRING("3333abcd", (char*)string);

    fmt_lpad(string, 8, 8, '4');

    TEST_ASSERT_EQUAL_STRING("3333abcd", (char*)string);

    fmt_lpad(string, 4, 8, 'x');

    TEST_ASSERT_EQUAL_STRING((char*)string, "xxxx3333");
}

Test *tests_fmt_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_fmt_byte_hex),
        new_TestFixture(test_fmt_bytes_hex),
        new_TestFixture(test_fmt_bytes_hex_reverse),
        new_TestFixture(test_fmt_hex_byte),
        new_TestFixture(test_fmt_hex_bytes),
        new_TestFixture(test_fmt_u32_hex),
        new_TestFixture(test_fmt_u64_hex),
        new_TestFixture(test_fmt_u32_dec),
        new_TestFixture(test_fmt_u64_dec_a),
        new_TestFixture(test_fmt_u64_dec_b),
        new_TestFixture(test_fmt_u64_dec_c),
        new_TestFixture(test_fmt_u16_dec),
        new_TestFixture(test_fmt_s32_dec_a),
        new_TestFixture(test_fmt_s32_dec_b),
        new_TestFixture(test_fmt_s64_dec_a),
        new_TestFixture(test_fmt_s64_dec_b),
        new_TestFixture(test_fmt_s64_dec_c),
        new_TestFixture(test_fmt_s16_dec),
        new_TestFixture(test_fmt_s16_dfp),
        new_TestFixture(test_fmt_s32_dfp),
        new_TestFixture(test_fmt_strlen),
        new_TestFixture(test_fmt_strnlen),
        new_TestFixture(test_fmt_str),
        new_TestFixture(test_scn_u32_dec),
        new_TestFixture(test_fmt_lpad),
    };

    EMB_UNIT_TESTCALLER(fmt_tests, NULL, NULL, fixtures);

    return (Test *)&fmt_tests;
}

void tests_fmt(void)
{
    TESTS_RUN(tests_fmt_tests());
}
/** @} */
