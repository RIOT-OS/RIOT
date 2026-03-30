/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Application for testing MTD EEPROM implementations
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <string.h>
#include <errno.h>

#include "embUnit.h"

#include "mtd.h"
#include "mtd_eeprom.h"

#define TEST_ADDRESS 22

static mtd_eeprom_t _dev = MTD_EEPROM_INIT_VAL;
static mtd_dev_t *dev = &_dev.base;

static void setup(void)
{
    int ret = mtd_init(dev);
    TEST_ASSERT_EQUAL_INT(0, ret);
    mtd_erase(dev, TEST_ADDRESS, dev->pages_per_sector * dev->page_size);
}

static void teardown(void)
{
    /* Erase whole EEPROM */
    mtd_erase(dev, 0, EEPROM_SIZE);
}

static void test_mtd_init(void)
{
    int ret = mtd_init(dev);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_erase(void)
{
    int ret;

    /* Unaligned erase */
    ret = mtd_erase(dev, 1, EEPROM_SIZE -1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase out of memory area */
    ret = mtd_erase(dev, 1, EEPROM_SIZE);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);
}

static void test_mtd_write_erase(void)
{
    const char buf[] = "ABCDEFGHIJKLMNO";

    uint8_t buf_empty[sizeof(buf)];
    memset(buf_empty, EEPROM_CLEAR_BYTE, sizeof(buf_empty));

    uint8_t buf_read[sizeof(buf)];
    memset(buf_read, 0, sizeof(buf_read));

    /* write */
    int ret = mtd_write(dev, buf, TEST_ADDRESS, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* erase */
    ret = mtd_erase(dev, TEST_ADDRESS, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* read */
    ret = mtd_read(dev, buf_read, TEST_ADDRESS, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read, sizeof(buf_empty)));
}

static void test_mtd_write_read(void)
{
    const char buf[] = "ABCDEFGHIJKLMNO";

    uint8_t buf_read[sizeof(buf)];
    memset(buf_read, 0, sizeof(buf_read));

    /* Basic write / read */
    int ret = mtd_write(dev, buf, TEST_ADDRESS, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* read */
    ret = mtd_read(dev, buf_read, TEST_ADDRESS, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read, sizeof(buf)));

    /* Out of bounds read */
    ret = mtd_read(dev, buf_read, TEST_ADDRESS + EEPROM_SIZE, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Out of bounds write */
    ret = mtd_write(dev, buf, TEST_ADDRESS + EEPROM_SIZE, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Unaligned write / read */
    ret = mtd_write(dev, &buf[1],
                    TEST_ADDRESS + !(TEST_ADDRESS % dev->page_size), sizeof(buf) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = mtd_read(dev, &buf_read[1],
                   TEST_ADDRESS + !(TEST_ADDRESS % dev->page_size), sizeof(buf_read) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

Test *tests_mtd_eeprom_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),
        new_TestFixture(test_mtd_write_erase),
        new_TestFixture(test_mtd_write_read),
    };

    EMB_UNIT_TESTCALLER(mtd_eeprom_tests, setup, teardown, fixtures);

    return (Test *)&mtd_eeprom_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_mtd_eeprom_tests());
    TESTS_END();
    return 0;
}
