/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_lwext4
 * @{
 *
 * @file
 * @brief       lwEXT4 integration with vfs
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "fs/lwext4_fs.h"
#include <ext4_super.h>
#include <ext4_mkfs.h>

#define ENABLE_DEBUG 0
#include <debug.h>

/**
 * @brief file system block size to use when formatting a new file system
 */
#ifndef CONFIG_EXT_BLOCKSIZE_DEFAULT
#define CONFIG_EXT_BLOCKSIZE_DEFAULT (1024)
#endif

/**
 * @brief Automatic mountpoints
 */
XFA_USE(vfs_mount_t, vfs_mountpoints_xfa);

/**
 * @brief Number of automatic mountpoints
 */
#define MOUNTPOINTS_NUMOF XFA_LEN(vfs_mount_t, vfs_mountpoints_xfa)

struct ext4_mountpoint *ext4_get_mount(const char *path)
{
    size_t strlen_path = strlen(path);
    for (unsigned i = 0; i < MOUNTPOINTS_NUMOF; ++i) {
        /* lwext4 wants terminating '/' for mountpoint, but VFS does not */
        size_t strlen_mp = strlen(vfs_mountpoints_xfa[i].mount_point) - 1;

        if (strlen_mp < strlen_path &&
            strncmp(path, vfs_mountpoints_xfa[i].mount_point, strlen_mp) == 0) {
            lwext4_desc_t *fs = vfs_mountpoints_xfa[i].private_data;
            return &fs->mp;
        }
    }

    DEBUG("lwext4: no mountpoint found for '%s'\n", path);

    return NULL;
}

static int _noop(struct ext4_blockdev *bdev)
{
    (void)bdev;
    return 0;
}

static int blockdev_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
                          uint32_t blk_cnt)
{
    mtd_dev_t *dev = bdev->bdif->p_user;

    uint32_t page = blk_id * dev->pages_per_sector;
    uint32_t size = blk_cnt * dev->pages_per_sector * dev->page_size;

    assert(blk_id <= UINT32_MAX);

    DEBUG("lwext4: read %"PRIu32" bytes from page %"PRIu32"\n", size, page);

    return -mtd_read_page(dev, buf, page, 0, size);
}

static int blockdev_bwrite(struct ext4_blockdev *bdev, const void *buf,
                           uint64_t blk_id, uint32_t blk_cnt)
{
    mtd_dev_t *dev = bdev->bdif->p_user;

    assert(blk_id <= UINT32_MAX);

    DEBUG("lwext4: write %"PRIu32" bytes to sector %"PRIu32"\n",
          blk_cnt * dev->pages_per_sector * dev->page_size, (uint32_t)blk_id);

    return -mtd_write_sector(dev, buf, blk_id, blk_cnt);
}

static int prepare(lwext4_desc_t *fs, const char *mount_point)
{
    mtd_dev_t *dev = fs->dev;
    struct ext4_blockdev_iface *iface = &fs->iface;

    memset(&fs->mp, 0, sizeof(fs->mp));
    memset(&fs->bdev, 0, sizeof(fs->bdev));
    memset(&fs->iface, 0, sizeof(fs->iface));

    strncpy(fs->mp.name, mount_point, CONFIG_EXT4_MAX_MP_NAME - 1);
    strcat(fs->mp.name, "/");

    int res = mtd_init(dev);
    if (res) {
        return res;
    }

    iface->open   = _noop;
    iface->bread  = blockdev_bread;
    iface->bwrite = blockdev_bwrite;
    iface->close  = _noop;
    iface->lock   = _noop;
    iface->unlock = _noop;

    iface->p_user = dev;
    iface->ph_bcnt = dev->sector_count;
    iface->ph_bsize = dev->pages_per_sector * dev->page_size;
    iface->ph_bbuf = malloc(iface->ph_bsize);

    fs->bdev.bdif = iface;
    fs->bdev.part_size = iface->ph_bcnt * iface->ph_bsize;

    return -ext4_block_init(&fs->bdev);
}

static mutex_t _lwext4_mutex;
static void _lock(void)
{
    mutex_lock(&_lwext4_mutex);
}

static void _unlock(void)
{
    mutex_unlock(&_lwext4_mutex);
}

static const struct ext4_lock _lwext4_os_lock = {
    .lock = _lock,
    .unlock = _unlock,
};

static int _mount(vfs_mount_t *mountp)
{
    /* if one of the lines below fail to compile you probably need to adjust
       vfs buffer sizes ;) */
    static_assert(VFS_DIR_BUFFER_SIZE >= sizeof(ext4_dir),
                  "ext4_dir must fit in VFS_DIR_BUFFER_SIZE");
    static_assert(VFS_FILE_BUFFER_SIZE >= sizeof(ext4_file),
                  "ext4_file must fit in VFS_FILE_BUFFER_SIZE");

    lwext4_desc_t *fs = mountp->private_data;
    struct ext4_mountpoint *mp = &fs->mp;
    struct ext4_bcache *bc = &mp->bc;

    if (mp->mounted) {
        return 0;
    }

    int res = prepare(fs, mountp->mount_point);
    if (res) {
        return res;
    }

    res = ext4_fs_init(&mp->fs, &fs->bdev, false);
    DEBUG("lwext4 mount: %s\n", strerror(res));

    if (res != EOK) {
        return -res;
    }

    size_t bsize = ext4_sb_get_block_size(&mp->fs.sb);
    ext4_block_set_lb_size(&fs->bdev, bsize);

    res = ext4_bcache_init_dynamic(bc, CONFIG_BLOCK_DEV_CACHE_SIZE, bsize);
    if (res != EOK) {
        return -res;
    }

    assert(bsize == bc->itemsize);

    bc->bdev = &fs->bdev;
    fs->bdev.bc = bc;
    fs->bdev.fs = &mp->fs;

    /*Bind block cache to block device*/
    res = ext4_block_bind_bcache(bc->bdev, bc);
    if (res != EOK) {
        ext4_bcache_cleanup(bc);
        ext4_block_fini(bc->bdev);
        ext4_bcache_fini_dynamic(bc);
        return -res;
    }

    mp->os_locks = &_lwext4_os_lock;

    res = ext4_recover(fs->mp.name);
    if (res != EOK && res != ENOTSUP) {
        DEBUG("ext4_recover: rc = %d\n", res);
        return -res;
    }

    res = ext4_journal_start(fs->mp.name);
    if (res != EOK) {
        DEBUG("ext4_journal_start: rc = %d\n", res);
        return -res;
    }

    mp->mounted = true;
    ext4_cache_write_back(fs->mp.name, 1);

    return -res;
}

static int _umount(vfs_mount_t *mountp)
{
    lwext4_desc_t *fs = mountp->private_data;
    struct ext4_mountpoint *mp = &fs->mp;

    int res;

    ext4_cache_write_back(fs->mp.name, 0);

    res = ext4_journal_stop(fs->mp.name);
    if (res != EOK) {
        DEBUG("ext4_journal_stop: fail %d", res);
        return false;
    }

    res = ext4_fs_fini(&mp->fs);
    if (res != EOK) {
        goto out;
    }

    ext4_bcache_cleanup(mp->fs.bdev->bc);
    ext4_bcache_fini_dynamic(mp->fs.bdev->bc);

    res = ext4_block_fini(mp->fs.bdev);

    if (fs->iface.ph_bbuf) {
        free(fs->iface.ph_bbuf);
        fs->iface.ph_bbuf = NULL;
    }

out:
    mp->mounted = false;
    mp->fs.bdev->fs = NULL;

    return -res;
}

#ifdef MODULE_LWEXT4_VFS_FORMAT
static int _format(vfs_mount_t *mountp)
{
    lwext4_desc_t *fs = mountp->private_data;
    struct ext4_mountpoint *mp = &fs->mp;

    if (mp->mounted) {
        return -EBUSY;
    }

    int res = prepare(fs, mountp->mount_point);
    if (res) {
        return res;
    }

    struct ext4_mkfs_info info = {
        .block_size = CONFIG_EXT_BLOCKSIZE_DEFAULT,
        .journal = CONFIG_JOURNALING_ENABLE,
    };
    assert(fs->dev->pages_per_sector * fs->dev->page_size <= info.block_size);

    res = ext4_mkfs(&mp->fs, &fs->bdev, &info, CONFIG_EXT_FEATURE_SET_LVL);

    free(fs->iface.ph_bbuf);
    fs->iface.ph_bbuf = NULL;

    return -res;
}
#endif /* MODULE_LWEXT4_VFS_FORMAT */

static int _mkdir(vfs_mount_t *mountp, const char *name, mode_t mode)
{
    (void)mountp;

    int res = ext4_dir_mk(name);
    if (res != EOK) {
        return -res;
    }

    if (mode) {
        return -ext4_mode_set(name, mode);
    }
    return 0;
}

static int _rmdir(vfs_mount_t *mountp, const char *name)
{
    (void)mountp;

    return -ext4_dir_rm(name);
}

static int _statvfs(vfs_mount_t *mountp, const char *restrict path, struct statvfs *restrict buf)
{
    (void)path;

    struct ext4_mount_stats stats;
    lwext4_desc_t *fs = mountp->private_data;

    int res = ext4_mount_point_stats(fs->mp.name, &stats);
    if (res) {
        return -res;
    }

    buf->f_blocks = stats.blocks_count;
    buf->f_bfree = stats.free_blocks_count;
    buf->f_bavail = stats.free_blocks_count;
    buf->f_bsize = stats.block_size;

    return 0;
}

static inline ext4_file * _get_ext4_file(vfs_file_t *f)
{
    /* The buffer in `private_data` is part of a union that also contains a
     * pointer, so the alignment is fine. Adding an intermediate cast to
     * uintptr_t to silence -Wcast-align
     */
    return (ext4_file *)(uintptr_t)f->private_data.buffer;
}

static int _open(vfs_file_t *filp, const char *name, int flags, mode_t mode)
{
    int res;

    ext4_file *file = _get_ext4_file(filp);

    res = ext4_fopen2(file, name, flags);
    if (res != EOK) {
        return -res;
    }

    file->mp = ext4_get_mount(name);

    if (mode && (flags & O_CREAT)) {
        return -ext4_mode_set(name, mode);
    }
    return 0;
}

static ssize_t _read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    ext4_file *file = _get_ext4_file(filp);

    int res = ext4_fread(file, dest, nbytes, &nbytes);

    if (res != EOK) {
        return -res;
    } else {
        return nbytes;
    }
}

static ssize_t _write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    ext4_file *file = _get_ext4_file(filp);

    int res = ext4_fwrite(file, src, nbytes, &nbytes);

    if (res != EOK) {
        return -res;
    } else {
        return nbytes;
    }
}

static off_t _lseek(vfs_file_t *filp, off_t off, int whence)
{
    ext4_file *file = _get_ext4_file(filp);
    int res = ext4_fseek(file, off, whence);
    if (res) {
        return -res;
    }

    return ext4_ftell(file);
}

static int _fstat(vfs_file_t *filp, struct stat *buf)
{
    ext4_file *file = _get_ext4_file(filp);

    buf->st_ino = file->inode;
    buf->st_size = file->fsize;
    buf->st_mode = S_IFREG;

    return 0;
}

static int _fsync(vfs_file_t *filp)
{
    return -ext4_cache_flush(filp->mp->mount_point);
}

static int _close(vfs_file_t *filp)
{
    ext4_file *file = _get_ext4_file(filp);

    return ext4_fclose(file);
}

static int _unlink(vfs_mount_t *mountp, const char *name)
{
    (void)mountp;
    return -ext4_fremove(name);
}

static inline ext4_dir *_get_ext4_dir(vfs_DIR *dirp)
{
    return (ext4_dir *)(uintptr_t)dirp->private_data.buffer;
}

static int _rename(vfs_mount_t *mountp, const char *from_path, const char *to_path)
{
    (void)mountp;
    return -ext4_frename(from_path, to_path);
}

static int _opendir(vfs_DIR *dirp, const char *dirname)
{
    ext4_dir *dir = _get_ext4_dir(dirp);

    /* lwext4 doesn't like an empty path (relative to mount point) */
    if (strcmp(dirp->mp->mount_point, dirname) == 0) {
        lwext4_desc_t *fs = dirp->mp->private_data;
        dirname = fs->mp.name;
    }

    return -ext4_dir_open(dir, dirname);
}

static int _readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    ext4_dir *dir = _get_ext4_dir(dirp);

    const ext4_direntry *dirent = ext4_dir_entry_next(dir);
    if (dirent == NULL) {
        return 0;
    }

    strncpy(entry->d_name, (char *)dirent->name, sizeof(entry->d_name));
    entry->d_name[sizeof(entry->d_name) - 1] = '\0';

    return 1;
}

static int _closedir(vfs_DIR *dirp)
{
    ext4_dir *dir = _get_ext4_dir(dirp);

    return -ext4_dir_close(dir);
}

static const vfs_file_system_ops_t lwext4_fs_ops = {
#ifdef MODULE_LWEXT4_VFS_FORMAT
    .format = _format,
#endif
    .mount = _mount,
    .umount = _umount,
    .unlink = _unlink,
    .mkdir = _mkdir,
    .rmdir = _rmdir,
    .rename = _rename,
    .stat = vfs_sysop_stat_from_fstat,
    .statvfs = _statvfs,
};

static const vfs_file_ops_t lwext4_file_ops = {
    .open = _open,
    .close = _close,
    .read = _read,
    .write = _write,
    .lseek = _lseek,
    .fstat = _fstat,
    .fsync = _fsync,
};

static const vfs_dir_ops_t lwext4_dir_ops = {
    .opendir = _opendir,
    .readdir = _readdir,
    .closedir = _closedir,
};

const vfs_file_system_t lwext4_file_system = {
    .fs_op = &lwext4_fs_ops,
    .f_op = &lwext4_file_ops,
    .d_op = &lwext4_dir_ops,
    .flags = VFS_FS_FLAG_WANT_ABS_PATH,
};
