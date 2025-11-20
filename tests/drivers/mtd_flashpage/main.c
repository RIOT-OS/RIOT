/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "embUnit.h"

#include "mtd.h"
#include "mtd_flashpage.h"

/* For MSP430 cpu's the last page holds the interrupt vector, although the api
   should not limit erasing that page, we don't want to break when testing */
#ifdef __MSP430__
#define LAST_AVAILABLE_PAGE    (FLASHPAGE_NUMOF - 2)
#else
#define LAST_AVAILABLE_PAGE    (FLASHPAGE_NUMOF - 1)
#endif

#if (__SIZEOF_POINTER__ == 2)
#define TEST_ADDRESS1       (uint16_t)((uintptr_t)flashpage_addr(LAST_AVAILABLE_PAGE) - (uintptr_t)CPU_FLASH_BASE)
#define TEST_ADDRESS2       (uint16_t)((uintptr_t)flashpage_addr(LAST_AVAILABLE_PAGE - 1) - (uintptr_t)CPU_FLASH_BASE)
#else
#define TEST_ADDRESS1       (uint32_t)((uintptr_t)flashpage_addr(LAST_AVAILABLE_PAGE) - (uintptr_t)CPU_FLASH_BASE)
#define TEST_ADDRESS2       (uint32_t)((uintptr_t)flashpage_addr(LAST_AVAILABLE_PAGE - 1) - (uintptr_t)CPU_FLASH_BASE)
#endif
/* Address of last flash page and not last available flashpage */
#define TEST_ADDRESS0       (uint32_t)((uintptr_t)flashpage_addr((FLASHPAGE_NUMOF - 1)) - (uintptr_t)CPU_FLASH_BASE)

#define PAGES_PER_SECTOR    8

static mtd_flashpage_t _dev = MTD_FLASHPAGE_INIT_VAL(PAGES_PER_SECTOR);
static mtd_dev_t *dev = &_dev.base;

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

    /* Out of bounds read */
    ret = mtd_read(dev, buf_read, TEST_ADDRESS0 + FLASHPAGE_SIZE - 1, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Out of bounds write */
    ret = mtd_write(dev, buf, TEST_ADDRESS0 + FLASHPAGE_SIZE - 1, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Unaligned write / read */
    ret = mtd_write(dev, &buf[1], TEST_ADDRESS1 + sizeof(buf_empty), sizeof(buf) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Only Cortex-M0 doesn't allow unaligned reads */
    ret = mtd_read(dev, &buf_read[1], TEST_ADDRESS1 + sizeof(buf_empty), sizeof(buf_read) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_write_read_page(void)
{
#ifdef MODULE_MTD_WRITE_PAGE
        const char buf[] __attribute__ ((aligned (FLASHPAGE_WRITE_BLOCK_ALIGNMENT)))
            = "abcdefghijklmno";

        uint8_t buf_empty[3];
        memset(buf_empty, FLASHPAGE_ERASE_STATE, sizeof(buf_empty));
        char buf_read[sizeof(buf) + sizeof(buf_empty)];
        memset(buf_read, 0, sizeof(buf_read));
        int ret;
        /* convert last flash page to MTD page */
        uint32_t last_vpage = LAST_AVAILABLE_PAGE * dev->pages_per_sector;
        size_t vpage_size = dev->page_size;

        /* write to the beginning of last available page */
        ret = mtd_write_page(dev, buf, last_vpage, 0, sizeof(buf));
        TEST_ASSERT_EQUAL_INT(0, ret);

        /* read back data from which some is erased */
        ret = mtd_read_page(dev, buf_read, last_vpage, 0, sizeof(buf_read));
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read, sizeof(buf)));
        TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read + sizeof(buf), sizeof(buf_empty)));

        /* clean */
        ret = mtd_erase_sector(dev, LAST_AVAILABLE_PAGE, 1);

        /* write to the beginning of the MTD page before the last available flash page */
        ret = mtd_write_page(dev, buf, last_vpage - 1, 0, sizeof(buf));
        TEST_ASSERT_EQUAL_INT(0, ret);

        /* read back data from which some is erased */
        ret = mtd_read_page(dev, buf_read, last_vpage - 1, 0, sizeof(buf_read));
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read, sizeof(buf)));
        TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read + sizeof(buf), sizeof(buf_empty)));

        /* clean*/
        ret = mtd_erase_sector(dev, LAST_AVAILABLE_PAGE, 1);

        /* write across flash page boundary */
        ret = mtd_write_page(dev, buf, last_vpage - 1, vpage_size - sizeof(buf) + 1, sizeof(buf));
        TEST_ASSERT_EQUAL_INT(0, ret);

        /* read back data from which some is erased */
        ret = mtd_read_page(dev, buf_read, last_vpage - 1, vpage_size - sizeof(buf) + 1, sizeof(buf_read));
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read, sizeof(buf)));
        TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read + sizeof(buf), sizeof(buf_empty)));

        /* Out of bounds read */
        ret = mtd_read_page(dev, buf_read,
                            FLASHPAGE_NUMOF * dev->pages_per_sector, 0, sizeof(buf_read));
        TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

        /* Out of bounds write */
        ret = mtd_write_page(dev, buf,
                             FLASHPAGE_NUMOF * dev->pages_per_sector, 0, sizeof(buf));
        TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);
#endif
}

#ifdef MODULE_PERIPH_FLASHPAGE_AUX
static bool mem_is_all_set(const uint8_t *buf, uint8_t c, size_t n)
{
    for (const uint8_t *end = buf + n; buf != end; ++buf) {
        if (*buf != c) {
            return false;
        }
    }

    return true;
}

static void test_mtd_aux_slot(void)
{
    mtd_dev_t *dev_aux = mtd_aux;

    mtd_init(dev_aux);

    uint32_t sector = dev_aux->sector_count - 2;

    static uint8_t buffer[FLASHPAGE_SIZE];

    uint32_t page_0 = dev_aux->pages_per_sector * sector;
    uint32_t page_1 = dev_aux->pages_per_sector * (sector + 1);
    uint32_t page_size = dev_aux->page_size;

    /* write dummy data to sectors */
    memset(buffer, 0x23, dev_aux->page_size);
    TEST_ASSERT_EQUAL_INT(mtd_write_page_raw(dev_aux, buffer, page_0, 0, page_size), 0);
    TEST_ASSERT_EQUAL_INT(mtd_write_page_raw(dev_aux, buffer, page_1, 0, page_size), 0);

    /* erase two sectors and check if they have been erased */
    TEST_ASSERT_EQUAL_INT(mtd_erase_sector(dev_aux, sector, 2), 0);
    TEST_ASSERT_EQUAL_INT(mtd_read_page(dev_aux, buffer, page_0, 0, page_size), 0);
    TEST_ASSERT(mem_is_all_set(buffer, 0xFF, page_size) || mem_is_all_set(buffer, 0x00, page_size));
    TEST_ASSERT_EQUAL_INT(mtd_read_page(dev_aux, buffer, page_1, 0, page_size), 0);
    TEST_ASSERT(mem_is_all_set(buffer, 0xFF, page_size) || mem_is_all_set(buffer, 0x00, page_size));

    /* write test data & read it back */
    const char test_str[] = "0123456789";
    uint32_t offset = 5;

    TEST_ASSERT_EQUAL_INT(mtd_write_page_raw(dev_aux, test_str, page_0, offset, sizeof(test_str)), 0);
    TEST_ASSERT_EQUAL_INT(mtd_read_page(dev_aux, buffer, page_0, offset, sizeof(test_str)), 0);
    TEST_ASSERT_EQUAL_INT(memcmp(test_str, buffer, sizeof(test_str)), 0);

    /* write across page boundary */
    offset = page_size - sizeof(test_str) / 2;
    TEST_ASSERT_EQUAL_INT(mtd_write_page_raw(dev_aux, test_str, page_0, offset, sizeof(test_str)), 0);
    TEST_ASSERT_EQUAL_INT(mtd_read_page(dev_aux, buffer, page_0, offset, sizeof(test_str)), 0);
    TEST_ASSERT_EQUAL_INT(memcmp(test_str, buffer, sizeof(test_str)), 0);

    /* write across sector boundary */
    offset = page_size - sizeof(test_str) / 2
           + (dev_aux->pages_per_sector - 1) * page_size;
    TEST_ASSERT_EQUAL_INT(mtd_write_page_raw(dev_aux, test_str, page_0, offset, sizeof(test_str)), 0);
    TEST_ASSERT_EQUAL_INT(mtd_read_page(dev_aux, buffer, page_0, offset, sizeof(test_str)), 0);
    TEST_ASSERT_EQUAL_INT(memcmp(test_str, buffer, sizeof(test_str)), 0);

    /* overwrite first test string, rely on MTD for read-modify-write */
    const char test_str_2[] = "Hello World!";
    offset = 5;
    TEST_ASSERT_EQUAL_INT(mtd_write_page(dev_aux, test_str_2, page_0, offset, sizeof(test_str_2)), 0);
    TEST_ASSERT_EQUAL_INT(mtd_read_page(dev_aux, buffer, page_0, offset, sizeof(test_str_2)), 0);
    TEST_ASSERT_EQUAL_INT(memcmp(test_str_2, buffer, sizeof(test_str_2)), 0);

    /* test write_page across sectors */
    offset = dev_aux->pages_per_sector * dev_aux->page_size - 2;
    TEST_ASSERT_EQUAL_INT(mtd_write_page(dev_aux, test_str, page_0, offset, sizeof(test_str)), 0);
    TEST_ASSERT_EQUAL_INT(mtd_read_page(dev_aux, buffer, page_0, offset, sizeof(test_str)), 0);
    TEST_ASSERT_EQUAL_INT(memcmp(test_str, buffer, sizeof(test_str)), 0);
}
#endif

Test *tests_mtd_flashpage_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),
        new_TestFixture(test_mtd_write_erase),
        new_TestFixture(test_mtd_write_read),
        new_TestFixture(test_mtd_write_read_page),
#ifdef MODULE_PERIPH_FLASHPAGE_AUX
        new_TestFixture(test_mtd_aux_slot),
#endif
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
