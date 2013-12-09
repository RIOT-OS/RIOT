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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "cpu.h"

#include "native_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

extern volatile tcb_t *active_thread;

ssize_t (*real_read)(int fd, void *buf, size_t count);
ssize_t (*real_write)(int fd, const void *buf, size_t count);

void _native_syscall_enter()
{
    _native_in_syscall++;
    DEBUG("> _native_in_syscall: %d\n", _native_in_syscall);
}

void _native_syscall_leave()
{
    _native_in_syscall--;
    DEBUG("< _native_in_syscall: %d\n", _native_in_syscall);
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
