/*
 * Copyright (C) 2017 Imagination Technologies
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
  * @ingroup    sys_newlib
  * @{
  *
  * @file
  * @brief Newlib system call implementation for use with the mips-mti-elf
  * toolchain newlib incorporating as semi-hosting interface called 'UHI'
  *
  * @author Neil Jones <neil.jones@imgtec.com>
  *
  * @}
  */

#include <unistd.h>
#include <reent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <mips/hal.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "log.h"
#include "periph/pm.h"

/**
 * * @brief manage the heap
 * */
extern char _sheap; /* start of the heap */
extern char _eheap; /* end of the heap */
char *heap_top = &_sheap + 4;

/* Only need to define these when MIPS newlib is not used */
#ifndef __mips__
/**
 * @brief Free resources on NewLib de-initialization, not used for RIOT
 */

/* __attribute__((used)) fixes linker errors when building with LTO, but without nano.specs */
__attribute__((used)) void _fini(void)
{
    /* nothing to do here */
}

/**
* @brief Exit a program without cleaning up files
*
* If your system doesn't provide this, it is best to avoid linking with subroutines that
* require it (exit, system).
*
* @param n the exit code, 0 for all OK, >0 for not OK
*/

void _exit(int n)
{
    exit(n);

    /* cppcheck-suppress unreachableCode
     * (reason: pm_off spins indefinitely after pulling the plug) */
    pm_off();
}

/**
* @brief Allocate memory from the heap.
*
* The current heap implementation is very rudimentary, it is only able to allocate
* memory. But it does not have any means to free memory again
*
* @return pointer to the newly allocated memory on success
* @return pointer set to address `-1` on failure
*/

void *_sbrk_r(struct _reent *r, ptrdiff_t incr)
{
    unsigned int state = irq_disable();
    void *res = heap_top;
    if ((heap_top + incr > &_eheap) || (heap_top + incr < &_sheap)) {
        r->_errno = ENOMEM;
        res = (void *)-1;
    } else {
        heap_top += incr;
    }
    irq_restore(state);
    return res;
}
#endif /*__mips__*/

/**
* @brief Get the process-ID of the current thread
*
* @return the process ID of the current thread
*/
pid_t _getpid(void)
{
    return thread_getpid();
}

/**
* @brief Get the process-ID of the current thread
*
* @return the process ID of the current thread
*/
pid_t _getpid_r(struct _reent *ptr)
{
    (void)ptr;
    return thread_getpid();
}

/**
* @brief Send a signal to a given thread
*
* @param r pointer to reent structure
* @param pid process ID to kill
* @param sig signal number to pass to process
*
* @return -1 on error
* @return 0 on success
*/
__attribute__ ((weak))
int _kill_r(struct _reent *r, pid_t pid, int sig)
{
    (void)pid;
    (void)sig;
    r->_errno = ESRCH; /* not implemented yet */
    return -1;
}

/**
* @brief Open a file
*
* This is a wrapper around @c _open
*
* @param r pointer to reent structure
* @param name file name to open
* @param flags flags, see man 3p open
* @param mode mode, file creation mode if the file is created when opening
*
* @return fd number (>= 0) on success
* @return -1 on error
*/
int _open_r(struct _reent *r, const char *name, int flags, int mode)
{
    (void)r;
    return open(name, flags, mode);
}

/**
* @brief Read bytes from an open file
*
* This is a wrapper around @c _read
*
* @param[in] r pointer to reent structure
* @param[in] fd open file descriptor obtained from @c open()
* @param[out] dest destination buffer
* @param[in] count maximum number of bytes to read
*
* @return number of bytes read on success
* @return -1 on error,
*/
_ssize_t _read_r(struct _reent *r, int fd, void *dest, size_t count)
{
    (void)r;
    return read(fd,dest,count);
}

/**
* @brief Write bytes to an open file
*
* This is a wrapper around @c _write
*
* @param[in] r pointer to reent structure
* @param[in] fd open file descriptor obtained from @c open()
* @param[in] src source data buffer
* @param[in] count maximum number of bytes to write
*
* @return number of bytes written on success
* @return -1 on error
*/
_ssize_t _write_r(struct _reent *r, int fd, const void *src, size_t count)
{
    (void)r;
    int res = write(fd, src, count);
    return res;
}

/**
* @brief Close an open file
*
* This is a wrapper around @c _close
*
* If this call returns an error, the fd should still be considered invalid and
* no further attempt to use it shall be made, not even to retry @c close()
*
* @param[in] r pointer to reent structure
* @param[in] fd open file descriptor obtained from @c open()
*
* @return 0 on success
* @return -1 on error
*/
int _close_r(struct _reent *r, int fd)
{
    (void)r;
    int res = close(fd);
    return res;
}

/**
* @brief Query or set options on an open file
*
* This is a wrapper around @c _fcntl
*
* @param[in] r pointer to reent structure
* @param[in] fd open file descriptor obtained from @c open()
* @param[in] cmd fcntl command, see man 3p fcntl
* @param[in] arg argument to fcntl command, see man 3p fcntl
*
* @return 0 on success
* @return -1 on error
*/
int _fcntl_r (struct _reent *r, int fd, int cmd, int arg)
{
    (void)r;
    int res = fcntl(fd, cmd, arg);
    return res;
}

/**
* @brief Seek to position in file
*
* This is a wrapper around @c _lseek
*
* @p whence determines the function of the seek and should be set to one of
* the following values:
*
* - @c SEEK_SET: Seek to absolute offset @p off
* - @c SEEK_CUR: Seek to current location + @p off
* - @c SEEK_END: Seek to end of file + @p off
*
* @param[in] r pointer to reent structure
* @param[in] fd open file descriptor obtained from @c open()
* @param[in] off seek offset
* @param[in] whence determines the seek method, see detailed description
*
* @return the new seek location in the file on success
* @return -1 on error
*/
_off_t _lseek_r(struct _reent *r, int fd, _off_t off, int whence)
{
    (void)r;
    int res = lseek(fd, off, whence);
    return res;
}

/**
* @brief Get status of an open file
*
* This is a wrapper around @c _fstat
*
* @param[in] r pointer to reent structure
* @param[in] fd open file descriptor obtained from @c open()
* @param[out] buf pointer to stat struct to fill
*
* @return 0 on success
* @return -1 on error
*/
int _fstat_r(struct _reent *r, int fd, struct stat *buf)
{
    (void)r;
    int res = fstat(fd, buf);
    return res;
}

/*
* @brief Unlink (delete) a file
*
* @param[in] r pointer to reent structure
* @param[in] path path to file to be deleted
*
* @return 0 on success
* @return -1 on error
*/
int _unlink_r(struct _reent *r, const char *path)
{
    (void)r;
    int res = unlink(path);
    return res;
}

/**
* @brief Query whether output stream is a terminal
*
* @param r pointer to reent structure
* @param fd descriptor of stream to query
*
* @return 0 for none tty
* @return 1 for tty
*
*/
int _isatty_r(struct _reent *r, int fd)
{
    r->_errno = 0;
    if(fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return 1;
    }
    return 0;
}

/**
* @brief Send a signal to a thread
*
* @param[in] pid the pid to send to
* @param[in] sig the signal to send
*
* @return 0 on success
* @return -1 on error
*
*/
__attribute__ ((weak))
int _kill(pid_t pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = ESRCH; /* not implemented yet */
    return -1;
}
