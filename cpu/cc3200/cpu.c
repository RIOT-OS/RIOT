/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc3200
 * @{
 *
 * @file
 * @brief           Implementation of the CPU initialization
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 */

#include "cpu.h"
#include "periph/init.h"
#include "periph_cpu.h"
#include "vendor/hw_ints.h"
#include "vendor/hw_types.h"
#include "vendor/rom.h"

/**
 * @brief Initialize CPU and reset device peripheral
 *
 */
void cpu_init(void)
{
    /* initializes the Cortex-M core */
    cortexm_init();

    /* trigger static peripheral initialization */
    periph_init();
}

/**
 * @brief init peripheral clock and perform a softreset
 *
 * @param reg pointer to register
 */
void reset_periph_clk(cc3200_arcm_reg_t *reg)
{
    volatile unsigned long ulDelay;

    /* enable & reset peripherals */
    reg->soft_reset |= PRCM_SOFT_RESET;

    /* wait for the hardware to perform reset */
    for (ulDelay = 0; ulDelay < 16; ulDelay++) {
    }

    /* deassert reset */
    reg->soft_reset &= ~PRCM_SOFT_RESET;
}
