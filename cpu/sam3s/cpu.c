/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3s
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include "cpu.h"

/**
 * @brief Initialize the CPU
 */
void cpu_init(void)
{
    /* disable the watchdog timer */
    WDT->WDT_MR |= WDT_MR_WDDIS;
    /* initialize the Cortex-M core */
    cortexm_init();
}
