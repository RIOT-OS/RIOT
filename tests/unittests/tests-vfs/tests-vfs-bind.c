/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for vfs_bind
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

#include "tests-vfs.h"

static const uint8_t str_data[] = "This is a test file";
                                /* 01234567890123456789 */
                                /* 0         1          */

#define _VFS_TEST_BIND_BUFSIZE 8

static ssize_t _mock_write(vfs_file_t *filp, const void *src, size_t nbytes);
static ssize_t _mock_read(vfs_file_t *filp, void *dest, size_t nbytes);

static volatile int _mock_write_calls = 0;
static volatile int _mock_read_calls = 0;

static vfs_file_ops_t _test_bind_ops = {
    .read = _mock_read,
    .write = _mock_write,
};

static ssize_t _mock_write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    void *dest = filp->private_data.ptr;
    ++_mock_write_calls;
    if (nbytes > _VFS_TEST_BIND_BUFSIZE) {
        nbytes = _VFS_TEST_BIND_BUFSIZE;
    }
    memcpy(dest, src, nbytes);
    return nbytes;
}

static ssize_t _mock_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    void *src = filp->private_data.ptr;
    ++_mock_read_calls;
    if (nbytes > _VFS_TEST_BIND_BUFSIZE) {
        nbytes = _VFS_TEST_BIND_BUFSIZE;
    }
    memcpy(dest, src, nbytes);
    return nbytes;
}

static void test_vfs_bind(void)
{
    int fd;
    uint8_t buf[_VFS_TEST_BIND_BUFSIZE];
    fd = vfs_bind(VFS_ANY_FD, O_RDWR, &_test_bind_ops, &buf[0]);
    TEST_ASSERT(fd >= 0);
    TEST_ASSERT(fd != STDIN_FILENO);
    TEST_ASSERT(fd != STDOUT_FILENO);
    TEST_ASSERT(fd != STDERR_FILENO);
    if (fd < 0) {
        return;
    }

    ssize_t nbytes;
    int ncalls = _mock_write_calls;
    nbytes = vfs_write(fd, &str_data[0], sizeof(str_data));
    TEST_ASSERT_EQUAL_INT(_mock_write_calls, ncalls + 1);
    TEST_ASSERT_EQUAL_INT(_VFS_TEST_BIND_BUFSIZE, nbytes);
    TEST_ASSERT_EQUAL_INT(0, memcmp(&str_data[0], &buf[0], nbytes));

    char strbuf[64];
    memset(strbuf, '\0', sizeof(strbuf));
    ncalls = _mock_read_calls;
    nbytes = vfs_read(fd, strbuf, sizeof(strbuf));
    TEST_ASSERT_EQUAL_INT(_mock_read_calls, ncalls + 1);
    TEST_ASSERT_EQUAL_INT(_VFS_TEST_BIND_BUFSIZE, nbytes);
    TEST_ASSERT_EQUAL_INT(0, memcmp(&buf[0], &strbuf[0], nbytes));

    int res = vfs_close(fd);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_vfs_bind__leak_fds(void)
{
    /* This test was added after a bug was discovered in the _allocate_fd code to
     * make sure that fds are not leaked when doing multiple bind/close calls */
    /* Try >VFS_MAX_OPEN_FILES times to open and close fds to see that they are
     * not leaked */
    for (unsigned int i = 0; i < VFS_MAX_OPEN_FILES; ++i) {
        test_vfs_bind();
    }
    /* The following will fail if the fds above are not properly freed */
    test_vfs_bind();
}

static void test_vfs_bind__allocate_invalid_fd(void)
{
    /* Check that fds >= VFS_MAX_OPEN_FILES fails with -ENFILE, to avoid out of
     * bounds array access */
    /* This test assumes FD numbering begins at 0, update this test if
     * VFS_MAX_OPEN_FILES is a valid fd number */
    int fd;
    fd = vfs_bind(VFS_MAX_OPEN_FILES, O_RDONLY, &_test_bind_ops, NULL);
    TEST_ASSERT_EQUAL_INT(-ENFILE, fd);
    fd = vfs_bind((VFS_MAX_OPEN_FILES + 1), O_RDONLY, &_test_bind_ops, NULL);
    TEST_ASSERT_EQUAL_INT(-ENFILE, fd);
}

Test *tests_vfs_bind_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_vfs_bind),
        new_TestFixture(test_vfs_bind__leak_fds),
        new_TestFixture(test_vfs_bind__allocate_invalid_fd),
    };

    EMB_UNIT_TESTCALLER(vfs_bind_tests, NULL, NULL, fixtures);

    return (Test *)&vfs_bind_tests;
}

/** @} */
