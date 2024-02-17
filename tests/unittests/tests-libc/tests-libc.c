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

static void test_libc_memchk(void)
{
    char buffer[32];
    memset(buffer, 0xff, sizeof(buffer));
    TEST_ASSERT_NULL(memchk(buffer, 0xff, sizeof(buffer)));

    buffer[5] = 5;
    TEST_ASSERT(memchk(buffer, 0xff, sizeof(buffer)) == &buffer[5]);
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
        new_TestFixture(test_libc_memchk),
        new_TestFixture(test_libc_endian),
    };

    EMB_UNIT_TESTCALLER(libc_tests, NULL, NULL, fixtures);

    return (Test *)&libc_tests;
}

void tests_libc(void)
{
    TESTS_RUN(tests_libc_tests());
}
