/**
 * Native CPU syscall managing
 *
 * Wrap system calls and system call invoking library calls to make
 * sure no context switches happen during a system call.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#include <dlfcn.h>
#else
#include <dlfcn.h>
#endif

#include <err.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "cpu.h"

#include "native_internal.h"

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#define LOCAL_DEBUG (1)
#endif
#include "debug.h"

extern volatile tcb_t *active_thread;

ssize_t (*real_read)(int fd, void *buf, size_t count);
ssize_t (*real_write)(int fd, const void *buf, size_t count);
void* (*real_malloc)(size_t size);
void (*real_free)(void *ptr);
void* (*real_calloc)(size_t nmemb, size_t size);
void* (*real_realloc)(void *ptr, size_t size);

void _native_syscall_enter()
{
    _native_in_syscall++;
#if LOCAL_DEBUG
    real_write(STDERR_FILENO, "> _native_in_syscall\n", 21);
#endif
}

void _native_syscall_leave()
{
#if LOCAL_DEBUG
    real_write(STDERR_FILENO, "< _native_in_syscall\n", 21);
#endif
    _native_in_syscall--;
    if (
            (_native_sigpend > 0)
            && (_native_in_isr == 0)
            && (_native_in_syscall == 0)
            && (native_interrupts_enabled == 1)
       )
    {
        _native_in_isr = 1;
        _native_cur_ctx = (ucontext_t *)active_thread->sp;
        native_isr_context.uc_stack.ss_sp = __isr_stack;
        native_isr_context.uc_stack.ss_size = SIGSTKSZ;
        native_isr_context.uc_stack.ss_flags = 0;
        makecontext(&native_isr_context, native_irq_handler, 0);
        if (swapcontext(_native_cur_ctx, &native_isr_context) == -1) {
            _native_syscall_leave(EXIT_FAILURE, "thread_yield: swapcontext");
        }
    }
}

void *malloc(size_t size)
{
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

void *calloc(size_t nmemb, size_t size)
{
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

ssize_t read(int fd, void *buf, size_t count)
{
    ssize_t r;

    _native_syscall_enter();
    r = real_read(fd, buf, count);
    _native_syscall_leave();

    return r;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    ssize_t r;

    _native_syscall_enter();
    //real_write(fd, "real_write: ", 12);
    r = real_write(fd, buf, count);
    _native_syscall_leave();

    return r;
}

int putchar(int c) {
    write(STDOUT_FILENO, &c, 1);
    return 0;
}

int puts(const char *s)
{
    int r;
    r = write(STDOUT_FILENO, (char*)s, strlen(s));
    putchar('\n');
    return r;
}

char *make_message(const char *format, va_list argp)
{
    int n;
    int size = 100;
    char *message, *temp;

    if ((message = malloc(size)) == NULL)
        return NULL;

    while (1) {
        n = vsnprintf(message, size, format, argp);
        if (n < 0)
            return NULL;
        if (n < size)
            return message;
        size = n + 1;
        if ((temp = realloc(message, size)) == NULL) {
            free(message);
            return NULL;
        } else {
            message = temp;
        }
    }
}

int printf(const char *format, ...)
{
    int r;
    va_list argp;
    char *m;

    va_start(argp, format);
    if ((m = make_message(format, argp)) == NULL) {
        err(EXIT_FAILURE, "malloc");
    }
    r = write(STDOUT_FILENO, m, strlen(m));
    va_end(argp);
    free(m);

    return r;
}


int vprintf(const char *format, va_list argp)
{
    int r;
    char *m;

    if ((m = make_message(format, argp)) == NULL) {
        err(EXIT_FAILURE, "malloc");
    }
    r = write(STDOUT_FILENO, m, strlen(m));
    free(m);

    return r;
}


void vwarn(const char *fmt, va_list args)
{
    char *m, *e;

    e = strerror(errno);

    if ((m = make_message(fmt, args)) == NULL) {
        write(STDERR_FILENO, "malloc\n", 7);
        exit(EXIT_FAILURE);
    }
    write(STDERR_FILENO, _progname, strlen(_progname));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, m, strlen(m));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, e, strlen(e));
    write(STDERR_FILENO, "\n", 1);
    free(m);
}

void vwarnx(const char *fmt, va_list args)
{
    char *m;

    if ((m = make_message(fmt, args)) == NULL) {
        write(STDERR_FILENO, "malloc\n", 7);
        exit(EXIT_FAILURE);
    }
    write(STDERR_FILENO, _progname, strlen(_progname));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, m, strlen(m));
    write(STDERR_FILENO, "\n", 1);
    free(m);
}

void verr(int eval, const char *fmt, va_list args)
{
    vwarn(fmt, args);
    exit(eval);
}

void verrx(int eval, const char *fmt, va_list args)
{
    vwarnx(fmt, args);
    exit(eval);
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
}

void errx(int eval, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    verrx(eval, fmt, argp);
}
