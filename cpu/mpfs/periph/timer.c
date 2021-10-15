/*
 * Copyright (C) 2021 Mesotic SAS
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_mpfs
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "plic.h"

#include "periph/timer.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static timer_cb_t _callback[2];
static void* _arg[2];
void timer_isr(int irq);
uint16_t prescaler = 1;

static inline TIMER_TypeDef *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

static inline uint32_t _timer_value(uint32_t val)
{
    return (val * prescaler);
}

static bool _is_oneshot(unsigned num)
{
    if (num == 1) {
        return (((TIMER_LO->TIM1_CTRL & TIM1_MODE_MASK) >> TIM1_MODE_SHIFT) == MSS_TIMER_ONE_SHOT_MODE) ?
                true : false;
    } else if (num == 2) {
        return (((TIMER_LO->TIM2_CTRL & TIM2_MODE_MASK) >> TIM2_MODE_SHIFT) == MSS_TIMER_ONE_SHOT_MODE) ?
                true : false;
    } else {
        assert(0);
    }
    return false;
}

int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
    /* MPFS Timer IP is feed by 600MHz / 4 by default like all others
       peripherals, and doesn't have a hardware prescaler. Thus we
       need to emulate it by software to match RIOT timer frequency */
    prescaler = ((CLOCK_CORECLOCK / 4) / freq);
    mss_config_clk_rst(MSS_PERIPH_TIMER, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);
    if (timer_config[tim].num == 1)
    {
        _callback[0] = cb;
        _arg[0] = arg;
        plic_set_isr_cb(TIMER1_PLIC, timer_isr);
        plic_set_priority(TIMER1_PLIC, 2);
        plic_enable_interrupt(TIMER1_PLIC);
    }
    else
    {
        _callback[1] = cb;
        _arg[1] = arg;
        plic_set_isr_cb(TIMER2_PLIC, timer_isr);
        plic_set_priority(TIMER2_PLIC, 2);
        plic_enable_interrupt(TIMER2_PLIC);
    }
    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    if (channel > 0) {
        return -1;
    }
    unsigned int state = irq_disable();
    unsigned int test = MSS_TIM1_get_current_value(dev(tim)) + timeout;
    int res = timer_set_absolute(tim, channel, test);
    irq_restore(state);
    printf("timer_set %d\n", test);
    return res;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if (channel > 0) {
        return -1;
    }

    if (timer_config[tim].num == 1)
    {
        MSS_TIM1_stop(dev(tim));
        MSS_TIM1_init(dev(tim), MSS_TIMER_ONE_SHOT_MODE);
        MSS_TIM1_load_immediate(dev(tim), 150 * value);
        MSS_TIM1_load_background(dev(tim), UINT32_MAX);
        MSS_TIM1_enable_irq(dev(tim));
        MSS_TIM1_start(dev(tim));
    }
    else
    {
        MSS_TIM2_init(dev(tim), MSS_TIMER_ONE_SHOT_MODE);
        MSS_TIM2_load_immediate(dev(tim), 150 * value);
        MSS_TIM2_enable_irq(dev(tim));
        MSS_TIM2_start(dev(tim));
    }

    return 0;
}

int timer_set_periodic(tim_t tim, int channel, unsigned int value, uint8_t flags)
{
    (void)flags;
    if (channel > 0) {
        return -1;
    }
    unsigned int mask = irq_disable();
    if (timer_config[tim].num == 1)
    {
        MSS_TIM1_stop(dev(tim));
        MSS_TIM1_init(dev(tim), MSS_TIMER_PERIODIC_MODE);
        MSS_TIM1_load_background(dev(tim), _timer_value(value));
        MSS_TIM1_enable_irq(dev(tim));
        MSS_TIM1_start(dev(tim));
    }
    else
    {
        MSS_TIM2_init(dev(tim), MSS_TIMER_PERIODIC_MODE);
        MSS_TIM2_load_background(dev(tim), _timer_value(value));
        MSS_TIM2_enable_irq(dev(tim));
        MSS_TIM2_start(dev(tim));
    }
    irq_restore(mask);
    return 0;
}

int timer_clear(tim_t tim, int channel)
{ 
    if (channel > 0) {
        return -1;
    }
    if (timer_config[tim].num == 1)
    {
        dev(tim)->TIM1_CTRL &= ~((uint32_t)TIM1_INTEN_MASK);
    }
    else
    {
        dev(tim)->TIM2_CTRL &= ~((uint32_t)TIM2_INTEN_MASK);
    }
    return 0;
}

unsigned int timer_read(tim_t tim)
{
    if (timer_config[tim].num == 1)
    {
        return UINT32_MAX - MSS_TIM2_get_current_value(dev(tim)); 
    }
    else
    {
        return UINT32_MAX - MSS_TIM2_get_current_value(dev(tim));
    }
}

void timer_start(tim_t tim)
{
    if (timer_config[tim].num == 1)
    {
        MSS_TIM1_start(dev(tim));
    }
    else
    {
        MSS_TIM2_start(dev(tim));
    }
}

void timer_stop(tim_t tim)
{
    if (timer_config[tim].num == 1)
    {
        MSS_TIM1_stop(dev(tim));
    }
    else
    {
        MSS_TIM2_stop(dev(tim));
    }
}

void timer_isr(int irq)
{
    /* Compare to n-1 IRQn so index start at 1 */
    int num = irq - RTC_MATCH_PLIC;
    if (num == 1) {
        if (_is_oneshot(num)) {
            MSS_TIM1_disable_irq(dev(0));
        }
        MSS_TIM1_clear_irq(dev(0));
    } else if (num == 2) {
        if (_is_oneshot(num)) {
            MSS_TIM2_disable_irq(dev(0));
        }
        MSS_TIM2_clear_irq(dev(0));
    } else {
        assert(0);
    }

    _callback[num-1](_arg[num-1], 0);
}
