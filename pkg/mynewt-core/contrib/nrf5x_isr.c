/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       mynewt-core isr mapping
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "mcu/mcu.h"
#include "cpu.h"

static void (*radio_isr_addr)(void);
static void (*rng_isr_addr)(void);
static void (*rtc0_isr_addr)(void);

void isr_radio(void)
{
    radio_isr_addr();
}

void isr_rng(void)
{
    rng_isr_addr();
}

void isr_rtc0(void)
{
    rtc0_isr_addr();
}

void nrf5x_hw_set_isr(int irqn, void (*addr)(void))
{
    switch (irqn) {
    case RADIO_IRQn:
        radio_isr_addr = addr;
        break;
    case RNG_IRQn:
        rng_isr_addr = addr;
        break;
    case RTC0_IRQn:
        rtc0_isr_addr = addr;
        break;
    }
}
