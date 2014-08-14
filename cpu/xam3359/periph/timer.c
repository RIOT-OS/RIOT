/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation for the SAM3X8E CPU
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"

#include "periph/timer.h"
#include "periph_conf.h"


typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * @brief Timer state memory
 */
//timer_conf_t config[TIMER_NUMOF];


/**
 * @brief Setup the given timer
 *
 * The SAM3X8E has 3 timers. Each timer has 3 independent channels.
 * RIOT uses the timers in WAVE mode with the following clock chaining:
 *
 * ----------                ----------
 * |        |                |        |-> IRQ-compareA
 * | TCx[2] | ---- TIOA2 --->| TCx[0] |-> IRQ-compareB
 * |        |        |       |        |-> IRQ-compareC
 * ----------        |       ----------
 *     ^             |
 *     |             |       ----------
 *     |             |       |        |-> IRQ-compareA
 * TIMER_CLOCK1      ------->| TCx[1] |-> IRQ-compareB
 *                           |        |-> IRQ-compareC
 *                           ----------
 *
 * For each timer, channel 0 is used to implement a prescaler. Channel 0 is
 * driven by the MCK / 2 (42MHz) (TIMER_CLOCK1).
 */
int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
 
            return -1;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    return 1;
}

int timer_clear(tim_t dev, int channel)
{
 

    return 1;
}

/*
 * The timer channels 1 and 2 are configured to run with the same speed and
 * have the same value (they run in parallel), so only on of them is returned.
 */
unsigned int timer_read(tim_t dev)
{
    switch (dev) {
            return 0;
    }
}

/*
 * For stopping the counting of channels 1 + 2, channel 0 is disabled.
 */
void timer_stop(tim_t dev)
{
	
}

void timer_start(tim_t dev)
{
   
}

void timer_irq_enable(tim_t dev)
{
  
}

void timer_irq_disable(tim_t dev)
{
  
}

void timer_reset(tim_t dev)
{
 
}


#if TIMER_0_EN
__attribute__ ((naked))
void TIMER_0_ISR1(void)
{
 
}

__attribute__ ((naked))
void TIMER_0_ISR2(void)
{
 
}
#endif /* TIMER_0_EN */


#if TIMER_1_EN
__attribute__ ((naked))
void TIMER_1_ISR1(void)
{
}

__attribute__ ((naked))
void TIMER_1_ISR2(void)
{
}
#endif /* TIMER_1_EN */


#if TIMER_2_EN
__attribute__ ((naked))
void TIMER_2_ISR1(void)
{
}

__attribute__ ((naked))
void TIMER_2_ISR2(void)
{
}
#endif /* TIMER_2_EN */
