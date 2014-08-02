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

/* Exception vectors and handler addresses.
It is 64 bytes and can be mapped (see documentation 1.4.2). */
.section .vectors
/* Exception Vectors */
                ldr     PC, Reset_Addr      /* Reset */
                ldr     PC, Undef_Addr      /* Undefined Instruction */
                ldr     PC, SWI_Addr        /* Software Interrupt */
                ldr     PC, PAbt_Addr       /* Prefetch Abort */
                ldr     PC, DAbt_Addr       /* Data Abort */
                nop                         /* Reserved Vector (holds Philips ISP checksum) */
                /* see page 71 of "Insiders Guide to the Philips ARM7-Based Microcontrollers" by Trevor Martin  */
/*                ldr     PC, [PC,#-0x0120]     /* Interrupt Request Interrupt (load from VIC) */
                ldr     PC, IRQ_Addr        /* Interrupt Request Interrupt (load from VIC) */
                ldr     r0, =__fiq_handler  /* Fast Interrupt Request Interrupt */
                ldr     pc, [r0]            /* jump to handler in pointer at __fiq_handler */

/* Exception vector handlers branching table */
Reset_Addr:     .word   Reset_Handler       /* defined in this module below  */
Undef_Addr:     .word   UNDEF_Routine       /* defined in main.c  */
SWI_Addr:       .word   ctx_switch          /* defined in main.c  */
PAbt_Addr:      .word   PABT_Routine        /* defined in main.c  */
DAbt_Addr:      .word   DABT_Routine        /* defined in main.c  */
IRQ_Addr:       .word   arm_irq_handler      /* defined in main.c  */

/* Begin of boot code */
.text
.arm
.section .init

.global _startup
.func   _startup

_startup:
                ldr    pc, =Reset_Handler

/*.func Reset_Handler */
Reset_Handler:

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

.section .init2                         /* copy .data section (Copy from ROM to RAM) */
.extern _etext
.extern _data
.extern _edata
/*
                ldr     R1, =_etext
                ldr     R2, =_data
                ldr     R3, =_edata
LoopRel:        cmp     R2, R3
                ldrlo   R0, [R1], #4
                strlo   R0, [R2], #4
                blo     LoopRel
*/
.section .init4                         /* Clear .bss section (Zero init)  */
.extern __bss_start
.extern __bss_end
/*
                mov     R0, #0
                ldr     R1, =__bss_start
                ldr     R2, =__bss_end
LoopZI:         cmp     R1, R2
                strlo   R0, [R1], #4
                blo     LoopZI
*/
                /* Enter the C code  */
.section .init9
                bl  bootloader
                b   kernel_init

                /* Infinite Loop */
.section .fini0
__main_exit:    B   __main_exit


.endfunc
.end
