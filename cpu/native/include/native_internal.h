/**
 * Native CPU internal declarations
 */

/**
 * @ingroup arch
 * @defgroup    native_cpu  Native CPU
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef _NATIVE_INTERNAL_H
#define _NATIVE_INTERNAL_H

#include <signal.h>
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

/**
 * external functions regularly wrapped in native for direct use
 */
extern ssize_t (*real_read)(int fd, void *buf, size_t count);
extern ssize_t (*real_write)(int fd, const void *buf, size_t count);
extern void* (*real_malloc)(size_t size);
extern void (*real_free)(void *ptr);
extern void* (*real_calloc)(size_t nmemb, size_t size);
extern void* (*real_realloc)(void *ptr, size_t size);
extern int (*real_getpid)(void);
extern int (*real_pipe)(int[2]);
extern int (*real_close)(int);
extern int (*real_fork)(void);
extern int (*real_dup2)(int, int);
extern int (*real_unlink)(const char *);
extern int (*real_execve)(const char *, char *const[], char *const[]);
extern int (*real_pause)(void);

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
extern const char *_native_unix_socket_path;

#ifdef MODULE_UART0
#include <sys/select.h>
extern fd_set _native_rfds;
#endif

ssize_t _native_read(int fd, void *buf, size_t count);
ssize_t _native_write(int fd, const void *buf, size_t count);

/**
 * register interrupt handler handler for interrupt sig
 */
int register_interrupt(int sig, void (*handler)(void));

/**
 * unregister interrupt handler for interrupt sig
 */
int unregister_interrupt(int sig);

//#include <sys/param.h>

#include "kernel_internal.h"
#include "sched.h"


/** @} */
#endif /* _NATIVE_INTERNAL_H */
