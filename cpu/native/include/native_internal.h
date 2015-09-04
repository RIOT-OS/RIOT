/*
 * Copyright (C) 2013, 2014 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * Native CPU internal declarations
 */

/**
 * @addtogroup    native_cpu
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef _NATIVE_INTERNAL_H
#define _NATIVE_INTERNAL_H

#include <signal.h>
#include <stdio.h>
/* enable signal handler register access on different platforms
 * check here for more:
 * http://sourceforge.net/p/predef/wiki/OperatingSystems/
 */
#if (defined(__FreeBSD__) || defined(__MACH__))
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
#endif // BSD/Linux
#include <netdb.h>
#include <ifaddrs.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Prototype for native's internal callbacks
 */
typedef void (*_native_callback_t)(void);

/**
 * internal functions
 */
void native_cpu_init(void);
void native_interrupt_init(void);
extern void native_hwtimer_pre_init(void);

void native_irq_handler(void);
extern void _native_sig_leave_tramp(void);

void _native_syscall_leave(void);
void _native_syscall_enter(void);
void _native_init_syscalls(void);

/**
 * external functions regularly wrapped in native for direct use
 */
extern ssize_t (*real_read)(int fd, void *buf, size_t count);
extern ssize_t (*real_write)(int fd, const void *buf, size_t count);
extern size_t (*real_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern void (*real_clearerr)(FILE *stream);
extern __attribute__((noreturn)) void (*real_exit)(int status);
extern void (*real_free)(void *ptr);
extern void* (*real_calloc)(size_t nmemb, size_t size);
extern void* (*real_malloc)(size_t size);
extern void* (*real_realloc)(void *ptr, size_t size);
extern void (*real_freeaddrinfo)(struct addrinfo *res);
extern void (*real_freeifaddrs)(struct ifaddrs *ifa);
extern void (*real_srandom)(unsigned int seed);
/* The ... is a hack to save includes: */
extern int (*real_accept)(int socket, ...);
/* The ... is a hack to save includes: */
extern int (*real_bind)(int socket, ...);
extern int (*real_chdir)(const char *path);
extern int (*real_close)(int);
/* The ... is a hack to save includes: */
extern int (*real_creat)(const char *path, ...);
extern int (*real_dup2)(int, int);
extern int (*real_execve)(const char *, char *const[], char *const[]);
extern int (*real_feof)(FILE *stream);
extern int (*real_ferror)(FILE *stream);
extern int (*real_fork)(void);
/* The ... is a hack to save includes: */
extern int (*real_getaddrinfo)(const char *node, ...);
extern int (*real_getifaddrs)(struct ifaddrs **ifap);
extern int (*real_getpid)(void);
extern int (*real_ioctl)(int fildes, int request, ...);
extern int (*real_listen)(int socket, int backlog);
extern int (*real_open)(const char *path, int oflag, ...);
extern int (*real_pause)(void);
extern int (*real_pipe)(int[2]);
/* The ... is a hack to save includes: */
extern int (*real_select)(int nfds, ...);
extern int (*real_setitimer)(int which, const struct itimerval
        *__restrict value, struct itimerval *__restrict ovalue);
extern int (*real_setsid)(void);
extern int (*real_setsockopt)(int socket, ...);
extern int (*real_socket)(int domain, int type, int protocol);
extern int (*real_printf)(const char *format, ...);
extern int (*real_unlink)(const char *);
extern long int (*real_random)(void);
extern const char* (*real_gai_strerror)(int errcode);
extern FILE* (*real_fopen)(const char *path, const char *mode);
extern mode_t (*real_umask)(mode_t cmask);
extern ssize_t (*real_writev)(int fildes, const struct iovec *iov, int iovcnt);

#ifdef __MACH__
#else
extern int (*real_clock_gettime)(clockid_t clk_id, struct timespec *tp);
#endif

/**
 * data structures
 */
extern volatile int native_interrupts_enabled;
extern volatile unsigned int _native_saved_eip;
extern int _sig_pipefd[2];
extern volatile int _native_sigpend;
extern volatile int _native_in_isr;
extern volatile int _native_in_syscall;

extern char __isr_stack[SIGSTKSZ];
extern char __end_stack[SIGSTKSZ];
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
 * register interrupt handler handler for interrupt sig
 */
int register_interrupt(int sig, _native_callback_t handler);

/**
 * unregister interrupt handler for interrupt sig
 */
int unregister_interrupt(int sig);

//#include <sys/param.h>

#ifdef __cplusplus
}
#endif

#include "kernel_internal.h"
#include "sched.h"


/** @} */
#endif /* _NATIVE_INTERNAL_H */
