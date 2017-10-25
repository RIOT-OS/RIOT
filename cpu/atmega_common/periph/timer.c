/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for the ATmega family
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 *
 * @}
 */

#include <avr/interrupt.h>

#include "board.h"
#include "cpu.h"
#include "thread.h"

#include "periph/timer.h"
#include "periph_conf.h"
#include "periph/pm.h"
#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   All timers have three channels
 */
#define CHANNELS                (3)

/**
 * @brief   We have 5 possible prescaler values
 */
#define PRESCALE_NUMOF          (5U)

/**
 * @brief   Possible prescaler values, encoded as 2 ^ val
 */
static const uint8_t prescalers[] = { 0, 3, 6, 8, 10 };

/**
 * @brief   Timer state context
 */
typedef struct {
    mega_timer_t *dev;          /**< timer device */
    volatile uint8_t *mask;     /**< address of interrupt mask register */
    volatile uint8_t *flag;     /**< address of interrupt flag register */
    timer_cb_t cb;              /**< interrupt callback */
    void *arg;                  /**< interrupt callback argument */
    uint8_t mode;               /**< remember the configured mode */
    uint8_t isrs;               /**< remember the interrupt state */
    int8_t num;                /** < hardware timer number */
} ctx_t;

/**
 * @brief   Allocate memory for saving the device states
 * @{
 */
#ifdef TIMER_NUMOF
static ctx_t ctx[] = {
#ifdef TIMER_0
    { TIMER_0, TIMER_0_MASK, TIMER_0_FLAG, NULL, NULL, 0, 0, -1 },
#endif
#ifdef TIMER_1
    { TIMER_1, TIMER_1_MASK, TIMER_1_FLAG, NULL, NULL, 0, 0, -1 },
#endif
#ifdef TIMER_2
    { TIMER_2, TIMER_2_MASK, TIMER_2_FLAG, NULL, NULL, 0, 0, -1 },
#endif
#ifdef TIMER_3
    { TIMER_3, TIMER_3_MASK, TIMER_3_FLAG, NULL, NULL, 0, 0, -1 },
#endif
};
#else
/* fallback if no timer is configured */
static ctx_t *ctx[] = {{ NULL }};
#endif
/** @} */

static inline void timer_poweroff(tim_t tim) {
    switch(ctx[tim].num) {
    #ifdef MEGA_TIMER0
        case 0 :
            power_timer0_disable();
#ifdef MODULE_PM_LAYERED
            pm_unblock(PM_INVALID_TIMER0);
#endif
        break;
    #endif
    #ifdef MEGA_TIMER1
        case 1 :
            power_timer1_disable();
#ifdef MODULE_PM_LAYERED
            pm_unblock(PM_INVALID_TIMER1);
#endif
        break;
    #endif
    #ifdef MEGA_TIMER1
        case 2 :
            power_timer2_disable();
#ifdef MODULE_PM_LAYERED
            pm_unblock(PM_INVALID_TIMER2);
#endif
        break;
    #endif
    #ifdef MEGA_TIMER3
        case 3 :
            power_timer3_disable();
#ifdef MODULE_PM_LAYERED
            pm_unblock(PM_INVALID_TIMER3);
#endif
        break;
    #endif
    #ifdef MEGA_TIMER4
        case 4 :
            power_timer4_disable();
#ifdef MODULE_PM_LAYERED
            pm_unblock(PM_INVALID_TIMER4);
#endif
        break;
    #endif
    #ifdef MEGA_TIMER5
        case 5 :
            power_timer5_disable();
#ifdef MODULE_PM_LAYERED
            pm_unblock(PM_INVALID_TIMER5);
#endif
        break;
    #endif
    }
}

static inline void timer_poweron(tim_t tim) {
    switch(ctx[tim].num) {
    #ifdef MEGA_TIMER0
        case 0 :
#ifdef MODULE_PM_LAYERED
            pm_block(PM_INVALID_TIMER0);
#endif
            power_timer0_enable();
        break;
    #endif
    #ifdef MEGA_TIMER1
        case 1 :
#ifdef MODULE_PM_LAYERED
            pm_block(PM_INVALID_TIMER1);
#endif
            power_timer1_enable();
        break;
    #endif
    #ifdef MEGA_TIMER1
        case 2 :
#ifdef MODULE_PM_LAYERED
            pm_block(PM_INVALID_TIMER2);
#endif
            power_timer2_enable();
        break;
    #endif
    #ifdef MEGA_TIMER3
        case 3 :
#ifdef MODULE_PM_LAYERED
            pm_block(PM_INVALID_TIMER3);
#endif
            power_timer3_enable();
        break;
    #endif
    #ifdef MEGA_TIMER4
        case 4 :
#ifdef MODULE_PM_LAYERED
            pm_block(PM_INVALID_TIMER4);
#endif
            power_timer4_enable();
        break;
    #endif
    #ifdef MEGA_TIMER5
        case 5 :
#ifdef MODULE_PM_LAYERED
            pm_block(PM_INVALID_TIMER5);
#endif
            power_timer5_enable();
        break;
    #endif
    }
}


/**
 * @brief Setup the given timer
 */
int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    DEBUG("timer.c: freq = %ld\n", freq);
    uint8_t pre = 0;


    #ifdef MEGA_TIMER0
    if(ctx[tim].dev == MEGA_TIMER0) {
        ctx[tim].num = 0;
    } else
    #endif
    #ifdef MEGA_TIMER1
    if(ctx[tim].dev == MEGA_TIMER1) {
        ctx[tim].num = 1;
    } else
    #endif
    #ifdef MEGA_TIMER2
    if(ctx[tim].dev == MEGA_TIMER2) {
        ctx[tim].num = 2;
    } else
    #endif
    #ifdef MEGA_TIMER3
    if(ctx[tim].dev == MEGA_TIMER3) {
        ctx[tim].num = 3;
    } else
    #endif
    #ifdef MEGA_TIMER4
    if(ctx[tim].dev == MEGA_TIMER4) {
        ctx[tim].num = 4;
    } else
    #endif
    #ifdef MEGA_TIMER5
    if(ctx[tim].dev == MEGA_TIMER5) {
        ctx[tim].num = 5;
    } else
    #endif

    if (ctx[tim].num == -1 ) {
        return -1;
    }

    /* make sure given device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* figure out if freq is applicable */
    for (; pre < PRESCALE_NUMOF; pre++) {
        if ((CLOCK_CORECLOCK >> prescalers[pre]) == freq) {
            break;
        }
    }
    if (pre == PRESCALE_NUMOF) {
        DEBUG("timer.c: prescaling failed!\n");
        return -1;
    }

    /* stop and reset timer */
    ctx[tim].dev->CRA = 0;
    ctx[tim].dev->CRB = 0;
    ctx[tim].dev->CRC = 0;
    ctx[tim].dev->CNT = 0;

    /* save interrupt context and timer mode */
    ctx[tim].cb   = cb;
    ctx[tim].arg  = arg;
    ctx[tim].mode = (pre + 1);

    timer_poweron(tim);

    /* enable timer with calculated prescaler */
    ctx[tim].dev->CRB = (pre + 1);
    DEBUG("timer.c: prescaler set at %d\n", pre + 1);

    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (channel >= CHANNELS) {
        return -1;
    }

    ctx[tim].dev->OCR[channel] = (uint16_t)value;
    *ctx[tim].flag &= ~(1 << (channel + OCF1A));
    *ctx[tim].mask |=  (1 << (channel + OCIE1A));

    return 1;
}

int timer_clear(tim_t tim, int channel)
{
    if (channel >= CHANNELS) {
        return -1;
    }

    *ctx[tim].mask &= ~(1 << (channel + OCIE1A));

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    return (unsigned int)ctx[tim].dev->CNT;
}

void timer_stop(tim_t tim)
{
    ctx[tim].dev->CRB = 0;
}

void timer_start(tim_t tim)
{
    ctx[tim].dev->CRB = ctx[tim].mode;
}

#ifdef TIMER_NUMOF
static inline void _isr(tim_t tim, int chan)
{
    __enter_isr();

    *ctx[tim].mask &= ~(1 << (chan + OCIE1A));
    ctx[tim].cb(ctx[tim].arg, chan);

    if (sched_context_switch_request) {
        thread_yield();
    }

    __exit_isr();
}
#endif

#ifdef TIMER_0
ISR(TIMER_0_ISRA, ISR_BLOCK)
{
    _isr(0, 0);
}

ISR(TIMER_0_ISRB, ISR_BLOCK)
{
    _isr(0, 1);
}

#ifdef TIMER_0_ISRC
ISR(TIMER_0_ISRC, ISR_BLOCK)
{
    _isr(0, 2);
}
#endif /* TIMER_0_ISRC */
#endif /* TIMER_0 */

#ifdef TIMER_1
ISR(TIMER_1_ISRA, ISR_BLOCK)
{
    _isr(1, 0);
}

ISR(TIMER_1_ISRB, ISR_BLOCK)
{
    _isr(1, 1);
}

ISR(TIMER_1_ISRC, ISR_BLOCK)
{
    _isr(1, 2);
}
#endif /* TIMER_1 */

#ifdef TIMER_2
ISR(TIMER_2_ISRA, ISR_BLOCK)
{
    _isr(2, 0);
}

ISR(TIMER_2_ISRB, ISR_BLOCK)
{
    _isr(2, 1);
}

ISR(TIMER_2_ISRC, ISR_BLOCK)
{
    _isr(2, 2);
}
#endif /* TIMER_2 */

#ifdef TIMER_3
ISR(TIMER_3_ISRA, ISR_BLOCK)
{
    _isr(2, 0);
}

ISR(TIMER_3_ISRB, ISR_BLOCK)
{
    _isr(2, 1);
}

ISR(TIMER_3_ISRC, ISR_BLOCK)
{
    _isr(2, 2);
}
#endif /* TIMER_3 */
