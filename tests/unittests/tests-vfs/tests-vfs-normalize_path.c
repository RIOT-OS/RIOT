/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for vfs_normalize_path
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "embUnit/embUnit.h"

#include "vfs.h"

#include "tests-vfs.h"

static void test_vfs_normalize_path__noop(void)
{
    static const char path[] = "/this/is/a/test";
    char buf[16];
    int res = vfs_normalize_path(buf, path, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(4, res);
    TEST_ASSERT_EQUAL_STRING((const char *)&path[0], (const char *)&buf[0]);
}

static void test_vfs_normalize_path__slashes(void)
{
    static const char path[] = "///////////////////////////////";
    static const char expected[] = "/";
    char buf[4];
    int res = vfs_normalize_path(buf, path, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_STRING((const char *)&expected[0], (const char *)&buf[0]);
}

static void test_vfs_normalize_path__dot(void)
{
    static const char path[] = "/abc/./def/././zxcv././.";
    static const char expected[] = "/abc/def/zxcv.";
    char buf[16];
    int res = vfs_normalize_path(buf, path, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(3, res);
    TEST_ASSERT_EQUAL_STRING((const char *)&expected[0], (const char *)&buf[0]);
}

static void test_vfs_normalize_path__reduce(void)
{
    static const char path[] = "/abc/../def";
    static const char expected[] = "/def";
    char buf[16];
    int res = vfs_normalize_path(buf, path, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_STRING((const char *)&expected[0], (const char *)&buf[0]);
}

static void test_vfs_normalize_path__trailing(void)
{
    static const char path[] = "/mydir/";
    static const char expected[] = "/mydir/";
    char buf[16];
    int res = vfs_normalize_path(buf, path, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_STRING((const char *)&expected[0], (const char *)&buf[0]);
}

static void test_vfs_normalize_path__outside(void)
{
    static const char path[] = "/somewhere/../..";
    static const char path2[] = "/../abdgh";
    char buf[16];
    int res = vfs_normalize_path(buf, path, sizeof(buf));
    TEST_ASSERT(res < 0);
    res = vfs_normalize_path(buf, path2, sizeof(buf));
    TEST_ASSERT(res < 0);
}

static void test_vfs_normalize_path__toolong(void)
{
    static const char path[] = "/abc";
    char buf[4];
    int res = vfs_normalize_path(buf, path, sizeof(buf));
    TEST_ASSERT(res < 0);
}

static void test_vfs_normalize_path__shorten(void)
{
#if 0
    /* Not supported by the current implementation */
    /* The current implementation needs enough buffer space to store the longest
     * prefix path before each ../ reduction */
    static const char path[] = "/qwerty/asdfghjkl/..";
    static const char expected[] = "/qwerty";
    char buf[8];
#endif
    static const char path[] = "/12345/6789/..";
    static const char expected[] = "/12345";
    char buf[12];
    int res = vfs_normalize_path(buf, path, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_STRING((const char *)&expected[0], (const char *)&buf[0]);
}

static void test_vfs_normalize_path__shorten_inplace(void)
{
    char path[] = "/qwerty/asdfghjkl/..";
    static const char expected[] = "/qwerty";
    int res = vfs_normalize_path(path, path, sizeof(path));
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_STRING((const char *)&expected[0], (const char *)&path[0]);
}

static void test_vfs_normalize_path__empty(void)
{
    char path[] = "";
    static const char expected[] = "";
    char buf[4];
    int res = vfs_normalize_path(buf, path, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING((const char *)&expected[0], (const char *)&path[0]);
}

Test *tests_vfs_normalize_path_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_vfs_normalize_path__noop),
        new_TestFixture(test_vfs_normalize_path__slashes),
        new_TestFixture(test_vfs_normalize_path__dot),
        new_TestFixture(test_vfs_normalize_path__reduce),
        new_TestFixture(test_vfs_normalize_path__trailing),
        new_TestFixture(test_vfs_normalize_path__outside),
        new_TestFixture(test_vfs_normalize_path__toolong),
        new_TestFixture(test_vfs_normalize_path__shorten),
        new_TestFixture(test_vfs_normalize_path__shorten_inplace),
        new_TestFixture(test_vfs_normalize_path__empty),
    };

    EMB_UNIT_TESTCALLER(vfs_normalize_path_tests, NULL, NULL, fixtures);

    return (Test *)&vfs_normalize_path_tests;
}
/** @} */
