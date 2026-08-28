/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf54l
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "kernel_init.h"
#include "periph/init.h"
#include "stdio_base.h"

/**
 * @brief   Initialize the CPU
 *
 * The CPU core runs from the internal 128 MHz oscillator out of reset, so no
 * clock initialization is needed for the basic peripherals (GPIO, UARTE2x and
 * TIMER2x are clocked from the 16 MHz peripheral clock).
 */
void cpu_init(void)
{
    /* call cortexm default initialization */
    cortexm_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}
