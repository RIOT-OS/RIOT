/*
 * SPDX-FileCopyrightText: 2024-2026 Université de Lille
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
 * @brief Define an XiPFS mountpoint along with its associated metadata
 *
 * @param[in] id    Identifier name for the mount point used by
 *                  functions that manipulate xipfs file systems
 * @param[in] path  The mount point of the file system in the VFS tree
 * @param[in] data  Pointer to the start of the mount point in Flash
 *                  memory.
 * @param[in] data_bytesize Size of the mount point in Flash memory.
 *
 * @warning data_bytesize must be an integral multiple of XIPFS_NVM_PAGE_SIZE.
 *
 * @see XIPFS_NEW_PARTITION
 * @see XIPFS_START_PARTITION_INCLUSION / XIPFS_END_PARTITION_INCLUSION
 */
#define XIPFS_PARTITION_CORE(id, path, data, data_bytesize) \
    static mutex_t execution_mutex_##id = MUTEX_INIT;       \
    static mutex_t mutex_##id = MUTEX_INIT;                 \
    static vfs_xipfs_mount_t id = {                         \
        .vfs_mp = {                                         \
            .fs = &xipfs_file_system,                       \
            .mount_point = path,                            \
        },                                                  \
        .magic = XIPFS_MAGIC,                               \
        .mount_path = path,                                 \
        .page_num = data_bytesize / XIPFS_NVM_PAGE_SIZE,    \
        .page_addr = (void *)data,                          \
        .execution_mutex = &execution_mutex_##id,           \
        .mutex = &mutex_##id                                \
    }

/**
 * @brief Allocate a new contiguous space aligned to a page in
 *        the non-volatile addressable memory of the MCU to serve as a
 *        partition for an xipfs file system.
 *
 * @param[in] id    Identifier name for the mount point used by
 *                  functions that manipulate xipfs file systems.
 * @param[in] path  The mount point of the file system in the VFS tree.
 * @param[in] num   The total number of pages allocated for the partition.
 */
#define XIPFS_NEW_PARTITION(id, path, num)                                      \
    FLASH_WRITABLE_INIT(xipfs_part_##id, num);                                  \
    XIPFS_PARTITION_CORE(id, path, xipfs_part_##id, num * XIPFS_NVM_PAGE_SIZE)

/**
 * @brief Define the beginning of an included XiPFS mount point.
 *
 * @param[in] id    Identifier name for the mount point used by functions that
 *                  manipulate xipfs file systems
 *
 * @remark This macro should be used together with
 *         XIPFS_END_PARTITION_INCLUSION to define a mount point inclusion.
 *
 * @warning Contrary to most other macros, please take care NOT to end this one
 *          with a semicolon character ';'.
 */
#define XIPFS_START_PARTITION_INCLUSION(id)         \
    __attribute__((aligned(XIPFS_NVM_PAGE_SIZE)))   \
    __attribute__((section(".rodata.xipfs." #id)))

/**
 * @brief Define the end of an included XiPFS mount point.
 *
 * @param[in] id    Identifier name for the mount point used by functions that
 *                  manipulate xipfs file systems.
 * @param[in] path  The mount point of the file system in the VFS tree.
 * @param[in] data  Pointer to the start of the mount point in Flash memory.
 * @param[in] data_bytesize Size of the mount point in Flash memory.
 *
 * @remark This macro should be used together with
 *         XIPFS_START_PARTITION_INCLUSION to define a mount point inclusion.
 */
#define XIPFS_END_PARTITION_INCLUSION(id, path, data, data_bytesize) \
    XIPFS_PARTITION_CORE(id, path, data, data_bytesize)

/**
 * @brief xipfs descriptor for vfs integration.
 *
 * execution_mutex is taken by:
 * - xipfs_extended_driver_execv,
 * - xipfs_extended_driver_safe_execv,
 * - deletion operations.
 *
 * mutex is taken by all operations except from:
 * - xipfs_extended_driver_execv
 * - xipfs_extended_driver_safe_execv.
 *
 * This two mutexes scheme:
 * - allows to call xipfs operation from within a code run by
 *   xipfs_extended_driver_execv,
 * - prevents from deleting files or directories when operations are
 *   performed on them.
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
 * This function constructs an xipfs mount point from a mtd_flashpage_t such
 * as mtd_flash_aux_slot. It allows to use XIPFS with the RIOT AUX slot mechanism.
 *
 * @param[in] flashpage         A valid mtd_flashpage_t pointer such as
 *                              &mtd_flash_aux_slot.
 * @param[in] path              A valid mounting point path.
 * @param[in] execution_mutex   A valid mutex pointer used for execution and
 *                              deletions operations.
 * @param[in] mutex             A valid mutex pointer used by all operations
 *                              except from execution.
 * @param[in,out] vfs_xipfs_mount   A valid vfs xipfs mount point pointer.
 *
 * @retval  -EINVAL when one of the parameters is NULL or when path is an empty
 *           string.
 * @retval  0 on success.
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
 * @param[in] full_path A full path such as `/nvme0p0/my_new_file`
 * @param[in] size      The file size in bytes.
 * @param[in] exec      0 for regular files, 1 for executable files.
 *
 * @retval <0 on errors.
 * @retval 0 on success.
 */
int xipfs_extended_driver_new_file(const char *full_path, uint32_t size,
                                   uint32_t exec);

/**
 * @brief Executes an executable file with arguments.
 *
 * @param[in] full_path A full path such as `/nvme0p0/my_executable_file`.
 * @param[in] argv      Executable arguments.
 *                      Cannot be NULL, argv[0] contains the executable
 *                      filename.
 *
 * @retval <0 on errors.
 * @retval >=0 on success.
 */
int xipfs_extended_driver_execv(const char *full_path, char *const argv[]);

#ifdef XIPFS_ENABLE_SAFE_EXEC_SUPPORT

/**
 * @brief   Runs an executable file according to arguments with
 *          MPU-based memory isolation.
 *
 * @param[in] full_path A full path such as `/nvme0p0/my_executable_file`
 * @param[in] argv      Executable arguments.
 *                      Cannot be NULL, argv[0] contains the executable
 *                      filename.
 *
 * @retval <0 on errors.
 * @retval >=0 on success.
 */
int xipfs_extended_driver_safe_execv(const char *full_path,
                                     char *const argv[]);

#endif /* XIPFS_ENABLE_SAFE_EXEC_SUPPORT */

#ifdef __cplusplus
}
#endif

#endif /* FS_XIPFS_FS_H */
/** @} */
