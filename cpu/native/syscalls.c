/*
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup cpu_native
 * @brief   Native CPU syscall managing
 * @{
 *
 * Wrap system calls and system call invoking library calls to make
 * sure no context switches happen during a system call.
 *
 * @}
 */

#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef MODULE_LIBC_GETTIMEOFDAY
# include <sys/time.h>
#endif
#include <ifaddrs.h>
#include <sys/stat.h>

#include "cpu.h"
#include "irq.h"
#ifdef MODULE_LIBC_GETTIMEOFDAY
# include "time_units.h"
# include "ztimer64.h"
#endif

#include "stdio_base.h"

#include "kernel_defines.h"
/* This header defines the system call function pointers */
#define NATIVE_SYSCALLS_DEFINITION 1
#include "native_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define _DEBUG_PREFIX "[native] syscalls: "
#define DEBUG_SYSCALLS(...) DEBUG(_DEBUG_PREFIX __VA_ARGS__)

#define _SYSCALL_ENTER_MESSAGE _DEBUG_PREFIX "> _native_pending_syscalls\n"
#define _SYSCALL_LEAVE_MESSAGE _DEBUG_PREFIX "< _native_pending_syscalls\n"

void _native_syscall_enter(void)
{
    _native_pending_syscalls_up();

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        real_write(STDERR_FILENO, _SYSCALL_ENTER_MESSAGE, sizeof(_SYSCALL_ENTER_MESSAGE) - 1);
    }
}

void _native_syscall_leave(void)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        real_write(STDERR_FILENO, _SYSCALL_LEAVE_MESSAGE, sizeof(_SYSCALL_LEAVE_MESSAGE) - 1);
    }

    _native_pending_syscalls_down();
    if (
            (_native_pending_signals > 0)
            && (_native_in_isr == 0)
            && (_native_pending_syscalls == 0)
            && (_native_interrupts_enabled)
            && (thread_get_active() != NULL)
       )
    {
        _native_in_isr = 1;
        _native_interrupts_enabled = false;

        _native_isr_context_make(_native_call_sig_handlers_and_switch);
        if (swapcontext(_native_user_context(), _native_isr_context) == -1) {
            err(EXIT_FAILURE, "_native_syscall_leave: swapcontext");
        }
    }
}

/* make use of TLSF if it is included, except when building with valgrind
 * support, where one probably wants to make use of valgrind's memory leak
 * detection abilities*/
#if (!(defined MODULE_TLSF) && !(defined NATIVE_MEMORY)) || (defined(HAVE_VALGRIND_H))
int _native_in_malloc = 0;
void *malloc(size_t size)
{
    /* dynamically load malloc when it's needed - this is necessary to
     * support g++ 5.2.0 as it uses malloc before startup runs */
    if (!real_malloc) {
        if (_native_in_malloc) {
            /* XXX: This is a dirty hack for behaviour that came along
             * with g++ 5.2.0.
             * Throw it out when whatever made it necessary it is fixed. */
            return NULL;
        }
        else {
            _native_in_malloc = 1;
            *(void **)(&real_malloc) = dlsym(RTLD_NEXT, "malloc");
            _native_in_malloc = 0;
        }
    }

    void *r;
    _native_syscall_enter();
    r = real_malloc(size);
    _native_syscall_leave();
    return r;
}

void free(void *ptr)
{
    _native_syscall_enter();
    real_free(ptr);
    _native_syscall_leave();
}

int _native_in_calloc = 0;
void *calloc(size_t nmemb, size_t size)
{
    /* dynamically load calloc when it's needed - this is necessary to
     * support profiling as it uses calloc before startup runs */
    if (!real_calloc) {
        if (_native_in_calloc) {
            /* XXX: This is a dirty hack to enable old dlsym versions to run.
             * Throw it out when Ubuntu 12.04 support runs out (in 2017-04)! */
            return NULL;
        }
        else {
            _native_in_calloc = 1;
            *(void **)(&real_calloc) = dlsym(RTLD_NEXT, "calloc");
            _native_in_calloc = 0;
        }
    }

    void *r;
    _native_syscall_enter();
    r = real_calloc(nmemb, size);
    _native_syscall_leave();
    return r;
}

void *realloc(void *ptr, size_t size)
{
    void *r;
    _native_syscall_enter();
    r = real_realloc(ptr, size);
    _native_syscall_leave();
    return r;
}
#endif /* !(defined MODULE_TLSF) || (defined(HAVE_VALGRIND_H)) */

ssize_t _native_read(int fd, void *buf, size_t count)
{
    ssize_t r;

    if (fd == STDIN_FILENO && IS_USED(MODULE_STDIN)) {
        return stdio_read(buf, count);
    }

    _native_syscall_enter();
    r = real_read(fd, buf, count);
    _native_syscall_leave();

    return r;
}

ssize_t _native_write(int fd, const void *buf, size_t count)
{
    ssize_t r;

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return stdio_write(buf, count);
    }

    _native_syscall_enter();
    r = real_write(fd, buf, count);
    _native_syscall_leave();

    return r;
}

ssize_t _native_writev(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t r = 0;

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        while (iovcnt--) {
            ssize_t res = stdio_write(iov->iov_base, iov->iov_len);

            if (res >= 0) {
                r += res;
            } else {
                return res;
            }

            if (res < (int)iov->iov_len) {
                break;
            }

            iov++;
        }

        return r;
    }

    _native_syscall_enter();
    r = real_writev(fd, iov, iovcnt);
    _native_syscall_leave();

    return r;
}

#if defined(__FreeBSD__)
#undef putchar
#endif
#ifndef MODULE_STDIO_NULL
int putchar(int c)
{
    char tmp = c;
    return _native_write(STDOUT_FILENO, &tmp, sizeof(tmp));
}

int puts(const char *s)
{
    int r;
    r = _native_write(STDOUT_FILENO, (char*)s, strlen(s));
    putchar('\n');
    return r;
}
#endif

int putc(int c, FILE *fp)
{
    char tmp = c;
    return _native_write(fileno(fp), &tmp, sizeof(tmp));
}

int fgetc(FILE *fp)
{
    return getc(fp);
}

int getchar(void) {
    return getc(stdin);
}

int getc(FILE *fp)
{
    char c;

    if (_native_read(fileno(fp), &c, sizeof(c)) <= 0) {
        return EOF;
    }

    return c;
}

/* Solve 'format string is not a string literal' as it is validly used in this
 * function */
__attribute__((__format__ (__printf__, 1, 0)))
char *make_message(const char *format, va_list argp)
{
    int size = 128;
    char *message, *temp;

    if ((message = malloc(size)) == NULL) {
        return NULL;
    }

    /* argp is undefined after calling vsnprintf, so we copy the list first */
    va_list argp_copy;
    va_copy(argp_copy, argp);

    while (1) {
        int n = vsnprintf(message, size, format, argp);
        if (n < 0) {
            free(message);
            va_end(argp_copy);
            return NULL;
        }
        if (n < size) {
            va_end(argp_copy);
            return message;
        }
        size = n + 1;
        if ((temp = realloc(message, size)) == NULL) {
            free(message);
            va_end(argp_copy);
            return NULL;
        }
        else {
            message = temp;
            /* copy the list back and try again */
            va_end(argp);
            va_copy(argp, argp_copy);
        }
    }
}

#ifndef MODULE_STDIO_NULL
int printf(const char *format, ...)
{
    int r;
    va_list argp;

    va_start(argp, format);
    r = vfprintf(stdout, format, argp);
    va_end(argp);

    return r;
}

int vprintf(const char *format, va_list argp)
{
    return vfprintf(stdout, format, argp);
}
#endif

int fprintf(FILE *fp, const char *format, ...)
{
    int r;
    va_list argp;

    va_start(argp, format);
    r = vfprintf(fp, format, argp);
    va_end(argp);

    return r;
}

int vfprintf(FILE *fp, const char *format, va_list argp)
{
    int r;
    char *m;

    if ((m = make_message(format, argp)) == NULL) {
        err(EXIT_FAILURE, "malloc");
    }
    r = _native_write(fileno(fp), m, strlen(m));
    free(m);

    return r;
}

void vwarn(const char *fmt, va_list args)
{
    char *m, *e;

    e = strerror(errno);

    if ((m = make_message(fmt, args)) == NULL) {
        _native_write(STDERR_FILENO, "malloc\n", 7);
        real_exit(EXIT_FAILURE);
    }
    _native_write(STDERR_FILENO, _progname, strlen(_progname));
    _native_write(STDERR_FILENO, ": ", 2);
    _native_write(STDERR_FILENO, m, strlen(m));
    _native_write(STDERR_FILENO, ": ", 2);
    _native_write(STDERR_FILENO, e, strlen(e));
    _native_write(STDERR_FILENO, "\n", 1);
    free(m);
}

void vwarnx(const char *fmt, va_list args)
{
    char *m;

    if ((m = make_message(fmt, args)) == NULL) {
        _native_write(STDERR_FILENO, "malloc\n", 7);
        real_exit(EXIT_FAILURE);
    }
    _native_write(STDERR_FILENO, _progname, strlen(_progname));
    _native_write(STDERR_FILENO, ": ", 2);
    _native_write(STDERR_FILENO, m, strlen(m));
    _native_write(STDERR_FILENO, "\n", 1);
    free(m);
}

void verr(int eval, const char *fmt, va_list args)
{
    vwarn(fmt, args);
    real_exit(eval);
}

void verrx(int eval, const char *fmt, va_list args)
{
    vwarnx(fmt, args);
    real_exit(eval);
}

void warn(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vwarn(fmt, argp);
    va_end(argp);
}

void warnx(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vwarnx(fmt, argp);
    va_end(argp);
}

void err(int eval, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    verr(eval, fmt, argp);
    va_end(argp);
}

void errx(int eval, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    verrx(eval, fmt, argp);
    va_end(argp);
}

int getpid(void)
{
    return thread_getpid();
}

#if (IS_USED(MODULE_LIBC_GETTIMEOFDAY))
int _gettimeofday(struct timeval *tp, void *restrict tzp)
{
    (void)tzp;
    uint64_t now = ztimer64_now(ZTIMER64_USEC);
    tp->tv_sec  = now / US_PER_SEC;
    tp->tv_usec = now - tp->tv_sec;
    return 0;
}
#endif

void _native_init_syscalls(void)
{
    *(void **)(&real_read) = dlsym(RTLD_NEXT, "read");
    *(void **)(&real_write) = dlsym(RTLD_NEXT, "write");
    *(void **)(&real_malloc) = dlsym(RTLD_NEXT, "malloc");
    *(void **)(&real_calloc) = dlsym(RTLD_NEXT, "calloc");
    *(void **)(&real_realloc) = dlsym(RTLD_NEXT, "realloc");
    *(void **)(&real_exit) = dlsym(RTLD_NEXT, "exit");
    *(void **)(&real_free) = dlsym(RTLD_NEXT, "free");
    *(void **)(&real_freeaddrinfo) = dlsym(RTLD_NEXT, "freeaddrinfo");
    *(void **)(&real_freeifaddrs) = dlsym(RTLD_NEXT, "freeifaddrs");
    *(void **)(&real_srandom) = dlsym(RTLD_NEXT, "srandom");
    *(void **)(&real_accept) = dlsym(RTLD_NEXT, "accept");
    *(void **)(&real_bind) = dlsym(RTLD_NEXT, "bind");
    *(void **)(&real_connect) = dlsym(RTLD_NEXT, "connect");
    *(void **)(&real_recv) = dlsym(RTLD_NEXT, "recv");
    *(void **)(&real_printf) = dlsym(RTLD_NEXT, "printf");
    *(void **)(&real_gai_strerror) = dlsym(RTLD_NEXT, "gai_strerror");
    *(void **)(&real_getaddrinfo) = dlsym(RTLD_NEXT, "getaddrinfo");
    *(void **)(&real_getifaddrs) = dlsym(RTLD_NEXT, "getifaddrs");
    *(void **)(&real_getpid) = dlsym(RTLD_NEXT, "getpid");
    *(void **)(&real_gettimeofday) = dlsym(RTLD_NEXT, "gettimeofday");
    *(void **)(&real_pipe) = dlsym(RTLD_NEXT, "pipe");
    *(void **)(&real_chdir) = dlsym(RTLD_NEXT, "chdir");
    *(void **)(&real_close) = dlsym(RTLD_NEXT, "close");
    *(void **)(&real_fcntl) = dlsym(RTLD_NEXT, "fcntl");
    *(void **)(&real_creat) = dlsym(RTLD_NEXT, "creat");
    *(void **)(&real_fork) = dlsym(RTLD_NEXT, "fork");
    *(void **)(&real_dup2) = dlsym(RTLD_NEXT, "dup2");
    *(void **)(&real_select) = dlsym(RTLD_NEXT, "select");
    *(void **)(&real_poll) = dlsym(RTLD_NEXT, "poll");
    *(void **)(&real_setsid) = dlsym(RTLD_NEXT, "setsid");
    *(void **)(&real_setsockopt) = dlsym(RTLD_NEXT, "setsockopt");
    *(void **)(&real_socket) = dlsym(RTLD_NEXT, "socket");
    *(void **)(&real_unlink) = dlsym(RTLD_NEXT, "unlink");
    *(void **)(&real_random) = dlsym(RTLD_NEXT, "random");
    *(void **)(&real_execve) = dlsym(RTLD_NEXT, "execve");
    *(void **)(&real_ioctl) = dlsym(RTLD_NEXT, "ioctl");
    *(void **)(&real_listen) = dlsym(RTLD_NEXT, "listen");
    *(void **)(&real_open) = dlsym(RTLD_NEXT, "open");
    *(void **)(&real_pause) = dlsym(RTLD_NEXT, "pause");
    *(void **)(&real_fopen) = dlsym(RTLD_NEXT, "fopen");
    *(void **)(&real_fread) = dlsym(RTLD_NEXT, "fread");
    *(void **)(&real_feof) = dlsym(RTLD_NEXT, "feof");
    *(void **)(&real_ferror) = dlsym(RTLD_NEXT, "ferror");
    *(void **)(&real_clearerr) = dlsym(RTLD_NEXT, "clearerr");
    *(void **)(&real_umask) = dlsym(RTLD_NEXT, "umask");
    *(void **)(&real_writev) = dlsym(RTLD_NEXT, "writev");
    *(void **)(&real_send) = dlsym(RTLD_NEXT, "send");
    *(void **)(&real_fclose) = dlsym(RTLD_NEXT, "fclose");
    *(void **)(&real_fseek) = dlsym(RTLD_NEXT, "fseek");
    *(void **)(&real_ftell) = dlsym(RTLD_NEXT, "ftell");
    *(void **)(&real_fputc) = dlsym(RTLD_NEXT, "fputc");
    *(void **)(&real_fgetc) = dlsym(RTLD_NEXT, "fgetc");
    *(void **)(&real_mkdir) = dlsym(RTLD_NEXT, "mkdir");
    *(void **)(&real_rmdir) = dlsym(RTLD_NEXT, "rmdir");
    *(void **)(&real_lseek) = dlsym(RTLD_NEXT, "lseek");
    *(void **)(&real_fstat) = dlsym(RTLD_NEXT, "fstat");
    *(void **)(&real_fsync) = dlsym(RTLD_NEXT, "fsync");
    *(void **)(&real_rename) = dlsym(RTLD_NEXT, "rename");
    *(void **)(&real_opendir) = dlsym(RTLD_NEXT, "opendir");
    *(void **)(&real_readdir) = dlsym(RTLD_NEXT, "readdir");
    *(void **)(&real_closedir) = dlsym(RTLD_NEXT, "closedir");
    *(void **)(&real_statvfs) = dlsym(RTLD_NEXT, "statvfs");
}
