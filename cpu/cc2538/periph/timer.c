/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for the CC2538 CPU
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "board.h"
#include "cpu.h"
#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define LOAD_VALUE               0xffff

#define TIMER_A_IRQ_MASK         0x000000ff
#define TIMER_B_IRQ_MASK         0x0000ff00

#define BIT(n)                   ( 1UL << (n) )

/* GPTIMER_CTL Bits: */
#define TBEN                     BIT(8)
#define TAEN                     BIT(0)

/* GPTIMER_TnMR Bits: */
#define TnCMIE                   BIT(5)
#define TnCDIR                   BIT(4)

/* GPTIMER_IMR Bits: */
#define TBMIM                    BIT(11)
#define TAMIM                    BIT(4)

/* Convert a gptimer instance pointer to a GPTimer number */
#define GPTIMER_GET_NUM(gptimer) ( ((uintptr_t)(gptimer) >> 12) & 0x3 )

#define match_bit(chan)          ( (chan)? TBMIM : TAMIM )

/**
 * @brief Timer state memory
 */
static timer_isr_ctx_t config[GPTIMER_NUMOF];

static const int IRQn_lut[GPTIMER_NUMOF] = {
    GPTIMER_0A_IRQn,
    GPTIMER_1A_IRQn,
    GPTIMER_2A_IRQn,
    GPTIMER_3A_IRQn
};

/* enable timer interrupts */
static inline void _irq_enable(tim_t dev);

/**
 * @brief Setup the given timer
 *
 */
int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    cc2538_gptimer_t *gptimer = timer_config[dev].dev;
    unsigned int gptimer_num;
    uint32_t chan_mode;

    DEBUG("%s(%u, %lu, %p, %p)\n", __FUNCTION__, dev, freq, cb, arg);

    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    gptimer_num = GPTIMER_GET_NUM(gptimer);

    /* Save the callback function: */
    assert(gptimer_num < GPTIMER_NUMOF);
    config[gptimer_num].cb  = cb;
    config[gptimer_num].arg = arg;

    /* Enable the clock for this timer: */
    SYS_CTRL_RCGCGPT |= (1 << gptimer_num);

    /* Disable this timer before configuring it: */
    gptimer->cc2538_gptimer_ctl.CTL = 0;

    if (timer_config[dev].cfg == GPTMCFG_32_BIT_TIMER) {
        /* Count up in periodic mode */
        chan_mode = TnCMIE | TnCDIR | GPTIMER_PERIODIC_MODE;

        if (timer_config[dev].channels > 1) {
            DEBUG("Invalid timer_config. Multiple channels are available only in 16-bit mode.");
            return -1;
        }

        if (freq != sys_clock_freq()) {
            DEBUG("In 32-bit mode, the GPTimer frequency must equal the system clock frequency (%u).", sys_clock_freq());
            return -1;
        }
    } else {
        /* Count down in periodic mode */
        chan_mode = TnCMIE | GPTIMER_PERIODIC_MODE;
    }

    gptimer->CFG = timer_config[dev].cfg;
    gptimer->cc2538_gptimer_tamr.TAMR = chan_mode;

    switch (timer_config[dev].channels) {
        case 1:
            /* Enable the timer: */
            gptimer->cc2538_gptimer_ctl.CTL = TAEN;
            break;

        case 2:
            gptimer->cc2538_gptimer_tbmr.TBMR = chan_mode;

            gptimer->TAILR = LOAD_VALUE;
            gptimer->TBILR = LOAD_VALUE;

            uint32_t prescaler = sys_clock_freq();
            prescaler += freq / 2;
            prescaler /= freq;
            if (prescaler >   0) prescaler--;
            if (prescaler > 255) prescaler = 255;

            gptimer->TAPR = prescaler;
            gptimer->TBPR = prescaler;

            /* Enable the timer: */
            gptimer->cc2538_gptimer_ctl.CTL = TBEN | TAEN;
            break;
    }

    /* Enable interrupts for given timer: */
    _irq_enable(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    /* get timer base register address */
    cc2538_gptimer_t *gptimer = timer_config[dev].dev;

    if ( (dev >= TIMER_NUMOF) || (channel >= timer_config[dev].channels) ) {
        return -1;
    }

    switch (channel) {
        case 0:
            /* clear any pending match interrupts */
            gptimer->ICR = TAMIM;

            /* set timeout value */
            gptimer->TAMATCHR = (gptimer->CFG == GPTMCFG_32_BIT_TIMER)? (gptimer->TAV + timeout) : (gptimer->TAV - timeout);
            gptimer->cc2538_gptimer_imr.IMR |= TAMIM; /**< Enable the Timer A Match Interrupt */
            break;

        case 1:
            /* clear any pending match interrupts */
            gptimer->ICR = TBMIM;

            /* set timeout value */
            gptimer->TBMATCHR = (gptimer->CFG == GPTMCFG_32_BIT_TIMER)? (gptimer->TBV + timeout) : (gptimer->TBV - timeout);
            gptimer->cc2538_gptimer_imr.IMR |= TBMIM; /**< Enable the Timer B Match Interrupt */
            break;
    }

    return 1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    DEBUG("%s(%u, %u, %u)\n", __FUNCTION__, dev, channel, value);

    /* get timer base register address */
    cc2538_gptimer_t *gptimer = timer_config[dev].dev;

    if ( (dev >= TIMER_NUMOF) || (channel >= timer_config[dev].channels) ) {
        return -1;
    }

    switch (channel) {
    case 0:
        /* clear any pending match interrupts */
        gptimer->ICR = TAMIM;

        gptimer->TAMATCHR = (gptimer->CFG == GPTMCFG_32_BIT_TIMER)? value : (LOAD_VALUE - value);
        gptimer->cc2538_gptimer_imr.IMR |= TAMIM; /**< Enable the Timer A Match Interrupt */
        break;

    case 1:
        /* clear any pending match interrupts */
        gptimer->ICR = TBMIM;

        gptimer->TBMATCHR = (gptimer->CFG == GPTMCFG_32_BIT_TIMER)? value : (LOAD_VALUE - value);
        gptimer->cc2538_gptimer_imr.IMR |= TBMIM; /**< Enable the Timer B Match Interrupt */
        break;
    }

    /* set timeout value */

    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    DEBUG("%s(%u, %u)\n", __FUNCTION__, dev, channel);

    if ( (dev >= TIMER_NUMOF) || (channel >= timer_config[dev].channels) ) {
        return -1;
    }

    timer_config[dev].dev->cc2538_gptimer_imr.IMR &= ~match_bit(channel);

    return 1;
}

/*
 * The timer channels 1 and 2 are configured to run with the same speed and
 * have the same value (they run in parallel), so only on of them is returned.
 */
unsigned int timer_read(tim_t dev)
{
    if (dev >= TIMER_NUMOF) {
        return 0;
    }

    cc2538_gptimer_t* gptimer = timer_config[dev].dev;
    if (gptimer->CFG == GPTMCFG_32_BIT_TIMER) {
        return gptimer->TAV;
    }
    else {
        return LOAD_VALUE - (gptimer->TAV & 0xffff);
    }
}

/*
 * For stopping the counting of all channels.
 */
void timer_stop(tim_t dev)
{
    DEBUG("%s(%u)\n", __FUNCTION__, dev);

    if (dev < TIMER_NUMOF) {
        timer_config[dev].dev->cc2538_gptimer_ctl.CTL = 0;
    }
}

void timer_start(tim_t dev)
{
    DEBUG("%s(%u)\n", __FUNCTION__, dev);

    if (dev < TIMER_NUMOF) {
        switch (timer_config[dev].channels) {
            case 1:
                timer_config[dev].dev->cc2538_gptimer_ctl.CTL = TAEN;
                break;

            case 2:
                timer_config[dev].dev->cc2538_gptimer_ctl.CTL = TBEN | TAEN;
                break;
        }
    }
}

static inline void _irq_enable(tim_t dev)
{
    DEBUG("%s(%u)\n", __FUNCTION__, dev);

    if (dev < TIMER_NUMOF) {
        IRQn_Type irqn = IRQn_lut[GPTIMER_GET_NUM(timer_config[dev].dev)];

        NVIC_SetPriority(irqn, TIMER_IRQ_PRIO);
        NVIC_EnableIRQ(irqn);

        if (timer_config[dev].channels == 2) {
            irqn++;
            NVIC_SetPriority(irqn, TIMER_IRQ_PRIO);
            NVIC_EnableIRQ(irqn);
        }
    }
}

static cc2538_gptimer_t* GPTIMER = GPTIMER0;

static void irq_handler_a(int n) {
    uint32_t mis;

    /* Latch the active interrupt flags */
    mis = GPTIMER[n].MIS & TIMER_A_IRQ_MASK;

    /* Clear the latched interrupt flags */
    GPTIMER[n].ICR = mis;

    if (mis & TAMIM) {
        /* This is a Timer A Match Interrupt */

        /* Disable further match interrupts for this timer/channel */
        GPTIMER[n].cc2538_gptimer_imr.IMR &= ~TAMIM;

        /* Invoke the callback function */
        assert(config[n].cb != NULL);
        config[n].cb(config[n].arg, 0);
    }

    cortexm_isr_end();
}

static void irq_handler_b(int n) {
    uint32_t mis;

    /* Latch the active interrupt flags */
    mis = GPTIMER[n].MIS & TIMER_B_IRQ_MASK;

    /* Clear the latched interrupt flags */
    GPTIMER[n].ICR = mis;

    if (mis & TBMIM) {
        /* This is a Timer B Match Interrupt */

        /* Disable further match interrupts for this timer/channel */
        GPTIMER[n].cc2538_gptimer_imr.IMR &= ~TBMIM;

        /* Invoke the callback function */
        assert(config[n].cb != NULL);
        config[n].cb(config[n].arg, 1);
    }

    cortexm_isr_end();
}

void isr_timer0_chan0(void) {irq_handler_a(0);}
void isr_timer0_chan1(void) {irq_handler_b(0);}
void isr_timer1_chan0(void) {irq_handler_a(1);}
void isr_timer1_chan1(void) {irq_handler_b(1);}
void isr_timer2_chan0(void) {irq_handler_a(2);}
void isr_timer2_chan1(void) {irq_handler_b(2);}
void isr_timer3_chan0(void) {irq_handler_a(3);}
void isr_timer3_chan1(void) {irq_handler_b(3);}
