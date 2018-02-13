/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_fs_tmpfs
 * @{
 *
 * @file
 * @brief       TempFS implementation
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "fs/tmpfs.h"
#include "vfs.h"
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef TMPFS_MALLOC
#define TMPFS_MALLOC malloc
#endif

#ifndef TMPFS_FREE
#define TMPFS_FREE free
#endif

/**
 * @brief   tmpfs file buffer element
 */
typedef struct {
    clist_node_t next;  /**< next buffer in list */
    void *buf;          /**< data buffer */
    size_t size;        /**< data buffer size */
} tmpfs_file_buf_t;

/**
 *  @brief  tmpfs file descriptor
 */
typedef struct {
    clist_node_t next;              /**< next file in the list */
    clist_node_t buf;               /**< buffer list of this file */
    char name[VFS_NAME_MAX + 1];    /**< file name */
    size_t size;                    /**< total file size */
} tmpfs_file_t;

static void free_file(tmpfs_file_t *file)
{
    clist_node_t *node;

    while ((node = clist_lpop(&file->buf)) != NULL) {
        tmpfs_file_buf_t *buf = container_of(node, tmpfs_file_buf_t, next);
        TMPFS_FREE(buf->buf);
        TMPFS_FREE(buf);
    }
    TMPFS_FREE(file);
}

static int _mount(vfs_mount_t *mountp)
{
    tmpfs_t *tmpfs = mountp->private_data;

    mutex_init(&tmpfs->lock);

    return 0;
}

static int _umount(vfs_mount_t *mountp)
{
    tmpfs_t *tmpfs = mountp->private_data;
    clist_node_t *node;

    mutex_lock(&tmpfs->lock);

    while ((node = clist_lpop(&tmpfs->files)) != NULL) {
        tmpfs_file_t *file = container_of(node, tmpfs_file_t, next);
        free_file(file);
    }

    mutex_unlock(&tmpfs->lock);

    return 0;
}

static tmpfs_file_t *find_file(tmpfs_t *tmpfs, const char *name)
{
    clist_node_t *node = tmpfs->files.next;

    if (node) {
        do {
            node = node->next;
            tmpfs_file_t *file = container_of(node, tmpfs_file_t, next);
            if (strcmp(file->name, name) == 0) {
                return file;
            }
        } while (node != tmpfs->files.next);
    }

    return NULL;
}

static int _unlink(vfs_mount_t *mountp, const char *name)
{
    tmpfs_t *tmpfs = mountp->private_data;

    mutex_lock(&tmpfs->lock);

    tmpfs_file_t *file = find_file(tmpfs, name);
    if (!file) {
        mutex_unlock(&tmpfs->lock);
        return -ENOENT;
    }

    clist_remove(&tmpfs->files, &file->next);
    free_file(file);

    mutex_unlock(&tmpfs->lock);

    return 0;
}

static int _rename(vfs_mount_t *mountp, const char *from_path, const char *to_path)
{
    tmpfs_t *tmpfs = mountp->private_data;

    mutex_lock(&tmpfs->lock);

    tmpfs_file_t *file = find_file(tmpfs, from_path);
    if (!file) {
        mutex_unlock(&tmpfs->lock);
        return -ENOENT;
    }

    strncpy(file->name, to_path, VFS_NAME_MAX);

    mutex_unlock(&tmpfs->lock);

    return 0;
}

static int _stat(vfs_mount_t *mountp, const char *restrict path, struct stat *restrict buf)
{
    tmpfs_t *tmpfs = mountp->private_data;

    mutex_lock(&tmpfs->lock);
    tmpfs_file_t *file = find_file(tmpfs, path);
    mutex_unlock(&tmpfs->lock);
    if (!file) {
        return -ENOENT;
    }

    buf->st_size = file->size;

    return 0;
}

static int count_file_size(clist_node_t *node, void *arg)
{
    size_t *total_size = arg;
    tmpfs_file_t *file = container_of(node, tmpfs_file_t, next);
    *total_size += file->size;

    return 0;
}

static int _statvfs(vfs_mount_t *mountp, const char *restrict path, struct statvfs *restrict buf)
{
    (void)path;
    tmpfs_t *tmpfs = mountp->private_data;
    size_t total_size = 0;

    mutex_lock(&tmpfs->lock);
    clist_foreach(&tmpfs->files, count_file_size, &total_size);
    mutex_unlock(&tmpfs->lock);

    buf->f_bsize = 1;           /* block size */
    buf->f_frsize = 1;          /* fundamental block size */
    buf->f_blocks = total_size; /* Blocks total */
    buf->f_bfree = 0;           /* Blocks free */
    buf->f_bavail = 0;          /* Blocks available to non-privileged processes */
    buf->f_flag = ST_NOSUID;
    buf->f_namemax = VFS_NAME_MAX;

    return 0;
}

static tmpfs_file_t *add_file(tmpfs_t *tmpfs, const char *name)
{
    tmpfs_file_t *file = TMPFS_MALLOC(sizeof(tmpfs_file_t));
    if (!file) {
        return NULL;
    }

    memset(file, 0, sizeof(*file));
    strncpy(file->name, name, sizeof(file->name) - 1);
    clist_rpush(&tmpfs->files, &file->next);

    return file;
}

static int _open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path)
{
    (void)mode;
    (void)abs_path;
    tmpfs_t *tmpfs = filp->mp->private_data;
    int ret = 0;

    mutex_lock(&tmpfs->lock);

    tmpfs_file_t *file = find_file(tmpfs, name);
    if (!file) {
        if ((flags & O_CREAT) && ((flags & O_ACCMODE) != O_RDONLY)) {
            file = add_file(tmpfs, name);
            if (!file) {
                ret = -ENOMEM;
            }
        }
        else {
            ret = -EINVAL;
        }
    }
    if (!ret) {
        filp->private_data.ptr = file;
        if (flags & O_APPEND) {
            filp->pos = ((tmpfs_file_t *)filp->private_data.ptr)->size;
        }
    }

    mutex_unlock(&tmpfs->lock);
    return ret;
}

static tmpfs_file_buf_t *find_buf_index(tmpfs_file_t *file, off_t pos, size_t *index)
{
    assert(index);
    size_t cur = 0;

    clist_node_t *node = file->buf.next;
    if (!node) {
        return NULL;
    }
    do {
        node = node->next;
        tmpfs_file_buf_t *buf = container_of(node, tmpfs_file_buf_t, next);
        if ((size_t)pos < cur + buf->size) {
            *index = pos - cur;
            return buf;
        }
        cur += buf->size;
    } while (node != file->buf.next);

    return NULL;
}

static tmpfs_file_buf_t *alloc_buf(tmpfs_file_t *file, size_t nbytes)
{
    tmpfs_file_buf_t *buf = TMPFS_MALLOC(sizeof(*buf));
    if (!buf) {
        return NULL;
    }
    void *data_buf = TMPFS_MALLOC(nbytes);
    if (!data_buf) {
        TMPFS_FREE(buf);
    }
    clist_rpush(&file->buf, &buf->next);
    buf->buf = data_buf;
    buf->size = nbytes;
    file->size += nbytes;

    return buf;
}

static ssize_t _write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    tmpfs_file_t *file = filp->private_data.ptr;
    tmpfs_file_buf_t *buf = NULL;
    size_t index;
    size_t written = 0;

    if ((size_t)filp->pos < file->size) {
        buf = find_buf_index(file, filp->pos, &index);
    }
    if (!buf) {
        buf = alloc_buf(file, nbytes);
        if (!buf) {
            return -ENOMEM;
        }
        index = 0;
    }

    while (nbytes) {
        if (nbytes <= buf->size - index) {
            memcpy((char *)buf->buf + index, src, nbytes);
            written += nbytes;
            nbytes = 0;
        }
        else {
            memcpy((char *)buf->buf + index, src, buf->size - index);
            nbytes -= buf->size - index;
            written += buf->size - index;
            src = (char *)src + buf->size - index;
            clist_node_t *node = &buf->next;
            if (node != file->buf.next) {
                buf = container_of(node->next, tmpfs_file_buf_t, next);
            }
            else {
                buf = alloc_buf(file, nbytes);
                if (!buf) {
                    return -ENOMEM;
                }
            }
            index = 0;
        }
    }

    filp->pos += written;

    return written;
}

static ssize_t _read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    tmpfs_file_t *file = filp->private_data.ptr;
    size_t index;
    tmpfs_file_buf_t *buf = find_buf_index(file, filp->pos, &index);
    size_t read = 0;

    if (!buf) {
        return -EOVERFLOW;
    }

    while (nbytes) {
        if (nbytes <= buf->size - index) {
            memcpy(dest, (char *)buf->buf + index, nbytes);
            read += nbytes;
            dest = (char *)dest + nbytes;
            nbytes = 0;
        }
        else {
            memcpy(dest, (char *)buf->buf + index, buf->size - index);
            read += buf->size - index;
            nbytes -= buf->size - index;
            dest = (char *)dest + buf->size - index;
            clist_node_t *node = &buf->next;
            if (node != file->buf.next) {
                buf = container_of(node->next, tmpfs_file_buf_t, next);
                index = 0;
            }
            else {
                return read;
            }
        }
    }

    return read;
}

static const vfs_file_system_ops_t fs_ops = {
    .umount = _umount,
    .mount = _mount,
    .rename = _rename,
    .unlink = _unlink,
    .stat = _stat,
    .statvfs = _statvfs,
};

static const vfs_file_ops_t f_ops = {
    .open = _open,
    .write = _write,
    .read = _read,
};

const vfs_file_system_t tmpfs_file_system = {
    .f_op = &f_ops,
    .fs_op = &fs_ops,
};
