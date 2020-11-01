/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x0
 * @{
 *
 * @file
 * @brief       implementation of the CPU initialization
 *
 * @author      Leon M. George <leon@georgemail.eu>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"
#include "vendor/driverlib/setup.h"

#ifndef HF_CLOCK_SOURCE
#define HF_CLOCK_SOURCE DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_RCOSC /* set 48MHz RCOSC */
#endif
#ifndef LF_CLOCK_SOURCE
#define LF_CLOCK_SOURCE DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_HF_RCOSC /* set 31.25kHz derived from 48MHz RCOSC */
#endif

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    /* Final trim of device */
    SetupTrimDevice();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();
}