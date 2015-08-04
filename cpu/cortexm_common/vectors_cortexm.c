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
 *
 * @}
 */

#include <stdint.h>

#include "board.h"
#include "panic.h"
#include "kernel_internal.h"
#include "vectors_cortexm.h"

/**
 * @brief   Memory markers, defined in the linker script
 * @{
 */
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;
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
 * @brief   Used for default ISR stubs
 */
#define WEAK    __attribute__((weak))

/**
 * @brief   Pre-start routine for CPU-specific settings
 */
WEAK void pre_startup (void)
{
}

/**
 * @brief   Post-start routine for CPU-specific settings
 */
WEAK void post_startup (void)
{
}

WEAK void reset_handler(void)
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

WEAK void hard_fault(void)
{
    core_panic(PANIC_HARD_FAULT, "HARD FAULT");
}

#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || \
    defined(CPU_ARCH_CORTEX_M4F)
WEAK void mem_manage_fault(void)
{
    core_panic(PANIC_MEM_MANAGE, "MEM MANAGE FAULT");
}

WEAK void bus_fault(void)
{
    core_panic(PANIC_BUS_FAULT, "BUS FAULT");
}

WEAK void usage_fault(void)
{
    core_panic(PANIC_USAGE_FAULT, "USAGE FAULT");
}

WEAK void debug_mon(void)
{
    core_panic(PANIC_DEBUG_MON, "NO DEBUG MON");
}
#endif /* #if defined(CPU_ARCH_CORTEX_M3) || ... */

void dummy_handler_default(void)
{
    core_panic(PANIC_DUMMY_HANDLER, "DUMMY HANDLER");
}

WEAK __attribute__((alias("dummy_handler_default"))) void isr_nmi(void);

WEAK __attribute__((alias("dummy_handler_default"))) void isr_pendsv(void);

WEAK __attribute__((alias("dummy_handler_default"))) void isr_svc(void);

WEAK __attribute__((alias("dummy_handler_default"))) void isr_systick(void);

__attribute__((used, section(".vectors.common"))) const void *vectors_common[] = {
    (void*) (&_estack),      /*  0 initial stack pointer value */
    
#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || defined(CPU_ARCH_CORTEX_M4F)
    /* Cortex-M3/4 handlers */
    (void*) reset_handler,   /*  1 entry point of the program */
    (void*) isr_nmi,         /*  2 non maskable interrupt handler */

    (void*) hard_fault,      /*  3 hard fault exception */
    (void*) mem_manage_fault,/*  4 memory manage exception */
    (void*) bus_fault,       /*  5 bus fault exception */
    (void*) usage_fault,     /*  6 usage fault exception */
    (void*) (0UL),           /*  7 Reserved */
    (void*) (0UL),           /*  8 Reserved */
    (void*) (0UL),           /*  9 Reserved */
    (void*) (0UL),           /* 10 Reserved */
    (void*) isr_svc,         /* 11 system call interrupt, in RIOT used for
                              *    switching into thread context on boot */
    (void*) debug_mon,       /* 12 debug monitor exception */
    (void*) (0UL),           /* 13 Reserved */
    (void*) isr_pendsv,      /* 14 pendSV interrupt, in RIOT the actual
                                   context switching is happening here */
    (void*) isr_systick      /* 15 SysTick interrupt, not used in RIOT */
#else
    /* Cortex-M0 handlers  */
    (void*) reset_handler,   /*  1 entry point of the program */
    (void*) isr_nmi,         /*  2 non maskable interrupt handler */

    (void*) hard_fault,      /*  3 hard fault exception */
    (void*) (0UL),           /*  4 Reserved */
    (void*) (0UL),           /*  5 Reserved */
    (void*) (0UL),           /*  6 Reserved */
    (void*) (0UL),           /*  7 Reserved */
    (void*) (0UL),           /*  8 Reserved */
    (void*) (0UL),           /*  9 Reserved */
    (void*) (0UL),           /* 10 Reserved */
    (void*) isr_svc,         /* 11 system call interrupt, in RIOT used for
                              *    switching into thread context on boot */
    (void*) (0UL),           /* 12 Reserved */
    (void*) (0UL),           /* 13 Reserved */
    (void*) isr_pendsv,      /* 14 pendSV interrupt, in RIOT the actual
                              *    context switching is happening here */
    (void*) isr_systick      /* 15 SysTick interrupt, not used in RIOT */
#endif /* #if defined(CPU_ARCH_CORTEX_M3) || ... */
};
