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
#include "mtd_flashpage.h"

/* For MSP430 cpu's the last page holds the interrupt vector, although the api
   should not limit erasing that page, we don't want to break when testing */
#if defined(CPU_CC430) || defined(CPU_MSP430FXYZ)
#define LAST_AVAILABLE_PAGE    (FLASHPAGE_NUMOF - 2)
#else
#define LAST_AVAILABLE_PAGE    (FLASHPAGE_NUMOF - 1)
#endif

#if (__SIZEOF_POINTER__ == 2)
#define TEST_ADDRESS1       (uint16_t)flashpage_addr(LAST_AVAILABLE_PAGE)
#define TEST_ADDRESS2       (uint16_t)flashpage_addr(LAST_AVAILABLE_PAGE - 1)
#else
#define TEST_ADDRESS1       (uint32_t)flashpage_addr(LAST_AVAILABLE_PAGE)
#define TEST_ADDRESS2       (uint32_t)flashpage_addr(LAST_AVAILABLE_PAGE - 1)
#endif
#define TEST_ADDRESS0       (FLASHPAGE_NUMOF - 1)

static mtd_dev_t _dev = MTD_FLASHPAGE_INIT_VAL(8);
static mtd_dev_t *dev = &_dev;

static void setup(void)
{
    int ret = mtd_init(dev);
    TEST_ASSERT_EQUAL_INT(0, ret);
    mtd_erase(dev, TEST_ADDRESS1, dev->pages_per_sector * dev->page_size);
    mtd_erase(dev, TEST_ADDRESS2, dev->pages_per_sector * dev->page_size);
}

static void teardown(void)
{
    mtd_erase(dev, TEST_ADDRESS1, dev->pages_per_sector * dev->page_size);
    mtd_erase(dev, TEST_ADDRESS2, dev->pages_per_sector * dev->page_size);
}

static void test_mtd_init(void)
{
    int ret = mtd_init(dev);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_erase(void)
{
    /* Erase last sector */
    int ret = mtd_erase(dev, TEST_ADDRESS1, FLASHPAGE_SIZE);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase with wrong size (less than sector size) */
    ret = mtd_erase(dev, TEST_ADDRESS1, dev->page_size);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Unaligned erase */
    ret = mtd_erase(dev, TEST_ADDRESS1 + dev->page_size, dev->page_size);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Erase 2 last available pages */
    ret = mtd_erase(dev, TEST_ADDRESS2,
                    FLASHPAGE_SIZE * 2);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase out of memory area */
    ret = mtd_erase(dev, TEST_ADDRESS0,
                    FLASHPAGE_SIZE * 2);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);
}

static void test_mtd_write_erase(void)
{
    const char buf[] = "ABCDEFGHIJKLMNO";

    uint8_t buf_empty[3];
    memset(buf_empty, FLASHPAGE_ERASE_STATE, sizeof(buf_empty));
    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    int ret = mtd_write(dev, buf, TEST_ADDRESS1, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = mtd_write(dev, buf, TEST_ADDRESS2, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase both sectors */
    ret = mtd_erase(dev, TEST_ADDRESS2, 2 * dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    uint8_t expected[sizeof(buf_read)];
    memset(expected, FLASHPAGE_ERASE_STATE, sizeof(expected));
    ret = mtd_read(dev, buf_read, TEST_ADDRESS1, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(expected, buf_read, sizeof(buf_read)));
    ret = mtd_read(dev, buf_read, TEST_ADDRESS2, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(expected, buf_read, sizeof(buf_read)));
}

static void test_mtd_write_read(void)
{
    const char buf[] __attribute__ ((aligned (FLASHPAGE_WRITE_BLOCK_ALIGNMENT)))
            = "ABCDEFGHIJKLMNO";

    uint8_t buf_empty[3];
    memset(buf_empty, FLASHPAGE_ERASE_STATE, sizeof(buf_empty));
    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    /* Basic write / read */
    int ret = mtd_write(dev, buf, TEST_ADDRESS1, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_read(dev, buf_read, TEST_ADDRESS1, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read + sizeof(buf), sizeof(buf_empty)));

    ret = mtd_erase(dev, TEST_ADDRESS1, dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Unaligned write / read */
    ret = mtd_write(dev, buf, TEST_ADDRESS1 + sizeof(buf_empty), sizeof(buf));
    TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

    /* Only Cortex-M0 doesn't allow unaligned reads */
#if defined(CPU_CORE_CORTEX_M0)
    ret = mtd_read(dev, buf_read, TEST_ADDRESS1 + sizeof(buf_empty), sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
#endif
}

Test *tests_mtd_flashpage_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),
        new_TestFixture(test_mtd_write_erase),
        new_TestFixture(test_mtd_write_read),
    };

    EMB_UNIT_TESTCALLER(mtd_flashpage_tests, setup, teardown, fixtures);

    return (Test *)&mtd_flashpage_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_mtd_flashpage_tests());
    TESTS_END();
    return 0;
}
/** @} */
