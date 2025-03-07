/*
 * Native CPU kernel_intern.h and sched.h implementation
 *
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * Author: Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * Author: Kaspar Schleiser <kaspar@schleiser.de>
 */

/**
 * @file
 * @brief Native CPU kernel_intern.h and sched.h implementation
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * In-process preemptive context switching utilizes POSIX ucontexts.
 * (ucontext provides for architecture independent stack handling)
 */

#include <err.h>
#include <stdio.h>
#include <unistd.h>

#ifdef HAVE_VALGRIND_H
#include <valgrind.h>
#define VALGRIND_DEBUG DEBUG
#elif defined(HAVE_VALGRIND_VALGRIND_H)
#include <valgrind/valgrind.h>
#define VALGRIND_DEBUG DEBUG
#else
#define VALGRIND_STACK_REGISTER(...) (0)
#define VALGRIND_DEBUG(...)
#endif

#include <stdlib.h>
#include <signal.h>
#include <string.h>

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
 * make the new context assign `_native_in_isr = 0` before resuming
 */
static void _native_mod_ctx_leave_sigh(ucontext_t *ctx)
{
#if defined(__FreeBSD__)
    _native_saved_eip = ((struct sigcontext *)ctx)->sc_eip;
    ((struct sigcontext *)ctx)->sc_eip = (unsigned int)&_native_sig_leave_handler;
#else /* Linux */
#if defined(__arm__)
    _native_saved_eip = ((ucontext_t *)ctx)->uc_mcontext.arm_pc;
    ((ucontext_t *)ctx)->uc_mcontext.arm_pc = (unsigned int)&_native_sig_leave_handler;
#else /* Linux/x86 */
  #ifdef __x86_64__
    _native_saved_eip = ctx->uc_mcontext.gregs[REG_RIP];
    ctx->uc_mcontext.gregs[REG_RIP] = (unsigned long)&_native_sig_leave_handler;
  #else
    _native_saved_eip = ctx->uc_mcontext.gregs[REG_EIP];
    ctx->uc_mcontext.gregs[REG_EIP] = (unsigned int)&_native_sig_leave_handler;
  #endif
#endif
#endif
}

/**
 * TODO: implement
 */
void thread_print_stack(void)
{
    CPU_DEBUG("thread_print_stack\n");
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
    DEBUG_CPU("... ISR: switching to user thread, calling setcontext(PID %" PRIkernel_pid ")\n\n", thread_getpid());

    (void) VALGRIND_STACK_REGISTER(stack_start, (char *)stack_start + stacksize);
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n",
                   stack_start, (void*)((char *)stack_start + stacksize));

    DEBUG("thread_stack_init\n");

    /* Use intermediate cast to uintptr_t to silence -Wcast-align. The stack
     * is aligned to word size above. */
    p = (ucontext_t *)(uintptr_t)((uint8_t *)stack_start + (stacksize - sizeof(ucontext_t)));
    stacksize -= sizeof(ucontext_t);

    if (getcontext(p) == -1) {
        err(EXIT_FAILURE, "thread_stack_init: getcontext");
    }

    p->uc_stack.ss_sp = stack_start;
    p->uc_stack.ss_size = stacksize;
    p->uc_stack.ss_flags = 0;
    p->uc_link = &end_context;

    if (sigemptyset(&(p->uc_sigmask)) == -1) {
        err(EXIT_FAILURE, "thread_stack_init: sigemptyset");
    }

    makecontext(p, (void (*)(void)) task_func, 1, arg);

    return (char *) p;
}

void isr_cpu_switch_context_exit(void)
{
    ucontext_t *ctx;

    DEBUG_CPU("_isr_schedule_and_switch\n");
    if (((sched_context_switch_request == 1) || (thread_get_active() == NULL))
        && IS_USED(MODULE_CORE_THREAD)) {
        sched_run();
    }

    DEBUG("isr_cpu_switch_context_exit: calling setcontext(%" PRIkernel_pid ")\n\n", thread_getpid());
    /* Use intermediate cast to uintptr_t to silence -Wcast-align.
     * stacks are manually word aligned in thread_static_init() */
    ctx = (ucontext_t *)(uintptr_t)(thread_get_active()->sp);

    native_interrupts_enabled = 1;
    _native_mod_ctx_leave_sigh(ctx);

    if (setcontext(ctx) == -1) {
        err(EXIT_FAILURE, "isr_cpu_switch_context_exit: setcontext");
    }
    errx(EXIT_FAILURE, "2 this should have never been reached!!");
}

void cpu_switch_context_exit(void)
{
#ifdef NATIVE_AUTO_EXIT
    if (sched_num_threads <= 1) {
        extern unsigned _native_retval;
        DEBUG_CPU("cpu_switch_context_exit: last task has ended. exiting.\n");
        real_exit(_native_retval);
    }
#endif

    if (_native_in_isr == 0) {
        irq_disable();
        _native_in_isr = 1;
        native_isr_context.uc_stack.ss_sp = __isr_stack;
        native_isr_context.uc_stack.ss_size = __isr_stack_size;
        native_isr_context.uc_stack.ss_flags = 0;
        makecontext(&native_isr_context, isr_cpu_switch_context_exit, 0);
        if (setcontext(&native_isr_context) == -1) {
            err(EXIT_FAILURE, "cpu_switch_context_exit: setcontext");
        }
        errx(EXIT_FAILURE, "1 this should have never been reached!!");
    }
    else {
        isr_cpu_switch_context_exit();
    }
    errx(EXIT_FAILURE, "3 this should have never been reached!!");
}

void isr_thread_yield(void)
{
    DEBUG_CPU("... ISR: switched to ISR context, scheduling\n");

    if (_native_sigpend > 0) {
        native_irq_handler();
        DEBUG_CPU("... ISR: pending signals, handling signals\n\n");
    }

    if (!IS_USED(MODULE_CORE_THREAD)) {
        return;
    }
    sched_run();

    /* Use intermediate cast to uintptr_t to silence -Wcast-align.
     * stacks are manually word aligned in thread_static_init() */
    ucontext_t *ctx = (ucontext_t *)(uintptr_t)(thread_get_active()->sp);
    DEBUG("isr_thread_yield: switching to(%" PRIkernel_pid ")\n\n",
          thread_getpid());

    native_interrupts_enabled = 1;
    _native_mod_ctx_leave_sigh(ctx);

    if (setcontext(ctx) == -1) {
        err(EXIT_FAILURE, "isr_thread_yield: setcontext");
    }
}

void thread_yield_higher(void)
{
    sched_context_switch_request = 1;

    if (_native_in_isr == 0 && native_interrupts_enabled) {
        /* Use intermediate cast to uintptr_t to silence -Wcast-align.
         * stacks are manually word aligned in thread_static_init() */
        ucontext_t *ctx = (ucontext_t *)(uintptr_t)(thread_get_active()->sp);
        DEBUG_CPU("yielding higher priority thread, switching to ISR context ...\n");
        _native_in_isr = 1;
        irq_disable();
        native_isr_context.uc_stack.ss_sp = __isr_stack;
        native_isr_context.uc_stack.ss_size = __isr_stack_size;
        native_isr_context.uc_stack.ss_flags = 0;
        makecontext(&native_isr_context, isr_thread_yield, 0);
        if (swapcontext(ctx, &native_isr_context) == -1) {
            err(EXIT_FAILURE, "thread_yield_higher: swapcontext");
        }
        irq_enable();
    }
}

void native_cpu_init(void)
{
    if (getcontext(&end_context) == -1) {
        err(EXIT_FAILURE, "native_cpu_init: getcontext");
    }

    end_context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    expect(end_context.uc_stack.ss_sp != NULL);
    end_context.uc_stack.ss_size = SIGSTKSZ;
    end_context.uc_stack.ss_flags = 0;
    makecontext(&end_context, sched_task_exit, 0);
    (void)VALGRIND_STACK_REGISTER(end_context.uc_stack.ss_sp,
                                  (char *)end_context.uc_stack.ss_sp + end_context.uc_stack.ss_size);
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n",
                   (void*)end_context.uc_stack.ss_sp,
                   (void*)((char *)end_context.uc_stack.ss_sp + end_context.uc_stack.ss_size));

    DEBUG("RIOT native cpu initialized.\n");
}
/** @} */
