/*
* Copyright (C) 2020 SSV Software Systems GmbH
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
 * @brief       mtd_aes module test
 *
 * @author      Juergen Fitschen <jfi@ssv-embedded.de>
 *
 * @}
 */

#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "embUnit.h"

#include "mtd.h"
#include "mtd_aes.h"


static uint8_t TEST_1_KEY[16] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static uint8_t TEST_1_COUNTER[16] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0x00, 0x00, 0x00, 0x00
};

static uint8_t TEST_1_PLAIN[] = {
    0xce, 0x69, 0xf0, 0xc2, 0xef, 0xb8, 0x19, 0x02,
    0x05, 0x14, 0xa3, 0x67, 0x83, 0xb3, 0xb0, 0xa9,
    0xdb, 0x47, 0x60, 0x4e, 0x42, 0xa9, 0xa7, 0xc1,
    0xb2, 0x79, 0xa4, 0x0d, 0xd1, 0xea, 0x52, 0xc6,
    0x5f, 0xec, 0x6c, 0x13, 0x17, 0xc9, 0xf4, 0x89,
    0x51, 0x71, 0x4d, 0xcb, 0x87, 0x96, 0xae, 0x27,
    0xfa, 0xb7, 0xfc, 0x11, 0x82, 0x2f, 0xc4, 0x64,
    0x77, 0x56, 0xb4, 0xe1, 0xac, 0x40, 0xad, 0x45
};

static uint8_t TEST_1_CIPHER[] = {
    0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
    0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce,
    0x98, 0x06, 0xf6, 0x6b, 0x79, 0x70, 0xfd, 0xff,
    0x86, 0x17, 0x18, 0x7b, 0xb9, 0xff, 0xfd, 0xff,
    0x5a, 0xe4, 0xdf, 0x3e, 0xdb, 0xd5, 0xd3, 0x5e,
    0x5b, 0x4f, 0x09, 0x02, 0x0d, 0xb0, 0x3e, 0xab,
    0x1e, 0x03, 0x1d, 0xda, 0x2f, 0xbe, 0x03, 0xd1,
    0x79, 0x21, 0x70, 0xa0, 0xf3, 0x00, 0x9c, 0xee
};
#define TEST_1_DATA_LEN (64)

/* Test mock object implementing a simple RAM-based mtd */
#define SECTOR_COUNT    (TEST_1_DATA_LEN / AES_BLOCK_SIZE / 2)
#define PAGE_PER_SECTOR (TEST_1_DATA_LEN / AES_BLOCK_SIZE / 2)
#define PAGE_SIZE       (AES_BLOCK_SIZE)

#define MEMORY_SIZE     (PAGE_SIZE * PAGE_PER_SECTOR * SECTOR_COUNT)

static uint8_t _dummy_memory[MEMORY_SIZE];

static int _read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    (void)dev;

    if (addr + size > sizeof(_dummy_memory)) {
        return -EOVERFLOW;
    }
    memcpy(buff, _dummy_memory + addr, size);

    return 0;
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

static const mtd_desc_t _driver = {
    .read = _read,
    .write = _write
};

static mtd_dev_t _parent = {
    .driver = &_driver,
    .sector_count = SECTOR_COUNT,
    .pages_per_sector = PAGE_PER_SECTOR,
    .page_size = PAGE_SIZE,
};

static mtd_aes_t _aes;

static mtd_dev_t *_aesmtd = &_aes.mtd;

static void test_mtd_read(void)
{
    static uint8_t _buffer[PAGE_SIZE];

    memcpy(_dummy_memory, TEST_1_CIPHER, TEST_1_DATA_LEN);
    mtd_aes_init(&_aes, &_parent, TEST_1_KEY, TEST_1_COUNTER);
    for (uint32_t i = 0; i < MEMORY_SIZE; i += PAGE_SIZE) {
        mtd_read(_aesmtd, _buffer, i, PAGE_SIZE);
        int cmp = memcmp(_buffer, &TEST_1_PLAIN[i], PAGE_SIZE);
        TEST_ASSERT_MESSAGE(0 == cmp, "wrong plaintext");
    }
}

static void test_mtd_write(void)
{
    memset(_dummy_memory, 0x00, TEST_1_DATA_LEN);
    mtd_aes_init(&_aes, &_parent, TEST_1_KEY, TEST_1_COUNTER);
    for (uint32_t i = 0; i < MEMORY_SIZE; i += PAGE_SIZE) {
        mtd_write(_aesmtd, &TEST_1_PLAIN[i], i, PAGE_SIZE);
    }

    int cmp = memcmp(_dummy_memory, TEST_1_CIPHER, TEST_1_DATA_LEN);
    TEST_ASSERT_MESSAGE(0 == cmp, "wrong plaintext");
}

Test *tests_mtd_aes_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_read),
        new_TestFixture(test_mtd_write),
    };

    EMB_UNIT_TESTCALLER(mtd_flashpage_tests, NULL, NULL, fixtures);

    return (Test *)&mtd_flashpage_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_mtd_aes_tests());
    TESTS_END();
    return 0;
}
