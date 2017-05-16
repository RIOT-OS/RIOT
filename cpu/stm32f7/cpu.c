/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f7
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @todo        Move this file into the stm32_common source tree
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph/init.h"

/**
 * @brief   Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize the system clock as configured in the periph_conf.h */
    stmclk_init_sysclk();
    /* trigger static peripheral initialization */
    periph_init();
}
