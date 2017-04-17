/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_s5p4418
 * @{
 *
 * @file
 * @brief       Low-level TIMER driver implementation
 *
 * @author      Phuong Dang <kamejoko80@yahoo.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "cpu.h"
#include "periph/timer.h"
#include "s5p4418_timer.h"
#include "s5p4418_irq.h"

#define TIMER_NUMOF 4

/**
 * @brief Struct holding the configuration data
 * @{
 */
typedef struct {
    timer_cb_t cb;          /* timeout callback */
    void *arg;              /* argument to the callback */
    uint32_t tick_count;    /* timer tick counter */
} timer_conf_t;

static timer_conf_t config[TIMER_NUMOF];
/**@}*/

void timer0_irq(void)
{
    /* Clear interrupt flag */
    s5p4418_timer_clear_irq_flag(TIMER_CH0);

    /* Allow re-entrant IRQ */
    __asm__ __volatile__("cpsie i");

    if(config[TIMER_CH0].cb != NULL)
    {
        config[TIMER_CH0].cb(config[TIMER_CH0].arg, TIMER_CH0);
    }
}

void timer1_irq(void)
{
    /* Clear interrupt flag */
    s5p4418_timer_clear_irq_flag(TIMER_CH1);

    /* Allow re-entrant IRQ */
    __asm__ __volatile__("cpsie i");

    if(config[TIMER_CH1].cb != NULL)
    {
        config[TIMER_CH1].cb(config[TIMER_CH1].arg, TIMER_CH1);
    }
}

void timer2_irq(void)
{
    /* Clear interrupt flag */
    s5p4418_timer_clear_irq_flag(TIMER_CH2);

    /* Allow re-entrant IRQ */
    __asm__ __volatile__("cpsie i");

    if(config[TIMER_CH2].cb != NULL)
    {
        config[TIMER_CH2].cb(config[TIMER_CH2].arg, TIMER_CH2);
    }
}

void timer3_irq(void)
{
    /* Clear interrupt flag */
    s5p4418_timer_clear_irq_flag(TIMER_CH3);

    /* Allow re-entrant IRQ */
    __asm__ __volatile__("cpsie i");

    if(config[TIMER_CH3].cb != NULL)
    {
        config[TIMER_CH3].cb(config[TIMER_CH3].arg, TIMER_CH3);
    }
}

int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
   bool result = false;

   /* Setup timer */
   s5p4418_timer_setup((uint8_t)tim, 0xFFFFFFFF, true);

   /* Store call back information */
   config[tim].cb = cb;
   config[tim].arg = arg;
   config[tim].tick_count = 0;

   /* Register IRQ */
   switch (tim)
   {
       case TIMER_CH0:
            result = s5p4418_timer_register_irq((uint8_t)tim, &timer0_irq, IRQ_PRIORITY_LEVEL_2);
       break;
       case TIMER_CH1:
            result = s5p4418_timer_register_irq((uint8_t)tim, &timer1_irq, IRQ_PRIORITY_LEVEL_2);
       break;
       case TIMER_CH2:
            result = s5p4418_timer_register_irq((uint8_t)tim, &timer2_irq, IRQ_PRIORITY_LEVEL_2);
       break;
       case TIMER_CH3:
            result = s5p4418_timer_register_irq((uint8_t)tim, &timer3_irq, IRQ_PRIORITY_LEVEL_2);
       break;
       default:
       break;
   }

   s5p4418_timer_start(tim, true);

   if (result)
   {
       return 0;
   }
   else
   {
       return -1;
   }
}

unsigned int timer_read(tim_t tim)
{
    config[tim].tick_count += s5p4418_timer_get_tick((uint8_t)tim);
    return (config[tim].tick_count);
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    /* Set timer */
    s5p4418_timer_set_cnt((uint8_t)tim, value - config[tim].tick_count);

    /* Start timer */
    s5p4418_timer_start((uint8_t)tim, false);

    return 0;
}

/* After timeout ticks IRQ happend */
int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    /* Set timer */
    s5p4418_timer_set_cnt((uint8_t)tim, timeout);

    /* Start timer */
    s5p4418_timer_start((uint8_t)tim, false);	

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    /* Set timer */
    s5p4418_timer_set_cnt((uint8_t)tim, 0);

    config[tim].tick_count = 0;

    return 0;
}

void timer_start(tim_t tim)
{
    s5p4418_timer_start((uint8_t)tim, false);
}

void timer_stop(tim_t tim)
{
    s5p4418_timer_stop((uint8_t)tim);
}

