/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* ***************************************************************************************************************

    startup.s                       STARTUP  ASSEMBLY  CODE
                                    -----------------------


    Module includes the interrupt vectors and start-up code.

  *************************************************************************************************************** */
.extern __start_start
.extern __stack_end
.extern __fiq_handler

/* Stack Positions */
.extern __stack_usr_start
.extern __stack_abt_start
.extern __stack_und_start
.extern __stack_fiq_start
.extern __start_irq_start
.extern __start_svc_start

/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs (program status registers) */
.set  MODE_USR, 0x10                    /* Normal User Mode                                         */
.set  MODE_FIQ, 0x11                    /* FIQ Processing Fast Interrupts Mode                      */
.set  MODE_IRQ, 0x12                    /* IRQ Processing Standard Interrupts Mode                  */
.set  MODE_SVC, 0x13                    /* Supervisor Processing Software Interrupts Mode           */
.set  MODE_ABT, 0x17                    /* Abort Processing memory Faults Mode                      */
.set  MODE_UND, 0x1B                    /* Undefined Processing Undefined Instructions Mode         */
.set  MODE_SYS, 0x1F                    /* System Running Priviledged Operating System Tasks  Mode  */

.set  I_BIT, 0x80                       /* when I bit is set, IRQ is disabled (program status registers) */
.set  F_BIT, 0x40                       /* when F bit is set, FIQ is disabled (program status registers) */

.text
.arm

.equiv  T_REG_BASE,       0x04000000
.equiv  T_REG_IE,         0x04000200
.equiv  T_REG_IF,         0x04000202
.equiv  T_REG_WAITCNT,    0x04000204
.equiv  T_REG_IME,        0x04000208
.equiv  T_REG_IFBIOS,     0x03FFFFF8

.linkage=global
.align 2
.arm
.global isrColor
.func   isrColor
isrColor:
    mov         r1, #T_REG_BASE

    @ Get IE & IF
    ldr         r0, [r1, #T_REG_IE - T_REG_BASE]!
    and         r0, r0, r0, lsr #16

    @ Ack IF
    strh        r0, [r1, #T_REG_IF  - T_REG_IE]

    mov	r3, #0x5000000
    ldrh	r3, [r3]
    mov	r2, #0x5000000
    add	r3, r3, #1
    lsl	r3, r3, #16
    lsr	r3, r3, #16
    strh	r3, [r2]

    @ Ack IFBIOS
    ldr         r2, [r1, #T_REG_IFBIOS - T_REG_IE]!
    orr         r2, r2, r0
    strh        r2, [r1], #8

    bx          lr
.endfunc

.linkage=global
.align 2
.arm
.global isrStub
.func   isrStub
isrStub:
    mov         r1, #T_REG_BASE

    @ Get IE & IF
    ldr         r0, [r1, #T_REG_IE - T_REG_BASE]!
    and         r0, r0, r0, lsr #16

    @ Ack IF
    strh        r0, [r1, #T_REG_IF  - T_REG_IE]

    @ Ack IFBIOS
    ldr         r2, [r1, #T_REG_IFBIOS - T_REG_IE]!
    orr         r2, r2, r0
    strh        r2, [r1], #8

    bx          lr
.endfunc


/* Begin of boot code */
.text
.arm
.section .init

.global _startup
.func   _startup

_startup:
                ldr    pc, =reset_handler

/*.func reset_handler */
reset_handler:

.section .init0
                /* Setup a stack for each mode - note that this only sets up a usable stack
                for User mode.   Also each mode is setup with interrupts initially disabled. */
                ldr   r0, = __stack_end
                msr   CPSR_c, #MODE_UND|I_BIT|F_BIT     /* Undefined Instruction Mode  */
                ldr   sp, =__stack_und_start
                msr   CPSR_c, #MODE_ABT|I_BIT|F_BIT     /* Abort Mode */
                ldr   sp, =__stack_abt_start
                msr   CPSR_c, #MODE_FIQ|I_BIT|F_BIT     /* FIQ Mode */
                ldr   sp, =__stack_fiq_start
                msr   CPSR_c, #MODE_IRQ|I_BIT|F_BIT     /* IRQ Mode */
                ldr   sp, =__stack_irq_start
                msr   CPSR_c, #MODE_SVC|I_BIT|F_BIT     /* Supervisor Mode */
                ldr   sp, =__stack_svc_start
                msr   CPSR_c, #MODE_SYS|I_BIT|F_BIT     /* User Mode */
                ldr   sp, =__stack_usr_start

                bl  bootloader
                b   kernel_init

                /* Infinite Loop */
.section .fini0
__main_exit:    B   __main_exit


.endfunc
.end
