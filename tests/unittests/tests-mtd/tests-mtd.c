/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <string.h>
#include <errno.h>

#include "embUnit.h"

#include "board.h"
#include "mtd.h"
#include "mtd_emulated.h"

#if MODULE_VFS
#  include <fcntl.h>
#  include <stdio.h>
#  include "vfs.h"
#endif

/* Test mock object using a mtd_emulated_t as the underlying device. */
#ifndef SECTOR_COUNT
#  define SECTOR_COUNT 4
#endif
#ifndef PAGE_PER_SECTOR
#  define PAGE_PER_SECTOR 4
#endif
#ifndef PAGE_SIZE
#  define PAGE_SIZE 128
#endif

MTD_EMULATED_DEV(0, SECTOR_COUNT, PAGE_PER_SECTOR, PAGE_SIZE);

/**
 * @brief   The device under test.
 *
 * The whole suite is run once per device, so that the emulated mtd is always
 * covered, next to the mtd of the board, if any. */
static mtd_dev_t *dev;

static void setup_teardown(void)
{
    mtd_erase(dev, 0, dev->pages_per_sector * dev->page_size);
}

static void setup_teardown_emulated(void)
{
    dev = &mtd_emulated_dev0.base;
    setup_teardown();
}

#ifdef MTD_0
static void setup_teardown_board(void)
{
    dev = MTD_0;
    setup_teardown();
}
#endif /* MTD_0 */

static void test_mtd_init(void)
{
    int ret = mtd_init(dev);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_erase(void)
{
    /* Erase first sector */
    int ret = mtd_erase(dev, 0, dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase with wrong size (les than sector size) */
    ret = mtd_erase(dev, 0, dev->page_size);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Unaligned erase */
    ret = mtd_erase(dev, dev->page_size, dev->page_size);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Erase 2nd - 3rd sector */
    ret = mtd_erase(dev, dev->pages_per_sector * dev->page_size,
                    dev->pages_per_sector * dev->page_size * 2);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase out of memory area */
    ret = mtd_erase(dev, dev->pages_per_sector * dev->page_size * dev->sector_count,
                    dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);
}

static void test_mtd_write_erase(void)
{
    const char buf[] = "ABCDEFGHIJK";
    uint8_t buf_empty[] = {0xff, 0xff, 0xff};
    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    int ret = mtd_write(dev, buf, sizeof(buf_empty), sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_erase(dev, 0, dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    uint8_t expected[sizeof(buf_read)];
    memset(expected, 0xff, sizeof(expected));
    ret = mtd_read(dev, buf_read, 0, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(expected, buf_read, sizeof(buf_read)));

}

static void test_mtd_write_read(void)
{
    const char buf[] = "ABCDEFGH";
    uint8_t buf_empty[] = {0xff, 0xff, 0xff};
    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    /* Basic write / read */
    int ret = mtd_write(dev, buf, 0, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_read(dev, buf_read, 0, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read + sizeof(buf), sizeof(buf_empty)));

    /* Unaligned write / read */
    ret = mtd_write(dev, buf, dev->page_size + sizeof(buf_empty), sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_read(dev, buf_read, dev->page_size, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read, sizeof(buf_empty)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read + sizeof(buf_empty), sizeof(buf)));

    /* out of bounds write (addr) */
    ret = mtd_write(dev, buf, dev->pages_per_sector * dev->page_size * dev->sector_count,
                    sizeof(buf));
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* out of bounds write (addr + count) */
    ret = mtd_write(dev, buf, (dev->pages_per_sector * dev->page_size * dev->sector_count)
                    - (sizeof(buf) / 2), sizeof(buf));
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* out of bounds write (more than page size) */
    const size_t page_size = dev->page_size;
    uint8_t buf_page[page_size + 1];
    memset(buf_page, 1, sizeof(buf_page));
    ret = mtd_write(dev, buf_page, 0, sizeof(buf_page));
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Read more than one page */
    ret = mtd_erase(dev, 0, dev->page_size * dev->pages_per_sector);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = mtd_write(dev, buf_page, 0, dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = mtd_write(dev, buf_page, dev->page_size, dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);
    memset(buf_page, 0, sizeof(buf_page));
    ret = mtd_read(dev, buf_page, 0, sizeof(buf_page));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(1, buf_page[0]);
    TEST_ASSERT_EQUAL_INT(1, buf_page[sizeof(buf_page) - 1]);

    /* pages overlap write */
    ret = mtd_write(dev, buf, dev->page_size - (sizeof(buf) / 2), sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = mtd_write(dev, buf_page, 1, sizeof(buf_page) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_write_read_flash(void)
{
    const uint8_t buf1[] = {0xee, 0xdd, 0xcc};
    const uint8_t buf2[] = {0x33, 0x33, 0x33};
    const uint8_t buf_expected[] = {0x22, 0x11, 0x0};
    uint8_t buf_empty[] = {0xff, 0xff, 0xff};
    char buf_read[sizeof(buf_expected) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    /* Test flash AND behavior: programming a location a second time may only
     * clear bits, it may never set a bit from 0 back to 1. */

    /* Basic write / read */
    int ret = mtd_write(dev, buf1, 0, sizeof(buf1));
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = mtd_write(dev, buf2, 0, sizeof(buf2));
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_read(dev, buf_read, 0, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_expected, buf_read, sizeof(buf_expected)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read + sizeof(buf_expected), sizeof(buf_empty)));
}

#if MODULE_VFS
static void test_mtd_vfs(void)
{
    int fd;
    fd = vfs_bind(VFS_ANY_FD, O_RDWR, &mtd_vfs_ops, dev);
    const char buf[] = "mnopqrst";
    uint8_t buf_empty[] = {0xff, 0xff, 0xff};
    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    int ret = vfs_lseek(fd, sizeof(buf_empty), SEEK_SET);
    TEST_ASSERT_EQUAL_INT(sizeof(buf_empty), ret);
    ret = vfs_write(fd, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), ret);
    ret = vfs_lseek(fd, 0, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = vfs_read(fd, buf_read, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(sizeof(buf_read), ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read, sizeof(buf_empty)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read + sizeof(buf_empty), sizeof(buf)));

    ret = vfs_lseek(fd, 0, SEEK_END);
    TEST_ASSERT(ret > 0);
    ret = vfs_write(fd, buf, sizeof(buf));
    /* Attempted to write past the device memory */
    TEST_ASSERT(ret < 0);
}
#endif

EMB_UNIT_TESTFIXTURES(fixtures) {
    new_TestFixture(test_mtd_init),
    new_TestFixture(test_mtd_erase),
    new_TestFixture(test_mtd_write_erase),
    new_TestFixture(test_mtd_write_read),
    new_TestFixture(test_mtd_write_read_flash),
#if MODULE_VFS
    new_TestFixture(test_mtd_vfs),
#endif
};

static Test *tests_mtd_emulated_tests(void)
{
    EMB_UNIT_TESTCALLER(mtd_emulated_tests, setup_teardown_emulated,
                        setup_teardown_emulated, fixtures);

    return (Test *)&mtd_emulated_tests;
}

#ifdef MTD_0
static Test *tests_mtd_board_tests(void)
{
    EMB_UNIT_TESTCALLER(mtd_board_tests, setup_teardown_board,
                        setup_teardown_board, fixtures);

    return (Test *)&mtd_board_tests;
}
#endif

void tests_mtd(void)
{
    TESTS_RUN(tests_mtd_emulated_tests());
#ifdef MTD_0
    TESTS_RUN(tests_mtd_board_tests());
#endif
}
/** @} */
