/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief Transparent-box unit tests of vfs functions stubs used when the file
 * system does not implement the requested function.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "embUnit/embUnit.h"

#include "vfs.h"

#include "tests-vfs.h"

static const vfs_file_system_ops_t null_fs_ops = {
    .mount   = NULL,
    .umount  = NULL,
    .unlink  = NULL,
    .statvfs = NULL,
    .stat    = NULL,
};

static const vfs_file_ops_t null_file_ops = {
    .close = NULL,
    .fstat = NULL,
    .lseek = NULL,
    .open  = NULL,
    .read  = NULL,
    .write = NULL,
};

static const vfs_dir_ops_t null_dir_ops = {
    .opendir  = NULL,
    .readdir  = NULL,
    .closedir = NULL,
};

static const vfs_file_system_t null_file_system = {
    .f_op  = &null_file_ops,
    .fs_op = &null_fs_ops,
    .d_op  = &null_dir_ops,
};

static vfs_mount_t _test_vfs_mount_null = {
    .mount_point = "/test",
    .fs = &null_file_system,
    .private_data = NULL,
};

static int _test_vfs_file_op_my_fd = -1;

static void setup(void)
{
    int res = vfs_mount(&_test_vfs_mount_null);
    if (res < 0) {
        _test_vfs_file_op_my_fd = -1;
        return;
    }
    _test_vfs_file_op_my_fd = vfs_open("/test/somefile", O_RDONLY, 0);
}

static void teardown(void)
{
    if (_test_vfs_file_op_my_fd >= 0) {
        vfs_close(_test_vfs_file_op_my_fd);
        _test_vfs_file_op_my_fd = -1;
    }
    vfs_umount(&_test_vfs_mount_null, false);
}

static void test_vfs_null_file_ops_close(void)
{
    TEST_ASSERT(_test_vfs_file_op_my_fd >= 0);
    int res = vfs_close(_test_vfs_file_op_my_fd);
    TEST_ASSERT_EQUAL_INT(0, res);
    _test_vfs_file_op_my_fd = -1; /* prevent double close */
}

static void test_vfs_null_file_ops_fcntl(void)
{
    TEST_ASSERT(_test_vfs_file_op_my_fd >= 0);
    int res = vfs_fcntl(_test_vfs_file_op_my_fd, F_GETFL, 0);
    TEST_ASSERT_EQUAL_INT(O_RDONLY, res);
    res = vfs_fcntl(_test_vfs_file_op_my_fd, F_GETFD, 0);
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
}

static void test_vfs_null_file_ops_lseek(void)
{
    TEST_ASSERT(_test_vfs_file_op_my_fd >= 0);
    off_t pos;
    pos = vfs_lseek(_test_vfs_file_op_my_fd, 4, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(4, pos);
    pos = vfs_lseek(_test_vfs_file_op_my_fd, 3, SEEK_CUR);
    TEST_ASSERT_EQUAL_INT(7, pos);
    pos = vfs_lseek(_test_vfs_file_op_my_fd, -7, SEEK_CUR);
    TEST_ASSERT_EQUAL_INT(0, pos);
    pos = vfs_lseek(_test_vfs_file_op_my_fd, -1, SEEK_CUR);
    TEST_ASSERT_EQUAL_INT(-EINVAL, pos);
    pos = vfs_lseek(_test_vfs_file_op_my_fd, 12345, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(12345, pos);
    pos = vfs_lseek(_test_vfs_file_op_my_fd, -1, SEEK_SET);
    TEST_ASSERT_EQUAL_INT(-EINVAL, pos);
    pos = vfs_lseek(_test_vfs_file_op_my_fd, 0, SEEK_END); /* not implemented in "file system" */
    TEST_ASSERT_EQUAL_INT(-EINVAL, pos);
    pos = vfs_lseek(_test_vfs_file_op_my_fd, 0, SEEK_CUR);
    TEST_ASSERT_EQUAL_INT(12345, pos);
}

static void test_vfs_null_file_ops_fstat(void)
{
    TEST_ASSERT(_test_vfs_file_op_my_fd >= 0);
    struct stat buf;
    int res = vfs_fstat(_test_vfs_file_op_my_fd, &buf);
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
}

static void test_vfs_null_file_ops_read(void)
{
    TEST_ASSERT(_test_vfs_file_op_my_fd >= 0);
    uint8_t buf[8];
    int res = vfs_read(_test_vfs_file_op_my_fd, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
    res = vfs_read(_test_vfs_file_op_my_fd, NULL, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(-EFAULT, res);
}

static void test_vfs_null_file_ops_write(void)
{
    TEST_ASSERT(_test_vfs_file_op_my_fd >= 0);
    static const char buf[] = "Unit test";
    int res = vfs_write(_test_vfs_file_op_my_fd, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(-EBADF, res);
    res = vfs_write(_test_vfs_file_op_my_fd, NULL, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(-EFAULT, res);
}

Test *tests_vfs_null_file_ops_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_vfs_null_file_ops_close),
        new_TestFixture(test_vfs_null_file_ops_fcntl),
        new_TestFixture(test_vfs_null_file_ops_lseek),
        new_TestFixture(test_vfs_null_file_ops_fstat),
        new_TestFixture(test_vfs_null_file_ops_read),
        new_TestFixture(test_vfs_null_file_ops_write),
    };

    EMB_UNIT_TESTCALLER(vfs_file_op_tests, setup, teardown, fixtures);

    return (Test *)&vfs_file_op_tests;
}

/** @} */
