/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       riotconf test
 *
 * @author      Fabian Hüßler <fabian.huessler@,l-pa.com>
 *
 * @}
 */

#include <string.h>

#include "container.h"
#include "embUnit.h"
#include "embUnit/AssertImpl.h"
#include "mtd.h"
#include "mtd_emulated.h"
#include "riotconf/hdr.h"
#include "riotconf/slot.h"
#include "riotconf/storage.h"

#ifndef TEST_SECTOR_COUNT
#define TEST_SECTOR_COUNT           64
#endif
#ifndef TEST_PAGES_PER_SECTOR
#define TEST_PAGES_PER_SECTOR       4
#endif
#ifndef TEST_PAGE_SIZE
#define TEST_PAGE_SIZE              128
#endif
#ifndef TEST_SECTOR_SIZE
#define TEST_SECTOR_SIZE            (TEST_PAGES_PER_SECTOR * TEST_PAGE_SIZE)
#endif
#define TEST_MTD_SIZE               (TEST_SECTOR_COUNT * TEST_PAGES_PER_SECTOR * TEST_PAGE_SIZE)

static mtd_emulated_t _mtd[RIOTCONF_SLOT_NUMOF];
static uint8_t _mtd_mem[RIOTCONF_SLOT_NUMOF][TEST_MTD_SIZE];
static uint8_t _mtd_buf[RIOTCONF_SLOT_NUMOF][TEST_SECTOR_SIZE];

static void _setup(void)
{
    for (int i = 0; i < RIOTCONF_SLOT_NUMOF; i++) {
        memset(_mtd_mem[i], 0, sizeof(_mtd_mem[i]));
        _mtd[i] = (mtd_emulated_t) {
            .base =  {
                .driver = &_mtd_emulated_driver,
                .sector_count = TEST_SECTOR_COUNT,
                .pages_per_sector = TEST_PAGES_PER_SECTOR,
                .page_size = TEST_PAGE_SIZE,
                .write_size = 1,
            },
            .size = ARRAY_SIZE(_mtd_mem[i]),
            .memory = _mtd_mem[i],
            .init_done = false,
        };
        mtd_init(&_mtd[i].base);
        riotconf_storage_init(i, &_mtd[i].base, 0, &_mtd_buf[i], ARRAY_SIZE(_mtd_buf[i]));
    }
}

static void _test_riotconf_slot_invalid(void)
{
    int ret;
    for (riotconf_slot_t i = 0; i < RIOTCONF_SLOT_NUMOF; i++) {
        riotconf_hdr_t hdr;
        ret = riotconf_slot_validate(i, &hdr);
        TEST_ASSERT_MESSAGE(ret, "Empty slot should not be valid");
    }
}

static void _test_riotconf_slot_invalid_checksum(void)
{
    char data[] = "Hello World!";
    riotconf_hdr_t hdr = {
        .magic = RIOTCONF_MAGIC,
        .checksum = 0x11223344,
        .size = sizeof(data),
    };
    riotconf_hdr_hton(&hdr);
    mtd_write(&_mtd[0].base, &hdr, 0, sizeof(hdr));
    mtd_write(&_mtd[0].base, data, sizeof(hdr), sizeof(data));
    memset(&hdr, 0, sizeof(hdr));
    int ret = riotconf_slot_validate(0, &hdr);
    TEST_ASSERT_MESSAGE(ret, "Checksum should be invalid");
}

static void _test_riotconf_slot_valid_checksum(void)
{
    char data[] = "Hello World!";
    riotconf_hdr_t hdr = {
        .magic = RIOTCONF_MAGIC,
        .checksum = 0x2393f289,
        .size = sizeof(data),
    };
    riotconf_hdr_hton(&hdr);
    mtd_write(&_mtd[0].base, &hdr, 0, sizeof(hdr));
    mtd_write(&_mtd[0].base, data, sizeof(hdr), sizeof(data));
    memset(&hdr, 0, sizeof(hdr));
    int ret = riotconf_slot_validate(0, &hdr);
    TEST_ASSERT_MESSAGE(!ret, "Checksum should be valid");
    TEST_ASSERT_EQUAL_INT(hdr.checksum, 0x2393f289);
    TEST_ASSERT(hdr.state & RIOTCONF_HDR_STATE_CHECKSUM_VALIDATED);
}

static void _test_riotconf_slot_read(void)
{
    char data[] = "Hello World!";
    riotconf_hdr_t hdr = {
        .magic = RIOTCONF_MAGIC,
        .checksum = 0x2393f289,
        .size = sizeof(data),
    };
    riotconf_hdr_hton(&hdr);
    mtd_write(&_mtd[0].base, &hdr, 0, sizeof(hdr));
    mtd_write(&_mtd[0].base, data, sizeof(hdr), sizeof(data));
    memset(&hdr, 0, sizeof(hdr));

    void *sec;
    size_t sec_size;
    int ret = riotconf_slot_start_read(0, &sec, &sec_size);
    TEST_ASSERT(sec == &_mtd_buf[0]);
    TEST_ASSERT_EQUAL_INT(sec_size, TEST_SECTOR_SIZE);
    riotconf_hdr_ntoh((riotconf_hdr_t *)sec);
    TEST_ASSERT(((riotconf_hdr_t *)sec)->magic == RIOTCONF_MAGIC);
    TEST_ASSERT(((riotconf_hdr_t *)sec)->checksum == 0x2393f289);
    TEST_ASSERT(((riotconf_hdr_t *)sec)->size == sizeof(data));

    ret = riotconf_slot_read(0, sec, 0, sizeof(data));
    TEST_ASSERT(!ret);
    TEST_ASSERT(!memcmp(data, sec, sizeof(data)));
    ret = riotconf_slot_read(0, sec, 1, sizeof(data) - 1);
    TEST_ASSERT(!ret);
    TEST_ASSERT(!memcmp(data + 1, sec, sizeof(data) - 1));
    riotconf_slot_finish_read(0);
}

static void _test_riotconf_slot_write(void)
{
    char data[] = "Hello World!";
    int ret = riotconf_slot_start_write(0, NULL);
    TEST_ASSERT(!ret);
    ret = riotconf_slot_write(0, data, 0, sizeof(data));
    TEST_ASSERT(!ret);
    riotconf_slot_finish_write(0, 1, 123, sizeof(data));

    void *sec;
    size_t sec_size;
    riotconf_slot_start_read(0, &sec, &sec_size);
    riotconf_hdr_ntoh(((riotconf_hdr_t *)sec));
    TEST_ASSERT(((riotconf_hdr_t *)sec)->magic == RIOTCONF_MAGIC);
    TEST_ASSERT(((riotconf_hdr_t *)sec)->size == sizeof(data));
    TEST_ASSERT(((riotconf_hdr_t *)sec)->sequence == 1);
    TEST_ASSERT(((riotconf_hdr_t *)sec)->version == 123);
    TEST_ASSERT(((riotconf_hdr_t *)sec)->state == 0);
}

static int _user_slot_compatible(const riotconf_hdr_t *hdr)
{
    return hdr->version == 123;
}

static void _test_riotconf_slot_highest(void)
{
    char data0[] = "0 Hello World! 0";
    riotconf_hdr_t hdr0 = {
        .magic = RIOTCONF_MAGIC,
        .checksum = 0x7462c952,
        .size = sizeof(data0),
        .version = 123
    };
    riotconf_hdr_hton(&hdr0);
    mtd_write(&_mtd[0].base, &hdr0, 0, sizeof(hdr0));
    mtd_write(&_mtd[0].base, data0, sizeof(hdr0), sizeof(data0));
    memset(&hdr0, 0, sizeof(hdr0));

    char data1[] = "1 Hello World! 1";
    riotconf_hdr_t hdr1 = {
        .magic = RIOTCONF_MAGIC,
        .checksum = 0x7565e7b5,
        .size = sizeof(data1),
        .sequence = 1,
        .version = 124
    };
    riotconf_hdr_hton(&hdr1);
    mtd_write(&_mtd[1].base, &hdr1, 0, sizeof(hdr1));
    mtd_write(&_mtd[1].base, data1, sizeof(hdr1), sizeof(data1));
    memset(&hdr1, 0, sizeof(hdr1));

    int ret = riotconf_slot_highest_seq(NULL);
    TEST_ASSERT_EQUAL_INT(1, ret);
    ret = riotconf_slot_highest_seq(_user_slot_compatible);
    TEST_ASSERT_EQUAL_INT(0, ret);
    riotconf_slot_invalidate(0);
    ret = riotconf_slot_highest_seq(_user_slot_compatible);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

Test *tests_riotconf(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(_test_riotconf_slot_invalid),
        new_TestFixture(_test_riotconf_slot_invalid_checksum),
        new_TestFixture(_test_riotconf_slot_valid_checksum),
        new_TestFixture(_test_riotconf_slot_read),
        new_TestFixture(_test_riotconf_slot_write),
        new_TestFixture(_test_riotconf_slot_highest),
    };

    EMB_UNIT_TESTCALLER(riotconf_tests, _setup, NULL, fixtures);
    return (Test *)&riotconf_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_riotconf());
    TESTS_END();
    return 0;
}
