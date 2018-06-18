/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  sys_fs_tmpfs TempFS file system
 * @ingroup   sys_fs
 * @brief     Temporary file system
 *
 * This file system implementation allows implement a RAM only file system with
 * dynamic memory allocation.
 *
 * This file system can be used for temporary file-like storage, everything is
 * removed when unmounting. This uses dynamic memory allocation and default
 * @p malloc and @p free functions can be overwritten with @p TMPFS_MALLOC and
 * @p TMPFS_FREE defines.
 *
 * The file system is composed of a chained list of files. Each file is a chained
 * list of file buffer.
 *
 * @{
 * @file
 * @brief   TempFS public API
 * @author  Vincent Dupont <vincent@otakeys.com>
 */

#ifndef FS_TMPFS_H
#define FS_TMPFS_H

#include "clist.h"
#include "vfs.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TMPFS_MAX_FILES
#define TMPFS_MAX_FILES     (16)
#endif

#ifndef TMPFS_BUF_SIZE
#define TMPFS_BUF_SIZE      (128)
#endif

#ifndef TMPFS_MAX_BUF
#define TMPFS_MAX_BUF       (128)
#endif

/**
 * @brief   tmpfs file system superblock
 */
typedef struct {
    clist_node_t files; /**< list of files */
    mutex_t lock;       /**< lock */
} tmpfs_t;

/**
 * @brief   TempFS file system driver
 *
 * For use with vfs_mount
 */
extern const vfs_file_system_t tmpfs_file_system;

#ifdef __cplusplus
}
#endif

#endif /* FS_TMPFS_H */

/** @} */
