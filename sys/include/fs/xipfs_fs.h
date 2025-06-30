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
 * @author      Gregory Guche <gregory.guche@univ-lille.fr>
 */

#ifndef FS_XIPFS_FS_H
#define FS_XIPFS_FS_H

#include "vfs.h"
#include "mtd_flashpage.h"
#include "mutex.h"
#include "include/xipfs.h"

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
 * @param path The mount point of the file system in the VFS tree
 *
 * @param num The total number of pages allocated for the
 * partition
 */
#define XIPFS_NEW_PARTITION(id, path, num)            \
    FLASH_WRITABLE_INIT(xipfs_part_##id, num);        \
    static mutex_t execution_mutex_##id = MUTEX_INIT; \
    static mutex_t mutex_##id = MUTEX_INIT;           \
    static vfs_xipfs_mount_t id = {                   \
        .vfs_mp = {                                   \
            .fs = &xipfs_file_system,                 \
            .mount_point = path,                      \
        },                                            \
        .magic = XIPFS_MAGIC,                         \
        .mount_path = path,                           \
        .page_num = num,                              \
        .page_addr = (void *)xipfs_part_##id,         \
        .execution_mutex = &execution_mutex_##id,     \
        .mutex = &mutex_##id                          \
    }


/**
 * @brief xipfs descriptor for vfs integration
 *
 * execution_mutex is taken by xipfs_extended_driver_execv and by deletion operations.
 * mutex is taken by all operations except from xipfs_extended_driver_execv
 *
 * This two mutexes scheme :
 * - allows to call xipfs operation from within a code run by xipfs_extended_driver_execv,
 * - prevents from deleting files or directories when operations are performed on them.
 */
typedef struct vfs_xipfs_mount_s {
    vfs_mount_t vfs_mp;      /**< VFS mount point          */
    unsigned magic;          /**< xipfs magic number       */
    const char *mount_path;  /**< mount point path         */
    size_t page_num;         /**< number of flash page     */
    void *page_addr;         /**< first flash page address */
    mutex_t *execution_mutex;/**< For execution and deletion operations */
    mutex_t *mutex;          /**< For regular and deletion operations */
} vfs_xipfs_mount_t;

/** The xipfs vfs driver */
extern const vfs_file_system_t xipfs_file_system;


/**
 * @brief vfs_xipfs_mount_t constructor from a mtd_flashpage_t.
 *
 * This function constructs an xipfs mount point from a mtd_flashpage_t such as mtd_flash_aux_slot.
 * It allows to use XIPFS with the RIOT AUX slot mechanism.
 *
 * @param flashpage A valid mtd_flashpage_t pointer such as &mtd_flash_aux_slot.
 * @param path A valid mounting point path.
 * @param execution_mutex A valid mutex pointer used for execution and deletions operations.
 * @param mutex A valid mutex pointer used by all operations except from execution.
 * @param vfs_xipfs_mount A valid vfs xipfs mount point pointer.
 *
 * @retval -EINVAL when one of the parameters is NULL or when path is an empty string.
 * @retval 0 on success.
 *
 * @note Both mutexes will be initialized by this function.
 */
int xipfs_construct_from_flashpage(mtd_flashpage_t *flashpage, const char *path,
                                   mutex_t *execution_mutex, mutex_t *mutex,
                                   vfs_xipfs_mount_t *vfs_xipfs_mount);


/* Extended driver handling executables */

/**
 * @brief Executable or regular new file.
 *
 * Allows to create a regular or executable new file within XiPFS.
 *
 * @param full_path A full path such as `/dev/nvme0p0/my_new_file`
 *
 * @param size The file size in bytes.
 *
 * @param exec 0 for regular files, 1 for executable files.
 *
 * @retval Less than 0 on errors.
 * @retval 0 on success.
 */
int xipfs_extended_driver_new_file(const char *full_path, uint32_t size, uint32_t exec);

/**
 * @brief Executes an executable file with arguments.
 *
 * @param full_path A full path such as `/dev/nvme0p0/my_executable_file`
 *
 * @param argv Executable arguments. Cannot be NULL, at least argv[0] contains the executable filename.
 *
 * @retval Less than 0 on errors.
 * @retval 0 on success.
 */
int xipfs_extended_driver_execv(const char *full_path, char *const argv[]);

#ifdef __cplusplus
}
#endif

#endif /* FS_XIPFS_FS_H */
/** @} */
