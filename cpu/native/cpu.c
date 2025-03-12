/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief  Native CPU kernel_intern.h and sched.h implementation
 * @author Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 *
 * In-process preemptive context switching utilizes POSIX ucontexts.
 * (ucontext provides for architecture independent stack handling)
 */

#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "util/valgrind.h"

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

static ucontext_t _end_context;

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

/* ========================================= */
/* ISR -> user  switch function */

void _isr_switch_to_user(void) {
    DEBUG_CPU("... ISR: switching to user thread, calling setcontext(PID %" PRIkernel_pid ")\n\n", thread_getpid());

    ucontext_t *context = _native_user_context();
    _native_interrupts_enabled = true;

    /* Get PC/LR. This is where we will resume execution on the userspace thread.
     *
     * arm64e: We want the presigned version. This enables us to authenticate the pointer
     * in _native_isr_leave. */
    _native_user_fptr = (uintptr_t)_context_get_fptr(context, /* presigned: */ true);

    /* Now we want to go to _native_isr_leave before resuming execution at _native_user_fptr.
     *
     * arm64: We supply an unsigned pointer, hence presigned=false.
     * This instructs _context_set_fptr to sign the pointer which is necessary given how
     * setcontext is implemented on macOS. */
    _context_set_fptr(context, (uintptr_t)_native_isr_leave, /* presigned: */ false);

    if (setcontext(context) == -1) {
        err(EXIT_FAILURE, "_isr_schedule_and_switch: setcontext");
    }
    errx(EXIT_FAILURE, "2 this should have never been reached!!");
}

/* ========================================= */

void _isr_context_switch_exit(void)
{
    DEBUG_CPU("_isr_schedule_and_switch\n");
    /* Schedule thread job if no active thread */
    if (((sched_context_switch_request == 1) || (thread_get_active() == NULL))
        && IS_USED(MODULE_CORE_THREAD)) {
        /* Schedule active thread */
        sched_run();
    }

    /* Switch to active userspace thread */
    _isr_switch_to_user();
}

/*               ^
 *               |
 *               |
 * cpu_switch_context_exit continues
 * in ISR context in _isr_context_switch_exit
 */

void cpu_switch_context_exit(void)
{
# ifdef NATIVE_AUTO_EXIT
    if (sched_num_threads <= 1) {
        extern unsigned _native_retval;
        DEBUG_CPU("cpu_switch_context_exit: last task has ended. exiting.\n");
        real_exit(_native_retval);
    }
# endif

    if (_native_in_isr == 0) {
        /* Disable interrupts while switching */
        irq_disable();
        _native_in_isr = 1;

        _native_isr_context_make(_isr_context_switch_exit);
        if (setcontext(_native_isr_context) == -1) {
            err(EXIT_FAILURE, "cpu_switch_context_exit: setcontext");
        }
        errx(EXIT_FAILURE, "1 this should have never been reached!!");
    }
    else {
        _isr_context_switch_exit();
    }
    errx(EXIT_FAILURE, "3 this should have never been reached!!");
}

/* ========================================= */


void _isr_thread_yield(void)
{
    DEBUG_CPU("... ISR: switched to ISR context, scheduling\n");

    if (_native_pending_signals > 0) {
        DEBUG_CPU("... ISR: pending signals, handling signals\n\n");
        _native_call_sig_handlers_and_switch();
    }

    if (!IS_USED(MODULE_CORE_THREAD)) {
        return;
    }

    /* Set active thread */
    sched_run();

    /* Switch to active userspace thread */
    _isr_switch_to_user();
}

/*               ^
 *               |
 *               |
 * thread_yield_higher continues
 * in ISR context in _isr_thread_yield
 */

void thread_yield_higher(void)
{
    sched_context_switch_request = 1;

    if (_native_in_isr == 0 && _native_interrupts_enabled) {
        DEBUG_CPU("yielding higher priority thread, switching to ISR context ...\n");

        _native_in_isr = 1;
        irq_disable();

        /* Create the ISR context, will execute isr_thread_yield */
        _native_isr_context_make(_isr_thread_yield);
        if (swapcontext(_native_user_context(), _native_isr_context) == -1) {
            err(EXIT_FAILURE, "thread_yield_higher: swapcontext");
        }
        irq_enable();
    }
}

/* ========================================= */

void native_cpu_init(void)
{
    if (getcontext(&_end_context) == -1) {
        err(EXIT_FAILURE, "native_cpu_init: getcontext");
    }

    /* The _end_context allows RIOT to execute code after a thread task func returns.
     * This works as follows (explanation based on libplatform)
     *  - In thread_stack_init, we call makecontext with the thread task func
     *    and uc_link = _end_context.
     *  - makecontext modifies the ucontext so that _ctx_start (in the libc/libplatform impl)
     *    is called when setcontext is executed. The thread task func resides in a register.
     *  - When the thread is started using setcontext, _ctx_start branches and links to the
     *    the task func. After the task func returns, _ctx_start would normally call exit.
     *    However, if _end_context is set, it calls setcontext on th bespoke _end_context.
     */
    _end_context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    expect(_end_context.uc_stack.ss_sp != NULL);
    _end_context.uc_stack.ss_size = SIGSTKSZ;
    _end_context.uc_stack.ss_flags = 0;
    makecontext(&_end_context, sched_task_exit, 0);

    (void)VALGRIND_STACK_REGISTER(_end_context.uc_stack.ss_sp,
                                  (char *)_end_context.uc_stack.ss_sp + _end_context.uc_stack.ss_size);
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n",
                   (void*)_end_context.uc_stack.ss_sp,
                   (void*)((char *)_end_context.uc_stack.ss_sp + _end_context.uc_stack.ss_size));

    DEBUG_CPU("RIOT native cpu initialized.\n");
}

/* ========================================= */

static inline void *align_stack(uintptr_t start, int *stacksize)
{
    const size_t alignment = sizeof(uintptr_t);
    const uintptr_t align_mask = alignment - 1;
    size_t unalignment = (start & align_mask)
                         ? (alignment - (start & align_mask)) : 0;
    start += unalignment;
    *stacksize -= unalignment;
    *stacksize &= ~align_mask;
    return (void *)start;
}

char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stacksize)
{
    ucontext_t *p;

    stack_start = align_stack((uintptr_t)stack_start, &stacksize);

    (void) VALGRIND_STACK_REGISTER(stack_start, (char *)stack_start + stacksize);
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n",
                   stack_start, (void*)((char *)stack_start + stacksize));

    DEBUG_CPU("thread_stack_init\n");

    /* Use intermediate cast to uintptr_t to silence -Wcast-align. The stack
     * is aligned to word size above. */
    p = (ucontext_t *)(uintptr_t)((uint8_t *)stack_start + (stacksize - sizeof(ucontext_t)));
    /* Stack guards might be in the way. */
    memset(p, 0, sizeof(ucontext_t));
    stacksize -= sizeof(ucontext_t);

    if (getcontext(p) == -1) {
        err(EXIT_FAILURE, "thread_stack_init: getcontext");
    }

    p->uc_stack.ss_sp = stack_start;
    p->uc_stack.ss_size = stacksize;
    p->uc_stack.ss_flags = 0;
    p->uc_link = &_end_context;

    if (sigemptyset(&(p->uc_sigmask)) == -1) {
        err(EXIT_FAILURE, "thread_stack_init: sigemptyset");
    }

    makecontext64(p, (void (*)(void))task_func, arg);

    return (char *) p;
}
