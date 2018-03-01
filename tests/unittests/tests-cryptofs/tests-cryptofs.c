/*
 * Copyright (C) 2017 OTA keys S.A.
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
#include "fs/littlefs_fs.h"
#include "fs/cryptofs.h"
#include "vfs.h"
#include "mtd.h"
#include "board.h"

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "embUnit/embUnit.h"

#include "tests-cryptofs.h"

/* Define MTD_0 in board.h to use the board mtd if any */
#ifdef MTD_0
#define _dev (MTD_0)
#else
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

static uint8_t dummy_memory[PAGE_PER_SECTOR * PAGE_SIZE * SECTOR_COUNT];

static int _init(mtd_dev_t *dev)
{
    (void)dev;

    memset(dummy_memory, 0xff, sizeof(dummy_memory));
    return 0;
}

static int _read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    (void)dev;

    if (addr + size > sizeof(dummy_memory)) {
        return -EOVERFLOW;
    }
    memcpy(buff, dummy_memory + addr, size);

    return size;
}

static int _write(mtd_dev_t *dev, const void *buff, uint32_t addr, uint32_t size)
{
    (void)dev;

    if (addr + size > sizeof(dummy_memory)) {
        return -EOVERFLOW;
    }
    if (size > PAGE_SIZE) {
        return -EOVERFLOW;
    }
    memcpy(dummy_memory + addr, buff, size);

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
    if (addr + size > sizeof(dummy_memory)) {
        return -EOVERFLOW;
    }
    memset(dummy_memory + addr, 0xff, size);

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
};

static mtd_dev_t dev = {
    .driver = &driver,
    .sector_count = SECTOR_COUNT,
    .pages_per_sector = PAGE_PER_SECTOR,
    .page_size = PAGE_SIZE,
};

static mtd_dev_t *_dev = (mtd_dev_t*) &dev;
#endif /* MTD_0 */

static littlefs_desc_t littlefs_desc;

static vfs_mount_t _test_littlefs_mount = {
    .fs = &littlefs_file_system,
    .mount_point = "/test-littlefs",
    .private_data = &littlefs_desc,
};

#define TEST_KEY {0xe5, 0xd2, 0xf0, 0xce, 0x95, 0x3f, 0x85, 0x0b, 0xce, 0x74, 0xdf, 0xac, 0xcc, 0xb1, 0x62, 0x30}

static cryptofs_t cryptofs_desc = {
    .key = TEST_KEY,
    .real_fs = &_test_littlefs_mount,
};

static vfs_mount_t _test_cryptofs_mount = {
    .fs = &cryptofs_file_system,
    .mount_point = "/cryptofs",
    .private_data = &cryptofs_desc,
};

static void test_cryptofs_setup(void)
{
    littlefs_desc.dev = _dev;
}

static void test_cryptofs_teardown(void)
{

}

static void tests_cryptofs_mount_umount(void)
{
    int res;
    res = vfs_format(&_test_littlefs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_mount(&_test_cryptofs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_umount(&_test_cryptofs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void tests_cryptofs_open_close(void)
{
    int res;
    res = vfs_mount(&_test_cryptofs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_open("/cryptofs/test.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(res >= 0);

    res = vfs_close(res);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_umount(&_test_cryptofs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void tests_cryptofs_write(void)
{
    const char buf[] = "TESTSTRING1234567890";
    char r_buf[2 * sizeof(buf)];

    int res;
    res = vfs_mount(&_test_cryptofs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    int fd = vfs_open("/cryptofs/test.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(fd >= 0);

    res = vfs_write(fd, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), res);

    res = vfs_lseek(fd, 0, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_read(fd, r_buf, sizeof(r_buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), res);
    TEST_ASSERT_EQUAL_STRING(&buf[0], &r_buf[0]);

    res = vfs_write(fd, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), res);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    fd = vfs_open("/cryptofs/test.txt", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    res = vfs_read(fd, r_buf, sizeof(r_buf));
    TEST_ASSERT_EQUAL_INT(sizeof(r_buf), res);
    TEST_ASSERT_EQUAL_STRING(&buf[0], &r_buf[0]);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_umount(&_test_cryptofs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
}

Test *tests_cryptofs_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_cryptofs_mount_umount),
        new_TestFixture(tests_cryptofs_open_close),
        new_TestFixture(tests_cryptofs_write),
    };

    EMB_UNIT_TESTCALLER(cryptofs_tests, test_cryptofs_setup, test_cryptofs_teardown, fixtures);

    return (Test *)&cryptofs_tests;
}

void tests_cryptofs(void)
{
    TESTS_RUN(tests_cryptofs_tests());
}
/** @} */
