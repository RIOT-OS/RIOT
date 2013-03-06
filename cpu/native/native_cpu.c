/**
 * Native CPU kernel_intern.h and sched.h implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */
#include <stdio.h>
#include <ucontext.h>
#include <err.h>

#include "kernel_intern.h"
#include "sched.h"

#include "cpu.h"
#include "cpu-conf.h"
#include "debug.h"

extern volatile tcb_t *active_thread;
static ucontext_t native_context;
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
    p = (ucontext_t*)stk;
    stk += sizeof(ucontext_t)/sizeof(void*);
    stacksize -= sizeof(ucontext_t);
#else
    p = (ucontext_t*)(stk + ((stacksize-sizeof(ucontext_t))/sizeof(void*)));
    stacksize -= sizeof(ucontext_t);
#endif

    if (getcontext(p) == -1) {
        err(1, "thread_stack_init(): getcontext()");
    }

    p->uc_stack.ss_sp = stk;
    p->uc_stack.ss_size = stacksize;
    p->uc_stack.ss_flags = 0;
    p->uc_link = &native_context;
    if (sigemptyset(&(p->uc_sigmask)) == -1) {
        err(1, "thread_stack_init(): sigemptyset()");
    }

    makecontext(p, task_func, 0);

    return (char *) p;
}

void cpu_switch_context_exit(void)
{
    DEBUG("XXX: cpu_switch_context_exit()\n");
    //active_thread = sched_threads[0];
    sched_run();

    DEBUG("XXX: cpu_switch_context_exit(): calling setcontext(%s)\n\n", active_thread->name);
    if (setcontext((ucontext_t*)(active_thread->sp)) == -1) {
        err(1, "cpu_switch_context_exit(): setcontext():");
    }
}

void thread_yield()
{
    ucontext_t *oc;

    DEBUG("thread_yield()\n");

    oc = (ucontext_t*)(active_thread->sp);
    sched_run();

    DEBUG("thread_yield(): calling swapcontext(%s)\n\n", active_thread->name);
    if (swapcontext(oc, (ucontext_t*)(active_thread->sp)) == -1) {
        err(1, "thread_yield(): swapcontext()");
    }
}

void native_cpu_init()
{
    if (getcontext(&native_context) == -1) {
        err(1, "native_context(): getcontext()");
    }
    native_context.uc_stack.ss_sp = __isr_stack;
    native_context.uc_stack.ss_size = SIGSTKSZ;
    native_context.uc_stack.ss_flags = 0;
    makecontext(&native_context, sched_task_exit, 0);
    puts("RIOT native cpu initialized.");
}
