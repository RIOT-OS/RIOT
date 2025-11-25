/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
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
#include <string.h>
#include <errno.h>

#include "embUnit.h"

#include "mtd.h"
#include "at25xxx.h"
#include "at25xxx/mtd.h"
#include "at25xxx_params.h"

#define TEST_ADDRESS       (uint16_t)((dev->sector_count - 1) * dev->page_size)

static at25xxx_t at25xxx;

static mtd_at25xxx_t _dev = {
    .base = {
        .driver = &mtd_at25xxx_driver
    },
    .at25xxx_eeprom = &at25xxx,
    .params = &at25xxx_params[0],
};

static mtd_dev_t *dev = (mtd_dev_t *)&_dev;

static void setup(void)
{
    int ret = mtd_init(dev);
    TEST_ASSERT_EQUAL_INT(0, ret);
    mtd_erase(dev, TEST_ADDRESS, dev->pages_per_sector * dev->page_size);
}

static void teardown(void)
{
    mtd_erase(dev, TEST_ADDRESS, dev->pages_per_sector * dev->page_size);
}

static void test_mtd_init(void)
{
    int ret = mtd_init(dev);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_erase(void)
{
    int ret = mtd_erase(dev, TEST_ADDRESS, dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_write_erase(void)
{
    uint8_t buf_empty[] = {0, 0, 0};
    const char buf[] = "MTD_AT25XXX_TEST_WRITE_ERASE";

    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    int ret = mtd_write(dev, buf, TEST_ADDRESS, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_erase(dev, TEST_ADDRESS, dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    uint8_t expected[sizeof(buf_read)];
    memset(expected, 0, sizeof(expected));
    ret = mtd_read(dev, buf_read, TEST_ADDRESS, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(expected, buf_read, sizeof(buf_read)));
}

static void test_mtd_write_read(void)
{
    uint8_t buf_empty[] = {0, 0, 0};
    const char buf[] = "MTD_AT25XXX_TEST_WRITE_ERASE";

    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    /* Basic write / read */
    int ret = mtd_write(dev, buf, TEST_ADDRESS, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_read(dev, buf_read, TEST_ADDRESS, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read + sizeof(buf), sizeof(buf_empty)));

    ret = mtd_erase(dev, TEST_ADDRESS, dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

Test *tests_mtd_at25xxx_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),
        new_TestFixture(test_mtd_write_erase),
        new_TestFixture(test_mtd_write_read),
    };

    EMB_UNIT_TESTCALLER(mtd_at25xxx_tests, setup, teardown, fixtures);

    return (Test *)&mtd_at25xxx_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_mtd_at25xxx_tests());
    TESTS_END();
    return 0;
}
/** @} */
