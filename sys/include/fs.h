/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    fs  Filesystems
 * @brief       Filesystem implementations, inspired by FUSE
 * @ingroup     sys
 * @{
 *
 * @file        fs.h
 * @brief       Common functionalities for filesytems
 *
 * @author      Freie Universität Berlin
 * @author      Martine Lenders
 */
#ifndef __FS_H_
#define __FS_H_
#include <stdint.h>
#include <sys/stat.h>

/**
 * @brief Allowed length of a mount point
 */
#define FS_MOUNT_POINT_LEN  (16)

/**
 * @brief Size of the filesystem table.
 */
#define FS_TABLE_SIZE       (1)

/**
 * @brief   Number of byte fs_file_info_t can be extended by an implementation
 */
#define FS_FILE_INFO_EXT_SIZE (2)

typedef int fid_t;

typedef enum {
    FS_NONE = 0,
    FS_NATIVE = 1
} fs_type_t;

/**
 * @brief   Filesystem table.
 */
typedef struct {
    char mount_point[FS_MOUNT_POINT_LEN];   /**< mount point of the filesystem */
    size_t mp_len;                          /**< length of mount point name */
    int device;                             /**< device the filesystem is on */
    fs_type_t type;                         /**< type of the filesystem */
} fs_table_t;

/**
 * @brief   an extendable file information type
 */
typedef struct {
    int flags;                          /**< Flags provided to fs_op_table_t::open()
                                             fs_op_table_t::release() */
    uint8_t ext[FS_FILE_INFO_EXT_SIZE]; /**< Filesystem specific extensions */
} fs_file_info_t;

/**
 * @brief   Operation type for fs_op_table_t::flock()
 */
typedef enum {
    FS_LOCK_SH = 0, /**< Place a shared lock. More then one thread may hold a
                         shared lock for a given file at a given time */
    FS_LOCK_EX,     /**< Place an exclusive lock. Only one thread may hold an
                         exclusive lock for a given file at given time */
    FS_LOCK_UN     /**< Remove an existing lock held by this process */
} fs_lock_op_t;

/**
 * @brief   Table of operations for a filesystem
 */
typedef struct {
    /**
     * @brief   Determines accessibility of a file.
     *
     * @param[in] path  Path to a file
     * @param[in] amode Accessibility flags to check
     *                  (bitwise-inclusive OR of R_OK, W_OK, or X_OK or existence
     *                  check with F_OK)
     *
     * @return 0 if successful
     * @return -EACCESS, if the requested acces would be denied to the file, or
     *                   search permission is denied for one of the directories
     *                   in the path prefix of *path*
     * @return -ELOOP, if too many symbolic links were encountered in resolving
     *                 *path*
     * @return -ENAMETOOLONG, if *path* is too long
     * @return -ENOENT, if a component of *path* does not exist or is a
     *                  dangling symbolic link
     * @return -ENOTDIR, if a component of used as a directory in *path* is not,
     *                   in fact, a directory
     * @return -EROFS, if write perission was requested for a file on a
     *                 read-only filesystem
     * @return -EINVAL, if *amode* was incorrectly specified
     * @return -EIO, if an I/O error occured
     * @return -ETXTBSY, if write access was requested to an executable which
     *                   is being executed
     */
    int (*access)(const char *path, int amode);

    /**
     * @brief  Changes mode of a file.
     *
     * On error there are no changes made to the file.
     OA*
     * @param[in] path  Path to a file
     * @param[in] mode  Mode to set
     *
     * @return 0 if successful
     * @return -EACCES, if search permission is denied on a component of the
     *                  path prefix of *path*
     * @return -EFAULT, if *path* points outside your accessible address space
     * @return -EIO, if an I/O error occured.
     * @return -ELOOP, if too many symbolic links were encountered in resolving
     *                 *path*
     * @return -ENAMETOOLONG, if *path* is too long
     * @return -ENOENT, if file referred to by *path* does not exist
     * @return -ENOTDIR, if a component used as a directory in *path* is not,
     *                   in fact, a directory
     */
    int (*chmod)(const char *path, mode_t mode);

    /**
     * @brief  Creates file with mode.
     *
     * @param[in] path  Path to the file
     * @param[in] mode  Mode to set
     * @param[out] info Information about the file. Can be set by create.
     *
     * @return 0, on success
     * @return -EACCES, if the parent directory does not allow write permission
     *                  or one of the directories in the path prefix of *path*
     *                  did not allow search permission
     * @return -EDQUOT, if the user's quota of disk blocks or inodes on the
     *                  filesystem has been exhausted
     * @return -EEXIST, if *path* already exists. This includes the case where
     *                  *path* is a symbolic link, dangling or not
     * @return -ELOOP, if too many symbolic links were encountered in resolving
     *                 *path*
     * @return -ENAMETOOLONG, if *path* was too long
     * @return -ENOENT, if a directory componenent in *path* does not exist or
     *                  is a dangling symbolic link
     * @return -ENOSPC, if the device containing *path* has no room for a new
     *                  node
     * @return -ENOTDIR, if a component used as a directory in *path* is not,
     *                   in fact, a directory
     * @return -EPERM, if *mode* requested creation of something other than
     *                 a regular file, or FIFO (named pipe); also returned
     *                 if the filesystem does not support type of node
     *                 requested
     * @return -EROFS, if filesystem is read-only.
     */
    int (*create)(const char *path, mode_t mode, fs_file_info_t *info);

    /**
     * @brief   Called on filesystem exit.
     */
    void (*destroy)(void);

    /**
     * @brief   Executes file at given path.
     *
     * @param[in] path  Path to executable file
     * @param[in] envp  NULL terminated array of environment variables in
     *                  the format "key=value"
     * @param[in] argv  NULL terminated array of arguments for the executable
     * @param[out] out  Return value of executable
     *
     * @return 0, on success
     * @return TODO, error codes
     */
    int (*execve)(const char *path, char *envp[], char *argv[], int *out);

    /**
     * @brief   Allocate requested space.
     *
     * If this function returns success then subsequent writes to the specified
     * range shall not fail due to the lack of free space on the file system
     * storage media.
     *
     * @param[in] path      Path to file.
     * @param[in] offset    Starting point for allocated region
     * @param[in] length    Size of allocated region
     * @param[in,out] info  File information handed by filesystem API
     *
     * @return 0, on success
     * @return -EBADF, if *path* is not a valid file or not open for writing
     * @return -EFBIG, if *offset* + *length* exeeds the maximum file size
     * @return -EINTR, if a signal was cought during execution
     * @return -EINVAL, if *offset* or *length* were less than or equal to 0.
     * @return -EIO, if an I/O error occured while reading from or writing to
     *               a filestem
     * @return -ENOSPC, if the is not enough space left on the device
     *                  containing the file referred to by *path*
     * @return -EOPNOSUPP, if the filesystem containing the file referred to
     *                     by *path* does not support this operation
     * @return -EPERM, if the file referred to by *path* is marked immutable
     * @return -ESPIPE, if *path* refers to a pipe or FIFO
     */
    int (*fallocate)(const char *path, off_t offset, off_t length,
                     fs_file_info_t *info);

    /**
     * @brief apply or remove an advisory lock on an open file
     *
     * @param[in] path      Path to file
     * @param[in] op        Locking operation to perform
     *                      (see fs_lock_op_t)
     * @param[in,out] info  File information handed by filesystem API
     *
     * @return 0, on success
     * @return TODO, error codes
     */
    int (*flock)(const char *path, fs_lock_op_t op, fs_file_info_t *info);

    /**
     * @brief Get attributes from an open file.
     *
     * This function is called instead of fs_op_table_t::getattr() if file
     * information is available.
     *
     * @return 0, on success.
     * @return TODO: error codes
     */
    int (*fstat)(const char *path, struct stat *stat, fs_file_info_t *info);

    /**
     * Synchronize file contents
     *
     * @param[in] path      Path to file
     * @param[in,out] info  File information handed by filesystem API
     *
     * @return 0 on success,
     * @return TODO error codes
     */
    int (*fsync)(const char *, fs_file_info_t *info);

    /**
     * Synchronize directory contents
     *
     * @param[in] path      Path to directory
     * @param[in,out] info  File information handed by filesystem API
     *
     * @return 0 on success,
     * @return TODO error codes
     */
    int (*fsyncdir)(const char *, fs_file_info_t *);

    /**
     * Initializes the filesystem
     */
    void (*init)(void);

    /**
     * Create a hard link to a file
     *
     * @param[in] src_path  Path name of source file
     * @param[in] tgt_path  Path name of the link to create
     *
     * @return 0 on success,
     * @return TODO error codes
     */
    int (*link)(const char *src_path, const char *tgt_path);

    /**
     * Crete a new directory
     *
     * @param[in] path  Path to the new directory
     * @param[in] mode  Mode to set the new directory to
     *
     * @return 0 on success,
     * @return TODO error codes
     */
    int (*mkdir)(const char *path, mode_t mode);

    /**
     * @brief Creates file node (reqular file, character device, block device,
     *        fifo, ...)
     *
     * If file system implements fs_op_table_t::create() this will be used
     * instead for reqular files
     *
     * @param[in] path  Path to the new file node
     * @param[in] mode  Mode to set the new file node to
     * @param[in] rdev  Device number (if the call creates a device)
     *
     * @return 0 on success,
     * @return TODO error codes
     */
    int (*mknod)(const char *path, mode_t mode, dev_t rdev);

    /**
     * @brief   Opens a file.
     *
     * Flags can be passed via *info*::flags
     *
     * @param[in] path      Path to the file.
     * @param[in,out] info  File information handed by filesystem API
     *
     * @return positive file id on success,
     * @return TODO error codes
     */
    fid_t (*open)(const char *path, fs_file_info_t *info);

    /**
     * @brief   Opens a directory.
     *
     * @param[in] path      Path to the directory.
     * @param[in,out] info  File information handed by the filesystem API.
     *
     * @return positive file id on success,
     * @return TODO error codes
     */
    fid_t (*opendir)(const char *path, fs_file_info_t *info);

    /**
     * @brief   Read from a file
     *
     * @param[in] path      Path to file.
     * @param[out] buf      Buffer to be filled.
     * @param[in] size      Size of *buf*.
     * @param[in] offset    Offset in *buf*.
     * @param[in,out] info  File information handed by the filesystem API.
     */
    int (*read)(const char *path, char *buf, size_t size, off_t offset,
                fs_file_info_t *info);

    int (*readdir)(const char *path, void *); // TODO

    /**
     * TODO
    int (*readlink)(const char *, char *, size_t);
    int (*release)(const char *);
    int (*releasedir)(const char *);
    int (*rename)(const char *, const char *);
    int (*rmdir)(const char *);
    int (*stat)(const char*, struct stat *);
    int (*statfs)(const char *, struct statvfs *);
    int (*symlink)(const char *, const char *);
    int (*truncate)(const char *, off_t);
    int (*unlink)(const char *);
    int (*utimens)(const char *, const struct timespec tv[2]);
    int (*write)(const char *, const char *, size_t, off_t);
    int (*write_buf)(const char *, off_t off);
     */
} fs_op_table_t;


/**
 * @brief      Mounts the given |device| at |mount_point|.
 * @param[in]  mount_point Path where the device should be mounted
 * @param[in]  device      Device to be mounted
 *                         TODO: What kind of value is this
 * @param[in]  type        The desired file system
 * @return 0, on success
 * @return -ENAMETOOLONG, if the given mount point exceeds the maximum length
 * @return -ENOMEM, if more than FS_TABLE_SIZE filesystems are mounted
 */
int fs_mount(const char *mount_point, int device, fs_type_t type);

/**
 * @brief   Unmounts the filesystem at the given path OR unmounts the given device
 * @param  mount_point Path to be unmounted
 * @param  device      Device to be unmounted
 *                     TODO: What kind of value is this
 * @return 0, on success
 * @return ENOENT, TODO
 * @return EINVAL, TODO
 */
int fs_unmount(const char *mount_point, int device);

/**
 * @brief   The operation table for various file systems
 * @internal
 *
 * For internal use only
 */
extern const fs_op_table_t fs_op_table[1];

/**
 * @brief   Searches a filesystem table entry based on the name
 *          of the path
 *
 * @internal
 *
 * For internal use only.
 *
 * @param[in] pathname  The path to search.
 *
 * @return  Filesystem table entry for the filesystem the file referenced by
 *          pathname is in. NULL if no entry is found.
 */
static fs_table_t *fs_search_entry(const char *pathname);

#endif /* __FS_H_ */
