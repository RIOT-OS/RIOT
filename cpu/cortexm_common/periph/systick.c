/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_systick
 * @{
 *
 * @file        systick.c
 * @brief       Low-level timer driver implementation based on SysTick
 *
 *              SysTick is a 24-bit Down-Counting Timer
 *              It runs at the same frequency as the CPU
 *              The count (VAL) register can only be set to 0 on write
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <assert.h>
#include "board.h"
#include "periph_conf.h"
#include "systick.h"

/* SysTick CTRL configuration */
#define ENABLE_MASK     (SysTick_CTRL_CLKSOURCE_Msk |\
                         SysTick_CTRL_ENABLE_Msk    |\
                         SysTick_CTRL_TICKINT_Msk)

#define ALARM_EXT_MAX   ((1UL << (8 * sizeof(alarm_ext))) - 1)

/* interrupt is always enabled to update time */
static bool alarm_enabled;

/* divider for CLOCK_CORECLOCK */
static uint16_t prescaler = 1;

/* full periods before alarm */
static uint16_t alarm_ext;

/* remaining time to alarm (last period length) */
static uint32_t alarm_val;

/* current time if the timer ran at freq Hz */
static uint32_t now;

/* offset in CLOCK_CORECLOCK ticks */
static uint32_t now_offset;

/* dummy SysTick callback */
__attribute__((weak))
void systick_callback(void)
{
    /* no-op */
}

void systick_timer_init(unsigned long freq)
{
    if (IS_USED(MODULE_SYSTICK_PRESCALER)) {
        assert(freq <= CLOCK_CORECLOCK);

        prescaler = CLOCK_CORECLOCK / freq;
        assert(CLOCK_CORECLOCK % freq == 0);
    } else {
        assert(freq == CLOCK_CORECLOCK);
    }

    SysTick->VAL  = 0;
    SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
    SysTick->CTRL = ENABLE_MASK;

    NVIC_SetPriority(SysTick_IRQn, CPU_DEFAULT_IRQ_PRIO);
    NVIC_EnableIRQ(SysTick_IRQn);
}

int systick_timer_set(unsigned int value)
{
    uint64_t val64 = value * prescaler;

    /* check if value fits with prescaler */
    if ((val64 >> 24) > ALARM_EXT_MAX) {
        return -1;
    }

    /* stop timer to prevent race condition */
    SysTick->CTRL = 0;

    alarm_ext = val64 >> 24;
    alarm_val = val64 & SysTick_LOAD_RELOAD_Msk;

    if (IS_USED(MODULE_SYSTICK_PRESCALER)) {
        now_offset += SysTick->LOAD - SysTick->VAL;
    } else {
        now += SysTick->LOAD - SysTick->VAL;
    }

    if (alarm_ext) {
        SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
    } else {
        SysTick->LOAD = alarm_val;
    }

    /* force reload */
    SysTick->VAL = 0;

    /* start the timer again */
    SysTick->CTRL = ENABLE_MASK;

    /* check if the last period is after the next reload */
    if (alarm_ext == 1) {
        SysTick->LOAD = alarm_val;
    }

    alarm_enabled = 1;

    return 0;
}

int systick_timer_set_absolute(unsigned int value)
{
    return systick_timer_set(value - systick_timer_read());
}

void systick_timer_clear(void)
{
    SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
    alarm_enabled = 0;
}

unsigned int systick_timer_read(void)
{
    uint32_t val, load;

    /* re-read registers if VAL wraps around */
    do {
        val  = SysTick->VAL;
        load = SysTick->LOAD;
    } while (SysTick->VAL > val);

    if (IS_USED(MODULE_SYSTICK_PRESCALER)) {
        uint32_t current_offset = load - val;
        return now + (current_offset + now_offset) / prescaler;
    } else {
        return now + load - val;
    }
}

void systick_timer_start(void)
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void systick_timer_stop(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void isr_systick(void)
{
    /* update clock */
    if (IS_USED(MODULE_SYSTICK_PRESCALER)) {
        now_offset += SysTick->LOAD;

        now += now_offset / prescaler;
        now_offset = now_offset % prescaler;
    } else {
        now += SysTick->LOAD;
    }

    if (!alarm_enabled) {
        return;
    }

    /* alarm handling */

    if (alarm_ext) {
        /* alarm happens after next period
         * set reload value to remaining alarm time
         */
        if (--alarm_ext == 1) {
            SysTick->LOAD = alarm_val;
        }

        return;
    }

    alarm_enabled = 0;

    systick_callback();

    /* We only need to trigger the scheduler after the callback was executed. */
    /* No need to trigger scheduling when just updating the counter.          */
    cortexm_isr_end();
}
