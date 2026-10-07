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
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpu.h"
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
 * to it.
 */
typedef struct {
    pthread_t pthread;              /**< host thread */
    int pipe_fd[2];                 /**< a byte is written to it to schedule the thread */
    thread_task_func_t task_func;   /**< RIOT thread function */
    void *arg;                      /**< argument to @ref task_func */
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
static int (*real_pthread_join)(pthread_t thread, void **retval);
/** @} */

/**
 * @brief   Host thread of the calling host thread, NULL for the main host thread
 */
static __thread native_thread_t *_self;

/**
 * @brief   Read end of the pipe of the calling host thread
 *
 * This is kept separately from @ref _self, as the control block may only be
 * accessed once the thread got scheduled: until then the RIOT stack holding
 * it may be reused (e.g. by tests/core/thread_flood).
 */
static __thread int _cpu_fd = -1;

/**
 * @brief   Used by an exiting thread to return from its host thread function
 */
static __thread jmp_buf *_exit_buf;

/**
 * @brief   Host thread of the last RIOT thread that exited, still to be joined
 */
static pthread_t _zombie;
static bool _have_zombie;

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
    if (_have_zombie) {
        _have_zombie = false;
        _native_pending_syscalls_up();
        real_pthread_join(_zombie, NULL);
        _native_pending_syscalls_down();
    }
}

/* Hand the CPU over to @p next. Must be called with all signals masked. */
static void _schedule(native_thread_t *next)
{
    char token = 0;

    if (real_write(next->pipe_fd[1], &token, 1) != 1) {
        err(EXIT_FAILURE, "native: failed to schedule host thread");
    }
}

/* Block until the calling host thread is the CPU owner again. Must be called
 * with all signals masked. */
static void _wait_for_cpu(void)
{
    char token;
    ssize_t res;

    do {
        res = real_read(_cpu_fd, &token, 1);
    } while ((res == -1) && (errno == EINTR));

    if (res != 1) {
        err(EXIT_FAILURE, "native: failed to wait for CPU");
    }

    _reap_zombie();
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
    _wait_for_cpu();
}

/**
 * TODO: implement
 */
void thread_print_stack(void)
{
    DEBUG_CPU("thread_print_stack\n");
    return;
}

/* This function calculates the ISR_usage */
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
    _schedule(next);

    /* the new CPU owner joins this host thread before touching anything,
     * so it is still safe to access our own thread local storage */
    longjmp(*_exit_buf, 1);
}

void thread_yield_higher(void)
{
    sched_context_switch_request = 1;

    if (_native_in_isr == 0 && _native_interrupts_enabled
        && _native_is_cpu_owner()) {
        DEBUG_CPU("yielding higher priority thread\n");
        _native_isr_run(false);
    }
}

void native_cpu_init(void)
{
    *(void **)&real_pthread_create = dlsym(RTLD_NEXT, "pthread_create");
    *(void **)&real_pthread_join = dlsym(RTLD_NEXT, "pthread_join");
    if (!real_pthread_create || !real_pthread_join) {
        errx(EXIT_FAILURE, "native_cpu_init: failed to look up host pthread functions");
    }

    DEBUG_CPU("RIOT native cpu initialized.\n");
}

static void *_thread_entry(void *arg)
{
    jmp_buf exit_buf;

    _cpu_fd = (intptr_t)arg;
    _exit_buf = &exit_buf;

    _wait_for_cpu();

    /* only now the control block on the RIOT stack is guaranteed to be valid */
    native_thread_t *self = _native_thread(thread_get_active());
    _self = self;

    if (setjmp(exit_buf) == 0) {
        /* RIOT threads start with interrupts enabled */
        _native_in_isr = 0;
        irq_enable();

        self->task_func(self->arg);
        sched_task_exit();
    }

    return NULL;
}

char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stacksize)
{
    native_thread_t *ctx;
    sigset_t old;
    int res;

    DEBUG_CPU("thread_stack_init\n");

    /* Place the host thread control block at the top of the RIOT stack.
     * The host thread uses a stack allocated by the host libc, as RIOT
     * stacks are typically too small for host code (and pthreads). */
    uintptr_t top = (uintptr_t)stack_start + stacksize - sizeof(native_thread_t);
    top &= ~(uintptr_t)(_Alignof(max_align_t) - 1);
    expect(top >= (uintptr_t)stack_start);
    ctx = (native_thread_t *)top;

    memset(ctx, 0, sizeof(*ctx));
    ctx->task_func = task_func;
    ctx->arg = arg;
    if (real_pipe(ctx->pipe_fd) == -1) {
        err(EXIT_FAILURE, "thread_stack_init: pipe");
    }
    /* don't leak the pipes on reboot (execve()) */
    real_fcntl(ctx->pipe_fd[0], F_SETFD, FD_CLOEXEC);
    real_fcntl(ctx->pipe_fd[1], F_SETFD, FD_CLOEXEC);

    /* The new host thread inherits our signal mask, it must not receive any
     * signals until it becomes the CPU owner. */
    _block_all_signals(&old);
    _native_pending_syscalls_up();
    res = real_pthread_create(&ctx->pthread, NULL, _thread_entry,
                              (void *)(intptr_t)ctx->pipe_fd[0]);
    _native_pending_syscalls_down();
    pthread_sigmask(SIG_SETMASK, &old, NULL);

    if (res != 0) {
        errx(EXIT_FAILURE, "thread_stack_init: pthread_create() failed: %s",
             strerror(res));
    }

    return (char *)ctx;
}
