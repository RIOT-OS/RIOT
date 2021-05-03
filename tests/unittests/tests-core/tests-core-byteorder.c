/*
 * Copyright (C) 2014 René Kijewski
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>

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

static void test_byteorder_bebuftohs(void)
{
    static const uint8_t bebuf[2] = { 0xAA, 0xBB };
    static const uint16_t host = 0xAABB;

    TEST_ASSERT_EQUAL_INT(host, byteorder_bebuftohs(bebuf));
}

static void test_byteorder_bebuftohl(void)
{
    static const uint8_t bebuf[4] = { 0xAA, 0xBB, 0xCC, 0xDD};
    static const uint32_t host = 0xAABBCCDD;

    TEST_ASSERT_EQUAL_INT(host, byteorder_bebuftohl(bebuf));
}

static void test_byteorder_htobebufs(void)
{
    static const uint8_t bebuf[2] = { 0xAA, 0xBB };
    static const uint16_t host = 0xAABB;

    uint8_t tmp[2] = {0};

    byteorder_htobebufs(tmp, host);

    TEST_ASSERT_EQUAL_INT(0, memcmp(bebuf, tmp, sizeof(tmp)));
}

static void test_byteorder_htobebufl(void)
{
    static const uint8_t bebuf[4] = { 0xAA, 0xBB, 0xCC, 0xDD};
    static const uint32_t host = 0xAABBCCDD;

    uint8_t tmp[4] = {0};

    byteorder_htobebufl(tmp, host);

    TEST_ASSERT_EQUAL_INT(0, memcmp(bebuf, tmp, sizeof(tmp)));
}

static void test_byteorder_htols(void)
{
    static const uint16_t host = 0x1234;
    le_uint16_t le = { .u8 = { 0x34, 0x12 } };
    TEST_ASSERT_EQUAL_INT(le.u16, byteorder_htols(host).u16);
    TEST_ASSERT_EQUAL_INT(host, byteorder_ltohs(le));
}

static void test_byteorder_htoll(void)
{
    static const uint32_t host = 0x12345678ul;
    le_uint32_t le = { .u8 = { 0x78, 0x56, 0x34, 0x12, } };
    TEST_ASSERT_EQUAL_INT(le.u32, byteorder_htoll(host).u32);
    TEST_ASSERT_EQUAL_INT(host, byteorder_ltohl(le));
}

static void test_byteorder_htolll(void)
{
    static const uint64_t host = 0x123456789abcdef0ull;
    le_uint64_t le = { .u8 = { 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 } };
    TEST_ASSERT_EQUAL_INT(le.u64, byteorder_htolll(host).u64);
    TEST_ASSERT_EQUAL_INT(host, byteorder_ltohll(le));
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
        new_TestFixture(test_byteorder_bebuftohs),
        new_TestFixture(test_byteorder_htobebufs),
        new_TestFixture(test_byteorder_bebuftohl),
        new_TestFixture(test_byteorder_htobebufl),
        new_TestFixture(test_byteorder_htols),
        new_TestFixture(test_byteorder_htoll),
        new_TestFixture(test_byteorder_htolll),
    };

    EMB_UNIT_TESTCALLER(core_byteorder_tests, NULL, NULL, fixtures);
    return (Test *)&core_byteorder_tests;
}
