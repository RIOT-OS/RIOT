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


/* Begin of boot code */
.text
.arm
.section .init

swi_addr:       .word   ctx_switch          /* defined in arm7_common/common.s */

setup_swi:
              ldr r0, =0x02002040
              adr r1, swi_switch
              ldm r1, {r1 - r3}
              stm r0, {r1 - r3}
              bx lr

swi_switch:
              .word 0xe59ff000 // ldr pc, [pc]
              .word 0
              .word swi_handler

swi_handler:
              /* un-do the garbage the bios did */
              /* https://github.com/camthesaxman/gba_bios/blob/master/asm/bios.s#L153 */
              pop	{r2, lr}
              eor	r11, r11, #0x0c
              msr	cpsr, r11
              pop {r11}
              msr	spsr, r11
              pop	{r11,r12,lr}
              ldr pc, swi_addr


.global _startup
.func   _startup

_startup:
                ldr    pc, =reset_handler

/*.func reset_handler */
reset_handler:

.section .init0
                ldr r0, =0x4000000
                mov r1, #0
                strh r1, [r0]
                bl setup_swi

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
