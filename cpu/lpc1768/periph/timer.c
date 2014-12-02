/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       Implementation of the low-level timer driver for the LPC1768
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "periph_conf.h"
#include "periph/timer.h"

/* guard file in case no timers are defined */
#if TIMER_0_EN

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    /* not yet implemented */
    return -1;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    /* not yet implemented */
    return -1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    /* not yet implemented */
    return -1;
}

int timer_clear(tim_t dev, int channel)
{
    /* not yet implemented */
    return -1;
}

unsigned int timer_read(tim_t dev)
{
    /* not yet implemented */
    return 0;
}

void timer_start(tim_t dev)
{
    /* not yet implemented */
}

void timer_stop(tim_t dev)
{
    /* not yet implemented */
}

void timer_irq_enable(tim_t dev)
{
    /* not yet implemented */
}

void timer_irq_disable(tim_t dev)
{
    /* not yet implemented */
}

void timer_reset(tim_t dev)
{
    /* not yet implemented */
}

#endif /* TIMER_0_EN */
