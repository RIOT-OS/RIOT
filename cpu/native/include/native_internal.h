/*
 * Copyright (C) 2013, 2014 Ludwig Knüpfer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * Native CPU internal declarations
 */

/**
 * @defgroup    cpu_native_stdio    STDIO for native
 * @ingroup     sys_stdio
 * @brief       Standard input/output backend for native
 *
 * This will hook up RIOT's stdio to the host's stdio fds. It is the default
 * stdio implementation of the board `native`.
 *
 * @see         cpu_native
 */

/**
 * @ingroup    cpu_native
 * @{
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#ifndef NATIVE_INTERNAL_H
#define NATIVE_INTERNAL_H

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <poll.h>
/* enable signal handler register access on different platforms
 * check here for more:
 * http://sourceforge.net/p/predef/wiki/OperatingSystems/
 */
#ifdef __FreeBSD__
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#include <ucontext.h>
#undef _XOPEN_SOURCE
#else
#include <ucontext.h>
#endif
#elif defined(__linux__)
#ifndef _GNU_SOURCE
#define GNU_SOURCE
#include <ucontext.h>
#undef GNU_SOURCE
#else
#include <ucontext.h>
#endif
#endif /* BSD/Linux */
#include <netdb.h>
#include <ifaddrs.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/uio.h>
#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "syscalls.h"
/**
 * Prototype for native's internal callbacks
 */
typedef void (*_native_callback_t)(void);

/**
 * @cond INTERNAL
 * internal functions
 */
void native_cpu_init(void);
void native_interrupt_init(void);

void native_irq_handler(void);
extern void _native_sig_leave_tramp(void);
extern void _native_sig_leave_handler(void);

void _native_syscall_leave(void);
void _native_syscall_enter(void);
void _native_init_syscalls(void);

/**

/**
 * data structures
 */
extern volatile int native_interrupts_enabled;
extern volatile uintptr_t _native_saved_eip;
extern int _sig_pipefd[2];
extern volatile int _native_sigpend;
extern volatile int _native_in_isr;
extern volatile int _native_in_syscall;

extern char __isr_stack[];
extern const size_t __isr_stack_size;
extern ucontext_t native_isr_context;
extern ucontext_t end_context;
extern ucontext_t *_native_cur_ctx, *_native_isr_ctx;

extern const char *_progname;
extern char **_native_argv;
extern pid_t _native_pid;
extern pid_t _native_id;
extern unsigned _native_rng_seed;
extern int _native_rng_mode; /**< 0 = /dev/random, 1 = random(3) */
extern const char *_native_unix_socket_path;

ssize_t _native_read(int fd, void *buf, size_t count);
ssize_t _native_write(int fd, const void *buf, size_t count);
ssize_t _native_writev(int fildes, const struct iovec *iov, int iovcnt);

/**
 * @endcond
 */

/**
 * register interrupt handler handler for interrupt sig
 */
int register_interrupt(int sig, _native_callback_t handler);

/**
 * unregister interrupt handler for interrupt sig
 */
int unregister_interrupt(int sig);

#ifdef __cplusplus
}
#endif

#include "sched.h"

/** @} */
#endif /* NATIVE_INTERNAL_H */
