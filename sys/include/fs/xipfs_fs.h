/*
 * Copyright (C) 2024 Université de Lille
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_xipfs  xipfs integration
 * @ingroup     pkg_xipfs
 * @brief       RIOT integration of xipfs
 *
 * @{
 *
 * @file
 * @brief       xipfs integration with vfs
 *
 * @author      Damien Amara <damien.amara@univ-lille.fr>
 */

#ifndef FS_XIPFS_FS_H
#define FS_XIPFS_FS_H

#include "vfs.h"
#include "xipfs_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def XIPFS_NEW_PARTITION
 *
 * @brief Allocate a new contiguous space aligned to a page in
 * the non-volatile addressable memory of the MCU to serve as a
 * partition for an xipfs file system
 *
 * @param id Identifier name for the mount point used by
 * functions that manipulate xipfs file systems
 *
 * @param mp The mount point of the file system in the VFS tree
 *
 * @param npage The total number of pages allocated for the
 * partition
 */
#define XIPFS_NEW_PARTITION(id, path, num)           \
    FLASH_WRITABLE_INIT(xipfs_part_##id, num);       \
    static vfs_xipfs_mount_t id = {                  \
        .vfs_mp = {                                  \
            .fs = &xipfs_file_system,                \
            .mount_point = path,                     \
        },                                           \
        .magic = XIPFS_MAGIC,                        \
        .mount_path = path,                          \
        .page_num = num,                             \
        .page_addr = (void *)xipfs_part_##id,        \
    }

/**
 * @brief xipfs descriptor for vfs integration
 */
typedef struct vfs_xipfs_mount_s {
    vfs_mount_t vfs_mp;      /**< VFS mount point         */
    unsigned magic;          /**< xipfs magic number      */
    const char *mount_path;  /**< mount point path        */
    size_t page_num;         /**< number of flash page    */
    void *page_addr;         /**< first flas page address */
} vfs_xipfs_mount_t;

/** The xipfs vfs driver */
extern const vfs_file_system_t xipfs_file_system;

#ifdef __cplusplus
}
#endif

#endif /* FS_XIPFS_FS_H */
/** @} */
