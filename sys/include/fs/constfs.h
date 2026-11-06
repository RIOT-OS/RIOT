/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup  sys_fs_constfs ConstFS static file system
 * @ingroup   sys_fs
 * @brief     Constant file system resident in arrays
 *
 * This is an example of how to implement a simple file system driver for the
 * RIOT VFS layer. The implementation uses an array of @c constfs_file_t objects
 * as its storage back-end.
 *
 * @{
 * @file
 * @brief   ConstFS public API
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stddef.h>
#include <stdint.h>

#include "vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A file in ConstFS (file name + contents)
 */
typedef struct {
    const char *path;  /**< file system relative path to file */
    const size_t size; /**< length of @c data */
    const void *data;  /**< pointer to file contents */
} constfs_file_t;

/**
 * @brief ConstFS file system superblock
 */
typedef struct {
    const size_t nfiles; /**< Number of files */
    const constfs_file_t *files; /**< Files array */
} constfs_t;

/**
 * @brief ConstFS file system driver
 *
 * For use with vfs_mount
 */
extern const vfs_file_system_t constfs_file_system;

#ifdef __cplusplus
}
#endif

/** @} */
