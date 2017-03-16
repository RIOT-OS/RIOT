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

static int _test_vfs_dir_op_status = -1;
static vfs_DIR _test_dir;

static void setup(void)
{
    int res = vfs_mount(&_test_vfs_mount_null);
    if (res < 0) {
        _test_vfs_dir_op_status = -1;
        return;
    }
    _test_vfs_dir_op_status = vfs_opendir(&_test_dir, "/test/mydir");
}

static void teardown(void)
{
    if (_test_vfs_dir_op_status >= 0) {
        vfs_closedir(&_test_dir);
        _test_vfs_dir_op_status = -1;
    }
    vfs_umount(&_test_vfs_mount_null);
}

static void test_vfs_null_dir_ops_opendir(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_dir_op_status);
    int res = vfs_opendir(NULL, "/test/mydir2");
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
}

static void test_vfs_null_dir_ops_closedir(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_dir_op_status);
    int res = vfs_closedir(&_test_dir);
    TEST_ASSERT_EQUAL_INT(0, res);
    res = vfs_closedir(&_test_dir);
    TEST_ASSERT_EQUAL_INT(-EBADF, res);
}

static void test_vfs_null_dir_ops_readdir(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_dir_op_status);
    vfs_dirent_t buf;
    int res = vfs_readdir(&_test_dir, &buf);
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
}

Test *tests_vfs_null_dir_ops_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_vfs_null_dir_ops_opendir),
        new_TestFixture(test_vfs_null_dir_ops_closedir),
        new_TestFixture(test_vfs_null_dir_ops_readdir),
    };

    EMB_UNIT_TESTCALLER(vfs_dir_op_tests, setup, teardown, fixtures);

    return (Test *)&vfs_dir_op_tests;
}

/** @} */
