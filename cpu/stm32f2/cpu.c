/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f2
 * @{
 *
 * @file
 * @brief       Implementation of the kernel cpu functions
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph/init.h"

void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize system clocks */
    stmclk_init_sysclk();
    /* trigger static peripheral initialization */
    periph_init();
}
