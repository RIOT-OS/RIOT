/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file        thread_arch.c
 * @brief       Implementation of the kernel's architecture dependent thread interface
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "arch/thread_arch.h"
#include "sched.h"
#include "irq.h"
#include "cpu.h"
#include "kernel_internal.h"


static void context_save(void);
static void context_restore(void);
static void enter_thread_mode(void);

/**
 * cortex m4 knows stacks and handles register backups
 *
 * so use different stack frame layout
 *
 *
 * with float storage
 * ------------------------------------------------------------------------------------------------------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR | S0 | S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8 | S9 | S10 | S11 | S12 | S13 | S14 | S15 | FPSCR |
 * ------------------------------------------------------------------------------------------------------------------------------------
 *
 * without
 *
 * --------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR |
 * --------------------------------------
 *
 */
char *thread_arch_stack_init(void  (*task_func)(void), void *stack_start, int stack_size)
{
    unsigned int * stk;
    stk = (unsigned int *)(stack_start + stack_size);

    /* marker */
    stk--;
    *stk = 0x77777777;

    /* TODO: fix FPU handling for Cortex-M4 */
    /*
    stk--;
    *stk = (unsigned int) 0;

    // S0 - S15
    for (int i = 15; i >= 0; i--) {
        stk--;
        *stk = i;
    }
    */

    /* FIXME xPSR */
    stk--;
    *stk = (unsigned int) 0x01000200;

    /* program counter */
    stk--;
    *stk = (unsigned int) task_func;

    /* link register, jumped to when thread exits */
    stk--;
    *stk = (unsigned int) sched_task_exit;

    /* r12 */
    stk--;
    *stk = (unsigned int) 0;

    /* r0 - r3 */
    for (int i = 3; i >= 0; i--) {
        stk--;
        *stk = i;
    }

    /* r11 - r4 */
    for (int i = 11; i >= 4; i--) {
        stk--;
        *stk = i;
    }

    /* lr means exception return code  */
    stk--;
    *stk = (unsigned int) 0xfffffffd; /* return to task-mode main stack pointer */

    return (char*) stk;
}

void thread_arch_stack_print(void)
{
    /* TODO */
}

/**
 *
 */
void thread_arch_start_threading(void)
{
    sched_run();
    enableIRQ();
    enter_thread_mode();
}

/**
 * @brief Set the MCU into Thread-Mode and load the initial task from the stack and run it
 *
 * This function is only called once on startup!
 */
void enter_thread_mode(void)
{
    /* switch to user mode use PSP instead of MSP in ISR Mode*/
    CONTROL_Type mode;
    mode.w = __get_CONTROL();
    mode.b.SPSEL = 1; /* select PSP */
    mode.b.nPRIV = 0; /* privilege */
    __set_CONTROL(mode.w);

    /* load pdc->stackpointer in r0 */
    asm("ldr    r0, =active_thread" );      /* r0 = &active_thread */
    asm("ldr    r0, [r0]"           );      /* r0 = *r0 = active_thread */
    asm("ldr    sp, [r0]"           );      /* sp = r0  restore stack pointer*/
    asm("pop    {r4}"               );      /* skip exception return */
    asm("pop    {r4-r11}"           );
    asm("pop    {r0-r3,r12,lr}"     );      /* get registers from stack */
    asm("pop    {r4}"               );      /* get PC */
    asm("pop    {r5}"               );      /* discard the xPSR entry */
    asm("mov    pc, r4"             );      /* load PC */
}

void thread_arch_yield(void)
{
    if (irq_arch_in()) {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;    /* set PendSV Bit */
    } else {
        asm("svc 0x01\n");                      /* trigger SVC interrupt, which will trigger the pendSV (needed?) */
    }
}

/**
 * @brief SVC interrupt handler (to be discussed if this is really needed)
 */
__attribute__((naked))
void isr_svc(void)
{
    /* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */
//    asm("push   {LR}");
    /* save exception return value */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

//    asm("pop    {r0}"               );      /* restore exception return value from stack */
    asm("bx     LR"                 );      /* load exception return value to PC causes end of exception*/
    /* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}

__attribute__((naked))
void isr_pendsv(void)
{
    context_save();
    sched_run();
    context_restore();
}

__attribute__( ( always_inline ) ) static __INLINE
void context_save(void)
{
    /* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */

    /* save unsaved registers */
    asm("mrs    r0, psp"            );      /* get stack pointer from user mode */
    asm("stmdb  r0!,{r4-r11}"       );      /* save regs */
    asm("stmdb  r0!,{lr}"           );      /* exception return value */
//  asm("vstmdb sp!, {s16-s31}"     );      /* FIXME save FPU registers if needed */
    asm("ldr    r1, =active_thread" );      /* load address of current tcb */
    asm("ldr    r1, [r1]"           );      /* dereference pdc */
    asm("str    r0, [r1]"           );      /* write r0 to pdc->sp means current threads stack pointer */
}

__attribute__( ( always_inline ) ) static __INLINE
void context_restore(void)
{
    asm("ldr    r0, =active_thread" );      /* load address of current TCB */
    asm("ldr    r0, [r0]"           );      /* dereference TCB */
    asm("ldr    r1, [r0]"           );      /* load tcb->sp to register 1 */
    asm("ldmia  r1!, {r0}"          );      /* restore exception return value from stack */
//  asm("pop    {s16-s31}"          );      /* FIXME load FPU register if needed depends on r0 exret */
    asm("ldmia  r1!, {r4-r11}"      );      /* restore other registers */
    asm("msr    psp, r1"            );      /* restore PSP register (user mode SP)*/
    asm("bx     r0"                 );      /* load exception return value to PC causes end of exception*/

    /* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}
