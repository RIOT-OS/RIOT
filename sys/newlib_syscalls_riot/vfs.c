/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup    sys_newlib_syscalls_riot
 * @{
 *
 * @file
 * @brief       Newlib <-> VFS translation layer.
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <reent.h>
#include <errno.h>

#include "vfs.h"
#include "stdio_base.h"

/**
 * @brief Open a file
 *
 * This is a wrapper around @c vfs_open
 *
 * @param r     pointer to reent structure
 * @param name  file name to open
 * @param flags flags, see man 3p open
 * @param mode  mode, file creation mode if the file is created when opening
 *
 * @return      fd number (>= 0) on success
 * @return      -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _open_r(struct _reent *r, const char *name, int flags, int mode)
{
    int fd = vfs_open(name, flags, mode);
    if (fd < 0) {
        /* vfs returns negative error codes */
        r->_errno = -fd;
        return -1;
    }
    return fd;
}

/**
 * @brief Read bytes from an open file
 *
 * This is a wrapper around @c vfs_read
 *
 * @param[in]  r      pointer to reent structure
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[out] dest   destination buffer
 * @param[in]  count  maximum number of bytes to read
 *
 * @return       number of bytes read on success
 * @return       -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
_ssize_t _read_r(struct _reent *r, int fd, void *dest, size_t count)
{
    int res = vfs_read(fd, dest, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return res;
}

/**
 * @brief Write bytes to an open file
 *
 * This is a wrapper around @c vfs_write
 *
 * @param[in]  r      pointer to reent structure
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[in]  src    source data buffer
 * @param[in]  count  maximum number of bytes to write
 *
 * @return       number of bytes written on success
 * @return       -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
_ssize_t _write_r(struct _reent *r, int fd, const void *src, size_t count)
{
    int res = vfs_write(fd, src, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return res;
}

/**
 * @brief Close an open file
 *
 * This is a wrapper around @c vfs_close
 *
 * If this call returns an error, the fd should still be considered invalid and
 * no further attempt to use it shall be made, not even to retry @c close()
 *
 * @param[in]  r      pointer to reent structure
 * @param[in]  fd     open file descriptor obtained from @c open()
 *
 * @return       0 on success
 * @return       -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _close_r(struct _reent *r, int fd)
{
    int res = vfs_close(fd);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return res;
}

/**
 * @brief Query or set options on an open file
 *
 * This is a wrapper around @c vfs_fcntl
 *
 * @param[in]  r      pointer to reent structure
 * @param[in]  fd     open file descriptor obtained from @c open()
 * @param[in]  cmd    fcntl command, see man 3p fcntl
 * @param[in]  arg    argument to fcntl command, see man 3p fcntl
 *
 * @return       0 on success
 * @return       -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _fcntl_r (struct _reent *r, int fd, int cmd, int arg)
{
    int res = vfs_fcntl(fd, cmd, arg);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return res;
}

/**
 * @brief Seek to position in file
 *
 * This is a wrapper around @c vfs_lseek
 *
 * @p whence determines the function of the seek and should be set to one of
 * the following values:
 *
 *  - @c SEEK_SET: Seek to absolute offset @p off
 *  - @c SEEK_CUR: Seek to current location + @p off
 *  - @c SEEK_END: Seek to end of file + @p off
 *
 * @param[in]  r        pointer to reent structure
 * @param[in]  fd       open file descriptor obtained from @c open()
 * @param[in]  off      seek offset
 * @param[in]  whence   determines the seek method, see detailed description
 *
 * @return the new seek location in the file on success
 * @return -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
_off_t _lseek_r(struct _reent *r, int fd, _off_t off, int whence)
{
    int res = vfs_lseek(fd, off, whence);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return res;
}

/**
 * @brief Get status of an open file
 *
 * This is a wrapper around @c vfs_fstat
 *
 * @param[in]  r        pointer to reent structure
 * @param[in]  fd       open file descriptor obtained from @c open()
 * @param[out] buf      pointer to stat struct to fill
 *
 * @return 0 on success
 * @return -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _fstat_r(struct _reent *r, int fd, struct stat *buf)
{
    int res = vfs_fstat(fd, buf);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return 0;
}

/**
 * @brief Status of a file (by name)
 *
 * This is a wrapper around @c vfs_fstat
 *
 * @param[in]  r        pointer to reent structure
 * @param[in]  name     path to file
 * @param[out] buf      pointer to stat struct to fill
 *
 * @return 0 on success
 * @return -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _stat_r(struct _reent *r, const char *name, struct stat *st)
{
    int res = vfs_stat(name, st);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return 0;
}

/**
 * @brief  Unlink (delete) a file
 *
 * @param[in]  r        pointer to reent structure
 * @param[in]  path     path to file to be deleted
 *
 * @return 0 on success
 * @return -1 on error, @c r->_errno set to a constant from errno.h to indicate the error
 */
int _unlink_r(struct _reent *r, const char *path)
{
    int res = vfs_unlink(path);
    if (res < 0) {
        /* vfs returns negative error codes */
        r->_errno = -res;
        return -1;
    }
    return 0;
}

/*
 * VFS does not (yet) support hard links (that's why the error is ENOSYS instead
 * of ENODEV.
 */
int _link_r(struct _reent *ptr, const char *old_name, const char *new_name)
{
    (void)old_name;
    (void)new_name;

    ptr->_errno = ENOSYS;

    return -1;
}
