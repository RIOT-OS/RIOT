/*
 * Copyright (C) 2016 OTA keys S.A.
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
#include "mtd_native.h"

MTD_NATIVE_DESC(_dev, "MEMORY.bin");

static void setup_teardown(void)
{
    mtd_dev_t *dev = (mtd_dev_t*) &_dev;

    mtd_erase(dev, 0, dev->pages_per_sector * dev->page_size);
}

static void test_mtd_init(void)
{
    mtd_dev_t *dev = (mtd_dev_t*) &_dev;

    int ret = mtd_init(dev);
    TEST_ASSERT_EQUAL_INT(ret, 1);
}

static void test_mtd_erase(void)
{
    mtd_dev_t *dev = (mtd_dev_t*) &_dev;

    int ret = mtd_erase(dev, 0, dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(ret, dev->pages_per_sector * dev->page_size);

    ret = mtd_erase(dev, 0, dev->pages_per_sector);
    TEST_ASSERT_EQUAL_INT(ret, -EOVERFLOW);
}

static void test_mtd_write_read(void)
{
    mtd_dev_t *dev = (mtd_dev_t*) &_dev;
    const char buf[] = "ABCDEFGH";
    uint8_t buf_empty[] = {0xff, 0xff, 0xff};
    char buf_read[sizeof(buf) + sizeof(buf_empty)];

    int ret = mtd_write(dev, buf, 0, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(ret, sizeof(buf));

    ret = mtd_read(dev, buf_read, 0, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(ret, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(memcmp(buf, buf_read, sizeof(buf)), 0);
    TEST_ASSERT_EQUAL_INT(memcmp(buf_empty, buf_read + sizeof(buf), sizeof(buf_empty)), 0);
}


Test *tests_mtd_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),
        new_TestFixture(test_mtd_write_read),
    };

    EMB_UNIT_TESTCALLER(mtd_tests, setup_teardown, setup_teardown, fixtures);

    return (Test *)&mtd_tests;
}

void tests_mtd(void)
{
    TESTS_RUN(tests_mtd_tests());
}
/** @} */
