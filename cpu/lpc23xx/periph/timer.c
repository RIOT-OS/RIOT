/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lpc23xx
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Implementation of the low-level timer driver for the LPC2387
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "irq.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/timer.h"

/**
 * @brief   Check the board config to make sure we do not exceed max number of
 *          timers
 */
#if TIMER_NUMOF > 4
#error "ERROR in timer configuration: too many timers defined"
#endif

/**
 * @brief   Interrupt context information for configured timers
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

static uint32_t _oneshot;

static inline void set_oneshot(tim_t tim, int chan)
{
    _oneshot |= (1 << chan) << (TIMER_CHANNEL_NUMOF * tim);
}

static inline void clear_oneshot(tim_t tim, int chan)
{
    _oneshot &= ~((1 << chan) << (TIMER_CHANNEL_NUMOF * tim));
}

static inline bool is_oneshot(tim_t tim, int chan)
{
    return _oneshot & ((1 << chan) << (TIMER_CHANNEL_NUMOF * tim));
}

/**
 * @brief   Forward declarations for interrupt functions
 * @{
 */
void tim_isr_0(void);
void tim_isr_1(void);
void tim_isr_2(void);
void tim_isr_3(void);
/** @} */

/**
 * @brief   Get the base pointer of a timer
 */
static inline lpc23xx_timer_t *get_dev(tim_t tim)
{
    switch (tim) {
        case 0:
            return TMR0;
#if TIMER_NUMOF > 1
        case 1:
            return TMR1;
#endif
#if TIMER_NUMOF > 2
        case 2:
            return TMR2;
#endif
#if TIMER_NUMOF > 3
        case 3:
            return TMR3;
#endif
        default:
            return NULL;
    }
}

static inline void pwr_clk_and_isr(tim_t tim, uint32_t scale)
{
    switch (tim) {
        case 0:
            PCONP |= (1 << 1);
            PCLKSEL0 &= ~(0x03 << 2);
            PCLKSEL0 |=  (scale << 2);
            install_irq(TIMER0_INT, &tim_isr_0, 1);
            break;
#if TIMER_NUMOF > 1
        case 1:
            PCONP |= (1 << 2);
            PCLKSEL0 &= ~(0x03 << 4);
            PCLKSEL0 |=  (scale << 4);
            install_irq(TIMER1_INT, &tim_isr_1, 1);
            break;
#endif
#if TIMER_NUMOF > 2
        case 2:
            PCONP |= (1 << 22);
            PCLKSEL1 &= ~(0x03 << 12);
            PCLKSEL1 |=  (scale << 12);
            install_irq(TIMER2_INT, &tim_isr_2, 1);
            break;
#endif
#if TIMER_NUMOF > 3
        case 3:
            PCONP |= (1 << 23);
            PCLKSEL1 &= ~(0x03 << 14);
            PCLKSEL1 |=  (scale << 14);
            install_irq(TIMER3_INT, &tim_isr_3, 1);
            break;
#endif
    }
}

int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
    /* get the timers base register */
    lpc23xx_timer_t *dev = get_dev(tim);

    /* make sure the timer device is valid */
    if (dev == NULL) {
        return -1;
    }

    uint32_t scale, prescale;
    lpc23xx_pclk_scale(CLOCK_PCLK, freq, &scale, &prescale);

    /* save the callback */
    isr_ctx[tim].cb = cb;
    isr_ctx[tim].arg = arg;
    /* enable power, config periph clock and install ISR vector */
    pwr_clk_and_isr(tim, scale);
    /* reset timer configuration (sets the timer to timer mode) */
    dev->TCR = 0;
    dev->CTCR = 0;
    /* configure the prescaler */
    dev->PR = prescale - 1;
    /* enable timer */
    dev->TCR = 1;
    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (((unsigned) tim >= TIMER_NUMOF) || ((unsigned) channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }

    lpc23xx_timer_t *dev = get_dev(tim);

    set_oneshot(tim, channel);

    dev->MR[channel] = value;
    /* Match Interrupt */
    dev->MCR |= (1 << (channel * 3));

    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    if (((unsigned) tim >= TIMER_NUMOF) || ((unsigned) channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }

    /* Interrupt will only be generated on increment,
       so a 0 timeout is not possible */
    if (timeout == 0) {
        timeout = 1;
    }

    lpc23xx_timer_t *dev = get_dev(tim);

    unsigned state = irq_disable();
    dev->TCR = 0;

    unsigned absolute = timeout + dev->TC;
    unsigned mask = 1 << (channel * 3);

    dev->MR[channel] = absolute;
    dev->MCR |= mask;
    set_oneshot(tim, channel);

    dev->TCR = 1;
    irq_restore(state);

    return 0;
}

int timer_set_periodic(tim_t tim, int channel, unsigned int value, uint8_t flags)
{
    if (((unsigned) tim >= TIMER_NUMOF) || ((unsigned) channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }

    bool stop = flags & TIM_FLAG_SET_STOPPED;
    lpc23xx_timer_t *dev = get_dev(tim);

    if (flags & TIM_FLAG_RESET_ON_SET) {
        /* reset the timer */
        dev->TCR = 2;
        /* start the timer */
        /* cppcheck-suppress redundantAssignment
         * (reason: TCR is volatile control register.
                    Bit 2 will put the timer into Reset
                    Bit 1 will control if the timer is running) */
        if (!stop) {
            dev->TCR = 1;
        }
    } else if (stop) {
        /* stop the timer */
        dev->TCR = 0;
    }

    clear_oneshot(tim, channel);

    uint8_t cfg = (flags & TIM_FLAG_RESET_ON_MATCH)
                ? 0x3   /* Match Interrupt & Reset on Match */
                : 0x1;  /* Match Interrupt */

    dev->MR[channel] = value;
    dev->MCR |= (cfg << (channel * 3));
    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    if (((unsigned) tim >= TIMER_NUMOF) || ((unsigned) channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }
    get_dev(tim)->MCR &= ~(1 << (channel * 3));
    return 0;
}

unsigned int timer_read(tim_t tim)
{
    return (unsigned int)(get_dev(tim)->TC);
}

void timer_start(tim_t tim)
{
    get_dev(tim)->TCR = 1;
}

void timer_stop(tim_t tim)
{
    get_dev(tim)->TCR = 0;
}

static inline void chan_handler(lpc23xx_timer_t *dev, unsigned tim_num, unsigned chan_num)
{
    if (is_oneshot(tim_num, chan_num)) {
        dev->MCR &= ~(1 << (chan_num * 3));
    }

    isr_ctx[tim_num].cb(isr_ctx[tim_num].arg, chan_num);
}

static inline void isr_handler(lpc23xx_timer_t *dev, int tim_num)
{
    uint32_t state = dev->IR;
    uint8_t chan = 0;

    /* clear interrupt flags */
    dev->IR = state;

    while (state) {
        state = bitarithm_test_and_clear(state, &chan);
        chan_handler(dev, tim_num, chan);
    }

    /* we must not forget to acknowledge the handling of the interrupt */
    VICVectAddr = 0;
}

void __attribute__((interrupt("IRQ"))) tim_isr_0(void)
{
    isr_handler(TMR0, 0);
}

#if TIMER_NUMOF > 1
void __attribute__((interrupt("IRQ"))) tim_isr_1(void)
{
    isr_handler(TMR1, 1);
}
#endif

#if TIMER_NUMOF > 2
void __attribute__((interrupt("IRQ"))) tim_isr_2(void)
{
    isr_handler(TMR2, 2);
}
#endif

#if TIMER_NUMOF > 3
void __attribute__((interrupt("IRQ"))) tim_isr_3(void)
{
    isr_handler(TMR3, 3);
}
#endif
