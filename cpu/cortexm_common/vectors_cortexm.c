/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Default implementations for Cortex-M specific interrupt and
 *              exception handlers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "panic.h"
#include "kernel_internal.h"
#include "vectors_cortexm.h"

/**
 * @brief   Memory markers, defined in the linker script
 * @{
 */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;
extern uint32_t _sram;
extern uint32_t _eram;
/** @} */

/** @brief Interrupt stack canary value
 *
 * @note 0xe7fe is the ARM Thumb machine code equivalent of asm("bl #-2\n") or
 * 'while (1);', i.e. an infinite loop.
 */
#define STACK_CANARY_WORD 0xE7FEE7FEu

/**
 * @brief   Required by g++ cross compiler
 */
void *__dso_handle;

/**
 * @brief   Pre-start routine for CPU-specific settings
 */
__attribute__((weak)) void pre_startup (void)
{
}

/**
 * @brief   Post-start routine for CPU-specific settings
 */
__attribute__((weak)) void post_startup (void)
{
}

void reset_handler_default(void)
{
    uint32_t *dst;
    uint32_t *src = &_etext;

    pre_startup();

    uint32_t *top;
    /* Fill stack space with canary values up until the current stack pointer */
    /* Read current stack pointer from CPU register */
    asm volatile ("mov %[top], sp" : [top] "=r" (top) : : );
    dst = &_sstack;
    while (dst < top) {
        *(dst++) = STACK_CANARY_WORD;
    }

    /* load data section from flash to ram */
    for (dst = &_srelocate; dst < &_erelocate; ) {
        *(dst++) = *(src++);
    }
    /* default bss section to zero */
    for (dst = &_szero; dst < &_ezero; ) {
        *(dst++) = 0;
    }

    post_startup();

    /* initialize the board (which also initiates CPU initialization) */
    board_init();

#if MODULE_NEWLIB
    /* initialize std-c library (this must be done after board_init) */
    extern void __libc_init_array(void);
    __libc_init_array();
#endif

    /* startup the kernel */
    kernel_init();
}

void nmi_default(void)
{
    core_panic(PANIC_NMI_HANDLER, "NMI HANDLER");
}

#ifdef DEVELHELP

/* Trampoline function to save stackpointer before calling hardfault handler */
void hard_fault_default(void)
{
    /* Get stackpointer where exception stackframe lies */
    __ASM volatile
    (
        "movs r0, #4                        \n" /* r0 = 0x4                   */
        "mov r1, lr                         \n" /* r1 = lr                    */
        "tst r1, r0                         \n" /* if(lr & 0x4)               */
        "bne use_psp                        \n" /* {                          */
        "mrs r0, msp                        \n" /*   r0 = msp                 */
        "b out                              \n" /* }                          */
        " use_psp:                          \n" /* else {                     */
        "mrs r0, psp                        \n" /*   r0 = psp                 */
        " out:                              \n" /* }                          */
        "mov r1, #0                         \n" /* corrupted = false          */
        "mov r2, sp                         \n" /* Cmp with SP is deprecated  */
        "cmp r0, r2                         \n" /* If msp is active stack-    */
        "bne hardfault                      \n" /* pointer, check if valid so */
        "cmp r0, %[eram]                    \n" /* so calling c-func works .  */
        "bge fix_msp                        \n" /* if(r0 == msp) {            */
        "cmn r0, %[sram]                    \n" /*   if( (r0 >= estack) ||    */
        "bl hardfault                       \n" /*       (r0 <  sstack) ) {   */
        " fix_msp:                          \n" /*     corrupted = true       */
        "mov r0, %[estack]                  \n" /*     r0 = _estack           */
        "mov sp, r0                         \n" /*     sp = _estack           */
        "mov r1, #1                         \n" /*   }                        */
        " hardfault:                        \n" /* }                          */
        "b hard_fault_handler               \n" /* hard_fault_handler(r0)     */
          :
          : [sram]   "r" (&_sram),
            [eram]   "r" (&_eram),
            [estack] "r" (&_estack)
          : "r0","r1","r2"
    );
}

void hard_fault_handler(uint32_t* sp, uint32_t corrupted)
{
    /* Make them volatile so that they won't get optimized out */
    volatile unsigned int r0;
    volatile unsigned int r1;
    volatile unsigned int r2;
    volatile unsigned int r3;
    volatile unsigned int r12;
    volatile unsigned int lr; /* Link register. */
    volatile unsigned int pc; /* Program counter. */
    volatile unsigned int psr;/* Program status register. */

    /* Sanity check stackpointer and give additional feedback about hardfault */
    if( corrupted ) {
        puts("Stackpointer corrupted, reset to top of stack");
    } else {
        r0  = sp[0];
        r1  = sp[1];
        r2  = sp[2];
        r3  = sp[3];
        r12 = sp[4];
        lr  = sp[5];
        pc  = sp[6];
        psr = sp[7];

        puts("\nContext before hardfault:");

        /* TODO: printf in ISR context might be a bad idea */
        printf("r0:  0x%x\n"
               "r1:  0x%x\n"
               "r2:  0x%x\n"
               "r3:  0x%x\n",
               r0, r1, r2, r3);
        printf("r12: 0x%x\n"
               "lr : 0x%x\n"
               "pc : 0x%x\n"
               "psr: 0x%x\n\n",
               r12, lr, pc, psr);
    }

    core_panic(PANIC_HARD_FAULT, "HARD FAULT HANDLER");
}
#else
void hard_fault_default(void)
{
    core_panic(PANIC_HARD_FAULT, "HARD FAULT HANDLER");
}
#endif /* DEVELHELP */

#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || \
    defined(CPU_ARCH_CORTEX_M4F)
void mem_manage_default(void)
{
    core_panic(PANIC_MEM_MANAGE, "MEM MANAGE HANDLER");
}

void bus_fault_default(void)
{
    core_panic(PANIC_BUS_FAULT, "BUS FAULT HANDLER");
}

void usage_fault_default(void)
{
    core_panic(PANIC_USAGE_FAULT, "USAGE FAULT HANDLER");
}

void debug_mon_default(void)
{
    core_panic(PANIC_DEBUG_MON, "DEBUG MON HANDLER");
}
#endif

void dummy_handler_default(void)
{
    core_panic(PANIC_DUMMY_HANDLER, "DUMMY HANDLER");
}
