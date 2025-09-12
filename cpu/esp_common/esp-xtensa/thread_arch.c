/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of kernel's architecture dependent interface
 *
 * This file implements kernel's architecture dependent interface for Xtensa
 * based ESP32x and ESP8266 SoCs
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

#include "esp_attr.h"
#include "rom/ets_sys.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if defined(CPU_ESP32)
#include "soc/dport_access.h"
#include "soc/dport_reg.h"
#elif defined(CPU_ESP8266)
#include "esp8266/rom_functions.h"
#include "sdk/sdk.h"
#endif /* CPU_ESP32 */

#include "esp/xtensa_ops.h"
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

#ifdef __XTENSA_CALL0_ABI__
#define task_exit sched_task_exit
#else /* __XTENSA_CALL0_ABI__ */
/* forward declarations */
NORETURN void task_exit(void);
#endif /* __XTENSA_CALL0_ABI__ */

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

    top_of_stack = (uint8_t*)((uintptr_t)stack_start + stack_size - 1);

    /* BEGIN - code from FreeRTOS port for Xtensa from Cadence */

    /* Create interrupt stack frame aligned to 16 byte boundary */
    sp = (uint8_t*)(((uintptr_t)(top_of_stack + 1) - XT_STK_FRMSZ - XT_CP_SIZE) & ~0xf);

    /* Clear whole stack with a known value to assist debugging */
#if !defined(DEVELHELP) && !IS_ACTIVE(SCHED_TEST_STACK)
        /* Unfortunately, this affects thread_measure_stack_free function */
        memset(stack_start, 0, stack_size);
#else
        memset(sp, 0, XT_STK_FRMSZ + XT_CP_SIZE);
#endif

    /* ensure that stack is big enough */
    assert (sp > (uint8_t*)stack_start);

    /* sp is aligned to 16 byte boundary, so cast is safe here. Use an
     * intermediate cast to uintptr_t to silence -Wcast-align false positive */
    XtExcFrame* exc_frame = (XtExcFrame*)(uintptr_t)sp;

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
    exc_frame->a2 = (uint32_t)arg;                 /* parameters for task_func */
#else
    /* for windowed ABI also set WOE and CALLINC (pretend task was 'call4'd). */
    exc_frame->ps = PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1);
    exc_frame->a4 = (uint32_t)task_exit;         /* task exit point*/
    exc_frame->a6 = (uint32_t)arg;               /* parameters for task_func */
#endif

#ifdef XT_USE_SWPRI
    /* Set the initial virtual priority mask value to all 1's. */
    exc_frame->vpri = 0xFFFFFFFF;
#endif

#if XCHAL_CP_NUM > 0
    /*
     * Init the coprocessor save area (see xtensa_context.h)
     * No access to TCB here, so derive indirectly. Stack growth is top to bottom.
     * p = (uint32_t *) xMPUSettings->coproc_area;
     */
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

#ifdef CPU_ESP8266
extern int MacIsrSigPostDefHdl(void);
unsigned int ets_soft_int_type = ETS_SOFT_INT_NONE;
#endif

/**
 * Context switches are realized using software interrupts since interrupt
 * entry and exit functions are the only way to save and restore complete
 * context including spilling the register windows to the stack
 */
void IRAM_ATTR thread_yield_isr(void* arg)
{
#ifdef CPU_ESP8266
    ETS_NMI_LOCK();

    if (ets_soft_int_type == ETS_SOFT_INT_HDL_MAC) {
        ets_soft_int_type = MacIsrSigPostDefHdl() ? ETS_SOFT_INT_YIELD
                                                  : ETS_SOFT_INT_NONE;
    }

    if (ets_soft_int_type == ETS_SOFT_INT_YIELD) {
        /*
         * set the context switch flag (indicates that context has to be
         * switched on exit from interrupt in _frxt_int_exit
         */
        ets_soft_int_type = ETS_SOFT_INT_NONE;
        _frxt_setup_switch();
    }

    ETS_NMI_UNLOCK();
#else
    /* clear the interrupt first */
#ifdef DPORT_CPU_INTR_FROM_CPU_0_REG
    DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, 0);
#else
    DPORT_WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, 0);
#endif
    /* set the context switch flag (indicates that context has to be switched
       is switch on exit from interrupt in _frxt_int_exit */

    _frxt_setup_switch();
#endif
}

/**
 * If we are already in an interrupt handler, the function simply sets the
 * context switch flag, which indicates that the context has to be switched
 * in the _frxt_int_exit function when exiting the interrupt. Otherwise, we
 * will generate a software interrupt to force the context switch when
 * terminating the software interrupt (see thread_yield_isr).
 */
void IRAM_ATTR thread_yield_higher(void)
{
    /* reset hardware watchdog */
    system_wdt_feed();

    /* yield next task */
#if defined(ENABLE_DEBUG) && defined(DEVELHELP)
    thread_t *active_thread = thread_get_active();
    if (active_thread) {
        DEBUG("%"PRIu32" old task %u %s %u\n", system_get_time(),
               active_thread->pid, active_thread->name,
               active_thread->sp - active_thread-> stack_start);
    }
#endif
    if (!irq_is_in()) {
#if defined(CPU_ESP8266)
        critical_enter();
        ets_soft_int_type = ETS_SOFT_INT_YIELD;
        WSR(BIT(ETS_SOFT_INUM), interrupt);
        critical_exit();
#elif defined(__XTENSA__)
        /* generate the software interrupt to switch the context */
        WSR(BIT(CPU_INUM_SOFTWARE), interrupt);
#elif defined(DPORT_CPU_INTR_FROM_CPU_0_REG)
        /* generate the software interrupt to switch the context */
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, DPORT_CPU_INTR_FROM_CPU_0);
#else
        /* generate the software interrupt to switch the context */
        DPORT_WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, SYSTEM_CPU_INTR_FROM_CPU_0);
#endif /* CPU_ESP8266 */
    }
    else {
        /* set the context switch flag */
        _frxt_setup_switch();
    }

#if defined(ENABLE_DEBUG) && defined(DEVELHELP)
    active_thread = thread_get_active();
    if (active_thread) {
        DEBUG("%"PRIu32" new task %u %s %u\n", system_get_time(),
               active_thread->pid, active_thread->name,
               active_thread->sp - active_thread-> stack_start);
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

#ifndef __XTENSA_CALL0_ABI__
static bool _initial_exit = true;
#endif /* __XTENSA_CALL0_ABI__ */

NORETURN void cpu_switch_context_exit(void)
{
    DEBUG("%s\n", __func__);

    /* Switch context to the highest priority ready task without context save */
#ifdef __XTENSA_CALL0_ABI__
    _frxt_dispatch();
#else /* __XTENSA_CALL0_ABI__ */
    if (_initial_exit) {
        _initial_exit = false;
        __asm__ volatile ("call0 _frxt_dispatch");
    }
    else {
        task_exit();
    }
#endif /* __XTENSA_CALL0_ABI__ */
    UNREACHABLE();
}

#ifndef __XTENSA_CALL0_ABI__
/**
 * The function is used on task exit to switch to the context to the next
 * running task. It realizes only the second half of a complete context by
 * simulating the exit from an interrupt handling where a context switch is
 * forced. The old context is not saved here since it is no longer needed.
 */
NORETURN void task_exit(void)
{
    extern volatile thread_t *sched_active_thread;
    extern volatile kernel_pid_t sched_active_pid;
    DEBUG("sched_task_exit: ending thread %" PRIkernel_pid "...\n",
          thread_getpid());

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
#endif /* __XTENSA_CALL0_ABI__ */
