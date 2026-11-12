/*
 * Copyright (C) 2023 HAW Hamburg
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

.extern ctx_switch_from_swi

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

.set  REG_IME, 0x4000208

.text
.arm


/* Begin of boot code */
.text
.arm
.section .init

/* Global start up, the cartridge header jumps here */
.global _startup
_startup:
                ldr    pc, =reset_handler

/* Will be copied to RAM at 0x02002040 by setup_swi */
/* This is the first code during a swi where we are in possesion over the control flow */
swi_interceptor:
                /* decodes to ldr pc, [pc] */
                .word 0xe59ff000
                /* pc is 8 bytes ahead, so we skip 4 bytes */
                .word 0
                /* These are the 4 bytes that [pc] will read, it's the address of our handler */
                .word swi_pre_handler

/* copies swi_interceptor into RAM */
setup_swi:
                /* Magic address the GBA jumps to when using (invalid) svc 0xd4000 */
                ldr r0, =0x02002040
                /* Address of our interceptor routine */
                adr r1, swi_interceptor

                ldm r1, {r1 - r3}
                stm r0, {r1 - r3}
                bx lr

/* The swi_interceptor (when in RAM) jumps here */
swi_pre_handler:
                /* un-do the garbage the bios did */
                /* https://github.com/camthesaxman/gba_bios/blob/master/asm/bios.s#L153 */
                pop {r2, lr}
                eor r11, r11, #0x0c
                msr cpsr, r11
                pop {r11}
                msr spsr, r11
                pop {r11,r12,lr}
                b ctx_switch_from_swi

.section .init0
.func
/* Reset function, actual start of RIOT */
reset_handler:
                /* Disable interrupts using the GBA control reg */
                ldr r0, =REG_IME
                mov r1, #0
                strh r1, [r0]

                /* Setup the swi hack to enable SVC for RIOT */
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
                msr   CPSR_c, #MODE_SYS|I_BIT|F_BIT     /* Sys Mode (we use it as User Mode) */
                ldr   sp, =__stack_usr_start

                /* call the cpu-bring-up helper from arm7_common/arm7_init.c */
                bl  bootloader
                /* start the RIOT kernel */
                b   kernel_init

                /* Infinite Loop */
.section .fini0
__main_exit:    b   __main_exit


.endfunc
.end
