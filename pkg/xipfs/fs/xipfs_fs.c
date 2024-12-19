/*
 * Copyright (C) 2024 Université de Lille
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_fs_xipfs
 * @{
 *
 * @file
 * @brief       xipfs integration with vfs
 *
 * @author      Damien Amara <damien.amara@univ-lille.fr>
 *
 * @}
 */

/*
 * The following define is required in order to use strnlen(3)
 * since glibc 2.10. Refer to the SYNOPSIS section of the
 * strnlen(3) manual and the feature_test_macros(7) manual for
 * more information
 */
#define _POSIX_C_SOURCE 200809L

/*
 * libc includes
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <mutex.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

/*
 * RIOT includes
 */
#define ENABLE_DEBUG 0
#include "debug.h"
#include "fs/xipfs_fs.h"
#include "periph/flashpage.h"

/*
 * xipfs includes
 */
#include "include/buffer.h"
#include "include/errno.h"
#include "include/file.h"
#include "include/flash.h"
#include "include/fs.h"
#include "include/path.h"
#include "include/xipfs.h"

/*
 * The eXecute In Place File System only makes sense if the
 * non-volatile memory of the target MCU is addressable
 */
#ifndef MODULE_PERIPH_FLASHPAGE_IN_ADDRESS_SPACE
#error "sys/fs/xipfs: the target MCU has no addressable NVM"
#endif /* !MODULE_PERIPH_FLASHPAGE_IN_ADDRESS_SPACE */

/*
 * Macro definitions
 */

/**
 * @internal
 *
 * @def UNUSED
 *
 * @brief The most versatile macro for disabling warnings
 * related to unused variables
 *
 * @param x The unused variable name
 */
#define UNUSED(x) ((void)(x))

/*
 * Global variables
 */

/**
 * @internal
 *
 * @brief xipfs global lock
 */
static mutex_t xipfs_mutex = MUTEX_INIT;

/*
 * Helper functions
 */

/**
 * @internal
 *
 * @pre dir must be a pointer that references an accessible
 * memory region
 *
 * @pre path must be a pointer that references a path which is
 * accessible, null-terminated, starts with a slash, normalized,
 * and shorter than XIPFS_PATH_MAX
 *
 * @brief Copy the directory name component of path, including
 * the final slash, into the memory region pointed to by dir
 *
 * @param dir A pointer to a memory region that respects the
 * preconditions for storing the directory name component
 *
 * @param path A pointer to a path that respects the
 * preconditions
 */
static void dirname(char *dir, const char *path)
{
    const char *end;
    size_t len, i;

    assert(dir != NULL);
    assert(path != NULL);
    assert(path[0] == '/');

    if (path[1] == '\0') {
        dir[0] = '/';
        dir[1] = '\0';
        return;
    }

    len = strnlen(path, XIPFS_PATH_MAX);
    assert(len < XIPFS_PATH_MAX);
    end = path + len - 1;

    if (*end == '/') {
        /* skip the trailing slash if the
         * path ends with one */
        end--;
    }

    while (end > path && *end != '/') {
        /* skip all characters that are not
         * slashes */
        end--;
    }

    if (end != path) {
        for (i = 0; path + i <= end; i++) {
            /* copy the characters of the directory
             * name component until end */
            dir[i] = path[i];
        }
        dir[i] = '\0';
    } else {
        /* no slashes found, except for the root */
        dir[0] = '/';
        dir[1] = '\0';
    }
}

/**
 * @internal
 *
 * @warning This function provides a workaround for xipfs-
 * specific functions that need to retrieve the xipfs mount
 * point structure directly, bypassing the VFS layer, as these
 * functions are not available in the VFS abstraction
 *
 * @brief Retrieves the xipfs mount point structure from a
 * specified path within the mount point
 *
 * @param path A path within the mount point for retrieving the
 * corresponding xipfs mount point structure
 *
 * @param mp A pointer to an accessible memory region for
 * storing the xipfs mount point structure
 *
 * @return Returns zero if the function succeeds or a negative
 * value otherwise
 */
static int get_xipfs_mp(const char *path, xipfs_mount_t *xipfs_mp)
{
    char dir[XIPFS_PATH_MAX];
    size_t count, len;
    int fd, ret;

    if (path[0] != '/') {
        return -EINVAL;
    }
    len = strnlen(path, XIPFS_PATH_MAX);
    if (len == XIPFS_PATH_MAX) {
        return -ENAMETOOLONG;
    }
    dirname(dir, path);

    if (len + strlen(".xipfs_infos") + 1 > XIPFS_PATH_MAX) {
        return -ENAMETOOLONG;
    }
    (void)strcat(dir, ".xipfs_infos");

    if ((ret = vfs_open(dir, O_RDONLY, 0)) < 0) {
        /* not a xipfs mount point */
        return ret;
    }
    fd = ret;

    count = 0;
    while (count < sizeof(*xipfs_mp)) {
        ret = vfs_read(fd, xipfs_mp, sizeof(*xipfs_mp));
        if (ret < 0) {
            /* error */
            return ret;
        }
        if (ret == 0) {
            /* EOF */
            break;
        }
        count += ret;
    }
    assert(count == sizeof(*xipfs_mp));

    (void)vfs_close(fd);

    return 0;
}

/**
 * @internal
 *
 * @pre full_path must be a pointer that references a path which
 * is accessible, null-terminated, starts with a slash,
 * normalized, and shorter than XIPFS_PATH_MAX
 *
 * @brief Returns a pointer to the first character of the
 * relative path derived from the absolute path retrieved from
 * the vfs_mp mount point structure
 *
 * @param vfs_mp A pointer to a memory region containing an
 * accessible VFS mount point structure
 *
 * @param full_path A pointer to a path that respects the
 * preconditions
 */
static const char *get_rel_path(xipfs_mount_t *mp,
                                const char *full_path)
{
    const char *p1, *p2;

    assert(mp != NULL);
    assert(full_path != NULL);

    p1 = mp->mount_path;
    p2 = full_path;

    while (*p1 != '\0') {
        if (*p1++ != *p2++) {
            return NULL;
        }
    }

    return p2;
}

static inline xipfs_mount_t *_get_xipfs_mount_t(vfs_mount_t *vfs_mp)
{
    vfs_xipfs_mount_t *vfs_xipfs_mp;
    xipfs_mount_t *xipfs_mp;

    vfs_xipfs_mp = (vfs_xipfs_mount_t *)(uintptr_t)vfs_mp;
    xipfs_mp = (xipfs_mount_t *)(uintptr_t)&vfs_xipfs_mp->magic;

    return xipfs_mp;
}

/*
 * Operations on open files
 */

static int _close(vfs_file_t *filp)
{
    xipfs_file_desc_t *descp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(filp->mp);
    descp = filp->private_data.ptr;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_close(mp, descp);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _fstat(vfs_file_t *filp, struct stat *buf)
{
    xipfs_file_desc_t *descp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(filp->mp);
    descp = filp->private_data.ptr;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_fstat(mp, descp, buf);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static off_t _lseek(vfs_file_t *filp, off_t off, int whence)
{
    xipfs_file_desc_t *descp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(filp->mp);
    descp = filp->private_data.ptr;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_lseek(mp, descp, off, whence);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _open(vfs_file_t *filp, const char *name, int flags,
                 mode_t mode)
{
    xipfs_file_desc_t *descp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(filp->mp);
    descp = filp->private_data.ptr;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_open(mp, descp, name, flags, mode);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static ssize_t _read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    xipfs_file_desc_t *descp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(filp->mp);
    descp = filp->private_data.ptr;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_read(mp, descp, dest, nbytes);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static ssize_t _write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    xipfs_file_desc_t *descp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(filp->mp);
    descp = filp->private_data.ptr;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_write(mp, descp, src, nbytes);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _fsync(vfs_file_t *filp)
{
    xipfs_file_desc_t *descp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(filp->mp);
    descp = filp->private_data.ptr;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_fsync(mp, descp, filp->pos);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

/*
 * Operations on open directories
 */

static int _opendir(vfs_DIR *dirp, const char *dirname)
{
    xipfs_dir_desc_t *descp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(dirp->mp);
    descp = dirp->private_data.ptr;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_opendir(mp, descp, dirname);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    xipfs_dir_desc_t *descp;
    xipfs_dirent_t *direntp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(dirp->mp);
    descp = dirp->private_data.ptr;
    direntp = (xipfs_dirent_t *)(uintptr_t)entry->d_name;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_readdir(mp, descp, direntp);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _closedir(vfs_DIR *dirp)
{
    xipfs_dir_desc_t *descp;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(dirp->mp);
    descp = dirp->private_data.ptr;

    mutex_lock(&xipfs_mutex);
    ret = xipfs_closedir(mp, descp);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

/*
 * Operations on mounted file systems
 */

static int _format(vfs_mount_t *vfs_mp)
{
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(vfs_mp);

    mutex_lock(&xipfs_mutex);
    ret = xipfs_format(mp);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _mount(vfs_mount_t *vfs_mp)
{
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(vfs_mp);

    mutex_lock(&xipfs_mutex);
    ret = xipfs_mount(mp);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _umount(vfs_mount_t *vfs_mp)
{
    UNUSED(vfs_mp);

    /* nothing to do */

    return 0;
}

static int _unlink(vfs_mount_t *vfs_mp, const char *name)
{
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(vfs_mp);

    mutex_lock(&xipfs_mutex);
    ret = xipfs_unlink(mp, name);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _mkdir(vfs_mount_t *vfs_mp, const char *name, mode_t mode)
{
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(vfs_mp);

    mutex_lock(&xipfs_mutex);
    ret = xipfs_mkdir(mp, name, mode);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _rmdir(vfs_mount_t *vfs_mp, const char *name)
{
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(vfs_mp);

    mutex_lock(&xipfs_mutex);
    ret = xipfs_rmdir(mp, name);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _rename(vfs_mount_t *vfs_mp, const char *from_path,
                   const char *to_path)
{
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(vfs_mp);

    mutex_lock(&xipfs_mutex);
    ret = xipfs_rename(mp, from_path, to_path);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _stat(vfs_mount_t *vfs_mp, const char *restrict path,
                 struct stat *restrict buf)
{
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(vfs_mp);

    mutex_lock(&xipfs_mutex);
    ret = xipfs_stat(mp, path, buf);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

static int _statvfs(vfs_mount_t *vfs_mp, const char *restrict path,
                    struct statvfs *restrict buf)
{
    struct xipfs_statvfs xipfs_buf;
    xipfs_mount_t *mp;
    int ret;

    mp = _get_xipfs_mount_t(vfs_mp);

    mutex_lock(&xipfs_mutex);
    ret = xipfs_statvfs(mp, path, &xipfs_buf);
    mutex_unlock(&xipfs_mutex);

    (void)memset(buf, 0, sizeof(*buf));
    buf->f_bsize   = xipfs_buf.f_bsize;
    buf->f_frsize  = xipfs_buf.f_frsize;
    buf->f_blocks  = xipfs_buf.f_blocks;
    buf->f_bfree   = xipfs_buf.f_bfree;
    buf->f_bavail  = xipfs_buf.f_bavail;
    buf->f_flag    = xipfs_buf.f_flag;
    buf->f_namemax = xipfs_buf.f_namemax;

    return ret;
}

/*
 * xipfs-specific functions
 */

int _new_file(const char *full_path, uint32_t size, uint32_t exec)
{
    xipfs_mount_t mp;
    const char *path;
    int ret;

    if (full_path == NULL) {
        return -EFAULT;
    }
    if ((ret = get_xipfs_mp(full_path, &mp)) < 0) {
        return ret;
    }
    if ((path = get_rel_path(&mp, full_path)) == NULL) {
        return -EIO;
    }

    mutex_lock(&xipfs_mutex);
    ret = xipfs_new_file(&mp, path, size, exec);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

int _execv(const char *full_path, char *const argv[])
{
    xipfs_mount_t mp;
    const char *path;
    int ret;

    if (full_path == NULL) {
        return -EFAULT;
    }
    if ((ret = get_xipfs_mp(full_path, &mp)) < 0) {
        return ret;
    }
    if ((path = get_rel_path(&mp, full_path)) == NULL) {
        return -EIO;
    }

    mutex_lock(&xipfs_mutex);
    ret = xipfs_execv(&mp, path, argv);
    mutex_unlock(&xipfs_mutex);

    return ret;
}

/*
 * File system driver structures
 */

static const vfs_file_ops_t xipfs_file_ops = {
    .close = _close,
    .fstat = _fstat,
    .lseek = _lseek,
    .open = _open,
    .read = _read,
    .write = _write,
    .fsync = _fsync,
};

static const vfs_dir_ops_t xipfs_dir_ops = {
    .opendir = _opendir,
    .readdir = _readdir,
    .closedir = _closedir,
};

static const vfs_file_system_ops_t xipfs_fs_ops = {
    .format = _format,
    .mount = _mount,
    .umount = _umount,
    .unlink = _unlink,
    .mkdir = _mkdir,
    .rmdir = _rmdir,
    .rename = _rename,
    .stat = _stat,
    .statvfs = _statvfs,
};

const vfs_file_system_t xipfs_file_system = {
    .fs_op = &xipfs_fs_ops,
    .f_op = &xipfs_file_ops,
    .d_op = &xipfs_dir_ops,
};
