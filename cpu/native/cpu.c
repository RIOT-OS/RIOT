/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief  Native CPU kernel_intern.h and sched.h implementation
 * @author Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 *
 * Every RIOT thread is backed by a host POSIX thread. Only the host thread
 * that currently owns the (emulated) CPU is allowed to run, all other host
 * threads are blocked reading from a pipe with all signals masked.
 *
 * Interrupts are emulated with POSIX signals. As only the CPU owner has
 * signals unmasked, process directed signals are always delivered to the
 * host thread of the active RIOT thread. If an interrupt handler (or a
 * voluntary yield) results in a different thread being scheduled, the CPU is
 * handed over to the host thread of that thread and the current host thread
 * blocks until it is scheduled again - if this happened inside a signal
 * handler, the interrupted thread will simply return from the signal handler
 * once resumed. This avoids any architecture specific context switching code.
 */

#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#if defined(__FreeBSD__)
#  include <pthread_np.h>
#endif
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpu.h"
#include "container.h"
#include "cpu_conf.h"
#include "irq.h"
#include "sched.h"
#include "test_utils/expect.h"

#ifdef MODULE_NETDEV_TAP
#include "netdev_tap.h"
extern netdev_tap_t netdev_tap;
#endif

#include "native_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"
#define DEBUG_CPU(...) DEBUG("[native] CPU: " __VA_ARGS__)

/**
 * @brief   Host thread backing a RIOT thread
 *
 * This is stored at the top of the RIOT thread stack, `thread_t::sp` points
 * to it. The remainder of the RIOT stack is used as stack of the host thread,
 * so that stack usage reported by e.g. `ps` is meaningful. It must be at least
 * `PTHREAD_STACK_MIN` large.
 *
 * The host thread is only created when the RIOT thread is scheduled for the
 * first time. Until then the RIOT stack may legitimately be reused (e.g. by
 * `tests/core/thread_flood`), and resources are only spent on threads that
 * actually run.
 */
typedef struct {
    pthread_t pthread;              /**< host thread, valid if @ref started */
    int pipe_fd[2];                 /**< a byte is written to it to schedule the thread */
    thread_task_func_t task_func;   /**< RIOT thread function */
    void *arg;                      /**< argument to @ref task_func */
    void *stack;                    /**< stack for the host thread */
    size_t stack_size;              /**< size of @ref stack */
    bool started;                   /**< host thread has been created */
} native_thread_t;

/**
 * @name    Host pthread functions
 *
 * The `posix_pthread` module provides RIOT implementations of these functions,
 * so the host ones have to be looked up explicitly.
 * @{
 */
static int (*real_pthread_create)(pthread_t *thread, const pthread_attr_t *attr,
                                  void *(*start_routine)(void *), void *arg);
static int (*real_pthread_exit)(void *retval);
static int (*real_pthread_join)(pthread_t thread, void **retval);
static pthread_t (*real_pthread_self)(void);
static int (*real_pthread_attr_init)(pthread_attr_t *attr);
static int (*real_pthread_attr_setstack)(pthread_attr_t *attr, void *stackaddr,
                                         size_t stacksize);
static int (*real_pthread_attr_destroy)(pthread_attr_t *attr);
/** @} */

/**
 * @brief   Host thread of the calling host thread, NULL for the main host thread
 */
static __thread native_thread_t *_self;

/**
 * @brief   Host thread of the last RIOT thread that exited, still to be joined
 */
static pthread_t _zombie;
static bool _have_zombie; /**< True if _zombie currently points to a zombie thread */

/**
 * @brief   Number of host thread management calls in progress, they allocate
 *          memory on behalf of the host libc
 */
static unsigned _host_calls;

/**
 * @brief   The calling host thread has left its RIOT thread and is only
 *          running host libc exit code
 */
static __thread bool _exited;

static inline native_thread_t *_native_thread(thread_t *thread)
{
    /* Use intermediate cast to uintptr_t to silence -Wcast-align.
     * The control block is aligned in thread_stack_init() */
    return (native_thread_t *)(uintptr_t)thread->sp;
}

static void _block_all_signals(sigset_t *old)
{
    sigset_t all;

    sigfillset(&all);
    if (pthread_sigmask(SIG_SETMASK, &all, old) != 0) {
        errx(EXIT_FAILURE, "native: pthread_sigmask() failed");
    }
}

/* Reap the host thread of an exited RIOT thread. Must be called by the new
 * CPU owner before doing anything else, so that the memory of the exited
 * thread (which might hold its host thread's state) can safely be reused. */
static void _reap_zombie(void)
{
    int res;

    if (!_have_zombie) {
        return;
    }

    _have_zombie = false;
    _native_pending_syscalls_up();
    _host_calls++;
    res = real_pthread_join(_zombie, NULL);
    _host_calls--;
    _native_pending_syscalls_down();

    if (res) {
        DEBUG_CPU("_reap_zombie: pthread_join() failed with %d\n", res);
    }
}

static void *_thread_entry(void *arg);

/* Create the host thread for @p next, which starts out as CPU owner.
 * Must be called with all signals masked, which the new thread inherits. */
static void _start(native_thread_t *next)
{
    pthread_attr_t attr;
    pthread_t pthread;
    int res;

    if (real_pipe(next->pipe_fd) == -1) {
        err(EXIT_FAILURE, "native: pipe");
    }
    /* don't leak the pipes on reboot (execve()) */
    real_fcntl(next->pipe_fd[0], F_SETFD, FD_CLOEXEC);
    real_fcntl(next->pipe_fd[1], F_SETFD, FD_CLOEXEC);
    next->started = true;

    _native_pending_syscalls_up();
    _host_calls++;
    real_pthread_attr_init(&attr);
    res = real_pthread_attr_setstack(&attr, next->stack, next->stack_size);
    if (res == 0) {
        res = real_pthread_create(&pthread, &attr, _thread_entry, next);
    }
    real_pthread_attr_destroy(&attr);
    _host_calls--;
    _native_pending_syscalls_down();

    if (res != 0) {
        errx(EXIT_FAILURE, "native: pthread_create() failed: %s", strerror(res));
    }
}

/* Hand the CPU over to @p next. Must be called with all signals masked. */
static void _schedule(native_thread_t *next)
{
    char token = 0;

    if (!next->started) {
        _start(next);
        return;
    }

    if (real_write(next->pipe_fd[1], &token, 1) != 1) {
        err(EXIT_FAILURE, "native: failed to schedule host thread");
    }
}

/* Block until the calling host thread is the CPU owner again. Must be called
 * with all signals masked. */
static void _wait_for_cpu(native_thread_t *self)
{
    char token;
    ssize_t res;

    do {
        res = real_read(self->pipe_fd[0], &token, 1);
    } while ((res == -1) && (errno == EINTR));

    if (res != 1) {
        err(EXIT_FAILURE, "native: failed to wait for CPU");
    }

    _reap_zombie();
}

bool _native_host_internal(void)
{
    return _host_calls || _exited;
}

bool _native_is_cpu_owner(void)
{
    thread_t *active = thread_get_active();
    return _self && active && (_native_thread(active) == _self);
}

void _native_switch_to_active(void)
{
    native_thread_t *next = _native_thread(thread_get_active());

    if (next == _self) {
        return;
    }

    DEBUG_CPU("switching to PID %" PRIkernel_pid "\n", thread_getpid());

    _schedule(next);
    _wait_for_cpu(_self);
}

/**
 * TODO: implement
 */
void thread_print_stack(void)
{
    DEBUG_CPU("thread_print_stack: not implemented yet!\n");
}

/* This function calculates the ISR stack usage */
int thread_isr_stack_usage(void)
{
    /* TODO */
    return -1;
}

void native_breakpoint(void)
{
    raise(SIGTRAP);
}

void cpu_switch_context_exit(void)
{
# ifdef NATIVE_AUTO_EXIT
    if (sched_num_threads <= 1) {
        extern unsigned _native_retval;
        DEBUG_CPU("cpu_switch_context_exit: last task has ended. exiting.\n");
        real_exit(_native_retval);
    }
# endif

    _block_all_signals(NULL);
    _native_interrupts_enabled = false;
    _native_in_isr = 1;

    if (IS_USED(MODULE_CORE_THREAD)) {
        sched_run();
    }

    native_thread_t *next = _native_thread(thread_get_active());

    if (_self == NULL) {
        /* called by kernel_init() on the main host thread: start the first
         * RIOT thread and let the main host thread sleep forever */
        DEBUG_CPU("cpu_switch_context_exit: starting first thread\n");
        _schedule(next);
        while (1) {
            pause();
        }
    }

    /* called by sched_task_exit(): the active RIOT thread has ended */
    DEBUG_CPU("cpu_switch_context_exit: thread exited\n");
    _reap_zombie();
    real_close(_self->pipe_fd[0]);
    real_close(_self->pipe_fd[1]);
    _zombie = _self->pthread;
    _have_zombie = true;
    _exited = true;
    _schedule(next);

    real_pthread_exit(NULL);
    UNREACHABLE();
}

void thread_yield_higher(void)
{
    sched_context_switch_request = 1;

    if ((_native_in_isr == 0) && _native_interrupts_enabled && _native_is_cpu_owner()) {
        DEBUG_CPU("yielding higher priority thread\n");
        _native_isr_run(false);
    }
}

void native_cpu_init(void)
{
    static const struct {
        void **fn;
        const char *name;
    } lookups[] = {
        { (void **)&real_pthread_create, "pthread_create" },
        { (void **)&real_pthread_exit, "pthread_exit" },
        { (void **)&real_pthread_join, "pthread_join" },
        { (void **)&real_pthread_self, "pthread_self" },
        { (void **)&real_pthread_attr_init, "pthread_attr_init" },
        { (void **)&real_pthread_attr_setstack, "pthread_attr_setstack" },
        { (void **)&real_pthread_attr_destroy, "pthread_attr_destroy" },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(lookups); i++) {
        *lookups[i].fn = dlsym(RTLD_NEXT, lookups[i].name);
        if (*lookups[i].fn == NULL) {
            errx(EXIT_FAILURE, "native_cpu_init: failed to look up %s()",
                 lookups[i].name);
        }
    }

    DEBUG_CPU("RIOT native cpu initialized.\n");
}

/* Name the host thread after the RIOT thread, so it shows up in debuggers */
static void _set_host_thread_name(native_thread_t *self, const char *name)
{
#if defined(__linux__) || defined(__FreeBSD__)
    /* Linux limits thread names to 16 bytes including the terminator */
    char buf[16];

    if (name == NULL) {
        return;
    }

    strncpy(buf, name, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    _native_pending_syscalls_up();
    pthread_setname_np(self->pthread, buf);
    _native_pending_syscalls_down();
#else
    (void)self;
    (void)name;
#endif
}

static void *_thread_entry(void *arg)
{
    native_thread_t *self = arg;

    /* the host thread starts out as CPU owner */
    self->pthread = real_pthread_self();
    _self = self;

    _reap_zombie();

    /* the name is only assigned after thread_stack_init() returned */
    _set_host_thread_name(self, thread_getname(thread_getpid()));

    /* RIOT threads start with interrupts enabled */
    _native_in_isr = 0;
    irq_enable();

    self->task_func(self->arg);
    sched_task_exit();

    return NULL;
}

char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size)
{
    native_thread_t *ctx;

    DEBUG_CPU("thread_stack_init\n");

    /* Place the host thread control block at the top of the RIOT stack,
     * the remainder is used as stack of the host thread. */
    uintptr_t top = (uintptr_t)stack_start + stack_size - sizeof(native_thread_t);
    top &= ~(uintptr_t)(_Alignof(max_align_t) - 1);
    expect(top >= (uintptr_t)stack_start);
    ctx = (native_thread_t *)top;

    memset(ctx, 0, sizeof(*ctx));
    ctx->task_func = task_func;
    ctx->arg = arg;

    uintptr_t bottom = ((uintptr_t)stack_start + _Alignof(max_align_t) - 1)
                       & ~(uintptr_t)(_Alignof(max_align_t) - 1);
    ctx->stack = (void *)bottom;
    ctx->stack_size = top - bottom;

    /* the host libc refuses to run a thread on a smaller stack */
    if (top < bottom || ctx->stack_size < (size_t)PTHREAD_STACK_MIN) {
        errx(EXIT_FAILURE, "thread_stack_init: stack too small, need at least "
             "%ld bytes (+ overhead), got %d", (long)PTHREAD_STACK_MIN, stack_size);
    }

    return (char *)ctx;
}
