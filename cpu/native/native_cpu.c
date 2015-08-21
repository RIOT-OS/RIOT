/**
 * Native CPU kernel_intern.h and sched.h implementation
 *
 * in-process preemptive context switching utilizes POSIX ucontexts.
 * (ucontext provides for architecture independent stack handling)
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#include <stdio.h>
#include <unistd.h>

#ifdef __MACH__
#define _XOPEN_SOURCE
#endif
#include <ucontext.h>
#ifdef __MACH__
#undef _XOPEN_SOURCE
#endif
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

#include "kernel_internal.h"
#include "kernel.h"
#include "irq.h"
#include "sched.h"

#include "cpu.h"
#include "cpu_conf.h"

#ifdef MODULE_NETDEV2_TAP
#include "netdev2_tap.h"
extern netdev2_tap_t netdev2_tap;
#endif

#include "native_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

ucontext_t end_context;
char __end_stack[SIGSTKSZ];

#ifdef MODULE_UART0
fd_set _native_rfds;
#endif

int reboot_arch(int mode)
{
    (void) mode;

    printf("\n\n\t\t!! REBOOT !!\n\n");
#ifdef MODULE_UART0
    /* TODO: close stdio fds */
#endif

#ifdef MODULE_NETDEV2_TAP
    netdev2_tap_cleanup(&netdev2_tap);
#endif

    if (real_execve(_native_argv[0], _native_argv, NULL) == -1) {
        err(EXIT_FAILURE, "reboot: execve");
    }

    errx(EXIT_FAILURE, "reboot: this should not have been reached");
}

/**
 * TODO: implement
 */
void thread_print_stack(void)
{
    DEBUG("thread_print_stack\n");
    return;
}

char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stacksize)
{
    char *stk;
    ucontext_t *p;

    VALGRIND_STACK_REGISTER(stack_start, (char *) stack_start + stacksize);
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n", stack_start, (void*)((int)stack_start + stacksize));

    DEBUG("thread_stack_init\n");

    stk = stack_start;

    p = (ucontext_t *)(stk + ((stacksize - sizeof(ucontext_t)) / sizeof(void *)));
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
    if ((sched_context_switch_request == 1) || (sched_active_thread == NULL)) {
        sched_run();
    }

    DEBUG("isr_cpu_switch_context_exit: calling setcontext(%" PRIkernel_pid ")\n\n", sched_active_pid);
    ctx = (ucontext_t *)(sched_active_thread->sp);

    /* the next context will have interrupts enabled due to ucontext */
    DEBUG("isr_cpu_switch_context_exit: native_interrupts_enabled = 1;\n");
    native_interrupts_enabled = 1;
    _native_in_isr = 0;

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
        disableIRQ();
        _native_in_isr = 1;
        native_isr_context.uc_stack.ss_sp = __isr_stack;
        native_isr_context.uc_stack.ss_size = SIGSTKSZ;
        native_isr_context.uc_stack.ss_flags = 0;
        makecontext(&native_isr_context, isr_cpu_switch_context_exit, 0);
        if (setcontext(&native_isr_context) == -1) {
            err(EXIT_FAILURE, "cpu_switch_context_exit: swapcontext");
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

    sched_run();
    ucontext_t *ctx = (ucontext_t *)(sched_active_thread->sp);
    DEBUG("isr_thread_yield: switching to(%" PRIkernel_pid ")\n\n", sched_active_pid);

    native_interrupts_enabled = 1;
    _native_in_isr = 0;
    if (setcontext(ctx) == -1) {
        err(EXIT_FAILURE, "isr_thread_yield: setcontext");
    }
}

void thread_yield_higher(void)
{
    ucontext_t *ctx = (ucontext_t *)(sched_active_thread->sp);
    if (_native_in_isr == 0) {
        _native_in_isr = 1;
        disableIRQ();
        native_isr_context.uc_stack.ss_sp = __isr_stack;
        native_isr_context.uc_stack.ss_size = SIGSTKSZ;
        native_isr_context.uc_stack.ss_flags = 0;
        makecontext(&native_isr_context, isr_thread_yield, 0);
        if (swapcontext(ctx, &native_isr_context) == -1) {
            err(EXIT_FAILURE, "thread_yield_higher: swapcontext");
        }
        enableIRQ();
    }
    else {
        isr_thread_yield();
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
    VALGRIND_DEBUG("VALGRIND_STACK_REGISTER(%p, %p)\n", __end_stack, (void*)((int)__end_stack + sizeof(__end_stack)));

    DEBUG("RIOT native cpu initialized.\n");
}
/** @} */
