/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_fs_constfs
 * @{
 *
 * @file
 * @brief       ConstFS implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

/* Required for strnlen in string.h, when building with -std=c99 */
#define _DEFAULT_SOURCE 1
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "fs/constfs.h"
#include "vfs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* File system operations */
static int constfs_mount(vfs_mount_t *mountp);
static int constfs_umount(vfs_mount_t *mountp);
static int constfs_unlink(vfs_mount_t *mountp, const char *name);
static int constfs_stat(vfs_mount_t *mountp, const char *restrict name, struct stat *restrict buf);
static int constfs_statvfs(vfs_mount_t *mountp, const char *restrict path, struct statvfs *restrict buf);

/* File operations */
static int constfs_close(vfs_file_t *filp);
static int constfs_fstat(vfs_file_t *filp, struct stat *buf);
static off_t constfs_lseek(vfs_file_t *filp, off_t off, int whence);
static int constfs_open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path);
static ssize_t constfs_read(vfs_file_t *filp, void *dest, size_t nbytes);
static ssize_t constfs_write(vfs_file_t *filp, const void *src, size_t nbytes);

/* Directory operations */
static int constfs_opendir(vfs_DIR *dirp, const char *dirname, const char *abs_path);
static int constfs_readdir(vfs_DIR *dirp, vfs_dirent_t *entry);
static int constfs_closedir(vfs_DIR *dirp);

static const vfs_file_system_ops_t constfs_fs_ops = {
    .mount = constfs_mount,
    .umount = constfs_umount,
    .unlink = constfs_unlink,
    .statvfs = constfs_statvfs,
    .stat = constfs_stat,
};

static const vfs_file_ops_t constfs_file_ops = {
    .close = constfs_close,
    .fstat = constfs_fstat,
    .lseek = constfs_lseek,
    .open  = constfs_open,
    .read  = constfs_read,
    .write = constfs_write,
};

static const vfs_dir_ops_t constfs_dir_ops = {
    .opendir = constfs_opendir,
    .readdir = constfs_readdir,
    .closedir = constfs_closedir,
};

const vfs_file_system_t constfs_file_system = {
    .f_op = &constfs_file_ops,
    .fs_op = &constfs_fs_ops,
    .d_op = &constfs_dir_ops,
};

/**
 * @internal
 * @brief Fill a file information struct with information about the file
 * pointed to by @p fp
 *
 * @param[in]  fp     file to query
 * @param[out] buf    output buffer
 */
static void _constfs_write_stat(const constfs_file_t *fp, struct stat *restrict buf);

static int constfs_mount(vfs_mount_t *mountp)
{
    /* perform any extra initialization here */
    (void) mountp; /* prevent warning: unused parameter */
    return 0;
}

static int constfs_umount(vfs_mount_t *mountp)
{
    /* free resources and perform any clean up here */
    (void) mountp; /* prevent warning: unused parameter */
    return 0;
}

static int constfs_unlink(vfs_mount_t *mountp, const char *name)
{
    /* Removing files is prohibited */
    (void) mountp; /* prevent warning: unused parameter */
    (void) name; /* prevent warning: unused parameter */
    return -EROFS;
}

static int constfs_stat(vfs_mount_t *mountp, const char *restrict name, struct stat *restrict buf)
{
    (void) name;
    /* Fill out some information about this file */
    if (buf == NULL) {
        return -EFAULT;
    }
    constfs_t *fs = mountp->private_data;
    /* linear search through the files array */
    for (size_t i = 0; i < fs->nfiles; ++i) {
        DEBUG("constfs_stat ? \"%s\"\n", fs->files[i].path);
        if (strcmp(fs->files[i].path, name) == 0) {
            DEBUG("constfs_stat: Found :)\n");
            _constfs_write_stat(&fs->files[i], buf);
            buf->st_ino = i;
            return 0;
        }
    }
    DEBUG("constfs_stat: Not found :(\n");
    return -ENOENT;
}

static int constfs_statvfs(vfs_mount_t *mountp, const char *restrict path, struct statvfs *restrict buf)
{
    (void) path;
    /* Fill out some information about this file system */
    if (buf == NULL) {
        return -EFAULT;
    }
    constfs_t *fs = mountp->private_data;
    /* clear out the stat buffer first */
    memset(buf, 0, sizeof(*buf));
    buf->f_bsize = sizeof(uint8_t); /* block size */
    buf->f_frsize = sizeof(uint8_t); /* fundamental block size */
    fsblkcnt_t f_blocks = 0;
    for (size_t i = 0; i < fs->nfiles; ++i) {
        f_blocks += fs->files[i].size;
    }
    buf->f_blocks = f_blocks;  /* Blocks total */
    buf->f_bfree = 0;          /* Blocks free */
    buf->f_bavail = 0;         /* Blocks available to non-privileged processes */
    buf->f_files = fs->nfiles; /* Total number of file serial numbers */
    buf->f_ffree = 0;          /* Total number of free file serial numbers */
    buf->f_favail = 0;         /* Number of file serial numbers available to non-privileged process */
    buf->f_fsid = 0;           /* File system id */
    buf->f_flag = (ST_RDONLY | ST_NOSUID); /* File system flags */
    buf->f_namemax = UINT8_MAX; /* Maximum file name length */
    return 0;
}

static int constfs_close(vfs_file_t *filp)
{
    /* perform any necessary clean ups */
    (void) filp; /* prevent warning: unused parameter */
    return 0;
}

static int constfs_fstat(vfs_file_t *filp, struct stat *buf)
{
    constfs_file_t *fp = filp->private_data.ptr;
    if (buf == NULL) {
        return -EFAULT;
    }
    _constfs_write_stat(fp, buf);
    return 0;
}

static off_t constfs_lseek(vfs_file_t *filp, off_t off, int whence)
{
    constfs_file_t *fp = filp->private_data.ptr;
    switch (whence) {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            off += filp->pos;
            break;
        case SEEK_END:
            off += fp->size;
            break;
        default:
            return -EINVAL;
    }
    if (off < 0) {
        /* the resulting file offset would be negative */
        return -EINVAL;
    }
    /* POSIX allows seeking past the end of the file, even with O_RDONLY */
    filp->pos = off;
    return off;
}

static int constfs_open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path)
{
    (void) mode;
    (void) abs_path;
    constfs_t *fs = filp->mp->private_data;
    DEBUG("constfs_open: %p, \"%s\", 0x%x, 0%03lo, \"%s\"\n", (void *)filp, name, flags, (unsigned long)mode, abs_path);
    /* We only support read access */
    if ((flags & O_ACCMODE) != O_RDONLY) {
        return -EROFS;
    }
    /* linear search through the files array */
    for (size_t i = 0; i < fs->nfiles; ++i) {
        DEBUG("constfs_open ? \"%s\"\n", fs->files[i].path);
        if (strcmp(fs->files[i].path, name) == 0) {
            DEBUG("constfs_open: Found :)\n");
            filp->private_data.ptr = (void *)&fs->files[i];
            return 0;
        }
    }
    DEBUG("constfs_open: Not found :(\n");
    return -ENOENT;
}

static ssize_t constfs_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    constfs_file_t *fp = filp->private_data.ptr;
    DEBUG("constfs_read: %p, %p, %lu\n", (void *)filp, dest, (unsigned long)nbytes);
    if ((size_t)filp->pos >= fp->size) {
        /* Current offset is at or beyond end of file */
        return 0;
    }

    if (nbytes > (fp->size - filp->pos)) {
        nbytes = fp->size - filp->pos;
    }
    memcpy(dest, fp->data + filp->pos, nbytes);
    DEBUG("constfs_read: read %lu bytes\n", (long unsigned)nbytes);
    filp->pos += nbytes;
    return nbytes;
}

static ssize_t constfs_write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    DEBUG("constfs_write: %p, %p, %lu\n", (void *)filp, src, (unsigned long)nbytes);
    /* Read only file system */
    DEBUG("constfs_write: read only FS\n");
    /* prevent warning: unused parameter */
    (void) filp;
    (void) src;
    (void) nbytes;
    return -EBADF;
}

static int constfs_opendir(vfs_DIR *dirp, const char *dirname, const char *abs_path)
{
    (void) abs_path;
    DEBUG("constfs_opendir: %p, \"%s\", \"%s\"\n", (void *)dirp, dirname, abs_path);
    if (strncmp(dirname, "/", 2) != 0) {
        /* We keep it simple and only support a flat file system, only a root directory */
        return -ENOENT;
    }
    dirp->private_data.value = 0;
    return 0;
}

static int constfs_readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    DEBUG("constfs_readdir: %p, %p\n", (void *)dirp, (void *)entry);
    constfs_t *fs = dirp->mp->private_data;
    int filenum = dirp->private_data.value;
    if ((size_t)filenum >= fs->nfiles) {
        /* End of stream */
        return 0;
    }
    const constfs_file_t *fp = &fs->files[filenum];
    if (fp->path == NULL) {
        return -EIO;
    }
    size_t len = strnlen(fp->path, VFS_NAME_MAX + 1);
    if (len > VFS_NAME_MAX) {
        /* name does not fit in vfs_dirent_t buffer */
        /* skipping past the broken entry */
        ++filenum;
        dirp->private_data.value = filenum;
        return -EAGAIN;
    }
    /* copy the string, including terminating null */
    memcpy(&entry->d_name[0], fp->path, len + 1);
    entry->d_ino = filenum;
    ++filenum;
    dirp->private_data.value = filenum;
    return 1;
}

static int constfs_closedir(vfs_DIR *dirp)
{
    /* Just an example, it's not necessary to define closedir if there is
     * nothing to clean up */
    (void) dirp;
    return 0;
}

static void _constfs_write_stat(const constfs_file_t *fp, struct stat *restrict buf)
{
    /* clear out the stat buffer first */
    memset(buf, 0, sizeof(*buf));
    buf->st_nlink = 1;
    buf->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH;
    buf->st_size = fp->size;
    buf->st_blocks = fp->size;
    buf->st_blksize = sizeof(uint8_t);
}
