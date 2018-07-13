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
#include "memarray.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   tmpfs file buffer element
 */
typedef struct {
    clist_node_t next;  /**< next buffer in list */
    size_t size;        /**< data buffer size */
    size_t offset;      /**< current offset in buf, = size if full buf is used */
    void *buf;          /**< data buffer */
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

static tmpfs_file_t _file_buf[TMPFS_MAX_FILES];
#define TMPFS_REAL_BUF_SIZE     (sizeof(tmpfs_file_buf_t) + TMPFS_BUF_SIZE)
static uint8_t _buf[TMPFS_MAX_BUF][TMPFS_REAL_BUF_SIZE];
static int _initialized = 0;
static mutex_t _buf_lock = MUTEX_INIT;
static memarray_t _file_array;
static memarray_t _buf_array;

static void free_file(tmpfs_file_t *file)
{
    clist_node_t *node;

    mutex_lock(&_buf_lock);
    while ((node = clist_lpop(&file->buf)) != NULL) {
        tmpfs_file_buf_t *buf = container_of(node, tmpfs_file_buf_t, next);
        memarray_free(&_buf_array, buf);
    }
    memarray_free(&_file_array, file);
    mutex_unlock(&_buf_lock);
}

static int _mount(vfs_mount_t *mountp)
{
    tmpfs_t *tmpfs = mountp->private_data;

    mutex_init(&tmpfs->lock);

    mutex_lock(&_buf_lock);
    if (!_initialized) {
        memarray_init(&_file_array, _file_buf, sizeof(tmpfs_file_t), TMPFS_MAX_FILES);
        memarray_init(&_buf_array, _buf, TMPFS_REAL_BUF_SIZE, TMPFS_MAX_BUF);
        _initialized = 1;
    }
    mutex_unlock(&_buf_lock);

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
    tmpfs_file_t *file;
    mutex_lock(&_buf_lock);
    file = memarray_alloc(&_file_array);
    mutex_unlock(&_buf_lock);
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
                DEBUG("tmpfs: open: error when adding file\n");
                ret = -ENOMEM;
            }
        }
        else {
            DEBUG("tmpfs: open: no file found\n");
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
    *index = 0;

    clist_node_t *node = file->buf.next;
    if (!node) {
        DEBUG("tmpfs: find_buf: no buf\n");
        return NULL;
    }
    do {
        DEBUG("tmpfs: find_buf: finding buf at pos=%d, cur=%u\n", (int)pos, (unsigned)cur);
        node = node->next;
        tmpfs_file_buf_t *buf = container_of(node, tmpfs_file_buf_t, next);
        if ((size_t)pos < cur + buf->size) {
            *index = pos - cur;
            return buf;
        }
        cur += buf->offset;
    } while (node != file->buf.next);

    DEBUG("tmpfs: find_buf: buf not found\n");

    return NULL;
}

static tmpfs_file_buf_t *alloc_buf(tmpfs_file_t *file)
{
    tmpfs_file_buf_t *buf;
    mutex_lock(&_buf_lock);
    buf = memarray_alloc(&_buf_array);
    mutex_unlock(&_buf_lock);
    if (!buf) {
        return NULL;
    }
    memset(buf, 0, sizeof(*buf));
    clist_rpush(&file->buf, &buf->next);
    buf->buf = (uint8_t *)buf + sizeof(*buf);
    buf->size = TMPFS_BUF_SIZE;

    return buf;
}

static ssize_t _write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    tmpfs_file_t *file = filp->private_data.ptr;
    tmpfs_file_buf_t *buf;
    size_t written = 0;
    size_t index;

    DEBUG("tmpfs: write: writing %u bytes (%p)\n", (unsigned)nbytes, src);

    buf = find_buf_index(file, filp->pos, &index);
    if (!buf) {
        buf = alloc_buf(file);
        if (!buf) {
            DEBUG("tmpfs: write: error when allocating buf\n");
            return -ENOMEM;
        }
        index = 0;
    }

    while (nbytes) {
        DEBUG("tmpfs: write: trying to write remaining %u bytes\n", (unsigned)nbytes);
        if (nbytes <= buf->size - index) {
            memcpy((char *)buf->buf + index, src, nbytes);
            written += nbytes;
            if (nbytes > buf->offset - index) {
                buf->offset += nbytes - (buf->offset - index);
            }
            nbytes = 0;
        }
        else {
            size_t to_write = buf->size - index;
            memcpy((char *)buf->buf + index, src, to_write);
            nbytes -= to_write;
            written += to_write;
            src = (char *)src + to_write;
            buf->offset += to_write - (buf->offset - index);
            file->size += to_write - (buf->offset - index);
            clist_node_t *node = &buf->next;
            if (node != file->buf.next) {
                buf = container_of(node->next, tmpfs_file_buf_t, next);
            }
            else {
                buf = alloc_buf(file);
                if (!buf) {
                    DEBUG("tmpfs: write: error when allocating buf\n");
                    break;
                }
            }
            index = 0;
        }
    }

    filp->pos += written;
    if ((size_t)filp->pos > file->size) {
        file->size = filp->pos;
    }

    DEBUG("tmpfs: write: %u bytes written, size=%u, buf->offset=%u\n",
          (unsigned)written, (unsigned)file->size, (unsigned)buf->offset);

    return written;
}

static ssize_t _read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    tmpfs_file_t *file = filp->private_data.ptr;
    size_t index;
    tmpfs_file_buf_t *buf = find_buf_index(file, filp->pos, &index);
    size_t read = 0;

    if (!buf) {
        DEBUG("tmpfs: read: no buf found\n");
        return -EOVERFLOW;
    }

    while (nbytes) {
        if (nbytes <= buf->offset - index) {
            memcpy(dest, (char *)buf->buf + index, nbytes);
            read += nbytes;
            nbytes = 0;
        }
        else {
            memcpy(dest, (char *)buf->buf + index, buf->offset - index);
            read += buf->offset - index;
            nbytes -= buf->offset - index;
            dest = (char *)dest + buf->offset - index;
            clist_node_t *node = &buf->next;
            if (node != file->buf.next) {
                buf = container_of(node->next, tmpfs_file_buf_t, next);
            }
            else {
                break;
            }
            index = 0;
        }
    }

    filp->pos += read;

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
