/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_spiffs
 * @{
 *
 * @file
 * @brief       SPIFFS integration with vfs
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>

#include "fs/spiffs_fs.h"

#include "kernel_defines.h"

#define ENABLE_DEBUG 0
#include <debug.h>

static int spiffs_err_to_errno(s32_t err);

#if SPIFFS_HAL_CALLBACK_EXTRA == 1
static int32_t _dev_read(struct spiffs_t *fs, u32_t addr, u32_t size, u8_t *dst)
{
    mtd_dev_t *dev = (mtd_dev_t *)fs->user_data;

    DEBUG("spiffs: read: from addr 0x%" PRIx32 " size 0x%" PRIx32 "\n", addr, size);

    return mtd_read(dev, dst, addr, size);
}

static int32_t _dev_write(struct spiffs_t *fs, u32_t addr, u32_t size, const u8_t *src)
{
    mtd_dev_t *dev = (mtd_dev_t *)fs->user_data;

    DEBUG("spiffs: write: from addr 0x%" PRIx32 " size 0x%" PRIx32 "\n", addr, size);

    return mtd_write(dev, src, addr, size);
}

static int32_t _dev_erase(struct spiffs_t *fs, u32_t addr, u32_t size)
{
    mtd_dev_t *dev = (mtd_dev_t *)fs->user_data;

    DEBUG("spiffs: erase: from addr 0x%" PRIx32" size 0x%" PRIx32 "\n", addr, size);

    return mtd_erase(dev, addr, size);
}
#else
#ifndef SPIFFS_MTD_DEV
#error "SPIFFS needs SPIFFS_HAL_CALLBACK_EXTRA or SPIFFS_MTD_DEV"
#endif
static int32_t _dev_read(u32_t addr, u32_t size, u8_t *dst)
{
    DEBUG("spiffs: read: from addr 0x%" PRIx32 " size 0x%" PRIx32 "\n", addr, size);

    return mtd_read(SPIFFS_MTD_DEV, dst, addr, size);
}

static int32_t _dev_write(u32_t addr, u32_t size, const u8_t *src)
{
    DEBUG("spiffs: write: from addr 0x%" PRIx32 " size 0x%" PRIx32 "\n", addr, size);

    return mtd_write(SPIFFS_MTD_DEV, src, addr, size);
}

static int32_t _dev_erase(u32_t addr, u32_t size)
{
    DEBUG("spiffs: erase: from addr 0x%" PRIx32" size 0x%" PRIx32 "\n", addr, size);

    return mtd_erase(SPIFFS_MTD_DEV, addr, size);
}
#endif

void spiffs_lock(struct spiffs_t *fs)
{
    spiffs_desc_t *fs_desc = container_of(fs, spiffs_desc_t, fs);

    DEBUG("spiffs: lock: fs_desc %p\n", (void*)fs_desc);
    mutex_lock(&fs_desc->lock);
}
void spiffs_unlock(struct spiffs_t *fs)
{
    spiffs_desc_t *fs_desc = container_of(fs, spiffs_desc_t, fs);

    DEBUG("spiffs: unlock: fs_desc %p\n", (void*)fs_desc);
    mutex_unlock(&fs_desc->lock);
}

static int prepare(spiffs_desc_t *fs_desc)
{
#if SPIFFS_HAL_CALLBACK_EXTRA == 1
    mtd_dev_t *dev = fs_desc->dev;
    fs_desc->fs.user_data = dev;
#else
    mtd_dev_t *dev = SPIFFS_MTD_DEV;
#endif

    int res = mtd_init(dev);

    if (res) {
        return res;
    }

    fs_desc->config.hal_read_f = _dev_read;
    fs_desc->config.hal_write_f = _dev_write;
    fs_desc->config.hal_erase_f = _dev_erase;

#if SPIFFS_SINGLETON == 0
    DEBUG("spiffs: mount: mtd page_size=%" PRIu32 ", pages_per_sector=%" PRIu32
          ", sector_count=%" PRIu32 "\n", dev->page_size, dev->pages_per_sector, dev->sector_count);
    uint32_t sector_count = (fs_desc->block_count == 0) ? dev->sector_count : fs_desc->block_count;
    /* inside memory area */
    assert(((fs_desc->base_addr / (dev->page_size * dev->pages_per_sector)) + sector_count)
           <= dev->sector_count);
    /* base addr is aligned on a sector */
    assert(fs_desc->base_addr % (dev->pages_per_sector * dev->page_size) == 0);
    fs_desc->config.phys_size = dev->page_size * dev->pages_per_sector * sector_count;
    fs_desc->config.log_block_size = dev->page_size * dev->pages_per_sector;
    fs_desc->config.log_page_size = dev->page_size;
    fs_desc->config.phys_addr = fs_desc->base_addr;
    fs_desc->config.phys_erase_block = dev->page_size * dev->pages_per_sector;
#endif

    return 0;
}

static int _format(vfs_mount_t *mountp)
{
    spiffs_desc_t *fs_desc = mountp->private_data;
    DEBUG("spiffs: format: private_data = %p\n", mountp->private_data);
    int res = prepare(fs_desc);
    if (res) {
        return -ENODEV;
    }

    s32_t ret = SPIFFS_mount(&fs_desc->fs,
                             &fs_desc->config,
                             fs_desc->work,
                             fs_desc->fd_space,
                             SPIFFS_FS_FD_SPACE_SIZE,
#if SPIFFS_CACHE == 1
                             fs_desc->cache,
                             SPIFFS_FS_CACHE_SIZE,
#else
                             NULL,
                             0,
#endif
                             NULL);

    if (ret == 0) {
        DEBUG("spiffs: format: unmount fs\n");
        SPIFFS_unmount(&fs_desc->fs);
    }
    DEBUG("spiffs: format: formatting fs\n");
    ret = SPIFFS_format(&fs_desc->fs);
    DEBUG("spiffs: mount: format ret %" PRId32 "\n", ret);
    return spiffs_err_to_errno(ret);
}

static int _mount(vfs_mount_t *mountp)
{
    spiffs_desc_t *fs_desc = mountp->private_data;
    DEBUG("spiffs: mount: private_data = %p\n", mountp->private_data);
    int res = prepare(fs_desc);
    if (res) {
        return -ENODEV;
    }

    s32_t ret = SPIFFS_mount(&fs_desc->fs,
                             &fs_desc->config,
                             fs_desc->work,
                             fs_desc->fd_space,
                             SPIFFS_FS_FD_SPACE_SIZE,
#if SPIFFS_CACHE == 1
                             fs_desc->cache,
                             SPIFFS_FS_CACHE_SIZE,
#else
                             NULL,
                             0,
#endif
                             NULL);

    return spiffs_err_to_errno(ret);
}

static int _umount(vfs_mount_t *mountp)
{
    spiffs_desc_t *fs_desc = mountp->private_data;

    SPIFFS_unmount(&fs_desc->fs);

    return 0;
}

static int _unlink(vfs_mount_t *mountp, const char *name)
{
    spiffs_desc_t *fs_desc = mountp->private_data;

    return spiffs_err_to_errno(SPIFFS_remove(&fs_desc->fs, name));
}

static int _rename(vfs_mount_t *mountp, const char *from_path, const char *to_path)
{
    spiffs_desc_t *fs_desc = mountp->private_data;

    return spiffs_err_to_errno(SPIFFS_rename(&fs_desc->fs, from_path, to_path));
}

static int _statvfs(vfs_mount_t *mountp, const char *restrict path, struct statvfs *restrict buf)
{
    (void)path;
    if (buf == NULL) {
        return -EFAULT;
    }
    spiffs_desc_t *fs_desc = mountp->private_data;
    memset(buf, 0, sizeof(*buf));

    uint32_t total;
    uint32_t used;
    int32_t ret = SPIFFS_info(&fs_desc->fs, &total, &used);
    if (ret < 0) {
        return spiffs_err_to_errno(ret);
    }

    buf->f_bsize = sizeof(uint8_t);  /* block size */
    buf->f_frsize = sizeof(uint8_t); /* fundamental block size */
    buf->f_blocks = total;           /* Blocks total */
    buf->f_bfree = total - used;     /* Blocks free */
    buf->f_bavail = total - used;    /* Blocks available to non-privileged processes */
    buf->f_flag = ST_NOSUID;         /* File system flags */
    buf->f_namemax = SPIFFS_OBJ_NAME_LEN; /* Maximum file name length */

    return 0;
}

static int _open(vfs_file_t *filp, const char *name, int flags, mode_t mode, const char *abs_path)
{
    spiffs_desc_t *fs_desc = filp->mp->private_data;
    (void) abs_path;
    DEBUG("spiffs: open: private_data = %p\n", filp->mp->private_data);

    spiffs_flags s_flags = 0;
    if ((flags & O_ACCMODE) == O_RDONLY) {
        s_flags |= SPIFFS_O_RDONLY;
    }
    if ((flags & O_APPEND) == O_APPEND) {
        s_flags |= SPIFFS_O_APPEND;
    }
    if ((flags & O_TRUNC) == O_TRUNC) {
        s_flags |= SPIFFS_O_TRUNC;
    }
    if ((flags & O_CREAT) == O_CREAT) {
        s_flags |= SPIFFS_O_CREAT;
    }
    if ((flags & O_ACCMODE) == O_WRONLY) {
        s_flags |= SPIFFS_O_WRONLY;
    }
    if ((flags & O_ACCMODE) == O_RDWR) {
        s_flags |= SPIFFS_O_RDWR;
    }
#ifdef __O_DIRECT
    if ((flags & __O_DIRECT) == __O_DIRECT) {
        s_flags |= SPIFFS_O_DIRECT;
    }
#endif
    if ((flags & O_EXCL) == O_EXCL) {
        s_flags |= SPIFFS_O_EXCL;
    }

    DEBUG("spiffs: open: %s (abs_path: %s), flags: 0x%x, mode: %d\n", name, abs_path, (int) s_flags, (int) mode);

    s32_t ret = SPIFFS_open(&fs_desc->fs, name, s_flags, mode);
    if (ret >= 0) {
        filp->private_data.value = ret;
        return ret;
    }
    else {
        return spiffs_err_to_errno(ret);
    }
}

static int _close(vfs_file_t *filp)
{
    spiffs_desc_t *fs_desc = filp->mp->private_data;

    return spiffs_err_to_errno(SPIFFS_close(&fs_desc->fs, filp->private_data.value));
}

static ssize_t _write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    spiffs_desc_t *fs_desc = filp->mp->private_data;

    return spiffs_err_to_errno(SPIFFS_write(&fs_desc->fs, filp->private_data.value, src, nbytes));
}

static ssize_t _read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    spiffs_desc_t *fs_desc = filp->mp->private_data;

    return spiffs_err_to_errno(SPIFFS_read(&fs_desc->fs, filp->private_data.value, dest, nbytes));
}

static off_t _lseek(vfs_file_t *filp, off_t off, int whence)
{
    spiffs_desc_t *fs_desc = filp->mp->private_data;

    int s_whence = 0;
    if (whence == SEEK_SET) {
        s_whence = SPIFFS_SEEK_SET;
    }
    else if (whence == SEEK_CUR) {
        s_whence = SPIFFS_SEEK_CUR;
    }
    else if (whence == SEEK_END) {
        s_whence = SPIFFS_SEEK_END;
    }

    return spiffs_err_to_errno(SPIFFS_lseek(&fs_desc->fs, filp->private_data.value, off, s_whence));
}

static int _fstat(vfs_file_t *filp, struct stat *buf)
{
    spiffs_desc_t *fs_desc = filp->mp->private_data;
    spiffs_stat stat;
    s32_t ret;

    memset(buf, 0, sizeof(*buf));

    ret = SPIFFS_fstat(&fs_desc->fs, filp->private_data.value, &stat);

    if (ret < 0) {
        return ret;
    }
    /* stat.name; */
    buf->st_ino = stat.obj_id;
    /* stat.pix; */
    buf->st_size = stat.size;
    /* stat.type;*/
    buf->st_mode = S_IFREG;

    return spiffs_err_to_errno(ret);
}

static int _opendir(vfs_DIR *dirp, const char *dirname, const char *abs_path)
{
    spiffs_desc_t *fs_desc = dirp->mp->private_data;
    spiffs_DIR *d = (spiffs_DIR *)&dirp->private_data.buffer[0];
    (void) abs_path;

    spiffs_DIR *res = SPIFFS_opendir(&fs_desc->fs, dirname, d);
    if (res == NULL) {
        return -ENOENT;
    }

    return 0;
}

static int _readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    spiffs_DIR *d = (spiffs_DIR *)&dirp->private_data.buffer[0];
    struct spiffs_dirent e;
    struct spiffs_dirent *ret;

    ret = SPIFFS_readdir(d, &e);
    if (ret == NULL) {
        s32_t err = SPIFFS_errno(d->fs);
        if (err != SPIFFS_OK && err > SPIFFS_ERR_INTERNAL) {
            DEBUG("spiffs: readdir: err=%" PRId32 "\n", err);
            return -EIO;
        }
    }

    if (ret) {
        entry->d_ino = e.obj_id;
        strncpy(entry->d_name, (char*) e.name, VFS_NAME_MAX);
        return 1;
    }
    else {
        return 0;
    }
}

static int _closedir(vfs_DIR *dirp)
{
    spiffs_DIR *d = (spiffs_DIR *)&dirp->private_data.buffer[0];

    return spiffs_err_to_errno(SPIFFS_closedir(d));
}

static int spiffs_err_to_errno (s32_t err)
{
    if (err >= 0) {
        return (int) err;
    }

    DEBUG("spiffs: error=%" PRId32 "\n", err);

    switch (err) {
    case SPIFFS_OK:
        return 0;
    case SPIFFS_ERR_NOT_MOUNTED:
        return -EINVAL;
    case SPIFFS_ERR_FULL:
        return -ENOSPC;
    case SPIFFS_ERR_NOT_FOUND:
        return -ENOENT;
    case SPIFFS_ERR_END_OF_OBJECT:
        return 0;
    case SPIFFS_ERR_DELETED:
        return -ENOENT;
    case SPIFFS_ERR_MOUNTED:
        return -EBUSY;
    case SPIFFS_ERR_ERASE_FAIL:
        return -EIO;
    case SPIFFS_ERR_MAGIC_NOT_POSSIBLE:
        return -ENOSPC;
    case SPIFFS_ERR_NO_DELETED_BLOCKS:
        return 0;
    case SPIFFS_ERR_FILE_EXISTS:
        return -EEXIST;
    case SPIFFS_ERR_NOT_A_FILE:
        return -ENOENT;
    case SPIFFS_ERR_RO_NOT_IMPL:
        return -EROFS;
    case SPIFFS_ERR_RO_ABORTED_OPERATION:
        return -SPIFFS_ERR_RO_ABORTED_OPERATION;
    case SPIFFS_ERR_PROBE_TOO_FEW_BLOCKS:
        return -ENOSPC;
    case SPIFFS_ERR_PROBE_NOT_A_FS:
        return -ENODEV;
    case SPIFFS_ERR_NAME_TOO_LONG:
        return -ENAMETOOLONG;
    case SPIFFS_ERR_NOT_FINALIZED:
        return -ENODEV;
    case SPIFFS_ERR_NOT_INDEX:
        return -ENODEV;
    case SPIFFS_ERR_OUT_OF_FILE_DESCS:
        return -ENFILE;
    case SPIFFS_ERR_FILE_CLOSED:
        return -ENOENT;
    case SPIFFS_ERR_FILE_DELETED:
        return -ENOENT;
    case SPIFFS_ERR_BAD_DESCRIPTOR:
        return -EBADF;
    case SPIFFS_ERR_IS_INDEX:
        return -ENOENT;
    case SPIFFS_ERR_IS_FREE:
        return -ENOENT;
    case SPIFFS_ERR_INDEX_SPAN_MISMATCH:
        return -EIO;
    case SPIFFS_ERR_DATA_SPAN_MISMATCH:
        return -EIO;
    case SPIFFS_ERR_INDEX_REF_FREE:
        return -EIO;
    case SPIFFS_ERR_INDEX_REF_LU:
        return -EIO;
    case SPIFFS_ERR_INDEX_REF_INVALID:
        return -EIO;
    case SPIFFS_ERR_INDEX_FREE:
        return -EIO;
    case SPIFFS_ERR_INDEX_LU:
        return -EIO;
    case SPIFFS_ERR_INDEX_INVALID:
        return -EIO;
    case SPIFFS_ERR_NOT_WRITABLE:
        return -EACCES;
    case SPIFFS_ERR_NOT_READABLE:
        return -EACCES;
    case SPIFFS_ERR_CONFLICTING_NAME:
        return -EEXIST;
    case SPIFFS_ERR_NOT_CONFIGURED:
        return -ENODEV;
    case SPIFFS_ERR_NOT_A_FS:
        return -ENODEV;
    }

    return (int) err;
}

static const vfs_file_system_ops_t spiffs_fs_ops = {
    .format = _format,
    .mount = _mount,
    .umount = _umount,
    .unlink = _unlink,
    .rename = _rename,
    .statvfs = _statvfs,
};

static const vfs_file_ops_t spiffs_file_ops = {
    .open = _open,
    .close = _close,
    .read = _read,
    .write = _write,
    .lseek = _lseek,
    .fstat = _fstat,
};

static const vfs_dir_ops_t spiffs_dir_ops = {
    .opendir = _opendir,
    .readdir = _readdir,
    .closedir = _closedir,
};

const vfs_file_system_t spiffs_file_system = {
    .fs_op = &spiffs_fs_ops,
    .f_op = &spiffs_file_ops,
    .d_op = &spiffs_dir_ops,
};
