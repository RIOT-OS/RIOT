/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_rp2040
 * @ingroup drivers_periph_timer
 * @{
 *
 * @file timer.c
 * @brief CPU specific low-level timer driver
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>

#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/timer.h"

static timer_isr_ctx_t timer_isr_ctx[TIMER_NUMOF];

static timer_conf_t timer_config[] = {
    {
        .is_running = false,
        .channel = {
            {
                .period_us = 0,
            },
            {
                .period_us = 0,
            },
            {
                .period_us = 0,
            },
            {
                .period_us = 0,
            }
        }
    }
};

static uint32_t get_time(void);
static void isr_timer(int index);
static void timer_reset_counter(void);

static uint32_t get_time(void) {
    uint32_t low;
    uint32_t high __attribute__((unused));

    /*
     * The sequence of reading the lower word first
     * and then the higher word must be maintained.
     */
    low = timer_hw->timelr;
    high = timer_hw->timehr;

    return low;
}

static void isr_timer(int ch) {
    timer_isr_ctx[0].cb(timer_isr_ctx[0].arg, ch);

    #ifdef TIM_FLAG_RESET_ON_MATCH
        #if TIM_FLAG_RESET_ON_MATCH
            timer_reset_counter();
        #endif
    #endif

    // Handle periodic timer.
    if (timer_config[0].channel[ch].period_us > 0) {
        timer_hw->alarm[ch] =
            get_time() + timer_config[0].channel[ch].period_us;
    }
}

static void timer_reset_counter(void) {
    timer_hw->timelw = 0;
    timer_hw->timehw = 0;
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg) {
    (void)freq;

    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    // Cortex-M0+ NVIC registers.
    io_rw_32 *ppb_nvic_icpr =
        (io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET);

    io_rw_32 *ppb_nvic_iser =
        (io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET);

    // Reset the timer hardware.
    resets_hw->reset |= RESETS_RESET_TIMER_BITS;
    resets_hw->reset &= ~RESETS_RESET_TIMER_BITS;

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_TIMER_BITS)) {}

    // Enable timer interrupts.
    timer_hw->inte |= (1 << 0);
    timer_hw->inte |= (1 << 1);
    timer_hw->inte |= (1 << 2);
    timer_hw->inte |= (1 << 3);

    // Enable CPU timer interrupts, clear any pending interrupts before enabling.
    *ppb_nvic_icpr |= (1 << 0);
    *ppb_nvic_icpr |= (1 << 1);
    *ppb_nvic_icpr |= (1 << 2);
    *ppb_nvic_icpr |= (1 << 3);

    *ppb_nvic_iser |= (1 << 0);
    *ppb_nvic_iser |= (1 << 1);
    *ppb_nvic_iser |= (1 << 2);
    *ppb_nvic_iser |= (1 << 3);

    timer_isr_ctx[dev].cb = cb;
    timer_isr_ctx[dev].arg = arg;

    timer_start(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout) {
    if ((dev >= TIMER_NUMOF) || (channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }

    // Clear periodic timer config.
    timer_config[dev].channel[channel].period_us = 0;

    timer_hw->alarm[channel] = get_time() + timeout;

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value) {
    if ((dev >= TIMER_NUMOF) || (channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }

    timer_stop(dev);

    // Clear periodic timer config.
    timer_config[0].channel[channel].period_us = 0;

    timer_reset_counter();

    timer_hw->alarm[channel] = value;

    timer_start(dev);

    return 0;
}

int timer_set_periodic(tim_t dev, int channel, unsigned int value, uint8_t flags) {
    (void)flags;

    if ((dev >= TIMER_NUMOF) || (channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }

    timer_stop(dev);

    #ifdef TIM_FLAG_RESET_ON_SET
        #if TIM_FLAG_RESET_ON_SET
            timer_reset_counter();
        #endif
    #endif

    timer_config[dev].channel[channel].period_us = value;

    timer_hw->alarm[channel] = get_time() + value;

    timer_start(dev);

    return 0;
}

int timer_clear(tim_t dev, int channel) {
    if ((dev >= TIMER_NUMOF) || (channel >= TIMER_CHANNEL_NUMOF)) {
        return -1;
    }

    timer_hw->alarm[channel] = 0;

    // Clear periodic timer config.
    timer_config[dev].channel[channel].period_us = 0;

    return 0;
}

unsigned int timer_read(tim_t dev) {
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    return (unsigned int)get_time();
}

void timer_start(tim_t dev) {
    assert(dev < TIMER_NUMOF);

    if (timer_config[dev].is_running == true) {
        return;
    }

    timer_hw->pause &= ~(TIMER_PAUSE_BITS);

    timer_config[dev].is_running = true;
}

void timer_stop(tim_t dev) {
    assert(dev < TIMER_NUMOF);

    if (timer_config[dev].is_running == false) {
        return;
    }

    timer_hw->pause |= TIMER_PAUSE_BITS;

    timer_config[dev].is_running = false;
}

// The interrupt handlers.

void isr_timer_0(void) {
    // Clear interrupt of the channel.
    timer_hw->intr |= (1 << 0);

    isr_timer(0);

    cortexm_isr_end();
}

void isr_timer_1(void) {
    // Clear interrupt of the channel.
    timer_hw->intr |= (1 << 1);

    isr_timer(1);

    cortexm_isr_end();
}

void isr_timer_2(void) {
    // Clear interrupt of the channel.
    timer_hw->intr |= (1 << 2);

    isr_timer(2);

    cortexm_isr_end();
}

void isr_timer_3(void) {
    // Clear interrupt of the channel.
    timer_hw->intr |= (1 << 3);

    isr_timer(3);

    cortexm_isr_end();
}
