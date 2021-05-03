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
#include "fs/littlefs2_fs.h"
#include "vfs.h"
#include "mtd.h"
#include "board.h"

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "embUnit.h"

/* Define MTD_0 in board.h to use the board mtd if any and if
 * CONFIG_USE_HARDWARE_MTD is defined (add CFLAGS=-DCONFIG_USE_HARDWARE_MTD to
 * the command line to enable it */
#if defined(MTD_0) && IS_ACTIVE(CONFIG_USE_HARDWARE_MTD)
#define USE_MTD_0
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

    return 0;
}

static int _read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    (void)dev;

    if (addr + size > sizeof(dummy_memory)) {
        return -EOVERFLOW;
    }
    memcpy(buff, dummy_memory + addr, size);

    return 0;
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

    return 0;
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

static littlefs2_desc_t littlefs_desc;

static vfs_mount_t _test_littlefs_mount = {
    .fs = &littlefs2_file_system,
    .mount_point = "/test-littlefs",
    .private_data = &littlefs_desc,
};

static void test_littlefs_setup(void)
{
    littlefs_desc.dev = _dev;
    vfs_mount(&_test_littlefs_mount);
}

static void test_littlefs_teardown(void)
{
    vfs_unlink("/test-littlefs/test.txt");
    vfs_unlink("/test-littlefs/test0.txt");
    vfs_unlink("/test-littlefs/test1.txt");
    vfs_unlink("/test-littlefs/a/test2.txt");
    vfs_rmdir("/test-littlefs/a");
    vfs_umount(&_test_littlefs_mount);
}

static void tests_littlefs_format(void)
{
    int res;
    vfs_umount(&_test_littlefs_mount);
    res = mtd_erase(_dev, 0, _dev->page_size * _dev->pages_per_sector * _dev->sector_count);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_mount(&_test_littlefs_mount);
    TEST_ASSERT(res < 0);

    /* 1. format an invalid file system (failed mount) */
    res = vfs_format(&_test_littlefs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_mount(&_test_littlefs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_umount(&_test_littlefs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    /* 2. format a valid file system */
    res = vfs_format(&_test_littlefs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void tests_littlefs_mount_umount(void)
{
    int res;
    res = vfs_umount(&_test_littlefs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_mount(&_test_littlefs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void tests_littlefs_open_close(void)
{
    int res;
    res = vfs_open("/test-littlefs/test.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(res >= 0);

    res = vfs_close(res);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void tests_littlefs_write(void)
{
    const char buf[] = "TESTSTRING";
    char r_buf[2 * sizeof(buf)];

    int res;
    int fd = vfs_open("/test-littlefs/test.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(fd >= 0);

    res = vfs_write(fd, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), res);

    res = vfs_lseek(fd, 0, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_read(fd, r_buf, sizeof(r_buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), res);
    TEST_ASSERT_EQUAL_STRING(&buf[0], &r_buf[0]);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    fd = vfs_open("/test-littlefs/test.txt", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    res = vfs_read(fd, r_buf, sizeof(r_buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), res);
    TEST_ASSERT_EQUAL_STRING(&buf[0], &r_buf[0]);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void tests_littlefs_unlink(void)
{
    const char buf[] = "TESTSTRING";

    int res;
    int fd = vfs_open("/test-littlefs/test.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(fd >= 0);

    res = vfs_write(fd, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), res);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_unlink("/test-littlefs/test.txt");
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void tests_littlefs_readdir(void)
{
    const char buf0[] = "TESTSTRING";
    const char buf1[] = "TESTTESTSTRING";
    const char buf2[] = "TESTSTRINGSTRING";

    int res;
    int fd0 = vfs_open("/test-littlefs/test0.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(fd0 >= 0);

    int fd1 = vfs_open("/test-littlefs/test1.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(fd1 >= 0);

    int fd2 = vfs_open("/test-littlefs/a/test2.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(fd2 < 0);

    res = vfs_mkdir("/test-littlefs/a", 0);
    TEST_ASSERT_EQUAL_INT(0, res);

    fd2 = vfs_open("/test-littlefs/a/test2.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(fd2 >= 0);

    res = vfs_write(fd0, buf0, sizeof(buf0));
    TEST_ASSERT_EQUAL_INT(sizeof(buf0), res);

    res = vfs_write(fd1, buf1, sizeof(buf1));
    TEST_ASSERT_EQUAL_INT(sizeof(buf1), res);

    res = vfs_write(fd2, buf2, sizeof(buf2));
    TEST_ASSERT_EQUAL_INT(sizeof(buf2), res);

    res = vfs_close(fd0);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_close(fd1);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_close(fd2);
    TEST_ASSERT_EQUAL_INT(0, res);

    vfs_DIR dirp;
    res = vfs_opendir(&dirp, "/test-littlefs");
    TEST_ASSERT_EQUAL_INT(0, res);

    vfs_dirent_t entry;
    int nb_files = 0;
    do {
        res = vfs_readdir(&dirp, &entry);
        if (res == 1 && (strcmp("/test0.txt", &(entry.d_name[0])) == 0 ||
                         strcmp("/test1.txt", &(entry.d_name[0])) == 0)) {
            nb_files++;
        }
    } while (res == 1);

    TEST_ASSERT_EQUAL_INT(2, nb_files);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_closedir(&dirp);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_opendir(&dirp, "/test-littlefs/a");
    TEST_ASSERT_EQUAL_INT(0, res);
    nb_files = 0;
    do {
        res = vfs_readdir(&dirp, &entry);
        if (res == 1 && strcmp("/test2.txt", &(entry.d_name[0])) == 0) {
            nb_files++;
        }
    } while (res == 1);

    TEST_ASSERT_EQUAL_INT(1, nb_files);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_unlink("/test-littlefs/test0.txt");
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_unlink("/test-littlefs/test1.txt");
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_unlink("/test-littlefs/a/test2.txt");
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_closedir(&dirp);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_rmdir("/test-littlefs/a");
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void tests_littlefs_rename(void)
{
    const char buf[] = "TESTSTRING";
    char r_buf[2 * sizeof(buf)];

    int res;
    int fd = vfs_open("/test-littlefs/test.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(fd >= 0);

    res = vfs_write(fd, buf, sizeof(buf));
    TEST_ASSERT(res == sizeof(buf));

    res = vfs_lseek(fd, 0, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_read(fd, r_buf, sizeof(r_buf));
    TEST_ASSERT(res == sizeof(buf));
    TEST_ASSERT_EQUAL_STRING(&buf[0], &r_buf[0]);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_rename("/test-littlefs/test.txt", "/test-littlefs/test1.txt");
    TEST_ASSERT_EQUAL_INT(0, res);

    fd = vfs_open("/test-littlefs/test.txt", O_RDONLY, 0);
    TEST_ASSERT(fd < 0);

    fd = vfs_open("/test-littlefs/test1.txt", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    res = vfs_lseek(fd, 0, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_read(fd, r_buf, sizeof(r_buf));
    TEST_ASSERT(res == sizeof(buf));
    TEST_ASSERT_EQUAL_STRING(&buf[0], &r_buf[0]);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_unlink("/test-littlefs/test1.txt");
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void tests_littlefs_statvfs(void)
{
    const char buf[] = "TESTSTRING";
    struct statvfs stat1;
    struct statvfs stat2;

    int res = vfs_statvfs("/test-littlefs/", &stat1);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(_dev->page_size * _dev->pages_per_sector, stat1.f_bsize);
    TEST_ASSERT_EQUAL_INT(_dev->page_size * _dev->pages_per_sector, stat1.f_frsize);
    TEST_ASSERT((_dev->pages_per_sector * _dev->page_size * _dev->sector_count) >=
                          stat1.f_blocks);

    int fd = vfs_open("/test-littlefs/test.txt", O_CREAT | O_RDWR, 0);
    TEST_ASSERT(fd >= 0);

    for (int i = 0; i < 128; ++i) {
        res = vfs_write(fd, buf, sizeof(buf));
        TEST_ASSERT(res == sizeof(buf));
    }

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_statvfs("/test-littlefs/", &stat2);
    TEST_ASSERT_EQUAL_INT(0, res);

    TEST_ASSERT_EQUAL_INT(_dev->page_size * _dev->pages_per_sector, stat2.f_bsize);
    TEST_ASSERT_EQUAL_INT(_dev->page_size * _dev->pages_per_sector, stat2.f_frsize);
    TEST_ASSERT(stat1.f_bfree > stat2.f_bfree);
    TEST_ASSERT(stat1.f_bavail > stat2.f_bavail);
}

Test *tests_littlefs(void)
{
#ifndef USE_MTD_0
    memset(dummy_memory, 0xff, sizeof(dummy_memory));
#endif

    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_littlefs_format),
        new_TestFixture(tests_littlefs_mount_umount),
        new_TestFixture(tests_littlefs_open_close),
        new_TestFixture(tests_littlefs_write),
        new_TestFixture(tests_littlefs_unlink),
        new_TestFixture(tests_littlefs_readdir),
        new_TestFixture(tests_littlefs_rename),
        new_TestFixture(tests_littlefs_statvfs),
    };

    EMB_UNIT_TESTCALLER(littlefs_tests, test_littlefs_setup, test_littlefs_teardown, fixtures);

    return (Test *)&littlefs_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_littlefs());
    TESTS_END();
    return 0;
}
/** @} */
