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

static void test_fmt_is_x(void)
{
    TEST_ASSERT_EQUAL_INT(1, fmt_is_digit('8'));
    TEST_ASSERT_EQUAL_INT(0, fmt_is_digit('a'));
    TEST_ASSERT_EQUAL_INT(0, fmt_is_digit('A'));
    TEST_ASSERT_EQUAL_INT(0, fmt_is_digit('\0'));

    TEST_ASSERT_EQUAL_INT(1, fmt_is_upper('A'));
    TEST_ASSERT_EQUAL_INT(0, fmt_is_upper('8'));
    TEST_ASSERT_EQUAL_INT(0, fmt_is_upper('a'));
    TEST_ASSERT_EQUAL_INT(0, fmt_is_upper('\0'));

    TEST_ASSERT_EQUAL_INT(1, fmt_is_number("123"));
    TEST_ASSERT_EQUAL_INT(1, fmt_is_number("00123"));
    TEST_ASSERT_EQUAL_INT(1, fmt_is_number("0"));
    TEST_ASSERT_EQUAL_INT(0, fmt_is_number("0xabc"));
    TEST_ASSERT_EQUAL_INT(0, fmt_is_number("muh"));

    /* white space is NOT ignored */
    TEST_ASSERT_EQUAL_INT(0, fmt_is_number(" 1"));
    TEST_ASSERT_EQUAL_INT(0, fmt_is_number("1 2"));
}

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

    /* Passing NULL just returns number of bytes that would have been written */
    TEST_ASSERT_EQUAL_INT(2, fmt_byte_hex(NULL, 0));
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

    /* Passing NULL just returns number of bytes that would have been written */
    TEST_ASSERT_EQUAL_INT(14, fmt_bytes_hex(NULL, val, 7));
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

    /* Passing NULL just returns number of bytes that would have been written */
    TEST_ASSERT_EQUAL_INT(8, fmt_bytes_hex_reverse(NULL, val, 4));
}

static void test_fmt_hex_byte(void)
{
    uint8_t byte;

    byte = fmt_hex_byte("00");
    TEST_ASSERT_EQUAL_INT(0x00, byte);

    byte = fmt_hex_byte("12");
    TEST_ASSERT_EQUAL_INT(0x12, byte);

    byte = fmt_hex_byte("AB");
    TEST_ASSERT_EQUAL_INT(0xAB, byte);

    byte = fmt_hex_byte("CD");
    TEST_ASSERT_EQUAL_INT(0xCD, byte);

    byte = fmt_hex_byte("EF");
    TEST_ASSERT_EQUAL_INT(0xEF, byte);

    byte = fmt_hex_byte("ab");
    TEST_ASSERT_EQUAL_INT(0xAB, byte);

    byte = fmt_hex_byte("cd");
    TEST_ASSERT_EQUAL_INT(0xCD, byte);

    byte = fmt_hex_byte("ef");
    TEST_ASSERT_EQUAL_INT(0xEF, byte);

    byte = fmt_hex_byte("Cd");
    TEST_ASSERT_EQUAL_INT(0xCD, byte);

    byte = fmt_hex_byte("eF");
    TEST_ASSERT_EQUAL_INT(0xEF, byte);

    byte = fmt_hex_byte("1234");
    TEST_ASSERT_EQUAL_INT(0x12, byte);
}

static void test_fmt_hex_bytes(void)
{
    uint8_t val = 0;
    uint8_t bytes = fmt_hex_bytes(&val, "");
    TEST_ASSERT_EQUAL_INT(0, bytes);
    TEST_ASSERT_EQUAL_INT(0, val);

    /* Odd length returns 0 */
    bytes = fmt_hex_bytes(&val, "A");
    TEST_ASSERT_EQUAL_INT(0, val);
    TEST_ASSERT_EQUAL_INT(0, bytes);

    /* If the input is odd, it is not written to the output at all */
    val = 0xFF;
    bytes = fmt_hex_bytes(&val, "ABCDE");
    TEST_ASSERT_EQUAL_INT(0xFF, val);
    TEST_ASSERT_EQUAL_INT(0, bytes);

    /* Passing NULL just returns number of bytes that would have been written */
    bytes = fmt_hex_bytes(NULL, "ABCDEF");
    TEST_ASSERT_EQUAL_INT(3, bytes);

    /* Odd length */
    bytes = fmt_hex_bytes(NULL, "ABCDE");
    TEST_ASSERT_EQUAL_INT(0, bytes);

    uint8_t val1[1] = { 0xff };
    bytes = fmt_hex_bytes(val1, "00");
    TEST_ASSERT_EQUAL_INT(1, bytes);
    TEST_ASSERT_EQUAL_INT(0, val1[0]);

    val1[0] = 0;
    bytes = fmt_hex_bytes(val1, "AB");
    TEST_ASSERT_EQUAL_INT(1, bytes);
    TEST_ASSERT_EQUAL_INT(0xAB, val1[0]);

    val1[0] = 0;
    bytes = fmt_hex_bytes(val1, "CD");
    TEST_ASSERT_EQUAL_INT(1, bytes);
    TEST_ASSERT_EQUAL_INT(0xCD, val1[0]);

    val1[0] = 0;
    bytes = fmt_hex_bytes(val1, "EF");
    TEST_ASSERT_EQUAL_INT(1, bytes);
    TEST_ASSERT_EQUAL_INT(0xEF, val1[0]);

    uint8_t val3[3];
    bytes = fmt_hex_bytes(val3, "0102aF");
    TEST_ASSERT_EQUAL_INT(3, bytes);
    TEST_ASSERT_EQUAL_INT(1, val3[0]);
    TEST_ASSERT_EQUAL_INT(2, val3[1]);
    TEST_ASSERT_EQUAL_INT(0xAF, val3[2]);
}

static void test_fmt_u16_hex(void)
{
    char out[8] = "zzzzzzzz";

    /* Check return count with null buffer input */
    TEST_ASSERT_EQUAL_INT(4, fmt_u16_hex(NULL, 0xBEEF));
    /* always four regardless of value size */
    TEST_ASSERT_EQUAL_INT(4, fmt_u16_hex(NULL, 0));

    TEST_ASSERT_EQUAL_INT(4, fmt_u16_hex(out, 0));
    TEST_ASSERT(memcmp(out, "0000zzzz", 8) == 0);

    TEST_ASSERT_EQUAL_INT(4, fmt_u16_hex(out, 0xBEEF));
    TEST_ASSERT(memcmp(out, "BEEFzzzz", 8) == 0);
}

static void test_fmt_u32_hex(void)
{
    char out[12] = "zzzzzzzzzzzz";

    /* Check return count with null buffer input */
    TEST_ASSERT_EQUAL_INT(8, fmt_u32_hex(NULL, 0xDEADBEEF));
    /* always eight regardless of value size */
    TEST_ASSERT_EQUAL_INT(8, fmt_u32_hex(NULL, 0));

    TEST_ASSERT_EQUAL_INT(8, fmt_u32_hex(out, 0));
    TEST_ASSERT(memcmp(out, "00000000zzzz", 12) == 0);

    TEST_ASSERT_EQUAL_INT(8, fmt_u32_hex(out, 0xDEADBEEF));
    TEST_ASSERT(memcmp(out, "DEADBEEFzzzz", 12) == 0);
}

static void test_fmt_u64_hex(void)
{
    char out[20] = "zzzzzzzzzzzzzzzzzzzz";

    /* Check return count with null buffer input */
    TEST_ASSERT_EQUAL_INT(16, fmt_u64_hex(NULL, 0x0DEAD0BEEF0CAFE0));
    /* always 16 regardless of value size */
    TEST_ASSERT_EQUAL_INT(16, fmt_u64_hex(NULL, 0));

    TEST_ASSERT_EQUAL_INT(16, fmt_u64_hex(out, 0));
    TEST_ASSERT(memcmp(out, "0000000000000000zzzz", 20) == 0);

    TEST_ASSERT_EQUAL_INT(16, fmt_u64_hex(out, 0x0DEAD0BEEF0CAFE0));
    TEST_ASSERT(memcmp(out, "0DEAD0BEEF0CAFE0zzzz", 20) == 0);
}

static void test_fmt_u16_dec(void)
{
    char out[8] = "zzzzzzzz";
    uint8_t chars = 0;

    /* Check return count with null buffer input */
    TEST_ASSERT_EQUAL_INT(5, fmt_u16_dec(NULL, 65535U));
    TEST_ASSERT_EQUAL_INT(1, fmt_u16_dec(NULL, 0));

    chars = fmt_u16_dec(out, 6556);
    TEST_ASSERT_EQUAL_INT(4, chars);
    TEST_ASSERT(memcmp(out, "6556zzzz", 8) == 0);

    chars = fmt_u16_dec(out, 65535);
    TEST_ASSERT_EQUAL_INT(5, chars);
    TEST_ASSERT(memcmp(out, "65535zzz", 8) == 0);
}

static void test_fmt_u32_dec(void)
{
    char out[16] = "zzzzzzzzzzzzzzzz";
    uint8_t chars = 0;

    /* Check return count with null buffer input */
    TEST_ASSERT_EQUAL_INT(10, fmt_u32_dec(NULL, 4294967295U));
    TEST_ASSERT_EQUAL_INT(5, fmt_u32_dec(NULL, 65535U));
    TEST_ASSERT_EQUAL_INT(1, fmt_u32_dec(NULL, 0));

    chars = fmt_u32_dec(out, 12345678);
    TEST_ASSERT_EQUAL_INT(8, chars);
    TEST_ASSERT(memcmp(out, "12345678zzzzzzzz", 16) == 0);

    chars = fmt_u32_dec(out, 1234567890);
    TEST_ASSERT_EQUAL_INT(10, chars);
    TEST_ASSERT(memcmp(out, "1234567890zzzzzz", 16) == 0);
}

static void test_fmt_u64_dec(void)
{
    char out[24] = "zzzzzzzzzzzzzzzzzzzzzzzz";
    uint8_t chars = 0;

    /* Check return count with null buffer input */
    TEST_ASSERT_EQUAL_INT(20, fmt_u64_dec(NULL, 18446744073709551615LLU));
    TEST_ASSERT_EQUAL_INT(10, fmt_u64_dec(NULL, 4294967295U));
    TEST_ASSERT_EQUAL_INT(5, fmt_u64_dec(NULL, 65535U));
    TEST_ASSERT_EQUAL_INT(1, fmt_u64_dec(NULL, 0));

    chars = fmt_u64_dec(out, 1234567890123456789LLU);
    TEST_ASSERT_EQUAL_INT(19, chars);
    TEST_ASSERT(memcmp(out, "1234567890123456789zzzzz", 24) == 0);
}

static void test_fmt_u64_dec_zero(void)
{
    char out[24] = "zzzzzzzzzzzzzzzzzzzzzzzz";
    uint8_t chars = 0;

    chars = fmt_u64_dec(out, 0);
    TEST_ASSERT_EQUAL_INT(1, chars);
    TEST_ASSERT(memcmp(out, "0zzzzzzzzzzzzzzzzzzzzzzz", 24) == 0);
}

static void test_fmt_u64_dec_u64max(void)
{
    char out[24] = "zzzzzzzzzzzzzzzzzzzzzzzz";
    uint8_t chars = 0;

    chars = fmt_u64_dec(out, 18446744073709551615LLU);
    TEST_ASSERT_EQUAL_INT(20, chars);
    TEST_ASSERT(memcmp(out, "18446744073709551615zzzz", 24) == 0);
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

    val = -1;
    fpp = -24;
    len = fmt_s32_dfp(NULL, val, fpp);
    TEST_ASSERT_EQUAL_INT(3 - fpp, len);
    act_len = fmt_s32_dfp(out, val, fpp);
    TEST_ASSERT_EQUAL_INT(3 - fpp, act_len);
    out[act_len] = '\0';
    TEST_ASSERT_EQUAL_STRING("-0.000000000000000000000001", (char *)out);

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

    TEST_ASSERT_EQUAL_INT(7, fmt_str(NULL, string1));
    TEST_ASSERT_EQUAL_INT(fmt_strlen(string1), fmt_str(&string2[0], string1));
    TEST_ASSERT_EQUAL_STRING(string1, &string2[0]);
}

static void test_fmt_char(void)
{
    char string[] = "zzzzzzzzz";

    TEST_ASSERT_EQUAL_INT(1, fmt_char(NULL, 'c'));
    TEST_ASSERT_EQUAL_INT(1, fmt_char(string, 'c'));
    string[1] = '\0';
    TEST_ASSERT_EQUAL_STRING("c", &string[0]);
}

static void test_fmt_to_lower(void)
{
    const char string_up[]  = "AbCdeFGHijkLM";
    char string[]           = "zzzzzzzzzzzzzzz";

    TEST_ASSERT_EQUAL_INT(fmt_strlen(string_up), fmt_to_lower(NULL, string_up));
    TEST_ASSERT_EQUAL_INT(fmt_strlen(string_up), fmt_to_lower(string, string_up));
    string[fmt_strlen(string_up)] = '\0';
    TEST_ASSERT_EQUAL_STRING("abcdefghijklm", &string[0]);
}

static void test_scn_u32_dec(void)
{
    const char *string1 = "123456789";
    uint32_t val1 = 123456789;
    uint32_t val2 = 12345;

    TEST_ASSERT_EQUAL_INT(val1, scn_u32_dec(string1, 9));
    TEST_ASSERT_EQUAL_INT(val2, scn_u32_dec(string1, 5));
}

static void test_scn_u32_hex(void)
{
    /* ´x´ is not a valid hexadecimal character */
    TEST_ASSERT_EQUAL_INT(0x0, scn_u32_hex("0xABCD", 4));
    /* so are these: */
    TEST_ASSERT_EQUAL_INT(0x9, scn_u32_hex("9 ABCD", 4));
    TEST_ASSERT_EQUAL_INT(0x9, scn_u32_hex("9-ABCD", 4));
    TEST_ASSERT_EQUAL_INT(0x9, scn_u32_hex("9+ABCD", 4));
    TEST_ASSERT_EQUAL_INT(0xab, scn_u32_hex("AB_CD", 4));
    TEST_ASSERT_EQUAL_INT(0x9, scn_u32_hex("9:3kCD", 4));
    TEST_ASSERT_EQUAL_INT(0x9, scn_u32_hex("9}3kCD", 4));
    TEST_ASSERT_EQUAL_INT(0x9, scn_u32_hex("9?3kCD", 4));

    /* Stop on the length argument or on the null terminator */
    TEST_ASSERT_EQUAL_INT(0xab12ce4f, scn_u32_hex("aB12cE4F", 8));
    TEST_ASSERT_EQUAL_INT(0xab1, scn_u32_hex("aB12cE4F", 3));
    TEST_ASSERT_EQUAL_INT(0xab12ce4f, scn_u32_hex("aB12cE4F", 9));
    TEST_ASSERT_EQUAL_INT(0xab, scn_u32_hex("aB", 9));
}

static void test_fmt_lpad(void)
{
    const char base[] = "abcd";
    char string[9] = {0};

    strcpy(string, base);

    TEST_ASSERT_EQUAL_INT(8, fmt_lpad(NULL, 4, 8, ' '));

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
        new_TestFixture(test_fmt_is_x),
        new_TestFixture(test_fmt_byte_hex),
        new_TestFixture(test_fmt_bytes_hex),
        new_TestFixture(test_fmt_bytes_hex_reverse),
        new_TestFixture(test_fmt_hex_byte),
        new_TestFixture(test_fmt_hex_bytes),
        new_TestFixture(test_fmt_u16_hex),
        new_TestFixture(test_fmt_u32_hex),
        new_TestFixture(test_fmt_u64_hex),
        new_TestFixture(test_fmt_u16_dec),
        new_TestFixture(test_fmt_u32_dec),
        new_TestFixture(test_fmt_u64_dec),
        new_TestFixture(test_fmt_u64_dec_zero),
        new_TestFixture(test_fmt_u64_dec_u64max),
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
        new_TestFixture(test_fmt_char),
        new_TestFixture(test_fmt_to_lower),
        new_TestFixture(test_scn_u32_dec),
        new_TestFixture(test_scn_u32_hex),
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
