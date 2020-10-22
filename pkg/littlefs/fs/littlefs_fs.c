/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_littlefs
 * @{
 *
 * @file
 * @brief       littlefs integration with vfs
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */


#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>

#include "fs/littlefs_fs.h"

#include "kernel_defines.h"

#define ENABLE_DEBUG 0
#include <debug.h>

static int littlefs_err_to_errno(ssize_t err)
{
    switch (err) {
    case LFS_ERR_OK:
        return 0;
    case LFS_ERR_IO:
        return -EIO;
    case LFS_ERR_CORRUPT:
        return -ENODEV;
    case LFS_ERR_NOENT:
        return -ENOENT;
    case LFS_ERR_EXIST:
        return -EEXIST;
    case LFS_ERR_NOTDIR:
        return -ENOTDIR;
    case LFS_ERR_ISDIR:
        return -EISDIR;
    case LFS_ERR_NOTEMPTY:
        return -ENOTEMPTY;
    case LFS_ERR_BADF:
        return -EBADF;
    case LFS_ERR_INVAL:
        return -EINVAL;
    case LFS_ERR_NOSPC:
        return -ENOSPC;
    case LFS_ERR_NOMEM:
        return -ENOMEM;
    default:
        return err;
    }
}

static int _dev_read(const struct lfs_config *c, lfs_block_t block,
                 lfs_off_t off, void *buffer, lfs_size_t size)
{
    littlefs_desc_t *fs = c->context;
    mtd_dev_t *mtd = fs->dev;

    DEBUG("lfs_read: c=%p, block=%" PRIu32 ", off=%" PRIu32 ", buf=%p, size=%" PRIu32 "\n",
          (void *)c, block, off, buffer, size);

    return mtd_read_page(mtd, buffer, (fs->base_addr + block) * mtd->pages_per_sector,
                         off, size);
}

static int _dev_write(const struct lfs_config *c, lfs_block_t block,
                  lfs_off_t off, const void *buffer, lfs_size_t size)
{
    littlefs_desc_t *fs = c->context;
    mtd_dev_t *mtd = fs->dev;

    DEBUG("lfs_write: c=%p, block=%" PRIu32 ", off=%" PRIu32 ", buf=%p, size=%" PRIu32 "\n",
          (void *)c, block, off, buffer, size);

    return mtd_write_page(mtd, buffer, (fs->base_addr + block) * mtd->pages_per_sector,
                          off, size);
}

static int _dev_erase(const struct lfs_config *c, lfs_block_t block)
{
    littlefs_desc_t *fs = c->context;
    mtd_dev_t *mtd = fs->dev;

    DEBUG("lfs_erase: c=%p, block=%" PRIu32 "\n", (void *)c, block);

    return mtd_erase_sector(mtd, fs->base_addr + block, 1);
}

static int _dev_sync(const struct lfs_config *c)
{
    (void)c;

    return 0;
}

static int prepare(littlefs_desc_t *fs)
{
    mutex_init(&fs->lock);
    mutex_lock(&fs->lock);

    int ret = mtd_init(fs->dev);

    if (ret) {
        return ret;
    }

    memset(&fs->fs, 0, sizeof(fs->fs));

    if (!fs->config.block_count) {
        fs->config.block_count = fs->dev->sector_count - fs->base_addr;
    }
    if (!fs->config.block_size) {
        fs->config.block_size = fs->dev->page_size * fs->dev->pages_per_sector;
    }
    if (!fs->config.prog_size) {
        fs->config.prog_size = fs->dev->page_size;
    }
    if (!fs->config.read_size) {
        fs->config.read_size = fs->dev->page_size;
    }
    fs->config.lookahead = LITTLEFS_LOOKAHEAD_SIZE;
    fs->config.lookahead_buffer = fs->lookahead_buf;
    fs->config.context = fs;
    fs->config.read = _dev_read;
    fs->config.prog = _dev_write;
    fs->config.erase = _dev_erase;
    fs->config.sync = _dev_sync;
#if LITTLEFS_FILE_BUFFER_SIZE
    fs->config.file_buffer = fs->file_buf;
#endif
#if LITTLEFS_READ_BUFFER_SIZE
    fs->config.read_buffer = fs->read_buf;
#endif
#if LITTLEFS_PROG_BUFFER_SIZE
    fs->config.prog_buffer = fs->prog_buf;
#endif

    return 0;
}

static int _format(vfs_mount_t *mountp)
{
    littlefs_desc_t *fs = mountp->private_data;

    DEBUG("littlefs: format: mountp=%p\n", (void *)mountp);
    int ret = prepare(fs);
    if (ret) {
        return -ENODEV;
    }

    ret = lfs_format(&fs->fs, &fs->config);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _mount(vfs_mount_t *mountp)
{
    /* if one of the lines below fail to compile you probably need to adjust
       vfs buffer sizes ;) */
    BUILD_BUG_ON(VFS_DIR_BUFFER_SIZE < sizeof(lfs_dir_t));
    BUILD_BUG_ON(VFS_FILE_BUFFER_SIZE < sizeof(lfs_file_t));

    littlefs_desc_t *fs = mountp->private_data;

    DEBUG("littlefs: mount: mountp=%p\n", (void *)mountp);
    int ret = prepare(fs);
    if (ret) {
        return -ENODEV;
    }

    ret = lfs_mount(&fs->fs, &fs->config);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _umount(vfs_mount_t *mountp)
{
    littlefs_desc_t *fs = mountp->private_data;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: umount: mountp=%p\n", (void *)mountp);

    int ret = lfs_unmount(&fs->fs);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _unlink(vfs_mount_t *mountp, const char *name)
{
    littlefs_desc_t *fs = mountp->private_data;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: unlink: mountp=%p, name=%s\n",
          (void *)mountp, name);

    int ret = lfs_remove(&fs->fs, name);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _rename(vfs_mount_t *mountp, const char *from_path, const char *to_path)
{
    littlefs_desc_t *fs = mountp->private_data;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: rename: mountp=%p, from=%s, to=%s\n",
          (void *)mountp, from_path, to_path);

    int ret = lfs_rename(&fs->fs, from_path, to_path);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _mkdir(vfs_mount_t *mountp, const char *name, mode_t mode)
{
    (void)mode;
    littlefs_desc_t *fs = mountp->private_data;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: mkdir: mountp=%p, name=%s, mode=%" PRIu32 "\n",
          (void *)mountp, name, (uint32_t)mode);

    int ret = lfs_mkdir(&fs->fs, name);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _rmdir(vfs_mount_t *mountp, const char *name)
{
    littlefs_desc_t *fs = mountp->private_data;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: rmdir: mountp=%p, name=%s\n",
          (void *)mountp, name);

    int ret = lfs_remove(&fs->fs, name);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path)
{
    littlefs_desc_t *fs = filp->mp->private_data;
    lfs_file_t *fp = (lfs_file_t *)&filp->private_data.buffer;
    (void) abs_path;
    (void) mode;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: open: filp=%p, fp=%p\n", (void *)filp, (void *)fp);

    int l_flags = 0;
    if ((flags & O_ACCMODE) == O_RDONLY) {
        l_flags |= LFS_O_RDONLY;
    }
    if ((flags & O_APPEND) == O_APPEND) {
        l_flags |= LFS_O_APPEND;
    }
    if ((flags & O_TRUNC) == O_TRUNC) {
        l_flags |= LFS_O_TRUNC;
    }
    if ((flags & O_CREAT) == O_CREAT) {
        l_flags |= LFS_O_CREAT;
    }
    if ((flags & O_ACCMODE) == O_WRONLY) {
        l_flags |= LFS_O_WRONLY;
    }
    if ((flags & O_ACCMODE) == O_RDWR) {
        l_flags |= LFS_O_RDWR;
    }
    if ((flags & O_EXCL) == O_EXCL) {
        l_flags |= LFS_O_EXCL;
    }

    DEBUG("littlefs: open: %s (abs_path: %s), flags: 0x%x\n", name, abs_path, (int) l_flags);

    int ret = lfs_file_open(&fs->fs, fp, name, l_flags);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _close(vfs_file_t *filp)
{
    littlefs_desc_t *fs = filp->mp->private_data;
    lfs_file_t *fp = (lfs_file_t *)&filp->private_data.buffer;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: close: filp=%p, fp=%p\n", (void *)filp, (void *)fp);

    int ret = lfs_file_close(&fs->fs, fp);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static ssize_t _write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    littlefs_desc_t *fs = filp->mp->private_data;
    lfs_file_t *fp = (lfs_file_t *)&filp->private_data.buffer;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: write: filp=%p, fp=%p, src=%p, nbytes=%u\n",
          (void *)filp, (void *)fp, (void *)src, (unsigned)nbytes);

    ssize_t ret = lfs_file_write(&fs->fs, fp, src, nbytes);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static ssize_t _read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    littlefs_desc_t *fs = filp->mp->private_data;
    lfs_file_t *fp = (lfs_file_t *)&filp->private_data.buffer;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: read: filp=%p, fp=%p, dest=%p, nbytes=%u\n",
          (void *)filp, (void *)fp, (void *)dest, (unsigned)nbytes);

    ssize_t ret = lfs_file_read(&fs->fs, fp, dest, nbytes);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static off_t _lseek(vfs_file_t *filp, off_t off, int whence)
{
    littlefs_desc_t *fs = filp->mp->private_data;
    lfs_file_t *fp = (lfs_file_t *)&filp->private_data.buffer;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: seek: filp=%p, fp=%p, off=%ld, whence=%d\n",
          (void *)filp, (void *)fp, (long)off, whence);

    int ret = lfs_file_seek(&fs->fs, fp, off, whence);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _stat(vfs_mount_t *mountp, const char *restrict path, struct stat *restrict buf)
{
    littlefs_desc_t *fs = mountp->private_data;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: stat: mountp=%p, path=%s, buf=%p\n",
          (void *)mountp, path, (void *)buf);

    struct lfs_info info;
    int ret = lfs_stat(&fs->fs, path, &info);
    mutex_unlock(&fs->lock);
    /* info.name */
    buf->st_size = info.size;
    switch (info.type) {
    case LFS_TYPE_REG:
        buf->st_mode = S_IFREG;
        break;
    case LFS_TYPE_DIR:
        buf->st_mode = S_IFDIR;
        break;
    }

    return littlefs_err_to_errno(ret);
}

static int _traverse_cb(void *param, lfs_block_t block)
{
    (void)block;
    unsigned long *nb_blocks = param;
    (*nb_blocks)++;

    return 0;
}

static int _statvfs(vfs_mount_t *mountp, const char *restrict path, struct statvfs *restrict buf)
{
    (void)path;
    littlefs_desc_t *fs = mountp->private_data;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: statvfs: mountp=%p, path=%s, buf=%p\n",
          (void *)mountp, path, (void *)buf);

    unsigned long nb_blocks = 0;
    int ret = lfs_traverse(&fs->fs, _traverse_cb, &nb_blocks);
    mutex_unlock(&fs->lock);

    buf->f_bsize = fs->fs.cfg->block_size;      /* block size */
    buf->f_frsize = fs->fs.cfg->block_size;     /* fundamental block size */
    buf->f_blocks = fs->fs.cfg->block_count;    /* Blocks total */
    buf->f_bfree = buf->f_blocks - nb_blocks;   /* Blocks free */
    buf->f_bavail = buf->f_blocks - nb_blocks;  /* Blocks available to non-privileged processes */
    buf->f_flag = ST_NOSUID;
    buf->f_namemax = LFS_NAME_MAX;

    return littlefs_err_to_errno(ret);
}

static int _opendir(vfs_DIR *dirp, const char *dirname, const char *abs_path)
{
    (void)abs_path;
    littlefs_desc_t *fs = dirp->mp->private_data;
    lfs_dir_t *dir = (lfs_dir_t *)&dirp->private_data.buffer;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: opendir: dirp=%p, dirname=%s (abs_path=%s)\n",
          (void *)dirp, dirname, abs_path);

    int ret = lfs_dir_open(&fs->fs, dir, dirname);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    littlefs_desc_t *fs = dirp->mp->private_data;
    lfs_dir_t *dir = (lfs_dir_t *)&dirp->private_data.buffer;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: readdir: dirp=%p, entry=%p\n",
          (void *)dirp, (void *)entry);

    struct lfs_info info;
    int ret = lfs_dir_read(&fs->fs, dir, &info);
    if (ret >= 0) {
        entry->d_ino = info.type;
        entry->d_name[0] = '/';
        strncpy(entry->d_name + 1, info.name, VFS_NAME_MAX - 1);
    }

    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static int _closedir(vfs_DIR *dirp)
{
    littlefs_desc_t *fs = dirp->mp->private_data;
    lfs_dir_t *dir = (lfs_dir_t *)&dirp->private_data.buffer;

    mutex_lock(&fs->lock);

    DEBUG("littlefs: closedir: dirp=%p\n", (void *)dirp);

    int ret = lfs_dir_close(&fs->fs, dir);
    mutex_unlock(&fs->lock);

    return littlefs_err_to_errno(ret);
}

static const vfs_file_system_ops_t littlefs_fs_ops = {
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

static const vfs_file_ops_t littlefs_file_ops = {
    .open = _open,
    .close = _close,
    .read = _read,
    .write = _write,
    .lseek = _lseek,
};

static const vfs_dir_ops_t littlefs_dir_ops = {
    .opendir = _opendir,
    .readdir = _readdir,
    .closedir = _closedir,
};

const vfs_file_system_t littlefs_file_system = {
    .fs_op = &littlefs_fs_ops,
    .f_op = &littlefs_file_ops,
    .d_op = &littlefs_dir_ops,
};
