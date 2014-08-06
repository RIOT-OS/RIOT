/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     x86-multithreading
 * @{
 *
 * @file
 * @brief       Multi-thread management for x86.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_interrupts.h"
#include "x86_reboot.h"
#include "x86_registers.h"
#include "x86_threading.h"
#include "cpu.h"
#include "irq.h"
#include "kernel_internal.h"
#include "ucontext.h"
#include "sched.h"
#include "stdbool.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static char isr_stack[SIGSTKSZ];
static ucontext_t isr_context;

static char end_stack[SIGSTKSZ];
static ucontext_t end_context;

bool x86_in_isr = true;

static kernel_pid_t fpu_owner = KERNEL_PID_UNDEF;

//static ucontext_t *cur_ctx, *isr_ctx;

int inISR(void)
{
    return x86_in_isr;
}

unsigned disableIRQ(void)
{
    unsigned long eflags = x86_pushf_cli();
    return (eflags & X86_IF) != 0;
}

unsigned enableIRQ(void)
{
    unsigned long eflags;
    asm volatile ("pushf; pop %0; sti" : "=g"(eflags));
    return (eflags & X86_IF) != 0;
}

void restoreIRQ(unsigned state)
{
    if (state) {
        asm volatile ("sti");
    }
    else {
        asm volatile ("cli");
    }
}

int inISR(void);

static void __attribute__((noreturn)) isr_thread_yield(void)
{
    sched_run();
    ucontext_t *ctx = (ucontext_t *) sched_active_thread->sp;
    DEBUG("isr_thread_yield(): switching to (%s, %p)\n\n", sched_active_thread->name, ctx->uc_context.ip);

    uint32_t cr0 = cr0_read();
    cr0 |= CR0_TS;
    cr0_write(cr0);

    x86_in_isr = false;
    setcontext(ctx);
}

void thread_yield(void)
{
    if (x86_in_isr) {
        isr_thread_yield();
    }

    unsigned old_intr = disableIRQ();

    x86_in_isr = true;
    isr_context.uc_stack.ss_sp = isr_stack;
    isr_context.uc_stack.ss_size = sizeof isr_stack;
    makecontext(&isr_context, isr_thread_yield, 0);
    swapcontext((ucontext_t *) sched_active_thread->sp, &isr_context);

    restoreIRQ(old_intr);
}

void isr_cpu_switch_context_exit(void)
{
    DEBUG("XXX: cpu_switch_context_exit(), num_tasks = %d\n", sched_num_threads);

    if (sched_num_threads <= 2) {
        /* there is always "idle" and "x86-hwtimer" */
        DEBUG("cpu_switch_context_exit(): last task has ended. Shutting down.\n");
        x86_shutdown();
    }

    if ((sched_context_switch_request == 1) || (sched_active_thread == NULL)) {
        sched_run();
    }

    ucontext_t *ctx = (ucontext_t *)(sched_active_thread->sp);
    DEBUG("XXX: cpu_switch_context_exit(): calling setcontext(%s, %p)\n\n", sched_active_thread->name, ctx->uc_context.ip);

    x86_in_isr = false;

    setcontext(ctx);
}

void cpu_switch_context_exit(void)
{
    dINT();

    if (!x86_in_isr) {
        x86_in_isr = true;
        isr_context.uc_stack.ss_sp = isr_stack;
        isr_context.uc_stack.ss_size = sizeof isr_stack;
        makecontext(&isr_context, isr_cpu_switch_context_exit, 0);
        setcontext(&isr_context);
    }
    else {
        isr_cpu_switch_context_exit();
    }
    __builtin_unreachable();
}

char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stacksize)
{
    DEBUG("thread_stack_init()\n");

    unsigned int *stk = stack_start;

    ucontext_t *p = (ucontext_t *)(stk + ((stacksize - sizeof(ucontext_t)) / sizeof(void *)));
    stacksize -= sizeof(ucontext_t);

    getcontext(p);
    p->uc_stack.ss_sp = stk;
    p->uc_stack.ss_size = stacksize;
    p->uc_link = &end_context;
    p->uc_context.flags |= X86_IF;
    makecontext(p, (makecontext_fun_t) task_func, 1, arg);

    return (char *) p;
}

static void fpu_used_interrupt(uint8_t intr_num, struct x86_pushad *orig_ctx, unsigned long error_code)
{
    static volatile struct x86_fxsave fpu_data;

    (void) intr_num;
    (void) orig_ctx;
    (void) error_code;

    asm volatile ("clts"); /* clear task switch flag */

    if (fpu_owner == sched_active_pid) {
        return;
    }

    if (fpu_owner != KERNEL_PID_UNDEF) {
        ucontext_t *ctx_owner = (ucontext_t *) sched_threads[fpu_owner]->sp;
        asm volatile ("fxsave (%0)" :: "r"(&fpu_data));
        ctx_owner->__fxsave = fpu_data;
    }

    ucontext_t *ctx_active = (ucontext_t *) sched_active_thread->sp;
    fpu_data = ctx_active->__fxsave;
    asm volatile ("fxrstor (%0)" :: "r"(&fpu_data));

    fpu_owner = sched_active_pid;
}

static void x86_thread_exit(void)
{
    dINT();
    if (fpu_owner == sched_active_pid) {
        fpu_owner = KERNEL_PID_UNDEF;
    }
    sched_task_exit();
}

void x86_init_threading(void)
{
    getcontext(&end_context);
    end_context.uc_stack.ss_sp = end_stack;
    end_context.uc_stack.ss_size = sizeof end_stack;
    makecontext(&end_context, x86_thread_exit, 0);

    x86_interrupt_handler_set(X86_INT_NM, fpu_used_interrupt);

    DEBUG("Threading initialized\n");
}
