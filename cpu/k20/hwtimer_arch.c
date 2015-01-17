/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20
 * @{
 *
 * @file
 * @brief       Implementation of the kernels hwtimer interface
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>

#include "cpu-conf.h"
#include "arch/hwtimer_arch.h"
#include "board.h"
#include "kinetis_lptmr.h"
#include "thread.h"


/**
 * @brief Callback function that is given to the low-level timer
 *
 * @param[in] channel   the channel of the low-level timer that was triggered
 */
void irq_handler(uint16_t value, void *arg);

/**
 * @brief Hold a reference to the hwtimer callback
 */
void (*timeout_handler)(int);

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timeout_handler = handler;

    /* Select the 1Khz LPO clock */
    //kinetis_lptmr_init(KINETIS_LPTMR0, KINETIS_LPTMR_CLOCK_LPO, KINETIS_LPTMR_PRE_DISABLE, true);

    /* Select the MCGIRCLK (4Mhz) and use a divider of 4 to achieve 1Mhz */
    kinetis_lptmr_init(KINETIS_LPTMR0, KINETIS_LPTMR_CLOCK_MCGIRCLK, KINETIS_LPTMR_PRE_4, true);

    kinetis_lptmr_set_callback(KINETIS_LPTMR0, irq_handler, NULL);
}

void hwtimer_arch_enable_interrupt(void)
{
    kinetis_lptmr_enable_interrupt(KINETIS_LPTMR0);
}

void hwtimer_arch_disable_interrupt(void)
{
    kinetis_lptmr_disable_interrupt(KINETIS_LPTMR0);
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    hwtimer_arch_enable_interrupt();
    kinetis_lptmr_set_offset(KINETIS_LPTMR0, offset);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    hwtimer_arch_enable_interrupt();
    kinetis_lptmr_set_absolute(KINETIS_LPTMR0, value);
}

void hwtimer_arch_unset(short timer)
{
    hwtimer_arch_disable_interrupt();
    kinetis_lptmr_clear(KINETIS_LPTMR0);
}

unsigned long hwtimer_arch_now(void)
{
    return kinetis_lptmr_read(KINETIS_LPTMR0);
}

void irq_handler(uint16_t value, void *arg)
{
    timeout_handler(KINETIS_LPTMR0);
}
