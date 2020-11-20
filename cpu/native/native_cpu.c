/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_native
 * @{
 *
 * @file
 * @brief Native CPU kernel_intern.h and sched.h implementation
 *
 * in-process preemptive context switching utilizes POSIX ucontexts.
 * (ucontext provides for architecture independent stack handling)
 *
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define __USE_GNU
#include <signal.h>
#undef __USE_GNU

#include <ucontext.h>
#include <err.h>

#ifdef HAVE_VALGRIND_H
#include <valgrind.h>
#define VALGRIND_DEBUG DEBUG
#elif defined(HAVE_VALGRIND_VALGRIND_H)
#include <valgrind/valgrind.h>
#define VALGRIND_DEBUG DEBUG
#else
#define VALGRIND_STACK_REGISTER(...)
#define VALGRIND_DEBUG(...)
#endif

#include <stdlib.h>

#include "irq.h"
#include "sched.h"

#include "cpu.h"
#include "cpu_conf.h"

#ifdef MODULE_NETDEV_TAP
#include "netdev_tap.h"
extern netdev_tap_t netdev_tap;
#endif

#include "native_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

ucontext_t end_context;
char __end_stack[SIGSTKSZ];

/**
 * make the new context assign `_native_in_isr = 0` before resuming
 */
static void _native_mod_ctx_leave_sigh(ucontext_t *ctx)
{
#ifdef __MACH__
    _native_saved_eip = ((ucontext_t *)ctx)->uc_mcontext->__ss.__eip;
    ((ucontext_t *)ctx)->uc_mcontext->__ss.__eip = (unsigned int)&_native_sig_leave_handler;
#elif defined(__FreeBSD__)
    _native_saved_eip = ((struct sigcontext *)ctx)->sc_eip;
    ((struct sigcontext *)ctx)->sc_eip = (unsigned int)&_native_sig_leave_handler;
#else /* Linux */
#if defined(__arm__)
    _native_saved_eip = ((ucontext_t *)ctx)->uc_mcontext.arm_pc;
    ((ucontext_t *)ctx)->uc_mcontext.arm_pc = (unsigned int)&_native_sig_leave_handler;
#else /* Linux/x86 */
    _native_saved_eip = ctx->uc_mcontext.gregs[REG_EIP];
    ctx->uc_mcontext.gregs[REG_EIP] = (unsigned int)&_native_sig_leave_handler;
#endif
#endif
}

/**
 * TODO: implement
 */
void thread_print_stack(void)
{
    DEBUG("thread_print_stack\n");
    return;
}

/* This function calculates the ISR_usage */
int thread_isr_stack_usage(void)
{
    /* TODO */
    return -1;
}

static inline void *align_stack(void *stack_start, int *stacksize)
{
    const size_t alignment = sizeof(uintptr_t);
    const uintptr_t align_mask = alignment - 1;
    uintptr_t start = (uintptr_t)stack_start;
    size_t unalignment = (start & align_mask)
                         ? (alignment - (start & align_mask)) : 0;
    start += unalignment;
    *stacksize -= unalignment;
    *stacksize &= ~align_mask;
    return (void *)start;
}

char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stacksize)
{
    char *stk;
    ucontext_t *p;

    stack_start = align_stack(stack_start, &stacksize);

    VALGRIND_STACK_REGISTER(stack_start, (char *)stack_start + stacksize);
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n",
                   stack_start, (void*)((int)stack_start + stacksize));

    DEBUG("thread_stack_init\n");

    stk = stack_start;

    /* Use intermediate cast to uintptr_t to silence -Wcast-align. The stack
     * is aligned to word size above. */
    p = (ucontext_t *)(uintptr_t)(stk + (stacksize - sizeof(ucontext_t)));
    stacksize -= sizeof(ucontext_t);

    if (getcontext(p) == -1) {
        err(EXIT_FAILURE, "thread_stack_init: getcontext");
    }

    p->uc_stack.ss_sp = stk;
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

    DEBUG("isr_cpu_switch_context_exit\n");
    if ((sched_context_switch_request == 1) || (thread_get_active() == NULL)) {
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
        DEBUG("cpu_switch_context_exit: last task has ended. exiting.\n");
        real_exit(EXIT_SUCCESS);
    }
#endif

    if (_native_in_isr == 0) {
        irq_disable();
        _native_in_isr = 1;
        native_isr_context.uc_stack.ss_sp = __isr_stack;
        native_isr_context.uc_stack.ss_size = SIGSTKSZ;
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
    DEBUG("isr_thread_yield\n");

    if (_native_sigpend > 0) {
        DEBUG("isr_thread_yield(): handling signals\n\n");
        native_irq_handler();
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
        _native_in_isr = 1;
        irq_disable();
        native_isr_context.uc_stack.ss_sp = __isr_stack;
        native_isr_context.uc_stack.ss_size = SIGSTKSZ;
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

    end_context.uc_stack.ss_sp = __end_stack;
    end_context.uc_stack.ss_size = SIGSTKSZ;
    end_context.uc_stack.ss_flags = 0;
    makecontext(&end_context, sched_task_exit, 0);
    VALGRIND_STACK_REGISTER(__end_stack, __end_stack + sizeof(__end_stack));
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n",
                   (void*)__end_stack, (void*)((int)__end_stack + sizeof(__end_stack)));

    DEBUG("RIOT native cpu initialized.\n");
}
/** @} */
