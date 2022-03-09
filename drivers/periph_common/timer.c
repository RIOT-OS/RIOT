/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Shared peripheral timer code
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "periph/timer.h"
#include "irq.h"

#ifndef PERIPH_TIMER_PROVIDES_SET
int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    unsigned int state = irq_disable();
    int res = timer_set_absolute(dev, channel, timer_read(dev) + timeout);
    irq_restore(state);
    return res;
}
#endif
