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
#include <string.h>

#include <periph/timer.h>
#include "cpu_conf.h"
#include <stdio.h>
#include "sched.h"
#include "thread.h"
#include "board.h"
#include "irq.h"
#include "timex.h"
#include "div.h"

#include <sys/time.h>

/*
 * setting TIMER_ACCURACY_SHIFT lower will improve accuracy
 * at the cost of more regular interrupts (hence less power efficient).
 * */
#define TIMER_ACCURACY_SHIFT (10)
#define TIMER_ACCURACY       (1 << TIMER_ACCURACY_SHIFT)
#define CHANNELS             (3)

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

/*
 * The mips toolchain C library does not implement gettimeofday()
 *
 * implement it here using the timer.
 *
 */
int gettimeofday(struct timeval *__restrict __p, void *__restrict __tz)
{
    uint64_t now = counter * US_PER_MS;
    __p->tv_sec = div_u64_by_1000000(now);
    __p->tv_usec = now - (__p->tv_sec * US_PER_SEC);

    return 0;
}

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    assert(dev == 0);

    (void)freq; /*Cannot adjust Frequency */

    timer_isr_ctx.cb = cb;
    timer_isr_ctx.arg = arg;

    /* Clear down soft counters */
    memset((void *)compares, 0, sizeof(compares));

    counter = (1 << TIMER_ACCURACY_SHIFT);

    /* Set compare up */
    mips_setcompare(mips_getcount() + TICKS_PER_US * TIMER_ACCURACY);

    /* Start the timer if stopped */
    mips32_bc_c0(C0_CAUSE, CR_DC);

    /* Enable Timer Interrupts */
    mips32_bs_c0(C0_STATUS, SR_HINT5);


    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    assert(dev == 0);
    assert(channel < CHANNELS);

    timeout >>= TIMER_ACCURACY_SHIFT;
    timeout <<= TIMER_ACCURACY_SHIFT;

    uint32_t status = irq_arch_disable();
    compares[channel] = counter + timeout;
    irq_arch_restore(status);

    return channel;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    assert(dev == 0);
    assert(channel < CHANNELS);

    value >>= TIMER_ACCURACY_SHIFT;
    value <<= TIMER_ACCURACY_SHIFT;

    uint32_t status = irq_arch_disable();
    compares[channel] = value;
    irq_arch_restore(status);

    return channel;
}

int timer_clear(tim_t dev, int channel)
{
    assert(dev == 0);
    assert(channel < CHANNELS);

    uint32_t status = irq_arch_disable();
    compares[channel] = 0;
    irq_arch_restore(status);

    return channel;
}

unsigned int timer_read(tim_t dev)
{
    assert(dev == 0);

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
    mips32_bs_c0(C0_STATUS, SR_HINT5);
}

void timer_irq_disable(tim_t dev)
{
    mips32_bc_c0(C0_STATUS, SR_HINT5);
}


void __attribute__ ((interrupt("vector=hw5"))) _mips_isr_hw5(void)
{
    register int cr = mips_getcr();

    if (cr & CR_TI) {
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

            if (sched_context_switch_request) {
                thread_yield();
            }
        }
        if (counter == compares[1]) {
            timer_isr_ctx.cb(timer_isr_ctx.arg, 1);

            if (sched_context_switch_request) {
                thread_yield();
            }
        }
        if (counter == compares[2]) {
            timer_isr_ctx.cb(timer_isr_ctx.arg, 2);

            if (sched_context_switch_request) {
                thread_yield();
            }
        }

        mips_setcompare(mips_getcount() + TICKS_PER_US * TIMER_ACCURACY);

    }
    else {
        spurious_int++;
    }
}
