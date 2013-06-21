/**
 * Native CPU kernel_intern.h and sched.h implementation
 *
 * in-process preemptive context switching utilizes POSIX ucontexts.
 * (ucontext provides for architecture independent stack handling)
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */
#include <stdio.h>
#ifdef __MACH__
#define _XOPEN_SOURCE
#endif
#include <ucontext.h>
#ifdef __MACH__
#undef _XOPEN_SOURCE
#endif
#include <err.h>

#include "kernel_intern.h"
#include "sched.h"

#include "cpu.h"
#include "cpu-conf.h"
#include "debug.h"

extern volatile tcb_t *active_thread;
static ucontext_t end_context;
static char __isr_stack[SIGSTKSZ];

/**
 * TODO: implement
 */
void thread_print_stack(void)
{
    DEBUG("XXX: thread_print_stack()\n");
    return;
}

char *thread_stack_init(void *task_func, void *stack_start, int stacksize)
{
    unsigned int *stk;
    ucontext_t *p;

    DEBUG("thread_stack_init()\n");

    stk = stack_start;

#ifdef NATIVESPONTOP
    p = (ucontext_t *)stk;
    stk += sizeof(ucontext_t) / sizeof(void *);
    stacksize -= sizeof(ucontext_t);
#else
    p = (ucontext_t *)(stk + ((stacksize - sizeof(ucontext_t)) / sizeof(void *)));
    stacksize -= sizeof(ucontext_t);
#endif

    if(getcontext(p) == -1) {
        err(1, "thread_stack_init(): getcontext()");
    }

    p->uc_stack.ss_sp = stk;
    p->uc_stack.ss_size = stacksize;
    p->uc_stack.ss_flags = 0;
    p->uc_link = &end_context;

    if(sigemptyset(&(p->uc_sigmask)) == -1) {
        err(1, "thread_stack_init(): sigemptyset()");
    }

    makecontext(p, task_func, 0);

    return (char *) p;
}

void cpu_switch_context_exit(void)
{
    ucontext_t *ctx;

    DEBUG("XXX: cpu_switch_context_exit()\n");
    //active_thread = sched_threads[0];
    sched_run();

    DEBUG("XXX: cpu_switch_context_exit(): calling setcontext(%s)\n\n", active_thread->name);
    ctx = (ucontext_t *)(active_thread->sp);
    eINT(); // XXX: workaround for bug (?) in sched_task_exit

    if(setcontext(ctx) == -1) {
        err(1, "cpu_switch_context_exit(): setcontext():");
    }
}

void thread_yield()
{
    /**
     * XXX: check whether it is advisable to switch context for sched_run()
     */
    ucontext_t *oc, *nc;

    DEBUG("thread_yield()\n");

    oc = (ucontext_t *)(active_thread->sp);

    sched_run();

    nc = (ucontext_t *)(active_thread->sp);

    if(nc != oc) {
        DEBUG("thread_yield(): calling swapcontext(%s)\n\n", active_thread->name);

        if(swapcontext(oc, nc) == -1) {
            err(1, "thread_yield(): swapcontext()");
        }
    }
    else {
        DEBUG("thread_yield(): old = new, returning to context (%s)\n\n", active_thread->name);
    }
}

void native_cpu_init()
{
    if(getcontext(&end_context) == -1) {
        err(1, "end_context(): getcontext()");
    }

    end_context.uc_stack.ss_sp = __isr_stack;
    end_context.uc_stack.ss_size = SIGSTKSZ;
    end_context.uc_stack.ss_flags = 0;
    makecontext(&end_context, sched_task_exit, 0);

    puts("RIOT native cpu initialized.");
}
/** @} */
