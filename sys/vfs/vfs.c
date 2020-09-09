/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_vfs
 * @{
 * @file
 * @brief   VFS layer implementation
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <errno.h> /* for error codes */
#include <string.h> /* for strncmp */
#include <stddef.h> /* for NULL */
#include <sys/types.h> /* for off_t etc */
#include <sys/stat.h> /* for struct stat */
#include <sys/statvfs.h> /* for struct statvfs */
#include <fcntl.h> /* for O_ACCMODE, ..., fcntl */
#include <unistd.h> /* for STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO */

#include "vfs.h"
#include "mutex.h"
#include "thread.h"
#include "kernel_types.h"
#include "clist.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
#if ENABLE_DEBUG
/* Since some of these functions are called by printf, we can't really call
 * printf from our functions or we end up in an infinite recursion. */
#include <unistd.h> /* for STDOUT_FILENO */
#define DEBUG_NOT_STDOUT(fd, ...) if (fd != STDOUT_FILENO) { DEBUG(__VA_ARGS__); }
#else
#define DEBUG_NOT_STDOUT(...)
#endif

/**
 * @internal
 * @brief Array of all currently open files
 *
 * This table maps POSIX fd numbers to vfs_file_t instances
 *
 * @attention STDIN, STDOUT, STDERR will use the three first items in this array.
 */
static vfs_file_t _vfs_open_files[VFS_MAX_OPEN_FILES];

/**
 * @internal
 * @brief List handle for list of all currently mounted file systems
 *
 * This singly linked list is used to dispatch vfs calls to the appropriate file
 * system driver.
 */
static clist_node_t _vfs_mounts_list;

/**
 * @internal
 * @brief Find an unused entry in the _vfs_open_files array and mark it as used
 *
 * If the @p fd argument is non-negative, the allocation fails if the
 * corresponding slot in the open files table is already occupied, no iteration
 * is done to find another free number in this case.
 *
 * If the @p fd argument is negative, the algorithm will iterate through the
 * open files table until it find an unused slot and return the number of that
 * slot.
 *
 * @param[in]  fd  Desired fd number, use VFS_ANY_FD for any free fd
 *
 * @return fd on success
 * @return <0 on error
 */
static inline int _allocate_fd(int fd);

/**
 * @internal
 * @brief Mark an allocated entry as unused in the _vfs_open_files array
 *
 * @param[in]  fd     fd to free
 */
static inline void _free_fd(int fd);

/**
 * @internal
 * @brief Initialize an entry in the _vfs_open_files array and mark it as used.
 *
 * @param[in]  fd           desired fd number, passed to _allocate_fd
 * @param[in]  f_op         pointer to file operations table
 * @param[in]  mountp       pointer to mount table entry, may be NULL
 * @param[in]  flags        file flags
 * @param[in]  private_data private_data initial value
 *
 * @return fd on success
 * @return <0 on error
 */
static inline int _init_fd(int fd, const vfs_file_ops_t *f_op, vfs_mount_t *mountp, int flags, void *private_data);

/**
 * @internal
 * @brief Find the file system associated with the file name @p name, and
 * increment the open_files counter
 *
 * A pointer to the vfs_mount_t associated with the found mount will be written to @p mountpp.
 * A pointer to the mount point-relative file name will be written to @p rel_path.
 *
 * @param[out] mountpp   write address of the found mount to this pointer
 * @param[in]  name      absolute path to file
 * @param[out] rel_path  (optional) output pointer for relative path
 *
 * @return mount index on success
 * @return <0 on error
 */
static inline int _find_mount(vfs_mount_t **mountpp, const char *name, const char **rel_path);

/**
 * @internal
 * @brief Check that a given fd number is valid
 *
 * @param[in]  fd    fd to check
 *
 * @return 0 if the fd is valid
 * @return <0 if the fd is not valid
 */
static inline int _fd_is_valid(int fd);

static mutex_t _mount_mutex = MUTEX_INIT;
static mutex_t _open_mutex = MUTEX_INIT;

int vfs_close(int fd)
{
    DEBUG("vfs_close: %d\n", fd);
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->f_op->close != NULL) {
        /* We will invalidate the fd regardless of the outcome of the file
         * system driver close() call below */
        res = filp->f_op->close(filp);
    }
    _free_fd(fd);
    return res;
}

int vfs_fcntl(int fd, int cmd, int arg)
{
    DEBUG("vfs_fcntl: %d, %d, %d\n", fd, cmd, arg);
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    /* The default fcntl implementation below only allows querying flags,
     * any other command requires insight into the file system driver */
    switch (cmd) {
        case F_GETFL:
            /* Get file flags */
            DEBUG("vfs_fcntl: GETFL: %d\n", filp->flags);
            return filp->flags;
        default:
            break;
    }
    /* pass on to file system driver */
    if (filp->f_op->fcntl != NULL) {
        return filp->f_op->fcntl(filp, cmd, arg);
    }
    return -EINVAL;
}

int vfs_fstat(int fd, struct stat *buf)
{
    DEBUG_NOT_STDOUT(fd, "vfs_fstat: %d, %p\n", fd, (void *)buf);
    if (buf == NULL) {
        return -EFAULT;
    }
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->f_op->fstat == NULL) {
        /* driver does not implement fstat() */
        return -EINVAL;
    }
    return filp->f_op->fstat(filp, buf);
}

int vfs_fstatvfs(int fd, struct statvfs *buf)
{
    DEBUG("vfs_fstatvfs: %d, %p\n", fd, (void *)buf);
    if (buf == NULL) {
        return -EFAULT;
    }
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->mp->fs->fs_op->fstatvfs == NULL) {
        /* file system driver does not implement fstatvfs() */
        if (filp->mp->fs->fs_op->statvfs != NULL) {
            /* Fall back to statvfs */
            return filp->mp->fs->fs_op->statvfs(filp->mp, "/", buf);
        }
        return -EINVAL;
    }
    return filp->mp->fs->fs_op->fstatvfs(filp->mp, filp, buf);
}

off_t vfs_lseek(int fd, off_t off, int whence)
{
    DEBUG("vfs_lseek: %d, %ld, %d\n", fd, (long)off, whence);
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->f_op->lseek == NULL) {
        /* driver does not implement lseek() */
        /* default seek functionality is naive */
        switch (whence) {
            case SEEK_SET:
                break;
            case SEEK_CUR:
                off += filp->pos;
                break;
            case SEEK_END:
                /* we could use fstat here, but most file system drivers will
                 * likely already implement lseek in a more efficient fashion */
                return -EINVAL;
            default:
                return -EINVAL;
        }
        if (off < 0) {
            /* the resulting file offset would be negative */
            return -EINVAL;
        }
        filp->pos = off;

        return off;
    }
    return filp->f_op->lseek(filp, off, whence);
}

int vfs_open(const char *name, int flags, mode_t mode)
{
    DEBUG("vfs_open: \"%s\", 0x%x, 0%03lo\n", name, flags, (long unsigned int)mode);
    if (name == NULL) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res = _find_mount(&mountp, name, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_open: no matching mount\n");
        return res;
    }
    mutex_lock(&_open_mutex);
    int fd = _init_fd(VFS_ANY_FD, mountp->fs->f_op, mountp, flags, NULL);
    mutex_unlock(&_open_mutex);
    if (fd < 0) {
        DEBUG("vfs_open: _init_fd: ERR %d!\n", fd);
        /* remember to decrement the open_files count */
        atomic_fetch_sub(&mountp->open_files, 1);
        return fd;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->f_op->open != NULL) {
        res = filp->f_op->open(filp, rel_path, flags, mode, name);
        if (res < 0) {
            /* something went wrong during open */
            DEBUG("vfs_open: open: ERR %d!\n", res);
            /* clean up */
            _free_fd(fd);
            return res;
        }
    }
    DEBUG("vfs_open: opened %d\n", fd);
    return fd;
}

ssize_t vfs_read(int fd, void *dest, size_t count)
{
    DEBUG("vfs_read: %d, %p, %lu\n", fd, dest, (unsigned long)count);
    if (dest == NULL) {
        return -EFAULT;
    }
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (((filp->flags & O_ACCMODE) != O_RDONLY) & ((filp->flags & O_ACCMODE) != O_RDWR)) {
        /* File not open for reading */
        return -EBADF;
    }
    if (filp->f_op->read == NULL) {
        /* driver does not implement read() */
        return -EINVAL;
    }
    return filp->f_op->read(filp, dest, count);
}


ssize_t vfs_write(int fd, const void *src, size_t count)
{
    DEBUG_NOT_STDOUT(fd, "vfs_write: %d, %p, %lu\n", fd, src, (unsigned long)count);
    if (src == NULL) {
        return -EFAULT;
    }
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (((filp->flags & O_ACCMODE) != O_WRONLY) & ((filp->flags & O_ACCMODE) != O_RDWR)) {
        /* File not open for writing */
        return -EBADF;
    }
    if (filp->f_op->write == NULL) {
        /* driver does not implement write() */
        return -EINVAL;
    }
    return filp->f_op->write(filp, src, count);
}

int vfs_opendir(vfs_DIR *dirp, const char *dirname)
{
    DEBUG("vfs_opendir: %p, \"%s\"\n", (void *)dirp, dirname);
    if ((dirp == NULL) || (dirname == NULL)) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res = _find_mount(&mountp, dirname, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_open: no matching mount\n");
        return res;
    }
    if (rel_path[0] == '\0') {
        /* if the trailing slash is missing we will get an empty string back, to
         * be consistent against the file system drivers we give the relative
         * path "/" instead */
        rel_path = "/";
    }
    if (mountp->fs->d_op == NULL) {
        /* file system driver does not support directories */
        return -EINVAL;
    }
    /* initialize dirp */
    memset(dirp, 0, sizeof(*dirp));
    dirp->mp = mountp;
    dirp->d_op = mountp->fs->d_op;
    if (dirp->d_op->opendir != NULL) {
        int res = dirp->d_op->opendir(dirp, rel_path, dirname);
        if (res < 0) {
            /* remember to decrement the open_files count */
            atomic_fetch_sub(&mountp->open_files, 1);
            return res;
        }
    }
    return 0;
}

int vfs_readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    DEBUG("vfs_readdir: %p, %p\n", (void *)dirp, (void *)entry);
    if ((dirp == NULL) || (entry == NULL)) {
        return -EINVAL;
    }
    if (dirp->d_op != NULL) {
        if (dirp->d_op->readdir != NULL) {
            return dirp->d_op->readdir(dirp, entry);
        }
    }
    return -EINVAL;
}

int vfs_closedir(vfs_DIR *dirp)
{
    DEBUG("vfs_closedir: %p\n", (void *)dirp);
    if (dirp == NULL) {
        return -EINVAL;
    }
    vfs_mount_t *mountp = dirp->mp;
    if (mountp == NULL) {
        return -EBADF;
    }
    int res = 0;
    if (dirp->d_op != NULL) {
        if (dirp->d_op->closedir != NULL) {
            res = dirp->d_op->closedir(dirp);
        }
    }
    memset(dirp, 0, sizeof(*dirp));
    atomic_fetch_sub(&mountp->open_files, 1);
    return res;
}

/**
 * @brief Check if the given mount point is mounted
 *
 * If the mount point is not mounted, _mount_mutex will be locked by this function
 *
 * @param mountp    mount point to check
 * @return 0 on success (mount point is valid and not mounted)
 * @return -EINVAL if mountp is invalid
 * @return -EBUSY if mountp is already mounted
 */
static int check_mount(vfs_mount_t *mountp)
{
    if ((mountp == NULL) || (mountp->fs == NULL) || (mountp->mount_point == NULL)) {
        return -EINVAL;
    }
    DEBUG("vfs_mount: -> \"%s\" (%p), %p\n",
          mountp->mount_point, (void *)mountp->mount_point, mountp->private_data);
    if (mountp->mount_point[0] != '/') {
        DEBUG("vfs: check_mount: not absolute mount_point path\n");
        return -EINVAL;
    }
    mountp->mount_point_len = strlen(mountp->mount_point);
    mutex_lock(&_mount_mutex);
    /* Check for the same mount in the list of mounts to avoid loops */
    clist_node_t *found = clist_find(&_vfs_mounts_list, &mountp->list_entry);
    if (found != NULL) {
        /* Same mount is already mounted */
        mutex_unlock(&_mount_mutex);
        DEBUG("vfs: check_mount: Already mounted\n");
        return -EBUSY;
    }

    return 0;
}

int vfs_format(vfs_mount_t *mountp)
{
    DEBUG("vfs_format: %p\n", (void *)mountp);
    int ret = check_mount(mountp);
    if (ret < 0) {
        return ret;
    }
    mutex_unlock(&_mount_mutex);

    if (mountp->fs->fs_op != NULL) {
        if (mountp->fs->fs_op->format != NULL) {
            return mountp->fs->fs_op->format(mountp);
        }
    }

    /* Format operation not supported */
    return -ENOTSUP;
}

int vfs_mount(vfs_mount_t *mountp)
{
    DEBUG("vfs_mount: %p\n", (void *)mountp);
    int ret = check_mount(mountp);
    if (ret < 0) {
        return ret;
    }

    if (mountp->fs->fs_op != NULL) {
        if (mountp->fs->fs_op->mount != NULL) {
            /* yes, a file system driver does not need to implement mount/umount */
            int res = mountp->fs->fs_op->mount(mountp);
            if (res < 0) {
                DEBUG("vfs_mount: error %d\n", res);
                mutex_unlock(&_mount_mutex);
                return res;
            }
        }
    }
    /* insert last in list */
    clist_rpush(&_vfs_mounts_list, &mountp->list_entry);
    mutex_unlock(&_mount_mutex);
    DEBUG("vfs_mount: mount done\n");
    return 0;
}


int vfs_umount(vfs_mount_t *mountp)
{
    DEBUG("vfs_umount: %p\n", (void *)mountp);
    int ret = check_mount(mountp);
    switch (ret) {
    case 0:
        DEBUG("vfs_umount: not mounted\n");
        mutex_unlock(&_mount_mutex);
        return -EINVAL;
    case -EBUSY:
        /* -EBUSY returned when fs is mounted, just continue */
        break;
    default:
        DEBUG("vfs_umount: invalid fs\n");
        return -EINVAL;
    }
    DEBUG("vfs_umount: -> \"%s\" open=%d\n", mountp->mount_point, atomic_load(&mountp->open_files));
    if (atomic_load(&mountp->open_files) > 0) {
        mutex_unlock(&_mount_mutex);
        return -EBUSY;
    }
    if (mountp->fs->fs_op != NULL) {
        if (mountp->fs->fs_op->umount != NULL) {
            int res = mountp->fs->fs_op->umount(mountp);
            if (res < 0) {
                /* umount failed */
                DEBUG("vfs_umount: ERR %d!\n", res);
                mutex_unlock(&_mount_mutex);
                return res;
            }
        }
    }
    /* find mountp in the list and remove it */
    clist_node_t *node = clist_remove(&_vfs_mounts_list, &mountp->list_entry);
    if (node == NULL) {
        /* not found */
        DEBUG("vfs_umount: ERR not mounted!\n");
        mutex_unlock(&_mount_mutex);
        return -EINVAL;
    }
    mutex_unlock(&_mount_mutex);
    return 0;
}

int vfs_rename(const char *from_path, const char *to_path)
{
    DEBUG("vfs_rename: \"%s\", \"%s\"\n", from_path, to_path);
    if ((from_path == NULL) || (to_path == NULL)) {
        return -EINVAL;
    }
    const char *rel_from;
    vfs_mount_t *mountp;
    int res = _find_mount(&mountp, from_path, &rel_from);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_rename: from: no matching mount\n");
        return res;
    }
    if ((mountp->fs->fs_op == NULL) || (mountp->fs->fs_op->rename == NULL)) {
        /* rename not supported */
        DEBUG("vfs_rename: rename not supported by fs!\n");
        /* remember to decrement the open_files count */
        atomic_fetch_sub(&mountp->open_files, 1);
        return -EPERM;
    }
    const char *rel_to;
    vfs_mount_t *mountp_to;
    res = _find_mount(&mountp_to, to_path, &rel_to);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_rename: to: no matching mount\n");
        /* remember to decrement the open_files count */
        atomic_fetch_sub(&mountp->open_files, 1);
        return res;
    }
    if (mountp_to != mountp) {
        /* The paths are on different file systems */
        DEBUG("vfs_rename: from_path and to_path are on different mounts\n");
        /* remember to decrement the open_files count */
        atomic_fetch_sub(&mountp->open_files, 1);
        atomic_fetch_sub(&mountp_to->open_files, 1);
        return -EXDEV;
    }
    res = mountp->fs->fs_op->rename(mountp, rel_from, rel_to);
    DEBUG("vfs_rename: rename %p, \"%s\" -> \"%s\"", (void *)mountp, rel_from, rel_to);
    if (res < 0) {
        /* something went wrong during rename */
        DEBUG(": ERR %d!\n", res);
    }
    else {
        DEBUG("\n");
    }
    /* remember to decrement the open_files count */
    atomic_fetch_sub(&mountp->open_files, 1);
    atomic_fetch_sub(&mountp_to->open_files, 1);
    return res;
}

/* TODO: Share code between vfs_unlink, vfs_mkdir, vfs_rmdir since they are almost identical */

int vfs_unlink(const char *name)
{
    DEBUG("vfs_unlink: \"%s\"\n", name);
    if (name == NULL) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res;
    res = _find_mount(&mountp, name, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_unlink: no matching mount\n");
        return res;
    }
    if ((mountp->fs->fs_op == NULL) || (mountp->fs->fs_op->unlink == NULL)) {
        /* unlink not supported */
        DEBUG("vfs_unlink: unlink not supported by fs!\n");
        /* remember to decrement the open_files count */
        atomic_fetch_sub(&mountp->open_files, 1);
        return -EPERM;
    }
    res = mountp->fs->fs_op->unlink(mountp, rel_path);
    DEBUG("vfs_unlink: unlink %p, \"%s\"", (void *)mountp, rel_path);
    if (res < 0) {
        /* something went wrong during unlink */
        DEBUG(": ERR %d!\n", res);
    }
    else {
        DEBUG("\n");
    }
    /* remember to decrement the open_files count */
    atomic_fetch_sub(&mountp->open_files, 1);
    return res;
}

int vfs_mkdir(const char *name, mode_t mode)
{
    DEBUG("vfs_mkdir: \"%s\", 0%03lo\n", name, (long unsigned int)mode);
    if (name == NULL) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res;
    res = _find_mount(&mountp, name, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_mkdir: no matching mount\n");
        return res;
    }
    if ((mountp->fs->fs_op == NULL) || (mountp->fs->fs_op->mkdir == NULL)) {
        /* mkdir not supported */
        DEBUG("vfs_mkdir: mkdir not supported by fs!\n");
        /* remember to decrement the open_files count */
        atomic_fetch_sub(&mountp->open_files, 1);
        return -EPERM;
    }
    res = mountp->fs->fs_op->mkdir(mountp, rel_path, mode);
    DEBUG("vfs_mkdir: mkdir %p, \"%s\"", (void *)mountp, rel_path);
    if (res < 0) {
        /* something went wrong during mkdir */
        DEBUG(": ERR %d!\n", res);
    }
    else {
        DEBUG("\n");
    }
    /* remember to decrement the open_files count */
    atomic_fetch_sub(&mountp->open_files, 1);
    return res;
}

int vfs_rmdir(const char *name)
{
    DEBUG("vfs_rmdir: \"%s\"\n", name);
    if (name == NULL) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res;
    res = _find_mount(&mountp, name, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_rmdir: no matching mount\n");
        return res;
    }
    if ((mountp->fs->fs_op == NULL) || (mountp->fs->fs_op->rmdir == NULL)) {
        /* rmdir not supported */
        DEBUG("vfs_rmdir: rmdir not supported by fs!\n");
        /* remember to decrement the open_files count */
        atomic_fetch_sub(&mountp->open_files, 1);
        return -EPERM;
    }
    res = mountp->fs->fs_op->rmdir(mountp, rel_path);
    DEBUG("vfs_rmdir: rmdir %p, \"%s\"", (void *)mountp, rel_path);
    if (res < 0) {
        /* something went wrong during rmdir */
        DEBUG(": ERR %d!\n", res);
    }
    else {
        DEBUG("\n");
    }
    /* remember to decrement the open_files count */
    atomic_fetch_sub(&mountp->open_files, 1);
    return res;
}

int vfs_stat(const char *restrict path, struct stat *restrict buf)
{
    DEBUG("vfs_stat: \"%s\", %p\n", path, (void *)buf);
    if (path == NULL || buf == NULL) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res;
    res = _find_mount(&mountp, path, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_stat: no matching mount\n");
        return res;
    }
    if ((mountp->fs->fs_op == NULL) || (mountp->fs->fs_op->stat == NULL)) {
        /* stat not supported */
        DEBUG("vfs_stat: stat not supported by fs!\n");
        /* remember to decrement the open_files count */
        atomic_fetch_sub(&mountp->open_files, 1);
        return -EPERM;
    }
    res = mountp->fs->fs_op->stat(mountp, rel_path, buf);
    /* remember to decrement the open_files count */
    atomic_fetch_sub(&mountp->open_files, 1);
    return res;
}

int vfs_statvfs(const char *restrict path, struct statvfs *restrict buf)
{
    DEBUG("vfs_statvfs: \"%s\", %p\n", path, (void *)buf);
    if (path == NULL || buf == NULL) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res;
    res = _find_mount(&mountp, path, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_statvfs: no matching mount\n");
        return res;
    }
    if ((mountp->fs->fs_op == NULL) || (mountp->fs->fs_op->statvfs == NULL)) {
        /* statvfs not supported */
        DEBUG("vfs_statvfs: statvfs not supported by fs!\n");
        /* remember to decrement the open_files count */
        atomic_fetch_sub(&mountp->open_files, 1);
        return -EPERM;
    }
    res = mountp->fs->fs_op->statvfs(mountp, rel_path, buf);
    /* remember to decrement the open_files count */
    atomic_fetch_sub(&mountp->open_files, 1);
    return res;
}

int vfs_bind(int fd, int flags, const vfs_file_ops_t *f_op, void *private_data)
{
    DEBUG("vfs_bind: %d, %d, %p, %p\n", fd, flags, (void*)f_op, private_data);
    if (f_op == NULL) {
        return -EINVAL;
    }
    mutex_lock(&_open_mutex);
    fd = _init_fd(fd, f_op, NULL, flags, private_data);
    mutex_unlock(&_open_mutex);
    if (fd < 0) {
        DEBUG("vfs_bind: _init_fd: ERR %d!\n", fd);
        return fd;
    }
    DEBUG("vfs_bind: bound %d\n", fd);
    return fd;
}

int vfs_normalize_path(char *buf, const char *path, size_t buflen)
{
    DEBUG("vfs_normalize_path: %p, \"%s\" (%p), %lu\n",
          (void *)buf, path, (void *)path, (unsigned long)buflen);
    size_t len = 0;
    int npathcomp = 0;
    const char *path_end = path + strlen(path); /* Find the terminating null byte */
    if (len >= buflen) {
        return -ENAMETOOLONG;
    }

    while(path <= path_end) {
        DEBUG("vfs_normalize_path: + %d \"%.*s\" <- \"%s\" (%p)\n",
              npathcomp, (int)len, buf, path, (void *)path);
        if (path[0] == '\0') {
            break;
        }
        while (path[0] == '/') {
            /* skip extra slashes */
            ++path;
        }
        if (path[0] == '.') {
            ++path;
            if (path[0] == '/' || path[0] == '\0') {
                /* skip /./ components */
                DEBUG("vfs_normalize_path: skip .\n");
                continue;
            }
            if (path[0] == '.' && (path[1] == '/' || path[1] == '\0')) {
                DEBUG("vfs_normalize_path: reduce ../\n");
                if (len == 0) {
                    /* outside root */
                    return -EINVAL;
                }
                ++path;
                /* delete the last component of the path */
                while (len > 0 && buf[--len] != '/') {}
                --npathcomp;
                continue;
            }
        }
        buf[len++] = '/';
        if (len >= buflen) {
            return -ENAMETOOLONG;
        }
        if (path[0] == '\0') {
            /* trailing slash in original path, don't increment npathcomp */
            break;
        }
        ++npathcomp;
        /* copy the path component */
        while (len < buflen && path[0] != '/' && path[0] != '\0') {
            buf[len++] = path[0];
            ++path;
        }
        if (len >= buflen) {
            return -ENAMETOOLONG;
        }
    }
    /* special case for "/": (otherwise it will be zero) */
    if (len == 1) {
        npathcomp = 1;
    }
    buf[len] = '\0';
    DEBUG("vfs_normalize_path: = %d, \"%s\"\n", npathcomp, buf);
    return npathcomp;
}

const vfs_mount_t *vfs_iterate_mounts(const vfs_mount_t *cur)
{
    clist_node_t *node;
    if (cur == NULL) {
        node = _vfs_mounts_list.next;
        if (node == NULL) {
            /* empty list */
            return NULL;
        }
    }
    else {
        node = cur->list_entry.next;
        if (node == _vfs_mounts_list.next) {
            return NULL;
        }
    }
    return container_of(node, vfs_mount_t, list_entry);
}

const vfs_file_t *vfs_file_get(int fd)
{
    if (_fd_is_valid(fd) == 0) {
        return &_vfs_open_files[fd];
    }
    else {
        return NULL;
    }
}

static inline int _allocate_fd(int fd)
{
    if (fd < 0) {
        for (fd = 0; fd < VFS_MAX_OPEN_FILES; ++fd) {
            if ((fd == STDIN_FILENO) || (fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
                /* Do not auto-allocate the stdio file descriptor numbers to
                 * avoid conflicts between normal file system users and stdio
                 * drivers such as stdio_uart, stdio_rtt which need to be able
                 * to bind to these specific file descriptor numbers. */
                continue;
            }
            if (_vfs_open_files[fd].pid == KERNEL_PID_UNDEF) {
                break;
            }
        }
    }
    if (fd >= VFS_MAX_OPEN_FILES) {
        /* The _vfs_open_files array is full */
        return -ENFILE;
    }
    else if (_vfs_open_files[fd].pid != KERNEL_PID_UNDEF) {
        /* The desired fd is already in use */
        return -EEXIST;
    }
    kernel_pid_t pid = thread_getpid();
    if (pid == KERNEL_PID_UNDEF) {
        /* This happens when calling vfs_bind during boot, before threads have
         * been started. */
        pid = -1;
    }
    _vfs_open_files[fd].pid = pid;
    return fd;
}

static inline void _free_fd(int fd)
{
    DEBUG("_free_fd: %d, pid=%d\n", fd, _vfs_open_files[fd].pid);
    if (_vfs_open_files[fd].mp != NULL) {
        atomic_fetch_sub(&_vfs_open_files[fd].mp->open_files, 1);
    }
    _vfs_open_files[fd].pid = KERNEL_PID_UNDEF;
}

static inline int _init_fd(int fd, const vfs_file_ops_t *f_op, vfs_mount_t *mountp, int flags, void *private_data)
{
    fd = _allocate_fd(fd);
    if (fd < 0) {
        return fd;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    filp->mp = mountp;
    filp->f_op = f_op;
    filp->flags = flags;
    filp->pos = 0;
    filp->private_data.ptr = private_data;
    return fd;
}

static inline int _find_mount(vfs_mount_t **mountpp, const char *name, const char **rel_path)
{
    size_t longest_match = 0;
    size_t name_len = strlen(name);
    mutex_lock(&_mount_mutex);

    clist_node_t *node = _vfs_mounts_list.next;
    if (node == NULL) {
        /* list empty */
        mutex_unlock(&_mount_mutex);
        return -ENOENT;
    }
    vfs_mount_t *mountp = NULL;
    do {
        node = node->next;
        vfs_mount_t *it = container_of(node, vfs_mount_t, list_entry);
        size_t len = it->mount_point_len;
        if (len < longest_match) {
            /* Already found a longer prefix */
            continue;
        }
        if (len > name_len) {
            /* path name is shorter than the mount point name */
            continue;
        }
        if ((len > 1) && (name[len] != '/') && (name[len] != '\0')) {
            /* name does not have a directory separator where mount point name ends */
            continue;
        }
        if (strncmp(name, it->mount_point, len) == 0) {
            /* mount_point is a prefix of name */
            /* special check for mount_point == "/" */
            if (len > 1) {
                longest_match = len;
            }
            mountp = it;
        }
    } while (node != _vfs_mounts_list.next);
    if (mountp == NULL) {
        /* not found */
        mutex_unlock(&_mount_mutex);
        return -ENOENT;
    }
    /* Increment open files counter for this mount */
    atomic_fetch_add(&mountp->open_files, 1);
    mutex_unlock(&_mount_mutex);
    *mountpp = mountp;
    if (rel_path != NULL) {
        *rel_path = name + longest_match;
    }
    return 0;
}

static inline int _fd_is_valid(int fd)
{
    if ((unsigned int)fd >= VFS_MAX_OPEN_FILES) {
        return -EBADF;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->pid == KERNEL_PID_UNDEF) {
        return -EBADF;
    }
    if (filp->f_op == NULL) {
        return -EBADF;
    }
    return 0;
}

/** @} */
