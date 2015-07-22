/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_zynq7000
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's architecture dependent thread
 *              interface
 *
 * Use the following register layout when saving context onto the stack:
 *
 * ---------- highest address (bottom of stack)
 * | R14/LR |
 * ----------
 * | R12/IP |
 * ----------
 * | R11/FP |
 * ----------
 * | R10    |
 * ----------
 * | R9     |
 * ----------
 * | R8     |
 * ----------
 * | R7     |
 * ----------
 * | R6     |
 * ----------
 * | R5     |
 * ----------
 * | R4     |
 * ----------
 * | R3     |
 * ----------
 * | R2     |
 * ----------
 * | R1     |
 * ----------
 * | R0     |
 * ----------
 * | CPSR   |
 * ---------- lowest address (top of stack)
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include <stdio.h>

#include "arch/thread_arch.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "cpu.h"
#include "kernel_internal.h"

static void __context_save(void);
static void __context_restore(void);

/**
 * @brief   Noticeable marker marking the beginning of a stack segment
 */
#define STACK_MARKER                (0x77777777)

/**
 * @brief   Initial program status register value for a newly created thread
 *
 * System mode, IRQ, FIQ enabled
 */
#define INITIAL_CPSR                (0x0000001F)

char *thread_arch_stack_init(thread_task_func_t task_func, void *arg,
		void *stack_start, int stack_size) {

	uint32_t *stk;
	stk = (uint32_t *) ((uintptr_t) stack_start + stack_size);

	/* adjust to 32 bit boundary by clearing the last two bits in the address */
	stk = (uint32_t *) (((uint32_t) stk) & ~((uint32_t) 0x3));

	/* stack start marker */
	stk--;
	*stk = STACK_MARKER;

	/* make sure the stack is double word aligned (8 bytes) */
	/* This is required in order to conform with Procedure Call Standard for the
	 * ARM® Architecture (AAPCS) */
	if (((uint32_t) stk & 0x7) != 0) {
		/* add a single word padding */
		--stk;
		*stk = ~((uint32_t) STACK_MARKER);
	}

	/* pc - initial program counter value := thread entry function */
	stk--;
	*stk = (uint32_t) task_func;

	/* r3 - contents of r3. required for threads starting from thread_arch_yield */
	stk--;
	*stk = (uint32_t) 3;

	/* lr - contains the return address when the thread exits */
	stk--;
	*stk = (uint32_t) sched_task_exit;

	/* r12 - r1 */
	for (int i = 12; i >= 1; i--) {
		stk--;
		*stk = i;
	}

	/* r0 - contains the thread function parameter */
	stk--;
	*stk = (uint32_t) arg;

	/* SPSR - initial status register */
	stk--;
	*stk = (uint32_t) INITIAL_CPSR;

	return (char*) stk;
}

void thread_arch_stack_print(void) {
	int count = 0;
	uint32_t *sp = (uint32_t *) sched_active_thread->sp;

	printf("printing the current stack of thread %" PRIkernel_pid "\n",
			thread_getpid());
	printf("  address:      data:\n");

	do {
		printf("  0x%08x:   0x%08x\n", (unsigned int) sp, (unsigned int) *sp);
		sp++;
		count++;
	} while (*sp != STACK_MARKER);

	printf("current stack size: %i byte\n", count);
}

__attribute__((naked)) void NORETURN thread_arch_start_threading(void) {
	sched_run();
	__context_restore();
	__asm__ __volatile__("pop     {r0,pc}\n");
	UNREACHABLE();
}

__attribute__ ((noinline)) void thread_arch_yield(void) {
    _in_isr = 0;
	__context_save();
	sched_run();
	__context_restore();
}

__attribute__((always_inline)) static inline void __context_save(void) {
	__asm__ __volatile__(

			"push	{r0-r12, lr}\n"
			"mrs	r0, cpsr\n"
			"cpsid	if\n"
			"push	{r0}\n"
			"ldr	r0, =sched_active_thread\n"
			"ldr	r0, [r0]\n"
			"str	sp, [r0]\n"
	);
}

__attribute__((always_inline)) static inline void __context_restore(void) {
	__asm__ __volatile__(
			"ldr	r0, =sched_active_thread\n"
			"ldr	r0, [r0]\n"
			"ldr	sp, [r0]\n"
			"pop	{r0}\n"
			"msr	cpsr, r0\n"
			"pop	{r0-r12, lr}\n"
	);
}
