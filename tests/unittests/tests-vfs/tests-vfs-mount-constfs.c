/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for vfs_mount, vfs_umount, ConstFS, VFS POSIX wrappers
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "embUnit/embUnit.h"

#include "vfs.h"
#include "fs/constfs.h"

#include "tests-vfs.h"

static const uint8_t bin_data[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};
static const uint8_t str_data[] = "This is a test file";
                                /* 01234567890123456789 */
                                /* 0         1          */
static const constfs_file_t _files[] = {
    {
        .path = "/test.txt",
        .data = str_data,
        .size = sizeof(str_data),
    },
    {
        .path = "/data.bin",
        .data = bin_data,
        .size = sizeof(bin_data),
    },
};

static const constfs_t fs_data = {
    .files = _files,
    .nfiles = sizeof(_files) / sizeof(_files[0]),
};

static vfs_mount_t _test_vfs_mount_invalid_mount = {
    .mount_point = "test",
    .fs = &constfs_file_system,
    .private_data = (void *)&fs_data,
};

static vfs_mount_t _test_vfs_mount = {
    .mount_point = "/test",
    .fs = &constfs_file_system,
    .private_data = (void *)&fs_data,
};

static void test_vfs_mount_umount(void)
{
    int res;
    res = vfs_mount(&_test_vfs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
    res = vfs_umount(&_test_vfs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_vfs_mount__invalid(void)
{
    int res;
    res = vfs_mount(NULL);
    TEST_ASSERT(res < 0);

    res = vfs_mount(&_test_vfs_mount_invalid_mount);
    TEST_ASSERT(res < 0);
}

static void test_vfs_umount__invalid_mount(void)
{
    int res;
    res = vfs_umount(NULL);
    TEST_ASSERT(res < 0);
    res = vfs_umount(&_test_vfs_mount);
    TEST_ASSERT(res < 0);
}

static void test_vfs_constfs_open(void)
{
    int res;
    res = vfs_mount(&_test_vfs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    int fd;
    fd = vfs_open("/test/notfound", O_RDONLY, 0);
    TEST_ASSERT(fd == -ENOENT);
    if (fd >= 0) {
        vfs_close(fd);
    }
    fd = vfs_open("/test/test.txt", O_WRONLY, 0);
    TEST_ASSERT(fd == -EROFS);
    if (fd >= 0) {
        vfs_close(fd);
    }
    fd = vfs_open("/test/test.txt", O_RDWR, 0);
    TEST_ASSERT(fd == -EROFS);
    if (fd >= 0) {
        vfs_close(fd);
    }
    fd = vfs_open("/test/test.txt", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);
    if (fd >= 0) {
        res = vfs_close(fd);
        TEST_ASSERT_EQUAL_INT(0, res);
    }

    res = vfs_umount(&_test_vfs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_vfs_constfs_read_lseek(void)
{
    int res;
    res = vfs_mount(&_test_vfs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    int fd = vfs_open("/test/test.txt", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    char strbuf[64];
    memset(strbuf, '\0', sizeof(strbuf));
    ssize_t nbytes;
    nbytes = vfs_read(fd, strbuf, sizeof(strbuf));
    TEST_ASSERT_EQUAL_INT(sizeof(str_data), nbytes);
    TEST_ASSERT_EQUAL_STRING((const char *)&str_data[0], (const char *)&strbuf[0]);

    off_t pos;
    /* lseek to the middle */
    memset(strbuf, '\0', sizeof(strbuf));
    pos = vfs_lseek(fd, sizeof(str_data) / 2, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(sizeof(str_data) / 2, pos);
    nbytes = vfs_read(fd, strbuf, sizeof(strbuf));
    TEST_ASSERT_EQUAL_INT((sizeof(str_data) + 1) / 2, nbytes); /* + 1 for rounding up */
    TEST_ASSERT_EQUAL_STRING((const char *)&str_data[sizeof(str_data) / 2], (const char *)&strbuf[0]);

    /* lseek to near the end */
    memset(strbuf, '\0', sizeof(strbuf));
    pos = vfs_lseek(fd, -1, SEEK_END);
    TEST_ASSERT_EQUAL_INT(sizeof(str_data) - 1, pos);
    nbytes = vfs_read(fd, strbuf, sizeof(strbuf));
    TEST_ASSERT_EQUAL_INT(1, nbytes);
    TEST_ASSERT_EQUAL_STRING((const char *)&str_data[sizeof(str_data) - 1], (const char *)&strbuf[0]);

    res = vfs_fcntl(fd, F_GETFL, 0);
    TEST_ASSERT_EQUAL_INT(O_RDONLY, res);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_umount(&_test_vfs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
}

#if MODULE_NEWLIB || defined(BOARD_NATIVE)
static void test_vfs_constfs__posix(void)
{
    int res;
    res = vfs_mount(&_test_vfs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);

    int fd = open("/test/test.txt", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    char strbuf[64];
    memset(strbuf, '\0', sizeof(strbuf));
    ssize_t nbytes;
    nbytes = read(fd, strbuf, sizeof(strbuf));
    TEST_ASSERT_EQUAL_INT(sizeof(str_data), nbytes);
    TEST_ASSERT_EQUAL_STRING((const char *)&str_data[0], (const char *)&strbuf[0]);

#if HAVE_FCNTL
    /* fcntl support is optional in newlib */
    res = fcntl(fd, F_GETFL, 0);
    TEST_ASSERT_EQUAL_INT(O_RDONLY, res);
#endif

    res = close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_umount(&_test_vfs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
}
#endif

Test *tests_vfs_mount_constfs_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_vfs_mount_umount),
        new_TestFixture(test_vfs_mount__invalid),
        new_TestFixture(test_vfs_umount__invalid_mount),
        new_TestFixture(test_vfs_constfs_open),
        new_TestFixture(test_vfs_constfs_read_lseek),
#if MODULE_NEWLIB || defined(BOARD_NATIVE)
        new_TestFixture(test_vfs_constfs__posix),
#endif
    };

    EMB_UNIT_TESTCALLER(vfs_mount_tests, NULL, NULL, fixtures);

    return (Test *)&vfs_mount_tests;
}

/** @} */
