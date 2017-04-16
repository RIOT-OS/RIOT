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
 * @brief       Unittest entry point for the VFS test group
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "embUnit/embUnit.h"

#include "tests-vfs.h"

Test *tests_vfs_bind_tests(void);
Test *tests_vfs_mount_constfs_tests(void);
Test *tests_vfs_open_close_tests(void);
Test *tests_vfs_normalize_path_tests(void);
Test *tests_vfs_null_file_ops_tests(void);
Test *tests_vfs_null_file_system_ops_tests(void);
Test *tests_vfs_null_dir_ops_tests(void);

void tests_vfs(void)
{
    TESTS_RUN(tests_vfs_open_close_tests());
    TESTS_RUN(tests_vfs_bind_tests());
    TESTS_RUN(tests_vfs_mount_constfs_tests());
    TESTS_RUN(tests_vfs_normalize_path_tests());
    TESTS_RUN(tests_vfs_null_file_ops_tests());
    TESTS_RUN(tests_vfs_null_file_system_ops_tests());
    TESTS_RUN(tests_vfs_null_dir_ops_tests());
}
/** @} */
