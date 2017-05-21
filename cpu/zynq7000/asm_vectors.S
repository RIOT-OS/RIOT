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
 * @brief       Assembler vector table for zynq7000
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

.section .vectors,"ax"
.syntax unified

.globl _vector_table
_vector_table:
	B	NVIC_SystemReset
	B	Undefined
	B	SVCHandler
	B	PrefetchAbortHandler
	B	DataAbortHandler
	NOP	/* Placeholder for address exception vector*/
	B	IRQHandler
	B	FIQHandler

.section .text,"ax"
IRQHandler:
	/* save lr_irq and spsr_irq onto system stack */
	sub		lr, lr, #4
	srsfd   sp!, #0x1f
	/* switch back to system mode */
	cps     #0x1f
	/* save AAPCS reg */
	push 	{r0-r3, r12}
	/* ensure stack is 8-byte aligned */
	and     r1, sp, #4
	sub     sp, sp, r1
	/* store adjustment and lr */
	push    {r1, lr}
	/* handle interrupt */
	bl	IRQInterrupt
	/* restore lr and unadjust stack */
	pop		{r1, lr}
	add		sp, sp, r1
	/* restore AAPCs reg */
	pop		{r0-r3, r12}
	/* return */
	rfefd	sp!

FIQHandler:
	sub		lr, lr, #4
	/* save AAPCS reg */
	push 	{r0-r3, r12, lr}
	/* handle interrupt */
	bl	FIQInterrupt
	/* restore AAPCs reg and return */
	pop		{r0-r3, r12, pc}

Undefined:
	b	NVIC_SystemReset

SVCHandler:
	/* TODO */
	bl	SWInterrupt


DataAbortHandler:
	/* dsb for Errata No: 	 775420 */
	dsb
	/* adjust lr so points and instruction which caused abort */
	sub		lr, lr, #8
	/* save AAPCS reg */
	push 	{r0-r3, r12, lr}
	/* handle abort */
	bl	DataAbortInterrupt
	/* restore AAPCs reg & return */
	pop		{r0-r3, r12, pc}

PrefetchAbortHandler:
	/* dsb for Errata No: 	 775420 */
	dsb
	/* adjust lr so points and instruction which caused abort */
	sub		lr, lr, #4
	/* save AAPCS reg */
	push 	{r0-r3, r12, lr}
	/* handle abort */
	bl	PrefetchAbortInterrupt
	/* restore AAPCs reg & return */
	pop		{r0-r3, r12, pc}

.end
