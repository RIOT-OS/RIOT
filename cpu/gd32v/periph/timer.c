/*
 * SPDX-FileCopyrightText: 2020 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "cpu.h"
#include "periph/timer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _timer_isr(unsigned irq);

/**
 * @brief   Interrupt context for each configured timer
 */
static timer_isr_ctx_t isr_ctx[TIMER_NUMOF];

/**
 * @brief   Get the timer device
 */
static inline TIMER_Type *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

#ifdef MODULE_PERIPH_TIMER_PERIODIC

/**
 * @brief   Helper macro to get channel bit in timer/channel bitmap
 */
#define CHAN_BIT(tim, chan) (1 << chan) << (TIMER_CHANNEL_NUMOF * (tim & 1))

/**
 * @brief   Bitmap for compare channel disable after match
 */
static uint8_t _oneshot[(TIMER_NUMOF + 1) / 2];

/**
 * @brief   Clear interrupt enable after the interrupt has fired
 */
static inline void set_oneshot(tim_t tim, int chan)
{
    _oneshot[tim >> 1] |= CHAN_BIT(tim, chan);
}

/**
 * @brief   Enable interrupt with every wrap-around of the timer
 */
static inline void clear_oneshot(tim_t tim, int chan)
{
    _oneshot[tim >> 1] &= ~CHAN_BIT(tim, chan);
}

static inline bool is_oneshot(tim_t tim, int chan)
{
    return _oneshot[tim >> 1] & CHAN_BIT(tim, chan);
}

#else /* !MODULE_PERIPH_TIMER_PERIODIC */

static inline void set_oneshot(tim_t tim, int chan)
{
    (void)tim;
    (void)chan;
}

static inline bool is_oneshot(tim_t tim, int chan)
{
    (void)tim;
    (void)chan;

    return true;
}

#endif /* MODULE_PERIPH_TIMER_PERIODIC */

int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* check if device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* remember the interrupt context */
    isr_ctx[tim].cb = cb;
    isr_ctx[tim].arg = arg;

    /* enable the peripheral clock */
    periph_clk_en(timer_config[tim].bus, timer_config[tim].rcu_mask);

    /* configure the timer as upcounter in continuous mode */
    dev(tim)->CTL0 = 0;
    dev(tim)->CTL1 = 0;
    dev(tim)->CAR = timer_config[tim].max;

    /* set prescaler */
    if (dev(tim) == TIMER0) {
        dev(tim)->PSC = ((periph_apb_clk(timer_config[tim].bus) / freq) - 1);
    }
    else {
        dev(tim)->PSC = (((periph_apb_clk(timer_config[tim].bus) * 2) / freq) - 1);
    }
    DEBUG("[timer]: %" PRIu32 "/%lu =  %" PRIu16 "\n",
          periph_apb_clk(timer_config[tim].bus), freq, dev(tim)->PSC);

    /* generate an update event to apply our configuration */
    dev(tim)->SWEVG = TIMER0_SWEVG_UPG_Msk;

    /* enable the timer's interrupt */
    clic_enable_interrupt(timer_config[tim].irqn, CPU_DEFAULT_IRQ_PRIO);
    clic_set_handler(timer_config[tim].irqn, _timer_isr);

    /* reset the counter and start the timer */
    timer_start(tim);

    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    DEBUG("[timer]: setting %u to %i\n", value, channel);
    if (channel >= (int)TIMER_CHANNEL_NUMOF) {
        return -1;
    }

    set_oneshot(tim, channel);

    TIMER_CHANNEL(tim, channel) = (value & timer_config[tim].max);

#ifdef MODULE_PERIPH_TIMER_PERIODIC
    if (dev(tim)->CAR == TIMER_CHANNEL(tim, channel)) {
        dev(tim)->CAR = timer_config[tim].max;
    }
#endif

    /* clear a possibly pending interrupt and enable the interrupt */
    dev(tim)->INTF &= ~(TIMER0_DMAINTEN_CH0IE_Msk << channel);
    dev(tim)->DMAINTEN |= (TIMER0_DMAINTEN_CH0IE_Msk << channel);

    return 0;
}

#ifdef MODULE_PERIPH_TIMER_PERIODIC
int timer_set_periodic(tim_t tim, int channel, unsigned int value,
                       uint8_t flags)
{
    if (channel >= (int)TIMER_CHANNEL_NUMOF) {
        return -1;
    }

    if (flags & TIM_FLAG_SET_STOPPED) {
        timer_stop(tim);
    }

    clear_oneshot(tim, channel);

    if (flags & TIM_FLAG_RESET_ON_SET) {
        /* setting COUNT gives us an interrupt on all channels */
        unsigned state = irq_disable();
        dev(tim)->CNT = 0;

        /* wait for the interrupt & clear it */
        while (dev(tim)->INTF == 0) {}
        dev(tim)->INTF = 0;

        irq_restore(state);
    }

    TIMER_CHANNEL(tim, channel) = value;

    /* clear a possibly pending interrupt before the interrupt is enabled */
    dev(tim)->INTF &= ~(TIMER0_DMAINTEN_CH0IE_Msk << channel);
    dev(tim)->DMAINTEN |= (TIMER0_DMAINTEN_CH0IE_Msk << channel);

    if (flags & TIM_FLAG_RESET_ON_MATCH) {
        dev(tim)->CAR = value;
    }

    return 0;
}
#endif /* MODULE_PERIPH_TIMER_PERIODIC */

int timer_clear(tim_t tim, int channel)
{
    if (channel >= (int)TIMER_CHANNEL_NUMOF) {
        return -1;
    }

    dev(tim)->DMAINTEN &= ~(TIMER0_DMAINTEN_CH0IE_Msk << channel);

#ifdef MODULE_PERIPH_TIMER_PERIODIC
    if (dev(tim)->CAR == TIMER_CHANNEL(tim, channel)) {
        dev(tim)->CAR = timer_config[tim].max;
    }
#endif

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    return (unsigned int)dev(tim)->CNT;
}

void timer_start(tim_t tim)
{
    dev(tim)->CTL0 |= TIMER0_CTL0_CEN_Msk;
}

void timer_stop(tim_t tim)
{
    dev(tim)->CTL0 &= ~(TIMER0_CTL0_CEN_Msk);
}

static void _irq_handler(tim_t tim)
{
    uint32_t top = dev(tim)->CAR;
    uint32_t status = dev(tim)->INTF & dev(tim)->DMAINTEN;

    dev(tim)->INTF = 0;

    for (unsigned int i = 0; status; i++) {
        uint32_t msk = 1 << (TIMER0_INTF_CH0IF_Pos + i);

        /* check if interrupt flag is set */
        if ((status & msk) == 0) {
            continue;
        }
        status &= ~msk;

        /* interrupt flag gets set for all channels > ARR */
        if (TIMER_CHANNEL(tim, i) > top) {
            continue;
        }

        /* disable Interrupt */
        if (is_oneshot(tim, i)) {
            dev(tim)->DMAINTEN &= ~msk;
        }

        isr_ctx[tim].cb(isr_ctx[tim].arg, i);
    }
}

static void _timer_isr(unsigned irq)
{
    switch (irq) {
#ifdef TIMER_0_IRQN
    case TIMER_0_IRQN:
        _irq_handler(TIMER_DEV(0));
        break;
#endif
#ifdef TIMER_1_IRQN
    case TIMER_1_IRQN:
        _irq_handler(TIMER_DEV(1));
        break;
#endif
#ifdef TIMER_2_IRQN
    case TIMER_2_IRQN:
        _irq_handler(TIMER_DEV(2));
        break;
#endif
#ifdef TIMER_3_IRQN
    case TIMER_3_IRQN:
        _irq_handler(TIMER_DEV(3));
        break;
#endif
#ifdef TIMER_4_IRQN
    case TIMER_4_IRQN:
        _irq_handler(TIMER_DEV(4));
        break;
#endif
    default:
        assert(false);
    }
}
