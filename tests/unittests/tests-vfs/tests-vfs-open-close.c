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
 * @brief       Unittests for vfs_open, vfs_close
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

static void test_vfs_close__invalid_fd(void)
{
    int res = vfs_close(-1);
    TEST_ASSERT(res < 0);
}

static void test_vfs_open__notfound(void)
{
    int fd = vfs_open("/notfound/path", O_RDONLY, 0);
    TEST_ASSERT(fd < 0);
}

Test *tests_vfs_open_close_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_vfs_open__notfound),
        new_TestFixture(test_vfs_close__invalid_fd),
    };

    EMB_UNIT_TESTCALLER(vfs_open_close_tests, NULL, NULL, fixtures);

    return (Test *)&vfs_open_close_tests;
}
/** @} */
