/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2023 Hugues Larrive
 *               2023 Gerson Fernando Budke
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
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <assert.h>
#include <avr/interrupt.h>

#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "thread.h"

#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   We have 5 possible prescaler values
 */
#define PRESCALE_NUMOF (5U)

/**
 * @brief   Possible prescaler values, encoded as 2 ^ val
 */
static const __flash uint8_t prescalers[] = { 0, 3, 6, 8, 10 };

/**
 * @brief   Timer state context
 */
typedef struct {
    mega_timer_t *dev;      /**< timer device */
    volatile uint8_t *mask; /**< address of interrupt mask register */
    volatile uint8_t *flag; /**< address of interrupt flag register */
    timer_cb_t cb;          /**< interrupt callback */
    void *arg;              /**< interrupt callback argument */
    uint8_t mode;           /**< remember the configured mode */
    uint8_t isrs;           /**< remember the interrupt state */
} ctx_t;

/**
 * @brief   Allocate memory for saving the device states
 */
static ctx_t ctx[] = {
#ifdef TIMER_0
    { TIMER_0, TIMER_0_MASK, TIMER_0_FLAG, NULL, NULL, 0, 0 },
#endif
#ifdef TIMER_1
    { TIMER_1, TIMER_1_MASK, TIMER_1_FLAG, NULL, NULL, 0, 0 },
#endif
#ifdef TIMER_2
    { TIMER_2, TIMER_2_MASK, TIMER_2_FLAG, NULL, NULL, 0, 0 },
#endif
#ifdef TIMER_3
    { TIMER_3, TIMER_3_MASK, TIMER_3_FLAG, NULL, NULL, 0, 0 },
#endif
};

static unsigned _oneshot;

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
 * @brief Setup the given timer
 */
int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
/*
 * A debug pin can be used to probe timer interrupts with an oscilloscope or
 * other time measurement equipment. Thus, determine when an interrupt occurs
 * and how long the timer ISR takes.
 * The pin should be defined in the makefile as follows:
 * CFLAGS += -DDEBUG_TIMER_PORT=PORTF -DDEBUG_TIMER_DDR=DDRF \
 *           -DDEBUG_TIMER_PIN=PORTF4
 */
#if defined(DEBUG_TIMER_PORT)
    DEBUG_TIMER_DDR |= (1 << DEBUG_TIMER_PIN);
    DEBUG_TIMER_PORT &= ~(1 << DEBUG_TIMER_PIN);
    DEBUG("Debug Pin: DDR 0x%02x Port 0x%02x Pin 0x%02x\n",
          &DEBUG_TIMER_DDR, &DEBUG_TIMER_PORT, (1 << DEBUG_TIMER_PIN));
#endif

    DEBUG("timer.c: freq = %ld\n", freq);
    uint8_t pre = 0;

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
        DEBUG("timer.c: prescaling from %lu Hz failed!\n", CLOCK_CORECLOCK);
        return -1;
    }

    /* stop and reset timer */
    ctx[tim].dev->CRA = 0;
    ctx[tim].dev->CRB = 0;
#ifdef TCCR1C
    ctx[tim].dev->CRC = 0;
#endif
    ctx[tim].dev->CNT = 0;

    /* save interrupt context and timer mode */
    ctx[tim].cb = cb;
    ctx[tim].arg = arg;
    ctx[tim].mode = (pre + 1);

    /* enable timer with calculated prescaler */
    ctx[tim].dev->CRB = (pre + 1);
    DEBUG("timer.c: prescaler set at %d\n", pre + 1);

    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (channel >= TIMER_CHANNEL_NUMOF) {
        return -1;
    }

    unsigned state = irq_disable();

    ctx[tim].dev->OCR[channel] = (uint16_t)value;
#if defined(OCF1A) && defined(OCF1B) && (OCF1A < OCF1B)
    /* clear spurious IRQs, if any */
    *ctx[tim].flag = (1 << (OCF1A + channel));
    /* unmask IRQ */
    *ctx[tim].mask |= (1 << (OCIE1A + channel));
#elif defined(OCF1A) && defined(OCF1B) && (OCF1A > OCF1B)
    /* clear spurious IRQs, if any */
    *ctx[tim].flag = (1 << (OCF1A - channel));
    /* unmask IRQ */
    *ctx[tim].mask |= (1 << (OCIE1A - channel));
#endif
    set_oneshot(tim, channel);

    irq_restore(state);

    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    if (channel >= TIMER_CHANNEL_NUMOF) {
        return -1;
    }

    unsigned state = irq_disable();
    unsigned absolute = ctx[tim].dev->CNT + timeout;

    ctx[tim].dev->OCR[channel] = absolute;
#if defined(OCF1A) && defined(OCF1B) && (OCF1A < OCF1B)
    /* clear spurious IRQs, if any */
    *ctx[tim].flag = (1 << (OCF1A + channel));
    /* unmask IRQ */
    *ctx[tim].mask |= (1 << (OCIE1A + channel));
#elif defined(OCF1A) && defined(OCF1B) && (OCF1A > OCF1B)
    /* clear spurious IRQs, if any */
    *ctx[tim].flag = (1 << (OCF1A - channel));
    /* unmask IRQ */
    *ctx[tim].mask |= (1 << (OCIE1A - channel));
#endif
    set_oneshot(tim, channel);

    if ((absolute - ctx[tim].dev->CNT) > timeout) {
        /* Timer already expired. Trigger the interrupt now and loop until it
         * is triggered.
         */
#if defined(OCF1A) && defined(OCF1B) && (OCF1A < OCF1B)
        while (!(*ctx[tim].flag & (1 << (OCF1A + channel)))) {
#elif defined(OCF1A) && defined(OCF1B) && (OCF1A > OCF1B)
        while (!(*ctx[tim].flag & (1 << (OCF1A - channel)))) {
#endif
            ctx[tim].dev->OCR[channel] = ctx[tim].dev->CNT;
        }
    }

    irq_restore(state);

    return 0;
}

int timer_set_periodic(tim_t tim, int channel, unsigned int value, uint8_t flags)
{
    int res = 0;

    if (channel >= TIMER_CHANNEL_NUMOF) {
        return -1;
    }

    if (flags & TIM_FLAG_RESET_ON_SET) {
        ctx[tim].dev->CNT = 0;
    }

    unsigned state = irq_disable();

    ctx[tim].dev->OCR[channel] = (uint16_t)value;

#if defined(OCF1A) && defined(OCF1B) && (OCF1A < OCF1B)
    /* clear spurious IRQs, if any */
    *ctx[tim].flag = (1 << (OCF1A + channel));
    /* unmask IRQ */
    *ctx[tim].mask |= (1 << (OCIE1A + channel));
#elif defined(OCF1A) && defined(OCF1B) && (OCF1A > OCF1B)
    /* clear spurious IRQs, if any */
    *ctx[tim].flag = (1 << (OCF1A - channel));
    /* unmask IRQ */
    *ctx[tim].mask |= (1 << (OCIE1A - channel));
#endif

    clear_oneshot(tim, channel);

    /* only OCR0 can be use to set TOP */
    if (channel == 0) {
        if (flags & TIM_FLAG_RESET_ON_MATCH) {
            /* enable CTC mode */
            ctx[tim].mode |= (1 << 3);
        } else {
            /* disable CTC mode */
            ctx[tim].mode &= (1 << 3);
        }
        /* enable timer or stop it */
        if (flags & TIM_FLAG_SET_STOPPED) {
            ctx[tim].dev->CRB = 0;
        } else {
            ctx[tim].dev->CRB = ctx[tim].mode;
        }
    } else {
        assert((flags & TIM_FLAG_RESET_ON_MATCH) == 0);
        res = -1;
    }

    irq_restore(state);

    return res;
}

int timer_clear(tim_t tim, int channel)
{
    if (channel >= TIMER_CHANNEL_NUMOF) {
        return -1;
    }

#if defined(OCIE1A) && defined(OCIE1B) && (OCIE1A < OCIE1B)
    *ctx[tim].mask &= ~(1 << (OCIE1A + channel));
#elif defined(OCIE1A) && defined(OCIE1B) && (OCIE1A > OCIE1B)
    *ctx[tim].mask &= ~(1 << (OCIE1A - channel));
#endif

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    /* CNT is a 16 bit register, but atomic access is implemented by hardware:
     * A read from the low byte causes the value in the high byte being stored
     * in parallel into a temporary register. The read of the high byte will
     * instead access the temporary register. However, the AVR only has one
     * temporary register that is used to implement atomic access to all 16 bit
     * registers. Thus, access has to be guarded by disabling IRQs.
     */
    unsigned state = irq_disable();
    unsigned result = ctx[tim].dev->CNT;
    irq_restore(state);
    return result;
}

void timer_stop(tim_t tim)
{
    ctx[tim].dev->CRB = 0;
}

void timer_start(tim_t tim)
{
    ctx[tim].dev->CRB = ctx[tim].mode;
}

uword_t timer_query_freqs_numof(tim_t dev)
{
    (void) dev;

    return ARRAY_SIZE(prescalers);
}

uint32_t timer_query_freqs(tim_t dev, uword_t index)
{
    (void)dev;

    if (index >= ARRAY_SIZE(prescalers)) {
        return 0;
    }

    return CLOCK_CORECLOCK >> prescalers[index];
}

#ifdef TIMER_NUMOF
static inline void _isr(tim_t tim, int chan)
{
#if defined(DEBUG_TIMER_PORT)
    DEBUG_TIMER_PORT |= (1 << DEBUG_TIMER_PIN);
#endif

    if (is_oneshot(tim, chan)) {
        timer_clear(tim, chan);
    }
    ctx[tim].cb(ctx[tim].arg, chan);

#if defined(DEBUG_TIMER_PORT)
    DEBUG_TIMER_PORT &= ~(1 << DEBUG_TIMER_PIN);
#endif
}
#endif

#ifdef TIMER_0
AVR8_ISR(TIMER_0_ISRA, _isr, 0, 0);
AVR8_ISR(TIMER_0_ISRB, _isr, 0, 1);
#ifdef TIMER_0_ISRC
AVR8_ISR(TIMER_0_ISRC, _isr, 0, 2);
#endif  /* TIMER_0_ISRC */
#endif  /* TIMER_0 */

#ifdef TIMER_1
AVR8_ISR(TIMER_1_ISRA, _isr, 1, 0);
AVR8_ISR(TIMER_1_ISRB, _isr, 1, 1);
#ifdef TIMER_1_ISRC
AVR8_ISR(TIMER_1_ISRC, _isr, 1, 2);
#endif  /* TIMER_0_ISRC */
#endif  /* TIMER_1 */

#ifdef TIMER_2
AVR8_ISR(TIMER_2_ISRA, _isr, 2, 0);
AVR8_ISR(TIMER_2_ISRB, _isr, 2, 1);
AVR8_ISR(TIMER_2_ISRC, _isr, 2, 2);
#endif /* TIMER_2 */

#ifdef TIMER_3
AVR8_ISR(TIMER_3_ISRA, _isr, 3, 0);
AVR8_ISR(TIMER_3_ISRB, _isr, 3, 1);
AVR8_ISR(TIMER_3_ISRC, _isr, 3, 2);
#endif /* TIMER_3 */
