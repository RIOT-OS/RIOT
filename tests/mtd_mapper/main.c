/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
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
 * @brief       mtd_mapper module test
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "embUnit.h"

#include "mtd.h"
#include "mtd_mapper.h"

/* Test mock object implementing a simple RAM-based mtd */
#ifndef SECTOR_COUNT
#define SECTOR_COUNT 16
#endif
#ifndef PAGE_PER_SECTOR
#define PAGE_PER_SECTOR 4
#endif
#ifndef PAGE_SIZE
#define PAGE_SIZE 64
#endif

#define MEMORY_SIZE         PAGE_SIZE * PAGE_PER_SECTOR * SECTOR_COUNT

#define REGION_FLASH_SIZE    MEMORY_SIZE / 2

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static uint8_t _dummy_memory[MEMORY_SIZE];

static uint8_t _buffer[PAGE_SIZE];

static int _init(mtd_dev_t *dev)
{
    (void)dev;

    return 0;
}

static int _read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    (void)dev;

    if (addr + size > sizeof(_dummy_memory)) {
        return -EOVERFLOW;
    }
    memcpy(buff, _dummy_memory + addr, size);

    return 0;
}

static int _read_page(mtd_dev_t *dev, void *buff, uint32_t page, uint32_t offset, uint32_t size)
{
    uint32_t addr = page * dev->page_size + offset;

    if (page >= dev->sector_count * dev->pages_per_sector) {
        return -EOVERFLOW;
    }

    if (offset > dev->page_size) {
        return -EOVERFLOW;
    }

    size = MIN(dev->page_size - offset, size);

    memcpy(buff, _dummy_memory + addr, size);

    return size;
}

static int _write(mtd_dev_t *dev, const void *buff, uint32_t addr,
                  uint32_t size)
{
    (void)dev;

    if (addr + size > sizeof(_dummy_memory)) {
        return -EOVERFLOW;
    }
    if (size > PAGE_SIZE) {
        return -EOVERFLOW;
    }
    memcpy(_dummy_memory + addr, buff, size);

    return 0;
}

static int _write_page(mtd_dev_t *dev, const void *buff, uint32_t page, uint32_t offset, uint32_t size)
{
    uint32_t addr = page * dev->page_size + offset;

    if (page >= dev->sector_count * dev->pages_per_sector) {
        return -EOVERFLOW;
    }

    if (offset > dev->page_size) {
        return -EOVERFLOW;
    }

    size = MIN(dev->page_size - offset, size);

    memcpy(_dummy_memory + addr, buff, size);

    return size;
}

static int _erase(mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    (void)dev;

    if (size % (PAGE_PER_SECTOR * PAGE_SIZE) != 0) {
        return -EOVERFLOW;
    }
    if (addr % (PAGE_PER_SECTOR * PAGE_SIZE) != 0) {
        return -EOVERFLOW;
    }
    if (addr + size > sizeof(_dummy_memory)) {
        return -EOVERFLOW;
    }
    memset(_dummy_memory + addr, 0xff, size);

    return 0;
}

static int _erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t count)
{
    uint32_t addr = sector * dev->page_size * dev->pages_per_sector;

    if (sector + count > dev->sector_count) {
        return -EOVERFLOW;
    }

    memset(_dummy_memory + addr, 0xff,
           count * dev->page_size * dev->pages_per_sector);

    return 0;
}

static int _power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;
    (void)power;
    return 0;
}

static const mtd_desc_t driver = {
    .init = _init,
    .read = _read,
    .write = _write,
    .erase = _erase,
    .power = _power,
    .read_page    = _read_page,
    .write_page   = _write_page,
    .erase_sector = _erase_sector,
};

static mtd_dev_t dev = {
    .driver = &driver,
    .sector_count = SECTOR_COUNT,
    .pages_per_sector = PAGE_PER_SECTOR,
    .page_size = PAGE_SIZE,
};

static mtd_mapper_parent_t _parent = MTD_PARENT_INIT(&dev);

static mtd_mapper_region_t _region_a = {
    .mtd = {
        .driver = &mtd_mapper_driver,
        .sector_count = SECTOR_COUNT / 2,
        .pages_per_sector = PAGE_PER_SECTOR,
        .page_size = PAGE_SIZE,
    },
    .parent = &_parent,
    .sector = 0,
};

static mtd_mapper_region_t _region_b = {
    .mtd = {
        .driver = &mtd_mapper_driver,
        .sector_count = SECTOR_COUNT / 2,
        .pages_per_sector = PAGE_PER_SECTOR,
        .page_size = PAGE_SIZE,
    },
    .parent = &_parent,
    .sector = SECTOR_COUNT / 2,
};

static mtd_dev_t *_dev_a = &_region_a.mtd;
static mtd_dev_t *_dev_b = &_region_b.mtd;

static void _test_mem(uint8_t *buffer, size_t len, uint8_t expected)
{
    for (size_t i = 0; i < len; i++) {
        TEST_ASSERT_EQUAL_INT(expected, buffer[i]);
    }
}

static void test_mtd_init(void)
{
    int ret = mtd_init(_dev_a);

    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = mtd_init(_dev_b);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_erase(void)
{
    /* Erase first region */
    int ret = mtd_erase(_dev_a, 0, REGION_FLASH_SIZE);

    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase second region */
    ret = mtd_erase(_dev_b, 0, REGION_FLASH_SIZE);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_erase(_dev_a, REGION_FLASH_SIZE, 1);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    ret = mtd_erase(_dev_b, REGION_FLASH_SIZE, 1);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);
}

static void test_mtd_read(void)
{
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += PAGE_SIZE) {
        mtd_read(_dev_a, _buffer, i, PAGE_SIZE);
        _test_mem(_buffer, PAGE_SIZE, 0xff);
    }

    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += PAGE_SIZE) {
        mtd_read(_dev_b, _buffer, i, PAGE_SIZE);
        _test_mem(_buffer, PAGE_SIZE, 0xff);
    }
}

static void test_mtd_write(void)
{
    static const uint8_t test_val_a = 0xAA;

    memset(_buffer, test_val_a, PAGE_SIZE);

    /* Write first region */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += PAGE_SIZE) {
        mtd_write(_dev_a, _buffer, i, PAGE_SIZE);
    }

    /* Check second region, should still be 0xFF */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += PAGE_SIZE) {
        mtd_read(_dev_b, _buffer, i, PAGE_SIZE);
        _test_mem(_buffer, PAGE_SIZE, 0xFF);
    }

    static const uint8_t test_val_b = 0xBB;
    memset(_buffer, test_val_b, PAGE_SIZE);

    /* Write second region */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += PAGE_SIZE) {
        mtd_write(_dev_b, _buffer, i, PAGE_SIZE);
    }

    /* Check second region after write, should now be 0xBB */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += PAGE_SIZE) {
        mtd_read(_dev_b, _buffer, i, PAGE_SIZE);
        _test_mem(_buffer, PAGE_SIZE, 0xBB);
    }

    /* Check first region, should still be 0xAA */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += PAGE_SIZE) {
        mtd_read(_dev_a, _buffer, i, PAGE_SIZE);
        _test_mem(_buffer, PAGE_SIZE, 0xAA);
    }
}

Test *tests_mtd_mapper_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),
        new_TestFixture(test_mtd_read),
        new_TestFixture(test_mtd_write),
    };

    EMB_UNIT_TESTCALLER(mtd_flashpage_tests, NULL, NULL, fixtures);

    return (Test *)&mtd_flashpage_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_mtd_mapper_tests());
    TESTS_END();
    return 0;
}
