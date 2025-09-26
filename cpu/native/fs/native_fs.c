/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup sys_fs_native
 * @brief   Native integration with virtual filesystem (VFS)
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "mutex.h"
#include "native_internal.h"
#include "fs/native_fs.h"
#include "vfs.h"

#define ENABLE_DEBUG 0
#include <debug.h>

/**
 * @brief Assign each native instance a sub-sirectory based on `_native_id`
 */
#ifndef CONFIG_NATIVE_ISOLATE_FS
#  define CONFIG_NATIVE_ISOLATE_FS 0
#endif

/* Not using static inline functions here because they are also assigned to. */
#define FD(filep) filp->private_data.value
#define DIRP(dirp) dirp->private_data.ptr

/* Reentrancy guard required by the various static locals of the implementation */
static mutex_t _lock;

static void _do_prefix(vfs_mount_t *mountp, const char *name, char *buffer, size_t len)
{
    const native_desc_t *fs_desc = mountp->private_data;
    size_t res;

    assert(len > strlen(fs_desc->hostpath));

    if (CONFIG_NATIVE_ISOLATE_FS) {
        res = snprintf(buffer, len, "%s/%u%s", fs_desc->hostpath, _native_id, name);
    } else {
        res = snprintf(buffer, len, "%s%s", fs_desc->hostpath, name);
    }

#  ifdef NDEBUG
    if (res > len) {
        puts("fs_native: path larger than PATH_MAX");
        exit(ENOBUFS);
    }
#  else
    assert(res <= len);
#  endif
}

static char *_prefix_path(vfs_mount_t *mountp, const char *name)
{
    static char buffer[PATH_MAX];

    _do_prefix(mountp, name, buffer, sizeof(buffer));

    return buffer;
}

static int _mount(vfs_mount_t *mountp)
{
    int res;
    mutex_lock(&_lock);

    /* create common root dir first */
    if (CONFIG_NATIVE_ISOLATE_FS) {
        char *parent = _prefix_path(mountp, "");
        /* remove node specific suffix */
        char *end = strrchr(parent, '/');
        *end = '\0';

        /* Ignoring errors: they're caught by the subsequent real_mkdir */
        real_mkdir(parent, 0777);
    }

    real_mkdir(_prefix_path(mountp, ""), 0777);
    res = errno == EEXIST ? 0 : -errno;

    mutex_unlock(&_lock);
    return res;
}

static int _unmount(vfs_mount_t *mountp)
{
    mutex_lock(&_lock);

    /* Ignoring errors: directories with any content are left in place. */
    real_rmdir(_prefix_path(mountp, ""));

    mutex_unlock(&_lock);
    return 0;
}

static int _mkdir(vfs_mount_t *mountp, const char *name, mode_t mode)
{
    int res = 0;
    mutex_lock(&_lock);

    if (real_mkdir(_prefix_path(mountp, name), mode) < 0) {
        res = -errno;
    }

    mutex_unlock(&_lock);
    return res;
}

static int _rmdir(vfs_mount_t *mountp, const char *name)
{
    int res = 0;
    mutex_lock(&_lock);

    if (real_rmdir(_prefix_path(mountp, name)) < 0) {
        res = -errno;
    }

    mutex_unlock(&_lock);
    return res;
}

static int _statvfs(vfs_mount_t *mountp, const char *restrict path, struct statvfs *restrict buf)
{
    int res = 0;
    mutex_lock(&_lock);

    if (real_statvfs(_prefix_path(mountp, path), buf) < 0) {
        res = -errno;
    }

    mutex_unlock(&_lock);
    return res;
}

static int _open(vfs_file_t *filp, const char *name, int flags, mode_t mode)
{
    int res = 0;
    mutex_lock(&_lock);

    if ((FD(filep) = real_open(_prefix_path(filp->mp, name), flags, mode)) < 0) {
        res = -errno;
    }

    mutex_unlock(&_lock);
    return res;
}

static ssize_t _read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    return real_read(FD(filep), dest, nbytes);
}

static ssize_t _write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    return real_write(FD(filep), src, nbytes);
}

static off_t _lseek(vfs_file_t *filp, off_t off, int whence)
{
    return real_lseek(FD(filep), off, whence);
}

static int _fstat(vfs_file_t *filp, struct stat *buf)
{
    return real_fstat(FD(filep), buf);
}

static int _fsync(vfs_file_t *filp)
{
    return real_fsync(FD(filep));
}

static int _close(vfs_file_t *filp)
{
    return real_close(FD(filep));
}

static int _unlink(vfs_mount_t *mountp, const char *name)
{
    int res = 0;
    mutex_lock(&_lock);

    if (real_unlink(_prefix_path(mountp, name)) < 0) {
        res = -errno;
    }

    mutex_unlock(&_lock);
    return res;
}

static int _rename(vfs_mount_t *mountp, const char *from_path, const char *to_path)
{
    static char path_a[PATH_MAX];
    static char path_b[PATH_MAX];

    int res = 0;
    mutex_lock(&_lock);

    _do_prefix(mountp, from_path, path_a, sizeof(path_a));
    _do_prefix(mountp, to_path, path_b, sizeof(path_b));

    if (real_rename(path_a, path_b) < 0) {
        res = -errno;
    }

    mutex_unlock(&_lock);
    return res;
}

static int _opendir(vfs_DIR *dirp, const char *dirname)
{
    int res = 0;
    mutex_lock(&_lock);

    if ((DIRP(dirp) = real_opendir(_prefix_path(dirp->mp, dirname))) == NULL) {
        res = -errno;
    }

    mutex_unlock(&_lock);
    return res;
}

static int _readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    struct dirent *dirent = real_readdir(DIRP(dirp));

    if (dirent == NULL) {
        return 0;
    }

    entry->d_ino = dirent->d_ino;
    strncpy(entry->d_name, (char *)dirent->d_name, sizeof(entry->d_name));

    return 1;
}

static int _closedir(vfs_DIR *dirp)
{
    return real_closedir(DIRP(dirp));
}

static int _fcntl(vfs_file_t *filp, int cmd, int arg)
{
    return real_fcntl(FD(filep), cmd, arg);
}

static const vfs_file_system_ops_t native_fs_ops = {
    .mount = _mount,
    .umount = _unmount,
    .unlink = _unlink,
    .mkdir = _mkdir,
    .rmdir = _rmdir,
    .rename = _rename,
    .stat = vfs_sysop_stat_from_fstat,
    .statvfs = _statvfs,
};

static const vfs_file_ops_t native_file_ops = {
    .open = _open,
    .close = _close,
    .read = _read,
    .write = _write,
    .lseek = _lseek,
    .fstat = _fstat,
    .fsync = _fsync,
    .fcntl = _fcntl,
};

static const vfs_dir_ops_t native_dir_ops = {
    .opendir = _opendir,
    .readdir = _readdir,
    .closedir = _closedir,
};

const vfs_file_system_t native_file_system = {
    .fs_op = &native_fs_ops,
    .f_op = &native_file_ops,
    .d_op = &native_dir_ops,
};
