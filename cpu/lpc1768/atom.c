/**
 * CPU speficic RIOT kernel function for NXP LPC1768
 *
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup  lpc1768
 * @{
 * @file    atom.c
 * @author Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include "sched.h"
#include "cpu.h"
#include "irq.h"
#include "kernel_internal.h"

NORETURN void sched_task_return(void);

unsigned int atomic_set_return(unsigned int* p, unsigned int uiVal) {
	//unsigned int cspr = disableIRQ();		//crashes
	dINT();
	unsigned int uiOldVal = *p;
	*p = uiVal;
	//restoreIRQ(cspr);						//crashes
	eINT();
	return uiOldVal;
}

NORETURN void cpu_switch_context_exit(void){
    sched_run();
    sched_task_return();
}


void thread_yield(void) {
	asm("svc 0x01\n");
}


__attribute__((naked))
void SVC_Handler(void)
{
	save_context();
	asm("bl sched_run");
	/* call scheduler update sched_active_thread variable with pdc of next thread
	 * the thread that has higest priority and is in PENDING state */
	restore_context();
}

 /* kernel functions */
void ctx_switch(void)
{
	/* Save return address on stack */
	/* stmfd   sp!, {lr} */

	/* disable interrupts */
	/* mov     lr, #NOINT|SVCMODE */
	/* msr     CPSR_c, lr */
	/* cpsid 	i */

	/* save other register */
	asm("nop");

	asm("mov r12, sp");
	asm("stmfd r12!, {r4-r11}");

	/* save user mode stack pointer in *sched_active_thread */
	asm("ldr     r1, =sched_active_thread"); /* r1 = &sched_active_thread */
	asm("ldr     r1, [r1]"); /* r1 = *r1 = sched_active_thread */
	asm("str     r12, [r1]"); /* store stack pointer in tasks pdc*/

	sched_task_return();
}
/* call scheduler so sched_active_thread points to the next task */
NORETURN void sched_task_return(void)
{
	/* load pdc->stackpointer in r0 */
	asm("ldr     r0, =sched_active_thread"); /* r0 = &sched_active_thread */
	asm("ldr     r0, [r0]"); /* r0 = *r0 = sched_active_thread */
	asm("ldr     sp, [r0]"); /* sp = r0  restore stack pointer*/
	asm("pop		{r4}"); /* skip exception return */
	asm(" pop		{r4-r11}");
	asm(" pop		{r0-r3,r12,lr}"); /* simulate register restor from stack */
//	asm("pop 		{r4}"); /*foo*/
	asm("pop		{pc}");

    UNREACHABLE();
}
/*
 * cortex m4 knows stacks and handles register backups
 *
 * so use different stack frame layout
 *
 *
 * with float storage
 * ------------------------------------------------------------------------------------------------------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR | S0 | S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8 | S9 | S10 | S11 | S12 | S13 | S14 | S15 | FPSCR |
 * ------------------------------------------------------------------------------------------------------------------------------------
 *
 * without
 *
 * --------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR |
 * --------------------------------------
 *
 *
 */
char * thread_stack_init(void *(*task_func)(void *), void *arg, void *stack_start, int stack_size)
{
	unsigned int * stk;
	stk = (unsigned int *) (stack_start + stack_size);

	/* marker */
	stk--;
	*stk = 0x77777777;

	//FIXME FPSCR
	stk--;
	*stk = (unsigned int) 0;

	//S0 - S15
	for (int i = 15; i >= 0; i--) {
		stk--;
		*stk = i;
	}

	//FIXME xPSR
	stk--;
	*stk = (unsigned int) 0x01000200;

	//program counter
	stk--;
	*stk = (unsigned int) task_func;

	/* link register */
	stk--;
	*stk = (unsigned int) 0x0;

	/* r12 */
	stk--;
	*stk = (unsigned int) 0;

	/* r1 - r3 */
	for (int i = 3; i >= 1; i--) {
		stk--;
		*stk = i;
	}

	/* r0 -> thread function parameter */
	stk--;
	*stk = (unsigned int) arg;

	/* r11 - r4 */
	for (int i = 11; i >= 4; i--) {
		stk--;
		*stk = i;
	}

	/* foo */
	/*stk--;
	*stk = (unsigned int) 0xDEADBEEF;*/

	/* lr means exception return code  */
	stk--;
	*stk = (unsigned int) 0xfffffff9; // return to taskmode main stack pointer

	return (char*) stk;
}
