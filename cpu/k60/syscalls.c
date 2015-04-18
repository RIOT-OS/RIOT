/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */


#include <unistd.h>
#include <sys/reent.h>
#include <sys/times.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "cpu.h"
#include "board.h"
#include "devopttab.h"
#include "devicemap.h"
#include "thread.h"
#if CFS_ENABLED
#include "cfs.h"
#endif /* CFS_ENABLED */
#include "mutex.h"
#include "ringbuffer.h"
#include "periph/uart.h"
#ifdef MODULE_UART0
#include "board_uart0.h"
#endif

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief           Syscall implementations for K60 CPU.
 *
 * @author          Joakim Gebart <joakim.gebart@eistec.se>
 */


/* Empty environment definition */
char *__env[1] = { 0 };
char **environ = __env;

/* Lock variable used to protect sbrk_r from clobbering the break variable when
 * called simultaneously from more than one thread. */
static mutex_t sbrk_mutex = MUTEX_INIT;

/* Align all sbrk arguments to this many bytes */
#define DYNAMIC_MEMORY_ALIGN 4

#ifndef MODULE_UART0
/**
 * @brief use mutex for waiting on incoming UART chars
 */
static mutex_t uart_rx_mutex;
static char rx_buf_mem[STDIO_RX_BUFSIZE];
static ringbuffer_t rx_buf;
#endif

/**
 * @brief Receive a new character from the UART and put it into the receive buffer
 */
static void stdio_rx_cb(void *arg, char data)
{
#ifndef MODULE_UART0
    (void)arg;

    ringbuffer_add_one(&rx_buf, data);
    mutex_unlock(&uart_rx_mutex);
#else

    if (uart0_handler_pid) {
        uart0_handle_incoming(data);

        uart0_notify_thread();
    }

#endif
}

/**
 * @brief Initialize NewLib, called by __libc_init_array() from the startup script
 */
void _init(void)
{
#ifndef MODULE_UART0
    mutex_init(&uart_rx_mutex);
    ringbuffer_init(&rx_buf, rx_buf_mem, STDIO_RX_BUFSIZE);
#endif
    uart_init(STDIO, STDIO_BAUDRATE, stdio_rx_cb, 0, 0);
}

/**
 * @brief Free resources on NewLib de-initialization, not used for RIOT
 */
void _fini(void)
{
    /* nothing to do here */
}


/* ************************ */
/* Process control syscalls */
/* ************************ */

void
_exit(int code)
{
#if DEVELHELP
    volatile int status; /* volatile to prevent optimizations to remove the variable from memory */
    status = code;
    (void)status; /* Suppress compiler warnings about unused variable */

    /* See local variable `status` during debugger break. */
    asm volatile ("bkpt #0");
#else
    NVIC_SystemReset();
#endif

    while (1);
}

int
_fork_r(struct _reent *r)
{
    /* return "not supported" */
    r->_errno = ENOTSUP;
    return -1;
}

int
_execve_r(struct _reent *r, const char *name, char *const *argv, char *const *env)
{
    /* Not supported */
    (void)name; /* Suppress compiler warnings about unused parameters */
    (void)argv;
    (void)env;

    r->_errno = ENOMEM;
    return -1;
}

int
_kill_r(struct _reent *r, int  pid, int  sig)
{
    /* Not supported */
    (void)pid; /* Suppress compiler warnings about unused parameters */
    (void)sig;

    r->_errno = EINVAL;
    return -1;
}

pid_t
_getpid(void)
{
    return sched_active_pid;
}

pid_t
_getpid_r(struct _reent *ptr)
{
    (void) ptr;
    return sched_active_pid;
}

clock_t
_times_r(struct _reent *r, struct tms *buf)
{
    /* Not supported, yet */
    (void)buf; /* Suppress compiler warnings about unused parameters */

    r->_errno = EACCES;
    return  -1;
}

int
_wait_r(struct _reent *r, int *status)
{
    /* Not supported, yet */
    (void)status; /* Suppress compiler warnings about unused parameters */

    r->_errno = ECHILD;
    return -1;
}

/* ******************************** */
/* File descriptor related syscalls */
/* ******************************** */

/**
 * @brief Internal helper for generating FDs
 *
 * @return An unallocated file descriptor, -1 if no free FD can be found.
 */
static int get_next_dev_fd(void);

static int get_next_dev_fd(void)
{
    int fd;

    for (fd = 0; fd < MAX_OPEN_DEVICES; ++fd) {
        if (devoptab_list[fd] == NULL) {
            return fd;
        }
    }

    return -1;
}

int
_open_r(struct _reent *r, const char *name, int flags, int mode)
{
    unsigned int i;
    int fd;
#if CFS_ENABLED
    int cfs_flags = 0;
#endif

    /* Search for devices */
    for (i = 0; i < devoptab_name_list.len; ++i) {
        if (strcmp(devoptab_name_list.data[i].name, name) == 0) {
            /* Device found */
            fd = get_next_dev_fd();

            if (fd < 0) {
                /* No free FDs. */
                /* ENFILE means too many file descriptors open, system-wide. */
                r->_errno = ENFILE;
                return -1;
            }

            /* Set up device operations table and call open method */
            devoptab_list[fd] = devoptab_name_list.data[i].devoptab;
            /* open_r method is mandatory */
            devoptab_list[fd]->open_r(r, name, flags, mode);
            return fd;
        }
    }

#if CFS_ENABLED

    /* Not a device name, try searching for files. */
    /* Translate POSIX O_* flags to CFS */
    if (flags & O_APPEND) {
        cfs_flags |= CFS_APPEND;
    }

    if (flags & O_RDWR) {
        cfs_flags |= CFS_READ | CFS_WRITE;
    }

    if (flags & O_RDONLY) {
        cfs_flags |= CFS_READ;
    }

    if (flags & O_WRONLY) {
        cfs_flags |= CFS_WRITE;
    }

    fd = cfs_open(name, cfs_flags);

    if (fd < 0) {
        /* Not found or whatever, CFS doesn't tell us why it failed. */
        r->_errno = ENOENT;
        return -1;
    }

    fd += MAX_OPEN_DEVICES; /* Remap from CFS FD number */
    return fd;
#else
    r->_errno = ENOENT;
    return -1;
#endif

}

int
_close_r(struct _reent *r, int fd)
{
    int ret;

    if (fd < 0) {
        /* invalid file descriptor */
        r->_errno = EBADF;
        return -1;
    }

    if (fd >= MAX_OPEN_DEVICES) {
#if CFS_ENABLED
        /* CFS file */
        fd -= MAX_OPEN_DEVICES; /* Remap to CFS FD number */
        cfs_close(fd);
#endif
        return 0; /* cfs_close does not indicate failures */
    }

    if (devoptab_list[fd] == NULL) {
        /* nothing mapped on that FD */
        r->_errno = EBADF;
        return -1;
    }

    if (devoptab_list[fd]->close_r == NULL) {
        /* Function not implemented */
        r->_errno = ENOSYS;
        return -1;
    }

    /* Call method from device operations table */
    ret = devoptab_list[fd]->close_r(r, fd);

    if (ret == 0) {
        /* Successfully closed, clear out device operations table entry to free up
         * the file descriptor. */
        devoptab_list[fd] = NULL;
    }

    return ret;
}

ssize_t
_read_r(struct _reent *r, int fd, void *ptr, size_t len)
{

    if (fd < 0) {
        /* invalid file descriptor */
        r->_errno = EBADF;
        return -1;
    }

    if (fd >= MAX_OPEN_DEVICES) {
#if CFS_ENABLED
        int ret;
        /* CFS file */
        fd -= MAX_OPEN_DEVICES; /* Remap to CFS FD number */
        /* this is not really reentrant */
        ret = cfs_read(fd, ptr, len);

        if (ret < 0) {
            r->_errno = EBADF;
        }

        return ret;
#else
        r->_errno = EBADF;
        return -1;
#endif
    }

    if (devoptab_list[fd] == NULL) {
        /* nothing mapped on that FD */
        r->_errno = EBADF;
        return -1;
    }

    if (devoptab_list[fd]->read_r == NULL) {
        /* Function not implemented */
        r->_errno = ENOSYS;
        return -1;
    }

    /* Call method from device operations table */
    return devoptab_list[fd]->read_r(r, fd, ptr, len);
}

ssize_t
_write_r(struct _reent *r, int fd, const void *ptr, size_t len)
{

    if (fd < 0) {
        /* invalid file descriptor */
        r->_errno = EBADF;
        return -1;
    }

    if (fd >= MAX_OPEN_DEVICES) {
#if CFS_ENABLED
        int ret;
        /* CFS file */
        fd -= MAX_OPEN_DEVICES; /* Remap to CFS FD number */
        /* this is not really reentrant */
        ret = cfs_write(fd, (const char *)ptr, len);

        if (ret < 0) {
            r->_errno = EBADF;
        }

        return ret;
#else
        r->_errno = EBADF;
        return -1;
#endif
    }

    if (devoptab_list[fd] == NULL) {
        /* nothing mapped on that FD */
        r->_errno = EBADF;
        return -1;
    }

    if (devoptab_list[fd]->write_r == NULL) {
        /* Function not implemented */
        r->_errno = ENOSYS;
        return -1;
    }

    /* Call method from device operations table */
    return devoptab_list[fd]->write_r(r, fd, ptr, len);
}

off_t
_lseek_r(struct _reent *r, int fd, off_t offset, int whence)
{

    if (fd < 0) {
        /* invalid file descriptor */
        r->_errno = EBADF;
        return -1;
    }

    if (fd >= MAX_OPEN_DEVICES) {
#if CFS_ENABLED
        int ret;
        /* CFS file */
        fd -= MAX_OPEN_DEVICES; /* Remap to CFS FD number */
        /* CFS_SEEK_* macros used by the CFS whence parameter is assumed to
         * correspond with POSIX constants */
        /* this is not really reentrant */
        ret = cfs_seek(fd, offset, whence);

        if (ret < 0) {
            r->_errno = EBADF;
        }

        return ret;
#else
        r->_errno = EBADF;
        return -1;
#endif
    }

    if (devoptab_list[fd] == NULL) {
        /* nothing mapped on that FD */
        r->_errno = EBADF;
        return -1;
    }

    if (devoptab_list[fd]->lseek_r == NULL) {
        /* Function not implemented */
        r->_errno = ENOSYS;
        return -1;
    }

    /* Call method from device operations table */
    return devoptab_list[fd]->lseek_r(r, fd, offset, whence);
}

int
_fstat_r(struct _reent *r, int fd, struct stat *st)
{

    if (fd < 0) {
        /* invalid file descriptor */
        r->_errno = EBADF;
        return -1;
    }

    if (fd >= MAX_OPEN_DEVICES) {
        /* CFS file */
        st->st_mode = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO; /* regular file, 0777 perms (-rwxrwxrwx) */
        /** \todo Handle file size with fstat */
        /* st->st_uid = 0; */
        /* st->st_gid = 0; */
        /* st->st_size = 0; */
        return 0;
    }

    if (devoptab_list[fd] != NULL) {
        /* Check device operations table to determine mode */
        st->st_mode = devoptab_list[fd]->st_mode;
        return 0;
    }
    else {
        /* nothing mapped on that FD */
        r->_errno = EBADF;
        return -1;
    }
}

int
_isatty_r(struct _reent *r, int fd)
{

    if (fd < 0) {
        /* invalid file descriptor */
        r->_errno = EBADF;
        return -1;
    }

    if (fd >= MAX_OPEN_DEVICES) {
        /* CFS file, not a TTY */
        r->_errno = ENOTTY;
        return 0;
    }

    if (devoptab_list[fd] != NULL) {
        /* Check device operations table to determine if it is considered a TTY */
        if (devoptab_list[fd]->isatty == 0) {
            r->_errno = ENOTTY;
        }

        return devoptab_list[fd]->isatty;
    }
    else {
        /* nothing mapped on that FD */
        r->_errno = EBADF;
        return -1;
    }
}

/* Compatibility define for newlib built without REENTRANT_SYSCALLS_PROVIDED */
int
_isatty(int fd)
{
    /* _REENT is an internal newlib macro, this may cause issues in some situations. */
    return _isatty_r(_REENT, fd);
}

/* **************************** */
/* File system related syscalls */
/* **************************** */

int
_stat_r(struct _reent *r, const char *file, struct stat *st)
{
    /* not supported, yet */
    (void)file; /* Suppress compiler warnings about unused parameters */
    (void)st;
    r->_errno = ENOENT;
    return -1;
}

int
_link_r(struct _reent *r, const char *old, const char *new)
{
    /* not supported, yet */
    (void)old; /* Suppress compiler warnings about unused parameters */
    (void)new;
    r->_errno = EMLINK;
    return -1;
}

int
_unlink_r(struct _reent *r, const char *name)
{
    /* not supported, yet */
    (void)name; /* Suppress compiler warnings about unused parameters */
    r->_errno = ENOENT;
    return -1;
}

/* ********************************** */
/* Memory management related syscalls */
/* ********************************** */

/* Beginning of unallocated RAM, defined by the linker script */
extern int _heap_start;
/* End of RAM area available for allocation */
extern int _heap_end;
/* Current edge of dynamically allocated space */
static void *current_break = (void *)(&_heap_start);

/**
 * Move the program break.
 *
 * This function can increase the size of the allocated memory.
 *
 * NEVER call this from ISRs (or anything that may call this function, e.g. malloc, free).
 */
void *_sbrk_r(struct _reent *r, ptrdiff_t increment)
{
    void *ret;
    /* Make sure we have exclusive access to the system break variable. */
    mutex_lock(&sbrk_mutex);

    /* Align memory increment to nearest DYNAMIC_MEMORY_ALIGN bytes upward */
    if (increment % DYNAMIC_MEMORY_ALIGN) {
        increment += DYNAMIC_MEMORY_ALIGN - (increment % DYNAMIC_MEMORY_ALIGN);
    }

    if (((uint8_t *)current_break + increment) < ((uint8_t *)(&_heap_end))) {
        ret = current_break;
        current_break = (void *)(((uint8_t *)current_break) + increment);
    }
    else {
        r->_errno = ENOMEM;
        ret = (void *) - 1;
    }

    mutex_unlock(&sbrk_mutex);
    return ret;
}
/** @} */
