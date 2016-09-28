/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <mips/cpu.h>
#include <mips/m32c0.h>
#include <mips/regdef.h>
#include <mips/asm.h>

#include <periph/timer.h>
#include "cpu_conf.h"
#include <stdio.h>
#include "sched.h"
#include "thread.h"
#include "board.h"
#include "irq.h"

/*
 * setting TIMER_ACCURACY_SHIFT lower will improve accuracy
 * at the cost of more regular interrupts (hence less power efficient).
 * */
#define TIMER_ACCURACY_SHIFT 10
#define TIMER_ACCURACY (1 << TIMER_ACCURACY_SHIFT)
#define CHANNELS 3

/* TICKS_PER_US must be set in the board file */
#ifndef TICKS_PER_US
#error "Please set TICK_PER_US in your board file"
#endif

/*
 * The base MIPS count / compare timer is fixed frequency at core clock / 2
 * and is pretty basic This timer is currently only supported in Vectored
 * Interrupt Mode (VI), EIC mode is not supported yet.
 *
 * RIOT's xtimer expects the timer to operate at 1MHZ or any 2^n multiple or
 * factor of this, thus we maintain a software timer which counts at 1MHz.
 * This is not particularly power efficient and may add latency too.
 *
 * If other SoC specific timers are available which are more flexible then
 * it is advised to use them, this timer is a fallback version
 * that should work on all MIPS implementations.
 *
 */

static timer_isr_ctx_t timer_isr_ctx;
volatile unsigned int counter;
volatile unsigned int compares[CHANNELS];
static volatile int spurious_int;

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
	int i;

	if (dev != 0) {
		return -1;
	}

	(void)freq; /*Cannot adjust Frequency */

	timer_isr_ctx.cb = cb;
	timer_isr_ctx.arg = arg;

	/* Clear down soft counters */
	for (i = 0; i < CHANNELS; i++) {
		compares[i] = 0;
	}

	counter = 1 << TIMER_ACCURACY_SHIFT;

	/* Set compare up */
	mips_setcompare(mips_getcount() + TICKS_PER_US * TIMER_ACCURACY);

	/* Start the timer if stopped */
	mips32_bc_c0(C0_CAUSE, CR_DC);

	/* Enable Timer Interrupts */
#if defined (PIC32MX) || defined(PIC32MZ)

#ifdef PIC32MZ
	volatile uint32_t * const _IEC0_SET = (uint32_t *) 0xbf8100c8;
	volatile uint32_t * const _IPC0 = (uint32_t *) 0xbf810140;
#else /*MX*/
	volatile uint32_t * const _IEC0_SET = (uint32_t *) 0xbf881068;
	volatile uint32_t * const _IPC0 = (uint32_t *) 0xbf881090;
#endif
	/* Enable IRQ 0 CPU timer interrupt */
	*_IEC0_SET = 0x1;

	/* Set IRQ 0 to priority 1.0 */
	*_IPC0 = 0x4;
#else
	mips32_bs_c0(C0_STATUS, SR_HINT5);
#endif

	return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
	if (dev != 0) {
		return -1;
	}

	if (channel >= CHANNELS) {
		return -1;
	}

	timeout >>= TIMER_ACCURACY_SHIFT;
	timeout <<= TIMER_ACCURACY_SHIFT;

	uint32_t status = irq_arch_disable();
	compares[channel] = counter + timeout;
	irq_arch_restore(status);

	return channel;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
	if (dev != 0) {
		return -1;
	}

	if (channel >= CHANNELS) {
		return -1;
	}

	value >>= TIMER_ACCURACY_SHIFT;
	value <<= TIMER_ACCURACY_SHIFT;

	uint32_t status = irq_arch_disable();
	compares[channel] = value;
	irq_arch_restore(status);

	return channel;
}

int timer_clear(tim_t dev, int channel)
{
	if (dev != 0) {
		return -1;
	}

	if (channel >= CHANNELS) {
		return -1;
	}

	uint32_t status = irq_arch_disable();
	compares[channel] = 0;
	irq_arch_restore(status);

	return channel;
}

unsigned int timer_read(tim_t dev)
{
	if (dev != 0) {
		return -1;
	}
	return counter;
}

void timer_start(tim_t dev)
{
	mips32_bc_c0(C0_CAUSE, CR_DC);
}

void timer_stop(tim_t dev)
{
	mips32_bs_c0(C0_CAUSE, CR_DC);
}

void timer_irq_enable(tim_t dev)
{
#if defined (PIC32MX) || defined(PIC32MZ)
#ifdef PIC32MZ
	volatile uint32_t * const _IEC0_SET = (uint32_t *) 0xbf8100c8;
#else /*MX*/
	volatile uint32_t * const _IEC0_SET = (uint32_t *) 0xbf881068;
#endif
	*_IEC0_SET = 0x1;
#else
	mips32_bs_c0(C0_STATUS, SR_HINT5);
#endif
}

void timer_irq_disable(tim_t dev)
{
#if defined (PIC32MX) || defined(PIC32MZ)
#ifdef PIC32MZ
	volatile uint32_t * const _IEC0_CLR = (uint32_t *) 0xbf8100c4;
#else /*MX*/
	volatile uint32_t * const _IEC0_CLR = (uint32_t *) 0xbf881064;
#endif
	*_IEC0_CLR = 0x1;
#else
	mips32_bc_c0(C0_STATUS, SR_HINT5);
#endif
}

/* note Compiler inserts GP context save + restore code (to current stack).*/
#if defined (PIC32MX) || defined(PIC32MZ)
/*
 * This is a hack - PIC32 uses EIC mode + MCU-ASE, currently the toolchain
 * does not support correct placement of EIC mode vectors (it is coming though)
 * But the default PIC interrupt controller defaults to non vectored mode
 * anyway with all interrupts coming via vector 0 which is equivalent to 'sw0'
 * in 'VI' mode.
 *
 * Thus all PIC32 interrupts should be decoded here (currently only Timer is
 * used)
 *
 * When toolchain support is available we could move to full vector mode but
 * this does take up significant space (MCU-ASE provides 256 vectors at 32B
 * spacing (the default) thats 8KB of vector space!), So a single entry point
 * may be better anyway.
 *
 */
void __attribute__ ((interrupt("vector=sw0"), keep_interrupts_masked)) _mips_isr_sw0(void)
#else
void __attribute__ ((interrupt("vector=hw5"))) _mips_isr_hw5(void)
#endif
{
	register int cr = mips_getcr();
	if (cr & CR_TI) {

#if defined(PIC32MX) || defined(PIC32MZ)
		/* ACK The timer interrupt*/
#ifdef PIC32MZ
		volatile uint32_t * const _IFS0_CLR = (uint32_t *) 0xbf810044;
#else /*MX*/
		volatile uint32_t * const _IFS0_CLR = (uint32_t *) 0xbf881034;
#endif
		*_IFS0_CLR = 0x1;
#endif


		uint32_t status = irq_arch_disable();
		counter += TIMER_ACCURACY;
		irq_arch_restore(status);

		if (counter == compares[0]) {
		/*
		 * The Xtimer code expects the ISR to take some time
		 * but our counter is a fake software one, so bump it a
		 * bit to give the impression some time elapsed in the ISR.
		 * Without this the callback ( _shoot(timer) on xtimer_core.c )
		 * never fires.
		 */
			counter += TIMER_ACCURACY;
			timer_isr_ctx.cb(timer_isr_ctx.arg, 0);

			if (sched_context_switch_request)
				thread_yield();
		}
		if (counter == compares[1]) {
			timer_isr_ctx.cb(timer_isr_ctx.arg, 1);

			if (sched_context_switch_request)
				thread_yield();
		}
		if (counter == compares[2]) {
			timer_isr_ctx.cb(timer_isr_ctx.arg, 2);

			if(sched_context_switch_request)
				thread_yield();
		}

		mips_setcompare(mips_getcount() + TICKS_PER_US * TIMER_ACCURACY);

	} else {
		spurious_int++;
	}
}

