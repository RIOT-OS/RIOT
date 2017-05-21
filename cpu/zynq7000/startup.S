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
 * @brief       Assembler startup code for zynq7000
 *
 * Very basic startup code. Single processor only. No cache.
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

.section .boot,"ax"
.syntax unified

.globl NVIC_SystemReset
NVIC_SystemReset:
/* We only want to run single processor so stop cpu1 */
	mrc	p15,0,r1,c0,c0,5
	and	r1, r1, #0xf
	cmp	r1, #0
	beq	_cpu0
_cpu1:
	wfe
	b	_cpu1

_cpu0:
	/* Get the cpu revision */
	mrc     p15, 0, r0, c0, c0, 0
	and     r5, r0, #0x00f00000
	and     r6, r0, #0x0000000f
	orr     r6, r6, r5, lsr #20-4

	/* ARM ERRATA 742230 */
    cmp     r6, #0x22                       /* only present up to r2p2 */
    mrcle   p15, 0, r10, c15, c0, 1         /* read diagnostic register */
    orrle   r10, r10, #(0x1 << 4)           /* set bit #4 */
    mcrle   p15, 0, r10, c15, c0, 1         /* write diagnostic register */

	/* ARM ERRATA 743622 */
	teq     r5, #0x00200000                 /* only present in r2p* */
	mrceq   p15, 0, r10, c15, c0, 1         /* read diagnostic register */
	orreq   r10, r10, #(0x1 << 6)           /* set bit #6 */
	mcreq   p15, 0, r10, c15, c0, 1         /* write diagnostic register */

	/* set VBAR to the _vector_table address */
	ldr	r0, =_vector_table
	mcr	p15, 0, r0, c12, c0, 0

	/* disable MMU/caches/flow prediction. CP15 register 1
	 * clear bits 0, 2, 11, 12  */
	mrc	p15, 0, r0, c1, c0, 0
	bic r0, r0, #0x1
	bic r0, r0, #(0x1 << 2)
	bic r0, r0, #(0x1 << 11)
	bic r0, r0, #(0x1 << 12)
	mcr	p15, 0, r0, c1, c0, 0

	/* clear SMP bit */
	mrc	p15, 0, r0, c1, c0, 1
	bic	r0, r0, #(0x1 << 6)
	mcr	p15, 0, r0, c1, c0, 1

	/* Clear cp15 regs with unknown reset values */
	mov	r0, #0x0
	mcr	p15, 0, r0, c5, c0, 0	/* DFSR */
	mcr	p15, 0, r0, c5, c0, 1	/* IFSR */
	mcr	p15, 0, r0, c6, c0, 0	/* DFAR */
	mcr	p15, 0, r0, c6, c0, 2	/* IFAR */
	mcr	p15, 0, r0, c9, c13, 2	/* PMXEVCNTR */
	mcr	p15, 0, r0, c13, c0, 2	/* TPIDRURW */
	mcr	p15, 0, r0, c13, c0, 3	/* TPIDRURO */
	mcr	p15, 5, r0, c15, c5, 2	/* Write Lockdown TLB VA */

	/* setup stack pointer for each banked mode */
	/* IRQ mode */
	cps     #0x12
	ldr	sp,=__irq_stack

	/* Supervisor mode */
	cps     #0x13
	ldr	sp,=__supervisor_stack

	/* Abort mode */
	cps     #0x17
	ldr	sp,=__abort_stack

	/* FIQ mode */
	cps     #0x11
	ldr	sp,=__fiq_stack

	/* Undefine mode */
	cps     #0x1b
	ldr	sp,=__undef_stack

	/* SYS mode */
	cps     #0x1f
	ldr	sp,=__stack

	/* Load MMU translation table */
	ldr	r0,=MMUTable
	mcr	15, 0, r0, c2, c0, 0

	/* Set all MMU domains to Manager */
	mov	r0,#0xFFFFFFFF
	mcr	p15,0,r0,c3,c0,0

	/* Enable MMU */
	mrc	p15, 0, r0, c1, c0, 0
	orr r0, r0, #0x1
	mcr	p15, 0, r0, c1, c0, 0
	dsb
	isb

	/* clear sbss */
	mov	r0, #0
	ldr 	r1,=__sbss_start		/* calculate beginning of the SBSS */
	ldr	r2,=__sbss_end		/* calculate end of the SBSS */

.Lloop_sbss:
	cmp	r1,r2
	bge	.Lenclsbss		/* If no SBSS, no clearing required */
	str	r0, [r1], #4
	b	.Lloop_sbss

.Lenclsbss:
	/* clear bss */
	ldr	r1,=__bss_start		/* calculate beginning of the BSS */
	ldr	r2,=__bss_end		/* calculate end of the BSS */

.Lloop_bss:
	cmp	r1,r2
	bge	.Lenclbss		/* If no BSS, no clearing required */
	str	r0, [r1], #4
	b	.Lloop_bss

.Lenclbss:

	/* setup board */
    bl	board_init

    /* run global constructors */
    bl __libc_init_array

	/* start RIOT */
	bl	kernel_init

    /* Cleanup global constructors */
    bl __libc_fini_array


