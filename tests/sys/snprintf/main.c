/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       snprintf test (correctness of standard format specifier
 *              implementation)
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static bool failed = false;

static void check(const char *expected, const char *got, int retval)
{
    if (retval != (int)strlen(expected)) {
        failed = true;
        printf("snprintf() returned %d, but expected %u\n",
               retval, (unsigned)strlen(expected));
    }

    if (strcmp(expected, got) != 0) {
        printf("Expected: \"%s\"\n"
               "Got:      \"%s\"\n",
               expected, got);
        failed = true;
    }
}

static void test_int8(void)
{
    static uint8_t u8 = 10;
    static int8_t s8 = -3;

    /* memory barrier to prevent the compiler from constant folding on the
     * snprintf calls */
    __asm__ volatile ("" ::: "memory");

    char buf[32];
    size_t len;

    len = snprintf(buf, sizeof(buf), "%" PRIu8, u8);
    check("10", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIx8, u8);
    check("a", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIx8, u8);
    check("0xa", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIX8, u8);
    check("A", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIX8, u8);
    check("0XA", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIo8, u8);
    check("12", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIo8, u8);
    check("012", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRId8, s8);
    check("-3", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIi8, s8);
    check("-3", buf, len);
}

static void test_int16(void)
{
    static uint16_t u16 = 45054;
    static int16_t s16 = -1337;

    /* memory barrier to prevent the compiler from constant folding on the
     * snprintf calls */
    __asm__ volatile ("" ::: "memory");

    char buf[32];
    size_t len;

    len = snprintf(buf, sizeof(buf), "%" PRIu16, u16);
    check("45054", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIx16, u16);
    check("affe", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIx16, u16);
    check("0xaffe", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIX16, u16);
    check("AFFE", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIX16, u16);
    check("0XAFFE", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIo16, u16);
    check("127776", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIo16, u16);
    check("0127776", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRId16, s16);
    check("-1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIi16, s16);
    check("-1337", buf, len);
}

static void test_int32(void)
{
    static uint32_t u32 = 2952663863;
    static int32_t s32 = -2147483648;

    /* memory barrier to prevent the compiler from constant folding on the
     * snprintf calls */
    __asm__ volatile ("" ::: "memory");

    char buf[32];
    size_t len;

    len = snprintf(buf, sizeof(buf), "%" PRIu32, u32);
    check("2952663863", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIx32, u32);
    check("affe1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIx32, u32);
    check("0xaffe1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIX32, u32);
    check("AFFE1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIX32, u32);
    check("0XAFFE1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIo32, u32);
    check("25777411467", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIo32, u32);
    check("025777411467", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRId32, s32);
    check("-2147483648", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIi32, s32);
    check("-2147483648", buf, len);
}

static void test_int64(void)
{
    static uint64_t u64 = 16045690984050070327ULL;
    static int64_t s64 = -9223372036854775807LL;

    /* memory barrier to prevent the compiler from constant folding on the
     * snprintf calls */
    __asm__ volatile ("" ::: "memory");

    char buf[32];
    size_t len;

    len = snprintf(buf, sizeof(buf), "%" PRIu64, u64);
    check("16045690984050070327", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIx64, u64);
    check("deadbeefaffe1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIx64, u64);
    check("0xdeadbeefaffe1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIX64, u64);
    check("DEADBEEFAFFE1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIX64, u64);
    check("0XDEADBEEFAFFE1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIo64, u64);
    check("1572555756765777411467", buf, len);

    len = snprintf(buf, sizeof(buf), "%#" PRIo64, u64);
    check("01572555756765777411467", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRId64, s64);
    check("-9223372036854775807", buf, len);

    len = snprintf(buf, sizeof(buf), "%" PRIi64, s64);
    check("-9223372036854775807", buf, len);
}

static void test_size(void)
{
    static size_t s = 42;
    static ssize_t ss = -1;
    static ptrdiff_t p = 1337;

    /* memory barrier to prevent the compiler from constant folding on the
     * snprintf calls */
    __asm__ volatile ("" ::: "memory");

    char buf[32];
    size_t len;

    len = snprintf(buf, sizeof(buf), "%zu", s);
    check("42", buf, len);

    len = snprintf(buf, sizeof(buf), "%zd", ss);
    check("-1", buf, len);

    len = snprintf(buf, sizeof(buf), "%td", p);
    check("1337", buf, len);

    len = snprintf(buf, sizeof(buf), "%tu", p);
    check("1337", buf, len);
}

static void test_flags_widths(void)
{
    static uint16_t u16 = 42;
    static int16_t s16 = -42;

    /* memory barrier to prevent the compiler from constant folding on the
     * snprintf calls */
    __asm__ volatile ("" ::: "memory");

    char buf[32];
    size_t len;

    len = snprintf(buf, sizeof(buf), "%03" PRIu16, u16);
    check("042", buf, len);

    len = snprintf(buf, sizeof(buf), "%3" PRIu16, u16);
    check(" 42", buf, len);

    len = snprintf(buf, sizeof(buf), "%-3" PRIu16, u16);
    check("42 ", buf, len);

    len = snprintf(buf, sizeof(buf), "%*" PRIu16, 8, u16);
    check("      42", buf, len);

    len = snprintf(buf, sizeof(buf), "%-*" PRIu16, 8, u16);
    check("42      ", buf, len);

    len = snprintf(buf, sizeof(buf), "%+" PRId16, (int16_t)u16);
    check("+42", buf, len);

    len = snprintf(buf, sizeof(buf), "%04" PRId16, s16);
    check("-042", buf, len);

    len = snprintf(buf, sizeof(buf), "%4" PRId16, s16);
    check(" -42", buf, len);

    len = snprintf(buf, sizeof(buf), "%-4" PRId16, s16);
    check("-42 ", buf, len);
}

int main(void)
{
    puts("Testing snprintf() implementation...");
    test_int8();
    test_int16();
    test_int32();
    test_int64();
    test_size();
    test_flags_widths();

    if (failed) {
        puts("TEST FAILED!");
    }
    else {
        puts("Test succeeded");
    }

    return 0;
}
