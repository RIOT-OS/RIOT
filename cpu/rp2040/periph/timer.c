/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rp2040
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       CPU specific low-level GPIO driver implementation for rp2040
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>

#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/timer.h"

static timer_isr_ctx_t timer_isr_ctx[NUM_TIMER];

static timer_conf_t timer_config[] = {
    {
        .is_running = false,
        .channel = {
            {
                .is_relative = false,
                .is_absolute = false,
                .is_periodic = false,
            },
            {
                .is_relative = false,
                .is_absolute = false,
                .is_periodic = false,
            },
            {
                .is_relative = false,
                .is_absolute = false,
                .is_periodic = false,
            },
            {
                .is_relative = false,
                .is_absolute = false,
                .is_periodic = false,
            }
        }
    }
};

static void timer_pause(bool pause);
static void timer_reset_counter(void);
static void handle_reset_on_match(void);
static void timer_arm_channel(bool arm, int channel);

static void timer_reset_counter(void) {
    timer_pause(true);

    timer_hw->timelw = 0;
    timer_hw->timehw = 0;

    timer_pause(false);
}

static void handle_reset_on_match(void) {
    #ifdef TIM_FLAG_RESET_ON_MATCH
    #if TIM_FLAG_RESET_ON_MATCH
    timer_reset_counter();
    #endif
    #endif
}

static void timer_pause(bool pause) {
    if(pause) {
        timer_hw->pause |= (TIMER_PAUSE_BITS << TIMER_PAUSE_LSB);
    }
    else {
        timer_hw->pause &= ~(TIMER_PAUSE_BITS << TIMER_PAUSE_LSB);
    }
}

static void timer_arm_channel(bool arm, int channel) {
    if(arm) {
        timer_hw->armed &= ~(0x1 << channel);
        timer_hw->inte |= (1 << channel);
    }
    else {
        timer_hw->armed |= (0x1 << channel);
        timer_hw->inte &= ~(1 << channel);
    }
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg) {
    assert(dev < NUM_TIMER);

    (void)freq; /* Cannot adjust Frequency */

    io_rw_32 *ppb_nvic_iser = (io_rw_32*)(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET);

    // Release reset of timer hardware.
    resets_hw->reset &= ~RESETS_RESET_TIMER_BITS;

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_TIMER_BITS)) {
        ;
    }

    *ppb_nvic_iser |= (1 << 0);
    *ppb_nvic_iser |= (1 << 1);
    *ppb_nvic_iser |= (1 << 2);
    *ppb_nvic_iser |= (1 << 3);

    timer_config[0].is_running = true;

    timer_isr_ctx[dev].cb = cb;
    timer_isr_ctx[dev].arg = arg;

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout) {
    assert(dev < NUM_TIMER);

    timer_arm_channel(false, channel);

    timer_pause(true);

    timer_config[0].channel[channel].is_relative = true;
    timer_config[0].channel[channel].is_absolute = false;
    timer_config[0].channel[channel].is_periodic = false;

    timer_hw->alarm[channel] = timer_hw->timelr + timeout;

    timer_arm_channel(true, channel);
    timer_pause(false);

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value) {
    assert(dev < NUM_TIMER);

    timer_arm_channel(false, channel);

    timer_pause(true);

    timer_config[0].channel[channel].is_relative = false;
    timer_config[0].channel[channel].is_absolute = true;
    timer_config[0].channel[channel].is_periodic = false;

    timer_hw->alarm[channel] = value;

    timer_arm_channel(true, channel);

    timer_pause(false);

    return 0;
}

int timer_set_periodic(tim_t dev, int channel, unsigned int value, uint8_t flags) {
    assert(dev < NUM_TIMER);

    (void)flags;

    timer_arm_channel(false, channel);

    timer_pause(true);

    #ifdef TIM_FLAG_RESET_ON_SET
    #if TIM_FLAG_RESET_ON_SET
    timer_reset_counter();
    #endif
    #endif

    timer_config[0].channel[channel].is_relative = false;
    timer_config[0].channel[channel].is_absolute = true;
    timer_config[0].channel[channel].is_periodic = true;

    timer_hw->alarm[channel] = value;

    timer_arm_channel(true, channel);
    timer_pause(false);

    return 0;
}

int timer_clear(tim_t dev, int channel) {
    assert(dev < NUM_TIMER);

    // Disarm channel.
    timer_arm_channel(false, channel);

    timer_hw->alarm[channel] = 0;

    timer_config[0].channel[channel].is_relative = false;
    timer_config[0].channel[channel].is_absolute = false;
    timer_config[0].channel[channel].is_periodic = false;

    // Disarm channel.
    timer_arm_channel(false, channel);

    return 0;
}

unsigned int timer_read(tim_t dev) {
    assert(dev < NUM_TIMER);

    return timer_hw->timelr;
}

void timer_start(tim_t dev) {
    assert(dev < NUM_TIMER);

    if (timer_config[0].is_running == true) {
        return;
    }

    timer_pause(false);

    timer_config[0].is_running = true;
}

void timer_stop(tim_t dev) {
    assert(dev < NUM_TIMER);

    if(timer_config[0].is_running == false) {
        return;
    }

    timer_pause(true);

    timer_config[0].is_running = false;
}

/**
 * The Timer interrupt handlers.
 */

void isr_timer_0(void) {
    handle_reset_on_match();

    timer_isr_ctx[0].cb(timer_isr_ctx[0].arg, 0);

    if (timer_config[0].channel[0].is_periodic) {
        timer_arm_channel(true, 0);
    }

    cortexm_isr_end();
}

void isr_timer_1(void) {
    timer_isr_ctx[0].cb(timer_isr_ctx[0].arg, 1);

    if (timer_config[0].channel[1].is_periodic) {
        timer_arm_channel(true, 1);
    }

    cortexm_isr_end();
}

void isr_timer_2(void) {
    timer_isr_ctx[0].cb(timer_isr_ctx[0].arg, 2);

    if (timer_config[0].channel[2].is_periodic) {
        timer_arm_channel(true, 2);
    }

    cortexm_isr_end();
}

void isr_timer_3(void) {
    timer_isr_ctx[0].cb(timer_isr_ctx[0].arg, 3);

    if (timer_config[0].channel[3].is_periodic) {
        timer_arm_channel(true, 3);
    }

    cortexm_isr_end();
}
