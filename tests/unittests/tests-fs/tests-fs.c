/*
* Copyright (C) 2015 Lucas Jenß
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

#include <stdio.h>
#include <errno.h>

#include "embUnit.h"
#include "tests-fs.h"

#include "fs.h"


static void test_fs_mount_limit(void)
{
    char mount_point[FS_MOUNT_POINT_LEN];

    int i;
    for(i=0; i<FS_TABLE_SIZE; i++) {
        sprintf(mount_point, "/mnt/test/%d", i);
        int ret = fs_mount(mount_point, 0, FS_NATIVE);
        TEST_ASSERT_MESSAGE(0 == ret, "Mounting should succeed FS_TABLE_SIZE times");
    }

    sprintf(mount_point, "/mnt/test/%d", i);
    int ret = fs_mount(mount_point, 0, FS_NATIVE);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, ret);
}

Test *tests_fs_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_fs_mount_limit),
    };

    EMB_UNIT_TESTCALLER(fs_tests, NULL, NULL, fixtures);

    return (Test *)&fs_tests;
}

void tests_fs(void)
{
    TESTS_RUN(tests_fs_tests());
}
