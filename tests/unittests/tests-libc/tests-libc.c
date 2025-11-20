/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <string.h>
#include <endian.h>
#include "string_utils.h"

#include "tests-libc.h"

static void test_libc_strscpy(void)
{
    char buffer[8];

    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "Hello", sizeof(buffer)), 5);
    TEST_ASSERT_EQUAL_INT(strcmp(buffer, "Hello"), 0);
    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "012345678", sizeof(buffer)), -E2BIG);
    TEST_ASSERT_EQUAL_INT(strcmp(buffer, "0123456"), 0);
    memset(buffer, 0, sizeof(buffer));
    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "01234567", sizeof(buffer)), -E2BIG);
    TEST_ASSERT_EQUAL_INT(strcmp(buffer, "0123456"), 0);
    memset(buffer, 0, sizeof(buffer));
    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "0123456", sizeof(buffer)), 7);
    TEST_ASSERT_EQUAL_INT(strcmp(buffer, "0123456"), 0);
    TEST_ASSERT_EQUAL_INT(strscpy(buffer, "empty", 0), -E2BIG);
}

static void test_libc_swprintf(void)
{
    string_writer_t sw;
    char buffer[32];
    int res;

    string_writer_init(&sw, buffer, sizeof(buffer));

    /* check that string can be written completely */
    res = swprintf(&sw, "Hello World!");
    TEST_ASSERT_EQUAL_INT(res, 12);
    TEST_ASSERT_EQUAL_INT(string_writer_len(&sw), 12);
    TEST_ASSERT_EQUAL_INT(strcmp("Hello World!", string_writer_str(&sw)), 0);

    /* check that we can add to the string */
    /* Writing 10 characters, returns 10 bytes written */
    res = swprintf(&sw, "0123456789");
    TEST_ASSERT_EQUAL_INT(res, 10);
    TEST_ASSERT_EQUAL_INT(strcmp("Hello World!0123456789", string_writer_str(&sw)), 0);

    /* The string does not fit completely into the buffer, so it gets truncated */
    res = swprintf(&sw, "01234567891");
    TEST_ASSERT_EQUAL_INT(res, -E2BIG);
    TEST_ASSERT_EQUAL_INT(strcmp("Hello World!0123456789012345678", string_writer_str(&sw)), 0);

    /* You can't write to a full buffer */
    res = swprintf(&sw, "###");
    TEST_ASSERT_EQUAL_INT(res, -E2BIG);

    /* check if string was truncated as expected */
    TEST_ASSERT_EQUAL_INT(string_writer_len(&sw), 32);
    TEST_ASSERT_EQUAL_INT(strcmp("Hello World!0123456789012345678",
                                 string_writer_str(&sw)), 0);
}

static void test_libc_memchk(void)
{
    char buffer[32];
    memset(buffer, 0xff, sizeof(buffer));
    TEST_ASSERT_NULL(memchk(buffer, 0xff, sizeof(buffer)));

    buffer[5] = 5;
    TEST_ASSERT(memchk(buffer, 0xff, sizeof(buffer)) == &buffer[5]);
}

static void test_libc_reverse_buf3(void)
{
    const char expected[3] = { 3, 2, 1 };
    char buffer[3] = { 1, 2, 3 };
    reverse_buf(buffer, sizeof(buffer));
    TEST_ASSERT(!memcmp(buffer, expected, sizeof(buffer)));
}

static void test_libc_reverse_buf4(void)
{
    const char expected[4] = { 4, 3, 2, 1 };
    char buffer[4] = { 1, 2, 3, 4 };
    reverse_buf(buffer, sizeof(buffer));
    TEST_ASSERT(!memcmp(buffer, expected, sizeof(buffer)));
}

/**
 * @name    Unit test ensuring `<endian.h>` is provided and correct across
 *          all platforms
 * @{
 */
union u16 {
    uint8_t as_bytes[2];
    uint16_t as_number;
};

union u32 {
    uint8_t as_bytes[4];
    uint32_t as_number;
};

union u64 {
    uint8_t as_bytes[8];
    uint64_t as_number;
};

static void test_libc_endian(void)
{
    /* format the numbers 0x0102, 0x01020304 and 0x0102030405060708
     * in little/big endian format by hand: */
    const uint16_t u16_host = 0x0102;
    const union u16 u16_be = { .as_bytes = { 0x01, 0x02 } };
    const union u16 u16_le = { .as_bytes = { 0x02, 0x01 } };
    const uint32_t u32_host = 0x01020304;
    const union u32 u32_be = { .as_bytes = { 0x01, 0x02, 0x03, 0x04 } };
    const union u32 u32_le = { .as_bytes = { 0x04, 0x03, 0x02, 0x01 } };
    const uint64_t u64_host = 0x0102030405060708;
    const union u64 u64_be = { .as_bytes = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 } };
    const union u64 u64_le = { .as_bytes = { 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 } };

    /* check from host to big/little endian */
    TEST_ASSERT_EQUAL_INT(htobe16(u16_host), u16_be.as_number);
    TEST_ASSERT_EQUAL_INT(htole16(u16_host), u16_le.as_number);
    TEST_ASSERT_EQUAL_INT(htobe32(u32_host), u32_be.as_number);
    TEST_ASSERT_EQUAL_INT(htole32(u32_host), u32_le.as_number);
    TEST_ASSERT_EQUAL_INT(htobe64(u64_host), u64_be.as_number);
    TEST_ASSERT_EQUAL_INT(htole64(u64_host), u64_le.as_number);

    /* check little/big endian to host */
    TEST_ASSERT_EQUAL_INT(be16toh(u16_be.as_number), u16_host);
    TEST_ASSERT_EQUAL_INT(le16toh(u16_le.as_number), u16_host);
    TEST_ASSERT_EQUAL_INT(be32toh(u32_be.as_number), u32_host);
    TEST_ASSERT_EQUAL_INT(le32toh(u32_le.as_number), u32_host);
    TEST_ASSERT_EQUAL_INT(be64toh(u64_be.as_number), u64_host);
    TEST_ASSERT_EQUAL_INT(le64toh(u64_le.as_number), u64_host);

    /* check that magic numbers in the constants are what is commonly expected */
    TEST_ASSERT_EQUAL_INT(LITTLE_ENDIAN, 1234);
    TEST_ASSERT_EQUAL_INT(BIG_ENDIAN, 4321);
    TEST_ASSERT_EQUAL_INT(PDP_ENDIAN, 3412);
}
/** @} */

Test *tests_libc_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_libc_strscpy),
        new_TestFixture(test_libc_swprintf),
        new_TestFixture(test_libc_memchk),
        new_TestFixture(test_libc_reverse_buf3),
        new_TestFixture(test_libc_reverse_buf4),
        new_TestFixture(test_libc_endian),
    };

    EMB_UNIT_TESTCALLER(libc_tests, NULL, NULL, fixtures);

    return (Test *)&libc_tests;
}

void tests_libc(void)
{
    TESTS_RUN(tests_libc_tests());
}
