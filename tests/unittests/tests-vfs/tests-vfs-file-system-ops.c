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

static int _test_vfs_fs_op_mount_res = -1;

static void setup(void)
{
    _test_vfs_fs_op_mount_res = vfs_mount(&_test_vfs_mount_null);
}

static void teardown(void)
{
    vfs_umount(&_test_vfs_mount_null);
}

static void test_vfs_null_fs_ops_mount(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_fs_op_mount_res);
    int res = vfs_mount(&_test_vfs_mount_null);
    /* Already mounted */
    TEST_ASSERT_EQUAL_INT(-EBUSY, res);
}

static void test_vfs_null_fs_ops_umount(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_fs_op_mount_res);
    int res = vfs_umount(&_test_vfs_mount_null);
    TEST_ASSERT_EQUAL_INT(0, res);
    res = vfs_umount(&_test_vfs_mount_null);
    /* Not mounted */
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
}

static void test_vfs_null_fs_ops_rename(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_fs_op_mount_res);
    int res = vfs_rename("/test/foo", "/test/bar");
    TEST_ASSERT_EQUAL_INT(-EPERM, res);
}

static void test_vfs_null_fs_ops_unlink(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_fs_op_mount_res);
    int res = vfs_unlink("/test/foo");
    TEST_ASSERT_EQUAL_INT(-EPERM, res);
}

static void test_vfs_null_fs_ops_mkdir(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_fs_op_mount_res);
    int res = vfs_mkdir("/test/foodir", 0);
    TEST_ASSERT_EQUAL_INT(-EPERM, res);
}

static void test_vfs_null_fs_ops_rmdir(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_fs_op_mount_res);
    int res = vfs_rmdir("/test/foodir");
    TEST_ASSERT_EQUAL_INT(-EPERM, res);
}

static void test_vfs_null_fs_ops_stat(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_fs_op_mount_res);
    struct stat buf;
    int res = vfs_stat("/test/foo", &buf);
    TEST_ASSERT_EQUAL_INT(-EPERM, res);
}

static void test_vfs_null_fs_ops_statvfs(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_fs_op_mount_res);
    struct statvfs buf;
    int res = vfs_statvfs("/test", &buf);
    TEST_ASSERT_EQUAL_INT(-EPERM, res);
}

static void test_vfs_null_fs_ops_fstatvfs(void)
{
    TEST_ASSERT_EQUAL_INT(0, _test_vfs_fs_op_mount_res);
    int fd = vfs_open("/test/baz", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);
    struct statvfs buf;
    int res = vfs_fstatvfs(fd, &buf);
    TEST_ASSERT_EQUAL_INT(-EINVAL, res);
}


Test *tests_vfs_null_file_system_ops_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_vfs_null_fs_ops_mount),
        new_TestFixture(test_vfs_null_fs_ops_umount),
        new_TestFixture(test_vfs_null_fs_ops_rename),
        new_TestFixture(test_vfs_null_fs_ops_unlink),
        new_TestFixture(test_vfs_null_fs_ops_mkdir),
        new_TestFixture(test_vfs_null_fs_ops_rmdir),
        new_TestFixture(test_vfs_null_fs_ops_stat),
        new_TestFixture(test_vfs_null_fs_ops_statvfs),
        new_TestFixture(test_vfs_null_fs_ops_fstatvfs),
    };

    EMB_UNIT_TESTCALLER(vfs_fs_op_tests, setup, teardown, fixtures);

    return (Test *)&vfs_fs_op_tests;
}

/** @} */
