/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

  /* GCC ARM assembler */

  .text
  .code 32
  .align 4  /* 0 */

  /* Constants */

  .equ  USERMODE,   0x10
  .equ  FIQMODE,    0x11
  .equ  IRQMODE,    0x12
  .equ  SVCMODE,    0x13
  .equ  ABORTMODE,  0x17
  .equ  UNDEFMODE,  0x1b
  .equ  SYSTEMMODE, 0x1f
  .equ  MODEMASK,   0x1f
  .equ  NOINT,      0xc0           /* Disable both IRQ & FIR */
  .equ  TBIT,       0x20
  .equ  IRQMASK,    0x80
  .equ  FIQMASK,    0x40


  /* External references */

  .extern  sched_active_thread
  .extern  sched_context_switch_request
  .extern  sched_run
  .extern  DEBUG_Routine

/* Public functions declared in this file */
  .global  arm_irq_handler
  .global  cpu_switch_context_exit
  .global  task_return
  .global  ctx_switch

ctx_switch:
    /* Save return address on stack */
    stmfd   sp!, {lr}

    /* disable interrupts */
    mov     lr, #NOINT|SVCMODE
    msr     CPSR_c, lr

ctx_switch2:
    /* get stack pointer from user mode into lr */
    stmfd   sp, {sp}^
    sub     sp, sp, #4
    ldmfd   sp!, {lr}

    /* save registers on user mode stack pointed to by lr */
    stmfd   lr, {r0-r14}^
    sub     lr, lr, #60

    /* restore saved return address from stack */
    ldmfd   sp!, {r1}

    /* get spsr into register */
    mrs r0, spsr

    /* store return address and spsr on user mode stack */
    stmfd   lr!, {r0, r1}

    /* save user mode stack pointer in *sched_active_thread */
    ldr     r1, =sched_active_thread   /* r1 = &sched_active_thread */
    ldr     r1, [r1]                /* r1 = *r1 = sched_active_thread */

    str     lr, [r1]                /* store stack pointer in tasks tcb*/
    /* now the calling task has all its registers saved on its stack and it's SP is saved in its tcb */


    /* call scheduler so sched_active_thread points to the next task */
    bl      sched_run
    b       task_return
    /* never coming back */

cpu_switch_context_exit:
    mov r0, #NOINT|SVCMODE
    msr cpsr, r0

    /* call scheduler so sched_active_thread points to the next task */
    bl      sched_run

    /* continue in task_return: */

task_return:
    /* load tcb->stackpointer in r0 */
    ldr     r0, =sched_active_thread   /* r0 = &sched_active_thread */
    ldr     r0, [r0]                /* r0 = *r0 = sched_active_thread */
    ldr     r0, [r0]

    /* restore saved spsr and return address from tasks stack */
    ldmfd   r0!, {r1,lr}
    msr     SPSR, r1

    /* Restore all registers, PC & cpsr */
    ldmfd   r0, {r0-lr}^

    /* return to task */
    movs pc, lr

/*----------------------------------------------------------------------------
*
*----------------------------------------------------------------------------*/
arm_irq_handler:
    sub     lr, lr, #4

    /* save interrupted tasks PC onto stack */
    stmfd  sp!, {lr}

    /* save all registers on isr stack */
    stmfd  sp!, {r0-r12}

    /* save spsr on stack */
    MRS R0, SPSR
    STMFD  SP!, {R0}

    MRS R1, CPSR
    MSR SPSR, R1

    /* jump into vic interrupt */
    mov    r0, #0xffffff00    /* lpc23xx */
    ldr    r0, [r0]
    add    lr,pc,#4

    mov     pc, r0

    /* restore spsr from stack */
    LDMFD  SP!, {R0}
    MSR SPSR, R0

    /* check if context switch was requested by irq */
    ldr r0, =sched_context_switch_request
    ldr r0, [r0]

    cmp r0, #0x00
    bne     switch_context_int

exit_irq_int:
    /* recover general purpose registers */
    ldmfd  sp!, {r0-r12}

    /* recover tasks PC into lr */
    ldmfd  sp!, {lr}

    /* now jump back into task */
    movs pc, lr

switch_context_int:
    /* recover general purpose registers */
    ldmfd  sp!, {r0-r12}

    b ctx_switch2
