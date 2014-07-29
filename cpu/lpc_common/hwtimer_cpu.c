/*
 * Copyright (C) 2014 INRIA
 *
 * The source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup cpu
 * @file
 * @internal
 * @brief       ARM kernel timer CPU dependent functions implementation
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

#include "cpu.h"
#include "bitarithm.h"
#include "hwtimer_cpu.h"
#include "arch/hwtimer_arch.h"
#include "irq.h"

#define VULP(x) ((volatile unsigned long*) (x))

/* High level interrupt handler */
static void (*int_handler)(int);

/* Timer 0-3 interrupt handler */
static void timer_irq(void) __attribute__((interrupt("IRQ")));

inline static unsigned long get_base_address(short timer)
{
    return (volatile unsigned long)(TMR0_BASE_ADDR + (timer / 8) * 0x6C000 + (timer / 4 - (timer / 8) * 2) * 0x4000);
}

static void timer_irq(void)
{
    short timer = 0;

    if (T0IR) {
        timer = 0;
    }
    else if (T1IR) {
        timer = 4;
    }
    else if (T2IR) {
        timer = 8;
    }

    volatile unsigned long base = get_base_address(timer);

    if (*VULP(base + TXIR) & BIT0) {
        *VULP(base + TXMCR) &= ~BIT0;
        *VULP(base + TXIR) = BIT0;
        int_handler(timer);
    }

    if (*VULP(base + TXIR) & BIT1) {
        *VULP(base + TXMCR) &= ~BIT3;
        *VULP(base + TXIR) = BIT1;
        int_handler(timer + 1);
    }

    if (*VULP(base + TXIR) & BIT2) {
        *VULP(base + TXMCR) &= ~BIT6;
        *VULP(base + TXIR) = BIT2;
        int_handler(timer + 2);
    }

    if (*VULP(base + TXIR) & BIT3) {
        *VULP(base + TXMCR) &= ~BIT9;
        *VULP(base + TXIR) = BIT3;
        int_handler(timer + 3);
    }

    VICVectAddr = 0;    /* acknowledge interrupt (if using VIC IRQ) */
}

static void timer0_init(uint32_t cpsr)
{
    PCONP |= PCTIM0;        /* power up timer */
    T0TCR = 2;              /* disable and reset timer */
    T0MCR = 0;              /* disable compare */
    T0CCR = 0;              /* capture is disabled */
    T0EMR = 0;              /* no external match output */
    T0PR = cpsr;            /* set prescaler */
    install_irq(TIMER0_INT, &timer_irq, 1);
    T0TCR = 1;              /* reset counter */
}

static void timer1_init(uint32_t cpsr)
{
    PCONP |= PCTIM1;        /* power up timer */
    T1TCR = 2;              /* disable and reset timer */
    T1MCR = 0;              /* disable compare */
    T1CCR = 0;              /* capture is disabled */
    T1EMR = 0;              /* no external match output */
    T1PR = cpsr;            /* set prescaler */
    install_irq(TIMER1_INT, &timer_irq, 1);
    T1TCR = 1;              /* reset counter */
}

static void timer2_init(uint32_t cpsr)
{
    PCONP |= PCTIM2;        /* power up timer */
    T2TCR = 2;              /* disable and reset timer */
    T2MCR = 0;              /* disable compare */
    T2CCR = 0;              /* capture is disabled */
    T2EMR = 0;              /* no external match output */
    T2PR = cpsr;            /* set prescaler */
    install_irq(TIMER2_INT, &timer_irq, 1);
    T2TCR = 1;              /* reset counter */
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    uint32_t cpsr;
    int_handler = handler;
    cpu_clock_scale(fcpu, HWTIMER_SPEED, &cpsr);
    timer0_init(cpsr);
    timer1_init(cpsr);
    timer2_init(cpsr);
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_enable_interrupt(void)
{
    VICIntEnable = 1 << TIMER0_INT; /* Enable Interrupt */
    VICIntEnable = 1 << TIMER1_INT; /* Enable Interrupt */
    VICIntEnable = 1 << TIMER2_INT; /* Enable Interrupt */
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_disable_interrupt(void)
{
    VICIntEnClr = 1 << TIMER0_INT;  /* Disable Interrupt */
    VICIntEnClr = 1 << TIMER1_INT;  /* Disable Interrupt */
    VICIntEnClr = 1 << TIMER2_INT;  /* Disable Interrupt */
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_set(unsigned long offset, short timer)
{
    /* Calculate base address of timer register */
    /* Timer 0-3 are matched to TIMER0 */
    /* Timer 4-7 are matched to TIMER1 */
    /* Timer 8-11 are matched to TIMER2 */
    volatile unsigned long base = get_base_address(timer);
    /* Calculate match register address of corresponding timer */
    timer %= 4;
    unsigned long cpsr = disableIRQ();
    volatile unsigned long *addr = VULP(base + TXMR0 + 4 * timer);
    /* Calculate match register value */
    unsigned long value = *VULP(base + TXTC) + offset;
    *addr = value;                                  /* set match register */
    *VULP(base + TXIR) = 0x01 << timer;             /* reset interrupt register value for corresponding match register */
    *VULP(base + TXMCR) &= ~(7 << (3 * timer));     /* Clear all bits */
    *VULP(base + TXMCR) |= (MR0I << (3 * timer));   /* enable interrupt for match register */
    restoreIRQ(cpsr);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    /* Calculate base address of timer register */
    /* Timer 0-3 are matched to TIMER0 */
    /* Timer 4-7 are matched to TIMER1 */
    /* Timer 8-11 are matched to TIMER2 */
    volatile unsigned long base = get_base_address(timer);
    /* Calculate match register address of corresponding timer */
    timer %= 4;
    volatile unsigned long *addr = VULP(base + TXMR0 + 4 * timer);
    /* Calculate match register value */
    *addr = value;                                  /* set match register */
    *VULP(base + TXIR) = 0x01 << timer;             /* reset interrupt register value for corresponding match register */
    *VULP(base + TXMCR) &= ~(7 << (3 * timer));     /* Clear all bits */
    *VULP(base + TXMCR) |= (MR0I << (3 * timer));   /* enable interrupt for match register */
}

/*---------------------------------------------------------------------------*/

void hwtimer_arch_unset(short timer)
{
    volatile unsigned long base = get_base_address(timer);
    timer %= 4;
    *VULP(base + TXMCR) &= ~(MR0I << (3 * timer));  /* disable interrupt for match register */
    *VULP(base + TXIR) = 0x01 << timer;             /* reset interrupt register value for corresponding match register */
}

/*---------------------------------------------------------------------------*/

unsigned long hwtimer_arch_now(void)
{
    return T0TC;
}

void hwtimer_arch_setcounter(unsigned int val)
{
    T0TC = val;
}
