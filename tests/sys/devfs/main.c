/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for DevFS
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "fs/devfs.h"
#include "random.h"

#include "embUnit.h"

static int _mock_open(vfs_file_t *filp, const char *name, int flags, mode_t mode);
static ssize_t _mock_read(vfs_file_t *filp, void *dest, size_t nbytes);
static ssize_t _mock_write(vfs_file_t *filp, const void *src, size_t nbytes);

static volatile int _mock_open_calls = 0;
static volatile int _mock_read_calls = 0;
static volatile int _mock_write_calls = 0;

static int _mock_private_data;

static const vfs_file_ops_t _mock_devfs_ops = {
    .open = _mock_open,
    .read = _mock_read,
    .write = _mock_write,
};

static int _mock_private_data_tag = 4321;

static devfs_t _mock_devfs_node = {
    .path = "/mock0",
    .f_op = &_mock_devfs_ops,
    .private_data = &_mock_private_data_tag,
};

static vfs_mount_t _test_devfs_mount = {
    .fs = &devfs_file_system,
    .mount_point = "/test",
    .private_data = &_mock_private_data,
};

static int _mock_open(vfs_file_t *filp, const char *name, int flags, mode_t mode)
{
    (void) name;
    (void) flags;
    (void) mode;
    if (filp->private_data.ptr != &_mock_private_data_tag) {
        return -4321;
    }
    int *np = filp->mp->private_data;
    ++(*np);
    ++_mock_open_calls;
    return 0;
}

static ssize_t _mock_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    (void) dest;
    (void) nbytes;
    if (filp->private_data.ptr != &_mock_private_data_tag) {
        return -4321;
    }
    int *np = filp->mp->private_data;
    ++(*np);
    ++_mock_read_calls;
    return 0;
}

static ssize_t _mock_write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    (void) src;
    (void) nbytes;
    if (filp->private_data.ptr != &_mock_private_data_tag) {
        return -4321;
    }
    int *np = filp->mp->private_data;
    ++(*np);
    ++_mock_write_calls;
    return 0;
}

static void test_devfs_register(void)
{
    int res = devfs_register(NULL);
    TEST_ASSERT(res < 0);

    res = devfs_register(&_mock_devfs_node);
    TEST_ASSERT(res == 0);

    res = devfs_register(&_mock_devfs_node);
    TEST_ASSERT(res < 0);

    res = devfs_unregister(&_mock_devfs_node);
    TEST_ASSERT(res == 0);

    res = devfs_unregister(&_mock_devfs_node);
    TEST_ASSERT(res < 0);
}

static void test_devfs_mount_open(void)
{
    _mock_private_data = 12345;
    int res;
    res = vfs_mount(&_test_devfs_mount);
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(_mock_private_data, 12345);

    res = devfs_register(&_mock_devfs_node);
    TEST_ASSERT_EQUAL_INT(0, res);

    int count = _mock_open_calls;
    int fd = vfs_open("/test/mock0", O_RDWR, 0);
    TEST_ASSERT(fd >= 0);
    TEST_ASSERT_EQUAL_INT(count + 1, _mock_open_calls);
    TEST_ASSERT_EQUAL_INT(_mock_private_data, 12346);

    res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = devfs_unregister(&_mock_devfs_node);
    TEST_ASSERT_EQUAL_INT(0, res);

    res = vfs_umount(&_test_devfs_mount, false);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_devfs_urandom(void)
{
    const uint8_t zeroes[8] = { 0 };
    int res;
    int fd = vfs_open("/dev/urandom", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    uint8_t buf[8] = { 0 };
    res = vfs_read(fd, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), res);
    TEST_ASSERT(memcmp(zeroes, buf, sizeof(buf)));
}

static void test_devfs_hwrng(void)
{
    const uint8_t zeroes[8] = { 0 };
    int res;
    int fd = vfs_open("/dev/hwrng", O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    uint8_t buf[8] = { 0 };
    res = vfs_read(fd, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), res);
    TEST_ASSERT(memcmp(zeroes, buf, sizeof(buf)));
}

Test *tests_devfs(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_devfs_register),
        new_TestFixture(test_devfs_mount_open),
        new_TestFixture(test_devfs_urandom),
        new_TestFixture(test_devfs_hwrng),
    };

    EMB_UNIT_TESTCALLER(devfs_tests, NULL, NULL, fixtures);

    return (Test *)&devfs_tests;
}

int main(void)
{
    extern void auto_init_devfs(void);
    auto_init_devfs();

    random_init(1);

    TESTS_START();
    TESTS_RUN(tests_devfs());
    TESTS_END();
    return 0;
}
/** @} */
