/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's architecture dependent thread interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

/*
 * PLEASE NOTE: Some parts of the code are taken from the FreeRTOS port for
 * Xtensa processors from Cadence Design Systems. These parts are marked
 * accordingly. For these parts, the following license is valid:
 *
 * Copyright (c) 2003-2015 Cadence Design Systems, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "log.h"
#include "thread.h"
#include "sched.h"

#include "esp_common.h"
#include "irq_arch.h"
#include "syscalls.h"
#include "tools.h"

#include "esp/xtensa_ops.h"
#include "rom/ets_sys.h"
#include "soc/dport_reg.h"
#include "xtensa/xtensa_context.h"

/* User exception dispatcher when exiting */
extern void _xt_user_exit(void);

/* Switch context to the highest priority ready task without context save */
extern void _frxt_dispatch(void);

/* Set an flag indicating that a task switch is required on return from interrupt */
extern void _frxt_setup_switch(void);

/* Switch context to the highest priority ready task with context save */
extern void vPortYield(void);
extern void vPortYieldFromInt(void);

/* forward declarations */
NORETURN void task_exit(void);

char* thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size)
{
    /* Stack layout after task stack initialization
     *
     *                                            +------------------------+
     *                                            |                        | TOP
     *                                            |  thread_control_block  |
     *        stack_start + stack_size        ==> |                        | top_of_stack+1
     *                                            +------------------------+
     *        top_of_stack                    ==> |                        |
     *                                            |        XT_CP_SA        |
     *                                            |       (optional)       |
     *                                            | ...                    | ...
     *                                            | cpstored               | XT_CPSTORED
     *        top_of_stack + 1 - XT_CP_SIZE   ==> | cpenable               | XT_CPENABLE
     *        (cp_state)                          +------------------------+
     *                                            |                        |
     *                                            |      XT_STK_FRAME      |
     *                                            |                        | XT_STK_...
     *                                            | a2 = arg               | XT_STK_A2
     *                                            | a1 = sp + XT_STK_FRMSZ | XT_STK_A1
     *                                            | a0 = sched_task_exit   | XT_STK_A0
     *                                            | ps = PS_UM | PS_EXCM   | XT_STK_PS
     *                                            | pc = task_func         | XT_STK_PC
     *   sp = top_of_stack + 1 - XT_CP_SIZE   ==> | exit = _xt_user_exit   | XT_STK_EXIT
     *                         - XT_STK_FRMSZ     +------------------------+
     *                                            |                        |
     *                                            | remaining stack space  |
     *                                            |   available for data   |
     *        stack_start (preallocated var)  ==> |                        | BOTTOM
     *                                            +------------------------+
     *
     * Initialized stack frame represents the registers as set when the
     * the task function would have been called.
     *
     * Registers in a called function
     *
     *   pc - PC at the beginning in the function
     *   a0 - return address from the function (return address to caller)
     *   a1 - current stack pointer at the beginning in the function
     *   a2 - first argument of the function
     */

    /* stack is [stack_start+0 ... stack_start+stack_size-1] */
    uint8_t *top_of_stack;
    uint8_t *sp;

    top_of_stack = (uint8_t*)((uint32_t)stack_start + stack_size - 1);

    /* BEGIN - code from FreeRTOS port for Xtensa from Cadence */

    /* Create interrupt stack frame aligned to 16 byte boundary */
    sp = (uint8_t*)(((uint32_t)(top_of_stack + 1) - XT_STK_FRMSZ - XT_CP_SIZE) & ~0xf);

    /* Clear whole stack with a known value to assist debugging */
    #if !defined(DEVELHELP) && !defined(SCHED_TEST_STACK)
        /* Unfortunatly, this affects thread_measure_stack_free function */
        memset(stack_start, 0, stack_size);
    #else
        memset(sp, 0, XT_STK_FRMSZ + XT_CP_SIZE);
    #endif

    /* ensure that stack is big enough */
    assert (sp > (uint8_t*)stack_start);

    XtExcFrame* exc_frame = (XtExcFrame*)sp;

    /* Explicitly initialize certain saved registers for call0 ABI */
    exc_frame->pc   = (uint32_t)task_func;         /* task entry point */
    exc_frame->a0   = (uint32_t)task_exit;         /* task exit point*/
    exc_frame->a1   = (uint32_t)sp + XT_STK_FRMSZ; /* physical top of stack frame */
    exc_frame->exit = (uint32_t)_xt_user_exit;     /* user exception exit dispatcher */

    /* Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode. */
    /* Also set entry point argument parameter. */
    #ifdef __XTENSA_CALL0_ABI__
    /* for CALL0 ABI set in parameter a2 to task argument */
    exc_frame->ps = PS_UM | PS_EXCM;
    exc_frame->a2 = (uint32_t)arg;               /* parameters for task_func */
    #else
    /* for Windowed Register ABI set PS.CALLINC=01 to handle task entry as
       call4 return address in a4 and parameter in a6 and */
    exc_frame->ps = PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1);
    exc_frame->a4 = (uint32_t)task_exit;         /* task exit point*/
    exc_frame->a6 = (uint32_t)arg;               /* parameters for task_func */
    #endif

    #ifdef XT_USE_SWPRI
    /* Set the initial virtual priority mask value to all 1's. */
    exc_frame->vpri = 0xFFFFFFFF;
    #endif

    #if XCHAL_CP_NUM > 0
    /* Init the coprocessor save area (see xtensa_context.h) */
    /* No access to TCB here, so derive indirectly. Stack growth is top to bottom. */
    /* p = (uint32_t *) xMPUSettings->coproc_area; */

    uint32_t *p;

    p = (uint32_t *)(((uint32_t)(top_of_stack + 1) - XT_CP_SIZE));
    p[0] = 0;
    p[1] = 0;
    p[2] = (((uint32_t) p) + 12 + XCHAL_TOTAL_SA_ALIGN - 1) & -XCHAL_TOTAL_SA_ALIGN;
    #endif

    /* END - code from FreeRTOS port for Xtensa from Cadence */
    DEBUG("%s start=%p size=%d top=%p sp=%p free=%u\n",
          __func__, stack_start, stack_size, top_of_stack, sp, sp-(uint8_t*)stack_start);
    return (char*)sp;
}

/**
 * Context switches are realized using software interrupts since interrupt
 * entry and exit functions are the only way to save and restore complete
 * context including spilling the register windows to the stack
 */
void IRAM_ATTR thread_yield_isr(void* arg)
{
    /* clear the interrupt first */
    DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, 0);
    /* set the context switch flag (indicates that context has to be switched
       is switch on exit from interrupt in _frxt_int_exit */
    _frxt_setup_switch();
}

/**
 * If we are already in an interrupt handler, the function simply sets the
 * context switch flag, which indicates that the context has to be switched
 * in the _frxt_int_exit function when exiting the interrupt. Otherwise, we
 * will generate a software interrupt to force the context switch when
 * terminating the software interrupt (see thread_yield_isr).
 */
void  thread_yield_higher(void)
{
    /* reset hardware watchdog */
    system_wdt_feed();

    /* yield next task */
    #if defined(ENABLE_DEBUG) && defined(DEVELHELP)
    if (sched_active_thread) {
        DEBUG("%u old task %u %s %u\n", system_get_time(),
               sched_active_thread->pid, sched_active_thread->name,
               sched_active_thread->sp - sched_active_thread-> stack_start);
    }
    #endif

    if (!irq_is_in()) {
        /* generate the software interrupt to switch the context */
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, DPORT_CPU_INTR_FROM_CPU_0);
    }
    else {
        /* set the context switch flag */
        _frxt_setup_switch();
    }

    #if defined(ENABLE_DEBUG) && defined(DEVELHELP)
    if (sched_active_thread) {
        DEBUG("%u new task %u %s %u\n", system_get_time(),
               sched_active_thread->pid, sched_active_thread->name,
               sched_active_thread->sp - sched_active_thread-> stack_start);
    }
    #endif

    /*
     * Instruction fetch synchronize: Waits for all previously fetched load,
     * store, cache, and special register write instructions that affect
     * instruction fetch to be performed before fetching the next instruction.
     */
    __asm__("isync");

    return;
}

void  thread_stack_print(void)
{
    /* Print the current stack to stdout. */

    #if defined(DEVELHELP)
    volatile thread_t* task = thread_get(sched_active_pid);
    if (task) {

        char* stack_top = task->stack_start + task->stack_size;
        int   size = stack_top - task->sp;
        printf("Printing current stack of thread %" PRIkernel_pid "\n", thread_getpid());
        esp_hexdump((void*)(task->sp), size >> 2, 'w', 8);
    }
    #else
    NOT_SUPPORTED();
    #endif
}

void  thread_print_stack(void)
{
    /* Prints human readable, ps-like thread information for debugging purposes. */
    /* because of Xtensa stack structure and call ABI, it is not possible to implement */
    NOT_YET_IMPLEMENTED();
    return;
}

#ifdef DEVELHELP

extern uint8_t port_IntStack;
extern uint8_t port_IntStackTop;

void thread_isr_stack_init(void)
{
    /* code from thread.c, please see the copyright notice there */

    /* assign each int of the stack the value of it's address */
    uintptr_t *stackmax = (uintptr_t *)&port_IntStackTop;
    uintptr_t *stackp = (uintptr_t *)&port_IntStack;

    while (stackp < stackmax) {
        *stackp = (uintptr_t) stackp;
        stackp++;
    }
}

int thread_isr_stack_usage(void)
{
    return &port_IntStackTop - &port_IntStack -
           thread_measure_stack_free((char*)&port_IntStack);
}

void *thread_isr_stack_pointer(void)
{
    /* Get the current ISR stack pointer. */
    return &port_IntStackTop;
}

void *thread_isr_stack_start(void)
{
    /* Get the start of the ISR stack. */
    return &port_IntStack;
}

void thread_isr_stack_print(void)
{
    printf("Printing current ISR\n");
    esp_hexdump(&port_IntStack, &port_IntStackTop-&port_IntStack, 'w', 8);
}

#else /* DEVELHELP */

void thread_isr_stack_init(void) {}

#endif /* DEVELHELP */

static bool _initial_exit = true;

/**
 * The function is used on task exit to switch to the context to the next
 * running task. It realizes only the second half of a complete context by
 * simulating the exit from an interrupt handling where a context switch is
 * forced. The old context is not saved here since it is no longer needed.
 */
NORETURN void task_exit(void)
{
    DEBUG("sched_task_exit: ending thread %" PRIkernel_pid "...\n",
          sched_active_thread ? sched_active_thread->pid : KERNEL_PID_UNDEF);

    (void) irq_disable();

    /* remove old task from scheduling if it is not already done */
    if (sched_active_thread) {
        sched_threads[sched_active_pid] = NULL;
        sched_num_threads--;
        sched_set_status((thread_t *)sched_active_thread, STATUS_STOPPED);
        sched_active_thread = NULL;
    }

    /* determine the new running task */
    sched_run();

    /* set the context switch flag (indicates that context has to be switched
       is switch on exit from interrupt in _frxt_int_exit */
    _frxt_setup_switch();

    /* set interrupt nesting level to the right value */
    irq_interrupt_nesting++;

    /* reset windowed registers */
    __asm__ volatile ("movi a2, 0\n"
                      "wsr a2, windowstart\n"
                      "wsr a2, windowbase\n"
                      "rsync\n");

    /* exit from simulated interrupt to switch to the new context */
    __asm__ volatile ("call0 _frxt_int_exit");

    /* should not be executed */
    UNREACHABLE();
}

NORETURN void cpu_switch_context_exit(void)
{
    DEBUG("%s\n", __func__);

    /* Switch context to the highest priority ready task without context save */
    if (_initial_exit) {
        _initial_exit = false;
        __asm__ volatile ("call0 _frxt_dispatch");
    }
    else {
        task_exit();
    }
    UNREACHABLE();
}
