/*
 * Copyright (C) 2014 René Kijewski
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"

#include "byteorder.h"

#include "tests-core.h"

static void test_byteorder_little_to_big_16(void)
{
    le_uint16_t little = { .u8 = { 0x12, 0x34 } };
    be_uint16_t big    = { .u8 = { 0x34, 0x12 } };
    TEST_ASSERT_EQUAL_INT(big.u16, byteorder_ltobs(little).u16);
}

static void test_byteorder_big_to_little_16(void)
{
    le_uint16_t little = { .u8 = { 0x12, 0x34 } };
    be_uint16_t big    = { .u8 = { 0x34, 0x12 } };
    TEST_ASSERT_EQUAL_INT(little.u16, byteorder_btols(big).u16);
}

static void test_byteorder_little_to_big_32(void)
{
    le_uint32_t little = { .u8 = { 0x12, 0x34, 0x56, 0x78 } };
    be_uint32_t big    = { .u8 = { 0x78, 0x56, 0x34, 0x12 } };
    TEST_ASSERT_EQUAL_INT(big.u32, byteorder_ltobl(little).u32);
}

static void test_byteorder_big_to_little_32(void)
{
    le_uint32_t little = { .u8 = { 0x12, 0x34, 0x56, 0x78 } };
    be_uint32_t big    = { .u8 = { 0x78, 0x56, 0x34, 0x12 } };
    TEST_ASSERT_EQUAL_INT(little.u32, byteorder_btoll(big).u32);
}

static void test_byteorder_little_to_big_64(void)
{
    le_uint64_t little = { .u8 = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 } };
    be_uint64_t big    = { .u8 = { 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 } };
    TEST_ASSERT_EQUAL_INT(big.u64, byteorder_ltobll(little).u64);
}

static void test_byteorder_big_to_little_64(void)
{
    le_uint64_t little = { .u8 = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 } };
    be_uint64_t big    = { .u8 = { 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 } };
    TEST_ASSERT_EQUAL_INT(little.u64, byteorder_btolll(big).u64);
}

static void test_byteorder_host_to_network_16(void)
{
    static const uint16_t host = 0x1234;
    network_uint16_t network = { .u8 = { 0x12, 0x34 } };
    TEST_ASSERT_EQUAL_INT(network.u16, byteorder_htons(host).u16);
    TEST_ASSERT_EQUAL_INT(host, byteorder_ntohs(network));
}

static void test_byteorder_host_to_network_32(void)
{
    static const uint32_t host = 0x12345678ul;
    network_uint32_t network = { .u8 = { 0x12, 0x34, 0x56, 0x78 } };
    TEST_ASSERT_EQUAL_INT(network.u32, byteorder_htonl(host).u32);
    TEST_ASSERT_EQUAL_INT(host, byteorder_ntohl(network));
}

static void test_byteorder_host_to_network_64(void)
{
    static const uint64_t host = 0x123456789abcdef0ull;
    network_uint64_t network = { .u8 = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 } };
    TEST_ASSERT_EQUAL_INT(network.u64, byteorder_htonll(host).u64);
    TEST_ASSERT_EQUAL_INT(host, byteorder_ntohll(network));
}

Test *tests_core_byteorder_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_byteorder_little_to_big_16),
        new_TestFixture(test_byteorder_big_to_little_16),
        new_TestFixture(test_byteorder_little_to_big_32),
        new_TestFixture(test_byteorder_big_to_little_32),
        new_TestFixture(test_byteorder_little_to_big_64),
        new_TestFixture(test_byteorder_big_to_little_64),
        new_TestFixture(test_byteorder_host_to_network_16),
        new_TestFixture(test_byteorder_host_to_network_32),
        new_TestFixture(test_byteorder_host_to_network_64),
    };

    EMB_UNIT_TESTCALLER(core_byteorder_tests, NULL, NULL, fixtures);
    return (Test *)&core_byteorder_tests;
}
