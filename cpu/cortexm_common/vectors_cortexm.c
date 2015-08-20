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
/** @} */

/** @brief Interrupt stack canary value
 *
 * @note 0xe7fe is the ARM Thumb machine code equivalent of asm("bl #-2\n") or
 * 'while (1);', i.e. an infinite loop.
 */
#define STACK_CANARY_WORD 0xE7FEE7FEu
#define _FAULT_ARRAY_LENGTH						26
extern const char* _fault_texts[_FAULT_ARRAY_LENGTH];

// fault causes for the Cortex-M series
#define ARRAY_SIZE(x)				(sizeof(x) / (sizeof(x[0])))
#define TST_BITMASK(x,tst)			(((x) & (tst)) == (tst))

const char* _fault_texts[_FAULT_ARRAY_LENGTH] = {
		"Instruction access violation",
		"Data access violation",
		NULL,
		"MemManage fault on unstacking for a return from exception",
		"MemManage fault on stacking for exception entry",
		NULL,
		NULL,
		"MemManage fault address register valid",
		"Instruction bus error",
		"Precise data bus error",
		"Imprecise data bus error",
		"BusFault on unstacking for a return from exception",
		"BusFault on stacking for exception entry",
		NULL,
		NULL,
		"BusFault address register (BFAR) valid ",
		"Undefined instruction UsageFault",
		"Invalid state UsageFault",
		"Invalid PC load UsageFault, caused by an invalid PC load by EXC_RETURN",
		"No coprocessor UsageFault. The processor does not support coprocessor instructions",
		NULL,
		NULL,
		NULL,
		NULL,
		"Unaligned access UsageFault",
		"Divide by zero UsageFault"
};

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

void __attribute__((naked)) __attribute__((interrupt)) decode_hard_fault(void)
{
	asm(
			"tst	lr, #4				\n" // test lr against 4, update the flags register, discard the result
			"ite	eq					\n" // if-then ? skip one instruction if lr is equal to 4
			"mrseq	r0, msp				\n" // 	 - lr == 4 : set main stackpointer as first arg
			"mrsne	r0, psp				\n" // 	 - else    : set process stackpointer as first arg
			"b hard_fault_default		\n" // call the hardfault handler
			"nop						\n"
	);
}

void __attribute__((optimize(0))) hard_fault_default(ptr_register_dump registers)
{
	// we want this variables in the BSS in case the HardFault is generated by a stack overflow
	static const char* system_state = NULL;
	static const char* causes[3];
	static uint32_t cause_length;

	cause_length = 0;

	// get the current system state
	system_state = (SCB->CFSR & 0xffff0000)
			? "UsageFault"
			: (SCB->CFSR & 0x0000ff00)
			? "BusFault"
			: "MemoryFault";

	// get hard fault cause
	for (uint32_t idx = 0; idx < ARRAY_SIZE(_fault_texts) && cause_length < ARRAY_SIZE(causes); ++idx)
		if (TST_BITMASK(SCB->CFSR, (1u << idx)) && _fault_texts[idx])
			causes[cause_length++] = _fault_texts[idx];

#if 1
	// print exception information to the debug usart
	printf("HardFault: %s\nCauses:\n", system_state);
	for (uint32_t idx = 0; idx < cause_length; ++idx)
		printf(" - %s\n", causes[idx]);

	printf("\nRegisters:\n");
	printf("R0   = 0x%lx (%ld)\n", registers->r0, registers->r0);
	printf("R1   = 0x%lx (%ld)\n", registers->r1, registers->r1);
	printf("R2   = 0x%lx (%ld)\n", registers->r2, registers->r2);
	printf("R3   = 0x%lx (%ld)\n", registers->r3, registers->r3);
	printf("R12  = 0x%lx (%ld)\n", registers->r12, registers->r12);
	printf("LR   = %p subroutine call return address\n", registers->lr);
	printf("PC   = %p program counter\n", registers->pc);
	printf("PSR  = 0x%08lx\n", registers->psr);
	printf("BFAR = 0x%08lx\n", SCB->BFAR);
	printf("CFSR = 0x%08lx\n", SCB->CFSR);
	printf("HFSR = 0x%08lx\n", SCB->HFSR);
	printf("DFSR = 0x%08lx\n", SCB->DFSR);
	printf("AFSR = 0x%08lx\n", SCB->AFSR);
	printf("SHCSR = 0x%08lx\n", SCB->SHCSR);
#endif

    core_panic(PANIC_HARD_FAULT, "HARD FAULT HANDLER");
}

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
