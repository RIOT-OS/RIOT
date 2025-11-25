/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  sys_fs_devfs DevFS device file system
 * @ingroup   sys_fs
 * @brief     Dynamic device file system
 *
 * This file system implementation allows devices to register file names for
 * easier access to device drivers from shell commands etc.
 *
 * The idea is similar to the /dev directory on Unix.
 *
 * @{
 * @file
 * @brief   DevFS public API
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef FS_DEVFS_H
#define FS_DEVFS_H

#include "clist.h"
#include "vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DevFS node typedef
 */
typedef struct devfs devfs_t;

/**
 * @brief A device "file" consists of a file name and an opaque pointer to device driver private data
 *
 * The file system is implemented as a linked list.
 */
struct devfs {
    clist_node_t list_entry;    /**< List item entry */
    const char *path;           /**< File system relative path to this node */
    const vfs_file_ops_t *f_op; /**< Pointer to file operations table for this device */
    void *private_data;         /**< Pointer to device driver specific data */
};

/**
 * @brief DevFS file system driver
 *
 * For use with vfs_mount
 */
extern const vfs_file_system_t devfs_file_system;

/**
 * @brief Register a node in DevFS
 *
 * The node will immediately become available to @c vfs_open, if DevFS is already
 * mounted somewhere.
 *
 * If DevFS is not mounted, the node will be registered and will become
 * available to @c vfs_open when DevFS is mounted.
 *
 * @param[in]  node    DevFS node to register
 *
 * @return 0 on success
 * @return <0 on error
 */
int devfs_register(devfs_t *node);

/**
 * @brief Remove a registration from DevFS
 *
 * The node will no longer be available to @c vfs_open, but any already opened FDs
 * will remain open.
 *
 * @param[in]  node    DevFS node to unregister
 *
 * @return 0 on success
 * @return <0 on error
 */
int devfs_unregister(devfs_t *node);

#ifdef __cplusplus
}
#endif

#endif /* FS_DEVFS_H */

/** @} */
