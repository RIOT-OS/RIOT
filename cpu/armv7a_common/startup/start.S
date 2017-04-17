//------------------------------------------------------------------------------
//
//  Copyright (C) 2017
//
//  Module      : STARTUP
//  File        : start.S
//  Description : ARMv7a startup code
//  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)
//  History     :
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CONSTANT DEFINITION
//------------------------------------------------------------------------------

.equ    USR_MODE,   0x10
.equ    FIQ_MODE,   0x11
.equ    IRQ_MODE,   0x12
.equ    SVC_MODE,   0x13
.equ    ABT_MODE,   0x17
.equ    UND_MODE,   0x1b
.equ    SYS_MODE,   0x1f
.equ    MODE_MASK,  0x1f
.equ    NO_INT,     0xc0

.equ    GPIOBOUT,   0xC001B000

//------------------------------------------------------------------------------
// EXTERNAL FUNCTION
//------------------------------------------------------------------------------

.extern irq_handler

//------------------------------------------------------------------------------
// MACRO DEFINITION
//------------------------------------------------------------------------------

#ifdef CPU_MODEL_S5P4418
.macro led1_on
    /* LED1 ON */
    ldr r0, =GPIOBOUT     // GPIOBOUT address
    ldr r1, [r0]          // Read GPIOBOUT to r1
    and r1, r1, #~(1<<12) // Clear bit 12
    str r1, [r0]          // LED 1 on
.endm

.macro led1_off
    /* LED1 OFF */
    ldr r0, =GPIOBOUT     // GPIOBOUT address
    ldr r1, [r0]          // Read GPIOBOUT to r1
    orr r1, r1, #(1<<12)  // Clear bit 12
    str r1, [r0]          // LED 1 off
.endm

.macro led2_on
    /* LED2 ON */
    ldr r0, =GPIOBOUT     // GPIOBOUT address
    ldr r1, [r0]          // Read GPIOBOUT to r1
    and r1, r1, #~(1<<11) // Clear bit 11
    str r1, [r0]          // LED 2 on
.endm

.macro led2_off
    /* LED2 OFF */
    ldr r0, =GPIOBOUT     // GPIOBOUT address
    ldr r1, [r0]          // Read GPIOBOUT to r1
    orr r1, r1, #(1<<11)  // Clear bit 11
    str r1, [r0]          // LED 2 off
.endm
#endif /* CPU_MODEL_S5P4418 */

.macro  def_irq_handler handler_name
    .align 4
    .weak    \handler_name
    .type    \handler_name, %function
\handler_name :
    b    .
    .size    \handler_name, . - \handler_name
.endm

//------------------------------------------------------------------------------
// ISR VECTOR SECTION
//------------------------------------------------------------------------------

    .section .isr_vector

    .global _start
_start:
    b   reset                             // Reset handler
    ldr pc, Undefined_Instruction_Handler // Undefined instruction handler
    ldr pc, Software_Interrupt_Handler    // Software interrupt handler
    ldr pc, Prefetch_Abort_Handler        // Prefetch abort handler
    ldr pc, Data_Abort_Handler            // Data abort handler
    nop                                   // Not use
    ldr pc, IRQ_Handler                   // IRQ handler
    ldr pc, FIQ_Handler                   // FIQ handler

Undefined_Instruction_Handler:
    .long udefined_instruction_handler
Software_Interrupt_Handler:
    .long swi_handler
Prefetch_Abort_Handler:
    .long prefetch_abort_handler
Data_Abort_Handler:
    .long data_abort_handler
IRQ_Handler:
    .long irq_handler
FIQ_Handler:
    .long fiq_handler

//------------------------------------------------------------------------------
// .TEXT SECTION
//------------------------------------------------------------------------------

.text
    .arm

//------------------------------------------------------------------------------
// RESET HANDLER
//------------------------------------------------------------------------------

    .align 4
    .global reset
reset:

    /* Set the cpu to supervisor mode */
    mrs r0, cpsr
    bic r0, r0, #0x1f
    orr r0, r0, #0xd3
    msr cpsr, r0

    /* Enable NEON & VFP unit */
    mrc p15, #0, r1, c1, c0, #2
    orr r1, r1, #(0xf << 20)
    mcr p15, #0, r1, c1, c0, #2
    mov r1, #0
    mcr p15, #0, r1, c7, c5, #4
    mov r0, #0x40000000
    fmxr fpexc, r0

    /* Cache init */
    mrc p15, 0, r0, c0, c0, 0       /* read main ID register */
    and r1, r0, #0x00f00000         /* variant */
    and r2, r0, #0x0000000f         /* revision */
    orr r2, r2, r1, lsr #20-4       /* combine variant and revision */
    cmp r2, #0x30
    mrceq   p15, 0, r0, c1, c0, 1   /* read ACTLR */
    orreq   r0, r0, #0x6            /* Enable DP1(2), DP2(1) */
    mcreq   p15, 0, r0, c1, c0, 1   /* write ACTLR */

    /* Invalidate L1 I/D */
    mov r0, #0                      /* set up for MCR */
    mcr p15, 0, r0, c8, c7, 0       /* invalidate TLBs */
    mcr p15, 0, r0, c7, c5, 0       /* invalidate icache */

    /* Disable mmu stuff and caches */
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #0x00002000         /* clear bits 13 (--v-) */
    bic r0, r0, #0x00000007         /* clear bits 2:0 (-cam) */
    orr r0, r0, #0x00000002         /* set bit 1 (--a-) align */
    orr r0, r0, #0x00000800         /* set bit 12 (z---) btb */
    mcr p15, 0, r0, c1, c0, 0

    /* Set Vector Base Address Register */
    ldr r0, =_start
    mcr p15, 0, r0, c12, c0, 0
    mrc p15, 0, r0, c1, c0, 0
    bic r0, #(1<<13)
    mcr p15, 0, r0, c1, c0, 0

    /* initialize stacks */
    bl  init_stacks

//------------------------------------------------------------------------------
// INITIALIZE MEMORY SECTIONS
//------------------------------------------------------------------------------

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

    subs    r3, r2
    ble    .flash_to_ram_loop_end

.flash_to_ram_loop:
    subs    r3, #1
    ldrb    r0, [r1, r3]
    strb    r0, [r2, r3]
    bgt    .flash_to_ram_loop
.flash_to_ram_loop_end:

    /* Zero fill the bss segment. */
    ldr   r1, = __bss_start__
    ldr   r2, = __bss_end__
    movs  r3, #0
    b  .fill_zero_bss
.loop_zero_bss:
    strb  r3, [r1], #1

.fill_zero_bss:
    cmp  r1, r2
    bcc  .loop_zero_bss

#ifdef CPU_MODEL_S5P4418
    /* Turn on LED1 & LED2 */
    led1_on
    led2_on
#endif

    /* Jump to main */
    bl  board_init

//------------------------------------------------------------------------------
// INITIALIZE STACKS
//------------------------------------------------------------------------------

init_stacks:

    cps  #UND_MODE
    ldr  sp, =__stack_und_end

    cps  #ABT_MODE
    ldr  sp, =__stack_abt_end

    cps  #IRQ_MODE
    ldr  sp, =__stack_irq_end

    cps  #FIQ_MODE
    ldr  sp, =__stack_fiq_end

    cps  #SYS_MODE
    ldr  sp, =__stack_sys_end

    cps  #SVC_MODE
    ldr  sp, =__stack_srv_end

    mov pc, lr

//------------------------------------------------------------------------------
// EXCEPTION HANDLER DEFINITON
//------------------------------------------------------------------------------

    def_irq_handler udefined_instruction_handler
    def_irq_handler swi_handler
    def_irq_handler prefetch_abort_handler
    def_irq_handler data_abort_handler
    def_irq_handler fiq_handler

//------------------------------------------------------------------------------
// STACK LOCATIONS
//------------------------------------------------------------------------------

    .align 4
_stack_fiq_end:
    .long   __stack_fiq_end
_stack_irq_end:
    .long   __stack_irq_end
_stack_abt_end:
    .long   __stack_abt_end
_stack_und_end:
    .long   __stack_und_end
_stack_srv_end:
    .long   __stack_srv_end
_stack_sys_end:
    .long   __stack_sys_end

.end
