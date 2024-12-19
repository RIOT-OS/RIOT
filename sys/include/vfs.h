/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  sys_vfs Virtual File System (VFS) layer
 * @ingroup   sys
 * @brief     Provides an interface for accessing files and directories from
 *            different devices and file systems
 *
 * This layer is modeled as a mix between POSIX syscalls (e.g. open) and the
 * Linux VFS layer implementation, with major reductions in the feature set, in
 * order to fit the resource constrained platforms that RIOT targets.
 *
 * The overall design goals are:
 * - Provide implementations for all newlib "file" syscalls
 * - Keep it simple, do not add every possible file operation from Linux VFS.
 * - Easy to map existing file system implementations for resource constrained systems onto the VFS layer API
 * - Avoid keeping a central `enum` of all file system drivers that has to be kept up to date with external packages etc.
 * - Use POSIX `<errno.h>` numbers as negative return codes for errors, avoid the global `errno` variable.
 * - Only absolute paths to files (no per-process working directory)
 * - No dynamic memory allocation
 *
 *
 * The API should be easy to understand for users who are familiar with the
 * POSIX file functions (open, close, read, write, fstat, lseek etc.)
 *
 * The VFS layer keeps track of mounted file systems and open files, the
 * `vfs_open` function searches the array of mounted file systems and dispatches
 * the call to the file system instance with the longest matching mount point prefix.
 * Subsequent calls to `vfs_read`, `vfs_write`, etc will do a look up in the
 * table of open files and dispatch the call to the correct file system driver
 * for handling.
 *
 * `vfs_mount` takes a string containing the mount point, a file system driver
 * specification (`struct file_system`), and an opaque pointer that only the FS
 * driver knows how to use, which can be used to keep driver parameters in order
 * to allow dynamic handling of multiple devices.
 *
 * @todo VFS layer reference counting and locking for open files and
 *       simultaneous access.
 *
 * @{
 * @file
 * @brief   VFS layer API declarations
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <sys/stat.h> /* for struct stat */
#include <sys/types.h> /* for off_t etc. */
#include <sys/statvfs.h> /* for struct statvfs */

#include "sched.h"
#include "clist.h"
#include "iolist.h"
#include "macros/utils.h"
#include "mtd.h"
#ifdef MODULE_NANOCOAP_FS
#include "net/sock/config.h"
#endif
#include "xfa.h"

#ifdef __cplusplus
extern "C" {
/* restrict is a C99 keyword, not valid in C++, but GCC and Clang have the
 * __restrict__ extension keyword which can be used instead */
#define restrict __restrict__
/* If the above is not supported by the compiler, you can replace it with an
 * empty definition instead: */
/* #define restrict */
#endif

/**
 * @brief   MAX7 Function to get the largest of 7 values
 */
#ifndef MAX7
#define MAX7(a, b, c, d, e, f, g) MAX(MAX(MAX(MAX(MAX((a), (b)), MAX((c), (d))), (e)), (f)), (g))
#endif

/**
 * @brief   VFS parameters for FAT
 * @{
 */
#ifdef MODULE_FATFS_VFS
#include "ffconf.h"

#  if FF_FS_TINY
#    define _FATFS_FILE_CACHE              (0)
#  else
#    define _FATFS_FILE_CACHE              FF_MAX_SS
#  endif

#  if FF_USE_FASTSEEK
#    if (__SIZEOF_POINTER__ == 8)
#      define _FATFS_FILE_SEEK_PTR         (8)
#    else
#      define _FATFS_FILE_SEEK_PTR         (4)
#    endif
#  else
#    define _FATFS_FILE_SEEK_PTR           (0)
#  endif

#  if FF_FS_EXFAT
#    define _FATFS_FILE_EXFAT              (48)
#    define _FATFS_DIR_EXFAT               (32)
#  else
#    define _FATFS_FILE_EXFAT              (0)
#    define _FATFS_DIR_EXFAT               (0)
#  endif

#  if FF_USE_LFN
#    if (__SIZEOF_POINTER__ == 8)
#      define _FATFS_DIR_LFN               (8)
#    else
#      define _FATFS_DIR_LFN               (4)
#    endif
#  else
#    define _FATFS_DIR_LFN                 (0)
#  endif

#  if (__SIZEOF_POINTER__ == 8)
#    define FATFS_VFS_DIR_BUFFER_SIZE      (64 + _FATFS_DIR_LFN + _FATFS_DIR_EXFAT)
#    define FATFS_VFS_FILE_BUFFER_SIZE     (64 + VFS_NAME_MAX + _FATFS_FILE_CACHE + \
                                           _FATFS_FILE_SEEK_PTR + _FATFS_FILE_EXFAT)
#  else
#    define FATFS_VFS_DIR_BUFFER_SIZE      (44 + _FATFS_DIR_LFN + _FATFS_DIR_EXFAT)
#    define FATFS_VFS_FILE_BUFFER_SIZE     (44 + VFS_NAME_MAX + _FATFS_FILE_CACHE + \
                                           _FATFS_FILE_SEEK_PTR + _FATFS_FILE_EXFAT)
#  endif
#else
#  define FATFS_VFS_DIR_BUFFER_SIZE        (1)
#  define FATFS_VFS_FILE_BUFFER_SIZE       (1)
#endif
/** @} */

/**
 * @brief   VFS parameters for littlefs
 * @{
 */
#ifdef MODULE_LITTLEFS
#  if (__SIZEOF_POINTER__ == 8)
#    define LITTLEFS_VFS_DIR_BUFFER_SIZE   (48)
#    define LITTLEFS_VFS_FILE_BUFFER_SIZE  (72)
#  else
#    define LITTLEFS_VFS_DIR_BUFFER_SIZE   (44)
#    define LITTLEFS_VFS_FILE_BUFFER_SIZE  (56)
#  endif
#else
#  define LITTLEFS_VFS_DIR_BUFFER_SIZE     (1)
#  define LITTLEFS_VFS_FILE_BUFFER_SIZE    (1)
#endif
/** @} */

/**
 * @brief   VFS parameters for littlefs2
 * @{
 */
#ifdef MODULE_LITTLEFS2
#  if (__SIZEOF_POINTER__ == 8)
#    define LITTLEFS2_VFS_DIR_BUFFER_SIZE  (56)
#    define LITTLEFS2_VFS_FILE_BUFFER_SIZE (104)
#  else
#    define LITTLEFS2_VFS_DIR_BUFFER_SIZE  (52)
#    define LITTLEFS2_VFS_FILE_BUFFER_SIZE (84)
#  endif
#else
#  define LITTLEFS2_VFS_DIR_BUFFER_SIZE    (1)
#  define LITTLEFS2_VFS_FILE_BUFFER_SIZE   (1)
#endif
/** @} */

/**
 * @brief   VFS parameters for spiffs
 * @{
 */
#ifdef MODULE_SPIFFS
#  define SPIFFS_VFS_DIR_BUFFER_SIZE       (12)
#  define SPIFFS_VFS_FILE_BUFFER_SIZE      (1)
#else
#  define SPIFFS_VFS_DIR_BUFFER_SIZE       (1)
#  define SPIFFS_VFS_FILE_BUFFER_SIZE      (1)
#endif
/** @} */

/**
 * @brief   VFS parameters for lwext4
 * @{
 */
#if defined(MODULE_LWEXT4) || DOXYGEN
#  define LWEXT4_VFS_DIR_BUFFER_SIZE       (308)   /**< sizeof(ext4_dir)  */
#  define LWEXT4_VFS_FILE_BUFFER_SIZE      (32)    /**< sizeof(ext4_file) */
#else
#  define LWEXT4_VFS_DIR_BUFFER_SIZE       (1)
#  define LWEXT4_VFS_FILE_BUFFER_SIZE      (1)
#endif
/** @} */

/**
 * @brief   VFS parameters for nanoCoAP FS
 * @{
 */
#if defined(MODULE_NANOCOAP_FS) || DOXYGEN
#  define NANOCOAP_FS_VFS_DIR_BUFFER_SIZE  \
          (4 + CONFIG_SOCK_URLPATH_MAXLEN) /**< sizeof(nanocoap_fs_dir_t) */
#  define NANOCOAP_FS_VFS_FILE_BUFFER_SIZE \
          (4 + CONFIG_SOCK_URLPATH_MAXLEN) /**< sizeof(nanocoap_fs_file_t) */
#else
#  define NANOCOAP_FS_VFS_DIR_BUFFER_SIZE   (1)
#  define NANOCOAP_FS_VFS_FILE_BUFFER_SIZE  (1)
#endif
/** @} */

/**
 * @brief   VFS parameters for xipfs
 * @{
 */
#ifdef MODULE_XIPFS
#  define XIPFS_VFS_DIR_BUFFER_SIZE        (68)
#  define XIPFS_VFS_FILE_BUFFER_SIZE       (12)
#else
#  define XIPFS_VFS_DIR_BUFFER_SIZE        (1)
#  define XIPFS_VFS_FILE_BUFFER_SIZE       (1)
#endif
/** @} */

#ifndef VFS_MAX_OPEN_FILES
/**
 * @brief Maximum number of simultaneous open files
 */
#define VFS_MAX_OPEN_FILES (16)
#endif

#ifndef VFS_DIR_BUFFER_SIZE
/**
 * @brief Size of buffer space in vfs_DIR
 *
 * This space is needed to avoid dynamic memory allocations for some file
 * systems where a single pointer is not enough space for its directory stream
 * state, e.g. SPIFFS.
 *
 * Guidelines:
 *
 * SPIFFS requires a sizeof(spiffs_DIR) (6-16 bytes, depending on target
 * platform and configuration) buffer for its DIR struct.
 *
 * @attention File system developers: If your file system requires a buffer for
 * DIR streams that is larger than a single pointer or @c int variable, ensure
 * that you have a preprocessor check in your header file (so that it is
 * impossible to attempt to mount the file system without running into a
 * compiler error):
 *
 * @attention @code
 * #if VFS_DIR_BUFFER_SIZE < 123
 * #error VFS_DIR_BUFFER_SIZE is too small, at least 123 bytes is required
 * #endif
 * @endcode
 *
 * @attention Put the check in the public header file (.h), do not put the check in the
 * implementation (.c) file.
 */
#define VFS_DIR_BUFFER_SIZE MAX7(FATFS_VFS_DIR_BUFFER_SIZE,       \
                                 LITTLEFS_VFS_DIR_BUFFER_SIZE,    \
                                 LITTLEFS2_VFS_DIR_BUFFER_SIZE,   \
                                 SPIFFS_VFS_DIR_BUFFER_SIZE,      \
                                 LWEXT4_VFS_DIR_BUFFER_SIZE,      \
                                 NANOCOAP_FS_VFS_DIR_BUFFER_SIZE, \
                                 XIPFS_VFS_DIR_BUFFER_SIZE        \
                                )
#endif

#ifndef VFS_FILE_BUFFER_SIZE
/**
 * @brief Size of buffer space in vfs_file_t
 *
 * Same as with VFS_DIR_BUFFER_SIZE some file systems (e.g. FatFs) require more space
 * to store data about their files.
 *
 *
 * Guidelines are same as with VFS_DIR_BUFFER_SIZE, so add the following snippet
 * to your fs header:
 *
 * @attention @code
 * #if VFS_FILE_BUFFER_SIZE < 123
 * #error VFS_FILE_BUFFER_SIZE is too small, at least 123 bytes is required
 * #endif
 * @endcode
 *
 * @attention Put the check in the public header file (.h), do not put the check in the
 * implementation (.c) file.
 */
#define VFS_FILE_BUFFER_SIZE MAX7(FATFS_VFS_FILE_BUFFER_SIZE,       \
                                  LITTLEFS_VFS_FILE_BUFFER_SIZE,    \
                                  LITTLEFS2_VFS_FILE_BUFFER_SIZE,   \
                                  SPIFFS_VFS_FILE_BUFFER_SIZE,      \
                                  LWEXT4_VFS_FILE_BUFFER_SIZE,      \
                                  NANOCOAP_FS_VFS_FILE_BUFFER_SIZE, \
                                  XIPFS_VFS_FILE_BUFFER_SIZE        \
                                 )
#endif

#ifndef VFS_NAME_MAX
/**
 * @brief Maximum length of the name in a @c vfs_dirent_t (not including terminating null)
 *
 * Maximum number of bytes in a filename (not including terminating null).
 *
 * Similar to the POSIX macro NAME_MAX
 */
#define VFS_NAME_MAX (31)
#endif

/**
 * @brief Used with vfs_bind to bind to any available fd number
 */
#define VFS_ANY_FD (-1)

/**
 * @brief Helper macro for VFS_AUTO_MOUNT
 *
 * @param[in] mtd   MTD device to use for filesystem
 */
#define VFS_MTD(mtd) { .dev = &mtd.base }

/**
 * @brief Define an automatic mountpoint
 *
 * @param[in] type  file system type
 *                  Can be littlefs, littlefs2, spiffs or fatfs
 *
 *                  Internally, file systems supporting this must name their
 *                  @ref vfs_file_system_t `${TYPE}_file_system`, and must use
 *                  a type named `${TYPE}_desc_t` for their private data
 * @param[in] mtd   file system backed device configuration
 * @param[in] path  Mount path
 * @param[in] idx   Unique index of the mount point
 */
#define VFS_AUTO_MOUNT(type, mtd, path, idx)        \
    static type ## _desc_t fs_desc_ ## idx = mtd;   \
                                                    \
    XFA(vfs_mount_t, vfs_mountpoints_xfa, 0)        \
    _mount_mtd_ ## idx = {                          \
        .fs = &type ## _file_system,                \
        .mount_point = path,                        \
        .private_data = &fs_desc_ ## idx,           \
    }

/* Forward declarations */
/**
 * @brief struct @c vfs_file_ops typedef
 */
typedef struct vfs_file_ops vfs_file_ops_t;

/**
 * @brief struct @c vfs_dir_ops typedef
 */
typedef struct vfs_dir_ops vfs_dir_ops_t;

/**
 * @brief struct @c vfs_file_system_ops typedef
 */
typedef struct vfs_file_system_ops vfs_file_system_ops_t;

/**
 * @brief struct @c vfs_mount_struct typedef
 */
/* not struct vfs_mount because of name collision with the function */
typedef struct vfs_mount_struct vfs_mount_t;

/**
 * @brief   MTD driver for VFS
 */
extern const vfs_file_ops_t mtd_vfs_ops;

/**
 * @brief   File system always wants the full VFS path
 */
#define VFS_FS_FLAG_WANT_ABS_PATH   (1 << 0)

/**
 * @brief A file system driver
 */
typedef struct {
    const vfs_file_ops_t *f_op;         /**< File operations table */
    const vfs_dir_ops_t *d_op;          /**< Directory operations table */
    const vfs_file_system_ops_t *fs_op; /**< File system operations table */
    const uint32_t flags;               /**< File system flags */
} vfs_file_system_t;

/**
 * @brief A mounted file system
 */
struct vfs_mount_struct {
    clist_node_t list_entry;     /**< List entry for the _vfs_mount_list list */
    const vfs_file_system_t *fs; /**< The file system driver for the mount point */
    const char *mount_point;     /**< Mount point, e.g. "/mnt/cdrom" */
    size_t mount_point_len;      /**< Length of mount_point string (set by vfs_mount) */
    uint16_t open_files;         /**< Number of currently open files and directories */
    void *private_data;          /**< File system driver private data, implementation defined */
};

/**
 * @brief Information about an open file
 *
 * Similar to, but not equal to, <tt>struct file</tt> in Linux
 */
typedef struct {
    const vfs_file_ops_t *f_op; /**< File operations table */
    vfs_mount_t *mp;            /**< Pointer to mount table entry */
    int flags;                  /**< File flags */
    off_t pos;                  /**< Current position in the file */
    kernel_pid_t pid;           /**< PID of the process that opened the file */
    union {
        void *ptr;              /**< pointer to private data */
        int value;              /**< alternatively, you can use private_data as an int */
        uint8_t buffer[VFS_FILE_BUFFER_SIZE]; /**< Buffer space, in case a single pointer is not enough */
    } private_data;             /**< File system driver private data, implementation defined */
} vfs_file_t;

/**
 * @brief Internal representation of a file system directory entry
 *
 * Used by opendir, readdir, closedir
 *
 * @attention This structure should be treated as an opaque blob and must not be
 * modified by user code. The contents should only be used by file system drivers.
 */
typedef struct {
    const vfs_dir_ops_t *d_op; /**< Directory operations table */
    vfs_mount_t *mp;           /**< Pointer to mount table entry */
    union {
        void *ptr;             /**< pointer to private data */
        int value;             /**< alternatively, you can use private_data as an int */
        uint8_t buffer[VFS_DIR_BUFFER_SIZE]; /**< Buffer space, in case a single pointer is not enough */
    } private_data;            /**< File system driver private data, implementation defined */
} vfs_DIR;

/**
 * @brief User facing directory entry
 *
 * Used to hold the output from readdir
 *
 * @note size, modification time, and other information is part of the file
 * status, not the directory entry.
 */
typedef struct {
    ino_t d_ino; /**< file serial number, unique for the file system ("inode" in Linux) */
    char  d_name[VFS_NAME_MAX + 1]; /**< file name, relative to its containing directory */
} vfs_dirent_t;

/**
 * @brief Operations on open files
 *
 * Similar, but not equal, to struct file_operations in Linux
 */
struct vfs_file_ops {
    /**
     * @brief Close an open file
     *
     * This function must perform any necessary clean ups and flush any internal
     * buffers in the file system driver.
     *
     * If an error occurs, the file will still be considered closed by the VFS
     * layer. Therefore, the proper clean up must still be performed by the file
     * system driver before returning any error code.
     *
     * @note This implementation does not consider @c -EINTR a special return code,
     * the file is still considered closed.
     *
     * @param[in]  filp     pointer to open file
     *
     * @return 0 on success
     * @return <0 on error, the file is considered closed anyway
     */
    int (*close) (vfs_file_t *filp);

    /**
     * @brief Query/set options on an open file
     *
     * @param[in]  filp     pointer to open file
     * @param[in]  cmd      fcntl command, see man 3p fcntl
     * @param[in]  arg      argument to fcntl command, see man 3p fcntl
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*fcntl) (vfs_file_t *filp, int cmd, int arg);

    /**
     * @brief Get status of an open file
     *
     * @param[in]  filp     pointer to open file
     * @param[out] buf      pointer to stat struct to fill
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*fstat) (vfs_file_t *filp, struct stat *buf);

    /**
     * @brief Seek to position in file
     *
     * @p whence determines the function of the seek and should be set to one of
     * the following values:
     *
     *  - @c SEEK_SET: Seek to absolute offset @p off
     *  - @c SEEK_CUR: Seek to current location + @p off
     *  - @c SEEK_END: Seek to end of file + @p off
     *
     * @param[in]  filp     pointer to open file
     * @param[in]  off      seek offset
     * @param[in]  whence   determines the seek method, see detailed description
     *
     * @return the new seek location in the file on success
     * @return <0 on error
     */
    off_t (*lseek) (vfs_file_t *filp, off_t off, int whence);

    /**
     * @brief Attempt to open a file in the file system at rel_path
     *
     * A file system driver should perform the necessary checks for file
     * existence etc in this function.
     *
     * The VFS layer will initialize the contents of @p *filp so that
     * @c filp->f_op points to the mounted file system's @c vfs_file_ops_t.
     * @c filp->private_data.ptr will be initialized to NULL, @c filp->pos will
     * be set to 0.
     *
     * @note @p name is an absolute path inside the file system, @p abs_path is
     * the path to the file in the VFS, example:
     * @p abs_path = "/mnt/hd/foo/bar", @p name = "/foo/bar"
     *
     * @note @p name and @p abs_path may point to different locations within the
     * same const char array and the strings may overlap
     *
     * @param[in]  filp     pointer to open file
     * @param[in]  name     null-terminated name of the file to open, relative to the file system root, including a leading slash
     * @param[in]  flags    flags for opening, see man 2 open, man 3p open
     * @param[in]  mode     mode for creating a new file, see man 2 open, man 3p open
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*open) (vfs_file_t *filp, const char *name, int flags, mode_t mode);

    /**
     * @brief Read bytes from an open file
     *
     * @param[in]  filp     pointer to open file
     * @param[in]  dest     pointer to destination buffer
     * @param[in]  nbytes   maximum number of bytes to read
     *
     * @return number of bytes read on success
     * @return <0 on error
     */
    ssize_t (*read) (vfs_file_t *filp, void *dest, size_t nbytes);

    /**
     * @brief Write bytes to an open file
     *
     * @param[in]  filp     pointer to open file
     * @param[in]  src      pointer to source buffer
     * @param[in]  nbytes   maximum number of bytes to write
     *
     * @return number of bytes written on success
     * @return <0 on error
     */
    ssize_t (*write) (vfs_file_t *filp, const void *src, size_t nbytes);

    /**
     * @brief Synchronize a file on storage
     *        Any pending writes are written out to storage.
     *
     * @param[in]  filp     pointer to open file
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*fsync) (vfs_file_t *filp);
};

/**
 * @brief Operations on open directories
 */
struct vfs_dir_ops {
    /**
     * @brief Open a directory for reading with readdir
     *
     * @param[in]  dirp     pointer to open directory
     * @param[in]  name     null-terminated name of the dir to open, relative to the file system root, including a leading slash
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*opendir) (vfs_DIR *dirp, const char *dirname);

    /**
     * @brief Read a single entry from the open directory dirp and advance the
     * read position by one
     *
     * @p entry will be populated with information about the next entry in the
     * directory stream @p dirp
     *
     * If @p entry was updated successfully, @c readdir shall return 1.
     *
     * If the end of stream was reached, @c readdir shall return 0 and @p entry
     * shall remain untouched.
     *
     * @param[in]  dirp     pointer to open directory
     * @param[out] entry    directory entry information
     *
     * @return 1 if @p entry was updated
     * @return 0 if @p dirp has reached the end of the directory index
     * @return <0 on error
     */
    int (*readdir) (vfs_DIR *dirp, vfs_dirent_t *entry);

    /**
     * @brief Close an open directory
     *
     * @param[in]  dirp     pointer to open directory
     *
     * @return 0 on success
     * @return <0 on error, the directory stream dirp should be considered invalid
     */
    int (*closedir) (vfs_DIR *dirp);
};

/**
 * @brief Operations on mounted file systems
 *
 * Similar, but not equal, to struct super_operations in Linux
 */
struct vfs_file_system_ops {
    /**
     * @brief Format the file system on the given mount point
     *
     * @param[in]   mountp  file system to format
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*format) (vfs_mount_t *mountp);

    /**
     * @brief Perform any extra processing needed after mounting a file system
     *
     * If this call returns an error, the whole vfs_mount call will signal a
     * failure.
     *
     * All fields of @p mountp will be initialized by vfs_mount beforehand,
     * @c private_data will be initialized to NULL.
     *
     * @param[in]  mountp  file system mount being mounted
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*mount) (vfs_mount_t *mountp);

    /**
     * @brief Perform the necessary clean up for unmounting a file system
     *
     * @param[in]  mountp  file system mount being unmounted
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*umount) (vfs_mount_t *mountp);

    /**
     * @brief Rename a file
     *
     * The file @p from_path will be renamed to @p to_path
     *
     * @note it is not possible to rename files across different file system
     *
     * @param[in]  mountp     file system mount to operate on
     * @param[in]  from_path  absolute path to existing file
     * @param[in]  to_path    absolute path to destination
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*rename) (vfs_mount_t *mountp, const char *from_path, const char *to_path);

    /**
     * @brief Unlink (delete) a file from the file system
     *
     * @param[in]  mountp  file system mount to operate on
     * @param[in]  name    name of the file to delete
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*unlink) (vfs_mount_t *mountp, const char *name);

    /**
     * @brief Create a directory on the file system
     *
     * @param[in]  mountp  file system mount to operate on
     * @param[in]  name    name of the directory to create
     * @param[in]  mode    file creation mode bits
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*mkdir) (vfs_mount_t *mountp, const char *name, mode_t mode);

    /**
     * @brief Remove a directory from the file system
     *
     * Only empty directories may be removed.
     *
     * @param[in]  mountp  file system mount to operate on
     * @param[in]  name    name of the directory to remove
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*rmdir) (vfs_mount_t *mountp, const char *name);

    /**
     * @brief Get file status
     *
     * @param[in]  mountp  file system mount to operate on
     * @param[in]  path    path to file being queried
     * @param[out] buf     pointer to stat struct to fill
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*stat) (vfs_mount_t *mountp, const char *restrict path, struct stat *restrict buf);

    /**
     * @brief Get file system status
     *
     * @p path is only passed for consistency against the POSIX statvfs function.
     * @c vfs_statvfs calls this function only when it has determined that
     * @p path belongs to this file system. @p path is a file system relative
     * path and does not necessarily name an existing file.
     *
     * @param[in]  mountp  file system mount to operate on
     * @param[in]  path    path to a file on the file system being queried
     * @param[out] buf     pointer to statvfs struct to fill
     *
     * @return 0 on success
     * @return <0 on error
     */
    int (*statvfs) (vfs_mount_t *mountp, const char *restrict path, struct statvfs *restrict buf);
};

/**
 * @brief   Allocate and bind file descriptors for  STDIN, STDERR, and STDOUT
 *
 * This function is meant to be called once during system initialization time.
 * It is typically called from the initialization of the selected STDIO
 * implementation.
 */
void vfs_bind_stdio(void);

/**
 * @brief Close an open file
 *
 * @param[in]  fd    fd number to close
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_close(int fd);

/**
 * @brief Query/set options on an open file
 *
 * @param[in]  fd    fd number to operate on
 * @param[in]  cmd   fcntl command, see man 3p fcntl
 * @param[in]  arg   argument to fcntl command, see man 3p fcntl
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_fcntl(int fd, int cmd, int arg);

/**
 * @brief Get status of an open file
 *
 * @param[in]  fd       fd number obtained from vfs_open
 * @param[out] buf      pointer to stat struct to fill
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_fstat(int fd, struct stat *buf);

/**
 * @brief Get file system status of the file system containing an open file
 *
 * @param[in]  fd       fd number obtained from vfs_open
 * @param[out] buf      pointer to statvfs struct to fill
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_fstatvfs(int fd, struct statvfs *buf);

/**
 * @brief Get file system status of the file system containing an open directory
 *
 * @param[in]  dirp     pointer to open directory
 * @param[out] buf      pointer to statvfs struct to fill
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_dstatvfs(vfs_DIR *dirp, struct statvfs *buf);

/**
 * @brief Seek to position in file
 *
 * @p whence determines the function of the seek and should be set to one of
 * the following values:
 *
 *  - @c SEEK_SET: Seek to absolute offset @p off
 *  - @c SEEK_CUR: Seek to current location + @p off
 *  - @c SEEK_END: Seek to end of file + @p off
 *
 * @param[in]  fd       fd number obtained from vfs_open
 * @param[in]  off      seek offset
 * @param[in]  whence   determines the seek method, see detailed description
 *
 * @return the new seek location in the file on success
 * @return <0 on error
 */
off_t vfs_lseek(int fd, off_t off, int whence);

/**
 * @brief Open a file
 *
 * @param[in]  name    file name to open
 * @param[in]  flags   flags for opening, see man 3p open
 * @param[in]  mode    file mode
 *
 * @return fd number on success (>= 0)
 * @return <0 on error
 */
int vfs_open(const char *name, int flags, mode_t mode);

/**
 * @brief Read bytes from an open file
 *
 * @param[in]  fd       fd number obtained from vfs_open
 * @param[out] dest     destination buffer to hold the file contents
 * @param[in]  count    maximum number of bytes to read
 *
 * @return number of bytes read on success
 * @return <0 on error
 *
 * For simple cases of only a single read from a file, the @ref
 * vfs_file_to_buffer function can be used.
 */
ssize_t vfs_read(int fd, void *dest, size_t count);

/**
 * @brief Read a line from an open text file
 *
 * Reads from a file until a `\r` or `\n` character is found.
 *
 * @param[in]  fd       fd number obtained from vfs_open
 * @param[out] dest     destination buffer to hold the line
 * @param[in]  count    maximum number of characters to read
 *
 * @return number of bytes read on success
 * @return <0 on error
 */
ssize_t vfs_readline(int fd, char *dest, size_t count);

/**
 * @brief Write bytes to an open file
 *
 * @param[in]  fd       fd number obtained from vfs_open
 * @param[in]  src      pointer to source buffer
 * @param[in]  count    maximum number of bytes to write
 *
 * @return number of bytes written on success
 * @return <0 on error
 *
 * For simple cases of only a single write to a file, the @ref
 * vfs_file_from_buffer function can be used.
 */
ssize_t vfs_write(int fd, const void *src, size_t count);

/**
 * @brief Write bytes from an iolist to an open file
 *
 * @param[in]  fd       fd number obtained from vfs_open
 * @param[in]  iolist   iolist to read from
 *
 * @return number of bytes written on success
 * @return <0 on error
 */
ssize_t vfs_write_iol(int fd, const iolist_t *iolist);

/**
 * @brief Synchronize a file on storage
 *        Any pending writes are written out to storage.
 *
 * @param[in]  fd       fd number obtained from vfs_open
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_fsync(int fd);

/**
 * @brief Open a directory for reading with readdir
 *
 * The data in @c *dirp will be initialized by @c vfs_opendir
 *
 * @param[out] dirp     pointer to directory stream struct for storing the state
 * @param[in]  dirname  null-terminated name of the dir to open, absolute file system path
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_opendir(vfs_DIR *dirp, const char *dirname);

/**
 * @brief Read a single entry from the open directory dirp and advance the
 * read position by one
 *
 * @p entry will be populated with information about the next entry in the
 * directory stream @p dirp
 *
 * @attention Calling vfs_readdir on an uninitialized @c vfs_DIR is forbidden
 * and may lead to file system corruption and random system failures.
 *
 * @param[in]  dirp     pointer to open directory
 * @param[out] entry    directory entry information
 *
 * @return 1 if @p entry was updated
 * @return 0 if @p dirp has reached the end of the directory index
 * @return <0 on error
 */
int vfs_readdir(vfs_DIR *dirp, vfs_dirent_t *entry);

/**
 * @brief Close an open directory
 *
 * @attention Calling vfs_closedir on an uninitialized @c vfs_DIR is forbidden
 * and may lead to file system corruption and random system failures.
 *
 * @param[in]  dirp     pointer to open directory
 *
 * @return 0 on success
 * @return <0 on error, the directory stream dirp should be considered invalid
 */
int vfs_closedir(vfs_DIR *dirp);

/**
 * @brief Format a file system
 *
 * @p mountp should have been populated in advance with a file system driver,
 * a mount point, and private_data (if the file system driver uses one).
 *
 * @pre @p mountp must not be mounted
 *
 * @param[in]  mountp   pointer to the mount structure of the filesystem to format
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_format(vfs_mount_t *mountp);

/**
 * @brief Format a file system
 *
 * The file system must not be mounted in order to be formatted.
 * Call @ref vfs_unmount_by_path first if necessary.
 *
 * @note This assumes mount points have been configured with @ref VFS_AUTO_MOUNT.
 *
 * @param[in]  path     Path of the pre-configured mount point
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_format_by_path(const char *path);

/**
 * @brief Mount a file system
 *
 * @p mountp should have been populated in advance with a file system driver,
 * a mount point, and private_data (if the file system driver uses one).
 *
 * @param[in]  mountp    pointer to the mount structure of the file system to mount
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_mount(vfs_mount_t *mountp);

/**
 * @brief Mount a file system with a pre-configured mount path
 *
 * @note This assumes mount points have been configured with @ref VFS_AUTO_MOUNT.
 *
 * @warning If the @ref pseudomodule_vfs_auto_format is used a format attempt will
 * be made if the mount fails.
 *
 * @param[in]  path     Path of the pre-configured mount point
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_mount_by_path(const char *path);

/**
 * @brief Unmount a file system with a pre-configured mount path
 *
 * @note This assumes mount points have been configured with @ref VFS_AUTO_MOUNT.
 *
 * @param[in]  path     Path of the pre-configured mount point
 * @param[in]  force    Unmount the filesystem even if there are still open files
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_unmount_by_path(const char *path, bool force);

/**
 * @brief Rename a file
 *
 * The file @p from_path will be renamed to @p to_path
 *
 * @note it is not possible to rename files across different file system
 *
 * @param[in]  from_path  absolute path to existing file
 * @param[in]  to_path    absolute path to destination
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_rename(const char *from_path, const char *to_path);

/**
 * @brief Unmount a mounted file system
 *
 * This will fail if there are any open files or directories on the mounted file system
 *
 * @param[in]  mountp    pointer to the mount structure of the file system to unmount
 * @param[in]  force    Unmount the filesystem even if there are still open files
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_umount(vfs_mount_t *mountp, bool force);

/**
 * @brief Unlink (delete) a file from a mounted file system
 *
 * @param[in]  name   name of file to delete
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_unlink(const char *name);

/**
 * @brief Create a directory on the file system
 *
 * @param[in]  name    name of the directory to create
 * @param[in]  mode    file creation mode bits
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_mkdir(const char *name, mode_t mode);

/**
 * @brief Remove a directory from the file system
 *
 * Only empty directories may be removed.
 *
 * @param[in]  name    name of the directory to remove
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_rmdir(const char *name);

/**
 * @brief Get file status
 *
 * @param[in]  path    path to file being queried
 * @param[out] buf     pointer to stat struct to fill
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_stat(const char *restrict path, struct stat *restrict buf);

/**
 * @brief Get file system status
 *
 * @p path can be any path that resolves to the file system being queried, it
 * does not have to be an existing file.
 *
 * @param[in]  path    path to a file on the file system being queried
 * @param[out] buf     pointer to statvfs struct to fill
 *
 * @return 0 on success
 * @return <0 on error
 */
int vfs_statvfs(const char *restrict path, struct statvfs *restrict buf);

/**
 * @brief Allocate a new file descriptor and give it file operations
 *
 * The new fd will be initialized with pointers to the given @p f_op file
 * operations table and @p private_data.
 *
 * This function can be used to give file-like functionality to devices, e.g. UART.
 *
 * @p private_data can be used for passing instance information to the file
 * operation handlers in @p f_op.
 *
 * @param[in]  fd            Desired fd number, use VFS_ANY_FD for any available fd
 * @param[in]  flags         not implemented yet
 * @param[in]  f_op          pointer to file operations table
 * @param[in]  private_data  opaque pointer to private data
 *
 * @return fd number on success (>= 0)
 * @return <0 on error
 */
int vfs_bind(int fd, int flags, const vfs_file_ops_t *f_op, void *private_data);

/**
 * @brief Normalize a path
 *
 * Normalizing a path means to remove all relative components ("..", ".") and
 * any double slashes.
 *
 * @note @p buf is allowed to overlap @p path if @p &buf[0] <= @p &path[0]
 *
 * @attention @p path must be an absolute path (starting with @c / )
 *
 * @param[out] buf        buffer to store normalized path
 * @param[in]  path       path to normalize
 * @param[in]  buflen     available space in @p buf
 *
 * @return number of path components in the normalized path on success
 * @return <0 on error
 */
int vfs_normalize_path(char *buf, const char *path, size_t buflen);

/**
 * @brief Iterate through all mounted file systems
 *
 * @attention Not thread safe! Do not mix calls to this function with other
 * calls which modify the mount table, such as vfs_mount() and vfs_umount()
 *
 * Set @p cur to @c NULL to start from the beginning
 *
 * @deprecated This will become an internal-only function after the 2022.04
 *   release, use @ref vfs_iterate_mount_dirs instead.
 *
 * @param[in]  cur  current iterator value
 *
 * @return     Pointer to next mounted file system in list after @p cur
 * @return     NULL if @p cur is the last element in the list
 */
const vfs_mount_t *vfs_iterate_mounts(const vfs_mount_t *cur);

/**
 * @brief Iterate through all mounted file systems by their root directories
 *
 * Unlike @ref vfs_iterate_mounts, this is thread safe, and allows thread safe
 * access to the mount point's stats through @ref vfs_dstatvfs. If mounts or
 * unmounts happen while iterating, this is guaranteed to report all file
 * systems that stayed mounted, and may report any that are transiently
 * mounted for up to as often as they are (re)mounted. Note that the volume
 * being reported can not be unmounted as @p dir is an open directory.
 *
 * Zero-initialize @p dir to start. As long as @c true is returned, @p dir is a
 * valid directory on which the user can call @ref vfs_readdir or @ref
 * vfs_dstatvfs (or even peek at its `.mp` if they dare ignore the warning in
 * @ref vfs_DIR).
 *
 * Users MUST NOT call @ref vfs_closedir if they intend to keep iterating, but
 * MUST call it when aborting iteration.
 *
 * Note that this requires all enumerated file systems to support the `opendir`
 * @ref vfs_dir_ops; any file system that does not support that will
 * prematurely terminate the mount point enumeration.
 *
 * @see @c sc_vfs.c (@c df command) for a usage example
 *
 * @param[inout]  dir     The root directory of the discovered mount point
 *
 * @return     @c true if another file system is mounted; @p dir then contains an open directory.
 * @return     @c false if the file system list is exhausted; @p dir is uninitialized then.
 */
bool vfs_iterate_mount_dirs(vfs_DIR *dir);

/**
 * @brief   Get information about the file for internal purposes
 *
 * @attention   Not thread safe! Do not modify any of the fields in the returned
 * struct.
 * @note        For file descriptor internal usage only.
 *
 * @internal
 * @param[in] fd    A file descriptor
 *
 * @return  Pointer to the file information struct if a file with @p fd exists.
 * @return  NULL, when no file with file descriptor @p fd exists.
 */
const vfs_file_t *vfs_file_get(int fd);

/** @brief  Implementation of `stat` using `fstat`
 *
 * This helper can be used by file system drivers that do not have any more
 * efficient implementation of `fs_op::stat` than opening the file and running
 * `f_op::fstat` on it.
 *
 * It can be set as `fs_op::stat` by a file system driver, provided it
 * implements `f_op::open` and `f_op::fstat` and `f_op::close`, and its `open`
 * accepts `NULL` in the `abs_path` position.
 */
int vfs_sysop_stat_from_fstat(vfs_mount_t *mountp,
        const char *restrict path,
        struct stat *restrict buf);

#ifdef __cplusplus
}
#endif

#endif /* VFS_H */

/** @} */
