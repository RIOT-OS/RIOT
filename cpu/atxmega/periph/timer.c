/*
 * SPDX-FileCopyrightText: 2021-2023 Gerson Fernando Budke <nandojve@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level TIMER driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <assert.h>
#include <avr/interrupt.h>

#include "board.h"
#include "cpu.h"
#include "cpu_pm.h"
#include "irq.h"
#include "periph/timer.h"
#include "periph_conf.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   We have 7 possible prescaler values
 */
#define PRESCALE_NUMOF      (7U)

/**
 * @brief   Possible prescaler values, encoded as 2 ^ val
 */
static const uint8_t prescalers[] = { 0, 1, 2, 3, 6, 8, 10 };

/**
 * @brief   Timer state context
 */
typedef struct {
    timer_cb_t cb;          /**< interrupt callback */
    void *arg;              /**< interrupt callback argument */
    uint8_t prescaler;      /**< remember the prescaler value */
    uint8_t channels;       /**< number of channels */
} ctx_t;

/**
 * @brief   Allocate memory for saving the device states
 * @{
 */
#ifdef TIMER_NUMOF
static ctx_t ctx[TIMER_NUMOF] = { { NULL } };
#else
/* fallback if no timer is configured */
static ctx_t *ctx[] = { { NULL } };
#endif
/** @} */

static uint32_t _oneshot;

static inline void set_oneshot(tim_t tim, int chan)
{
    _oneshot |= (1 << chan) << (TIMER_CH_MAX_NUMOF * tim);
}

static inline void clear_oneshot(tim_t tim, int chan)
{
    _oneshot &= ~((1 << chan) << (TIMER_CH_MAX_NUMOF * tim));
}

static inline bool is_oneshot(tim_t tim, int chan)
{
    return _oneshot & ((1 << chan) << (TIMER_CH_MAX_NUMOF * tim));
}

/**
 * @brief Setup the given timer
 */
int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    DEBUG("timer.c: freq = %ld, Core Clock = %ld\n", freq, CLOCK_CORECLOCK);

    TC0_t *dev;
    uint8_t pre;
    uint8_t ch;

    assert(TIMER_CH_MAX_NUMOF * TIMER_NUMOF < 32);

    /* make sure given device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* figure out if freq is applicable */
    for (pre = 0; pre < PRESCALE_NUMOF; pre++) {
        if ((CLOCK_CORECLOCK >> prescalers[pre]) == freq) {
            break;
        }
    }
    if (pre == PRESCALE_NUMOF) {
        DEBUG("timer.c: prescaling failed!\n");
        return -1;
    }

    pm_periph_enable(timer_config[tim].pwr);

    dev = timer_config[tim].dev;

    /* stop and reset timer */
    dev->CTRLA = 0;                    /* Stop */
    dev->CTRLFSET = TC_CMD_RESET_gc;   /* Force Reset */

    /* save interrupt context and timer Prescaler */
    ctx[tim].cb = cb;
    ctx[tim].arg = arg;
    ctx[tim].prescaler = (0x07 & (pre + 1));

    /* Check enabled channels */
    ctx[tim].channels = 0;
    for (ch = 0; ch < TIMER_CH_MAX_NUMOF; ch++) {
        if (timer_config[tim].int_lvl[ch] != CPU_INT_LVL_OFF) {
            ctx[tim].channels++;
        }
    }

    if (timer_config[tim].type != TC_TYPE_0
        && timer_config[tim].type != TC_TYPE_4) {
        if (ctx[tim].channels > 2) {
            DEBUG("timer.c: wrong number of channels. max value is 2.\n");
            return -1;
        }
    }

    if (timer_config[tim].type == TC_TYPE_2) {
        DEBUG("timer.c: Timer version %d is current not supported.\n",
            timer_config[tim].type);
        return -1;
    }

    /* Normal Counter with rollover */
    dev->CTRLB = TC_WGMODE_NORMAL_gc;

    /* Compare or Capture disable all channels */
    dev->INTCTRLB = 0;

    /* Free running counter */
    dev->PER = 0xFFFF;

    DEBUG("timer.c: prescaler set to %d \n", ctx[tim].prescaler);
    dev->CTRLA = ctx[tim].prescaler;

    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    if (channel >= ctx[tim].channels) {
        return -1;
    }

    DEBUG("Setting timer %i channel %i to %04x\n", tim, channel, value);

    TC0_t *dev = timer_config[tim].dev;

    dev->INTCTRLB &= ~(TC0_CCAINTLVL_gm << (channel * 2));
    dev->INTFLAGS |= TC0_CCAIF_bm << channel;

    uint8_t irq_state = irq_disable();

    *(((uint16_t *)(&dev->CCA)) + channel) = (uint16_t)value;

    irq_restore(irq_state);
    set_oneshot(tim, channel);

    dev->INTCTRLB |= (timer_config[tim].int_lvl[channel] << (channel * 2));

    return 0;
}

int timer_set_periodic(tim_t tim, int channel, unsigned int value, uint8_t flags)
{
    (void)flags;

    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    if (channel > 0 || ctx[tim].channels != 1) {
        DEBUG("Only channel 0 can be set as periodic and channels must be 1\n");

        return -1;
    }

    DEBUG("Setting timer %i channel 0 to %i and flags %i (repeating)\n",
          tim, value, flags);

    TC0_t *dev = timer_config[tim].dev;
    uint8_t irq_state = irq_disable();

    dev->CTRLA     = 0;
    dev->CTRLFSET  = TC_CMD_RESET_gc;
    dev->CTRLB     = TC_WGMODE_FRQ_gc;
    dev->INTCTRLB  = 0;
    dev->INTFLAGS |= TC0_CCAIF_bm;
    dev->CCA       = (uint16_t)value;
    dev->INTCTRLB  = timer_config[tim].int_lvl[0];
    dev->CTRLA     = ctx[tim].prescaler;

    clear_oneshot(tim, channel);
    irq_restore(irq_state);

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    if (channel >= ctx[tim].channels) {
        return -1;
    }

    DEBUG("timer_clear channel %d\n", channel  );

    TC0_t *dev = timer_config[tim].dev;

    /* Compare or Capture Disable */
    dev->INTCTRLB &= ~(TC0_CCAINTLVL_gm << (channel * 2));

    /* Clear Interrupt Flag
     * The CCxIF is automatically cleared when the corresponding
     * interrupt vector is executed.*/
    dev->INTFLAGS |= TC0_CCAIF_bm << channel;

    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    if (channel >= ctx[tim].channels) {
        return -1;
    }

    TC0_t *dev = timer_config[tim].dev;

    /* Compare or Capture Disable */
    dev->INTCTRLB &= ~(TC0_CCAINTLVL_gm << (channel * 2));

    /* Clear Interrupt Flag */
    dev->INTFLAGS |= TC0_CCAIF_bm << channel;

    uint8_t irq_state = irq_disable();

    /* set value to compare with rollover */
    uint16_t absolute = dev->CNT + timeout;
    *(((uint16_t *)(&dev->CCA)) + channel) = absolute;

    irq_restore(irq_state);
    set_oneshot(tim, channel);

    /* Compare or Capture Enable */
    dev->INTCTRLB |= (timer_config[tim].int_lvl[channel] << (channel * 2));

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    DEBUG("timer_read\n");
    return (unsigned int)timer_config[tim].dev->CNT;
}

void timer_stop(tim_t tim)
{
    if (tim >= TIMER_NUMOF) {
        return;
    }

    DEBUG("timer_stop\n");
    timer_config[tim].dev->CTRLA = 0;
    timer_config[tim].dev->CTRLFSET = TC_CMD_RESTART_gc;
    pm_periph_disable(timer_config[tim].pwr);
}

void timer_start(tim_t tim)
{
    if (tim >= TIMER_NUMOF) {
        return;
    }

    DEBUG("timer_start\n");
    pm_periph_enable(timer_config[tim].pwr);
    timer_config[tim].dev->CTRLA = ctx[tim].prescaler;
}

#ifdef TIMER_NUMOF
static inline void _isr(tim_t tim, int channel)
{
    DEBUG("timer %d _isr channel %d\n", tim, channel);

    if (is_oneshot(tim, channel)) {
        timer_config[tim].dev->INTCTRLB &= ~(TC0_CCAINTLVL_gm << (channel * 2));
    }

    if (ctx[tim].cb) {
        ctx[tim].cb(ctx[tim].arg, channel);
    }
}
#endif

#ifdef TIMER_0_ISRA
AVR8_ISR(TIMER_0_ISRA, _isr, 0, 0);
#endif
#ifdef TIMER_0_ISRB
AVR8_ISR(TIMER_0_ISRB, _isr, 0, 1);
#endif
#ifdef TIMER_0_ISRC
AVR8_ISR(TIMER_0_ISRC, _isr, 0, 2);
#endif
#ifdef TIMER_0_ISRD
AVR8_ISR(TIMER_0_ISRD, _isr, 0, 3);
#endif /* TIMER_0 */

#ifdef TIMER_1_ISRA
AVR8_ISR(TIMER_1_ISRA, _isr, 1, 0);
#endif
#ifdef TIMER_1_ISRB
AVR8_ISR(TIMER_1_ISRB, _isr, 1, 1);
#endif
#ifdef TIMER_1_ISRC
AVR8_ISR(TIMER_1_ISRC, _isr, 1, 2);
#endif
#ifdef TIMER_1_ISRD
AVR8_ISR(TIMER_1_ISRD, _isr, 1, 3);
#endif /* TIMER_1 */

#ifdef TIMER_2_ISRA
AVR8_ISR(TIMER_2_ISRA, _isr, 2, 0);
#endif
#ifdef TIMER_2_ISRB
AVR8_ISR(TIMER_2_ISRB, _isr, 2, 1);
#endif
#ifdef TIMER_2_ISRC
AVR8_ISR(TIMER_2_ISRC, _isr, 2, 2);
#endif
#ifdef TIMER_2_ISRD
AVR8_ISR(TIMER_2_ISRD, _isr, 2, 3);
#endif /* TIMER_2 */

#ifdef TIMER_3_ISRA
AVR8_ISR(TIMER_3_ISRA, _isr, 3, 0);
#endif
#ifdef TIMER_3_ISRB
AVR8_ISR(TIMER_3_ISRB, _isr, 3, 1);
#endif
#ifdef TIMER_3_ISRC
AVR8_ISR(TIMER_3_ISRC, _isr, 3, 2);
#endif
#ifdef TIMER_3_ISRD
AVR8_ISR(TIMER_3_ISRD, _isr, 3, 3);
#endif /* TIMER_3 */

#ifdef TIMER_4_ISRA
AVR8_ISR(TIMER_4_ISRA, _isr, 4, 0);
#endif
#ifdef TIMER_4_ISRB
AVR8_ISR(TIMER_4_ISRB, _isr, 4, 1);
#endif
#ifdef TIMER_4_ISRC
AVR8_ISR(TIMER_4_ISRC, _isr, 4, 2);
#endif
#ifdef TIMER_4_ISRD
AVR8_ISR(TIMER_4_ISRD, _isr, 4, 3);
#endif /* TIMER_4 */

#ifdef TIMER_5_ISRA
AVR8_ISR(TIMER_5_ISRA, _isr, 5, 0);
#endif
#ifdef TIMER_5_ISRB
AVR8_ISR(TIMER_5_ISRB, _isr, 5, 1);
#endif
#ifdef TIMER_5_ISRC
AVR8_ISR(TIMER_5_ISRC, _isr, 5, 2);
#endif
#ifdef TIMER_5_ISRD
AVR8_ISR(TIMER_5_ISRD, _isr, 5, 3);
#endif /* TIMER_5 */

#ifdef TIMER_6_ISRA
AVR8_ISR(TIMER_6_ISRA, _isr, 6, 0);
#endif
#ifdef TIMER_6_ISRB
AVR8_ISR(TIMER_6_ISRB, _isr, 6, 1);
#endif
#ifdef TIMER_6_ISRC
AVR8_ISR(TIMER_6_ISRC, _isr, 6, 2);
#endif
#ifdef TIMER_6_ISRD
AVR8_ISR(TIMER_6_ISRD, _isr, 6, 3);
#endif /* TIMER_6 */

#ifdef TIMER_7_ISRA
AVR8_ISR(TIMER_7_ISRA, _isr, 7, 0);
#endif
#ifdef TIMER_7_ISRB
AVR8_ISR(TIMER_7_ISRB, _isr, 7, 1);
#endif
#ifdef TIMER_7_ISRC
AVR8_ISR(TIMER_7_ISRC, _isr, 7, 2);
#endif
#ifdef TIMER_7_ISRD
AVR8_ISR(TIMER_7_ISRD, _isr, 7, 3);
#endif /* TIMER_7 */
