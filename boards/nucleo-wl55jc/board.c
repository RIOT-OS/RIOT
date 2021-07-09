/*
 * Copyright (C) 2021  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-wl55jc
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Nucleo-wl55jc board
 *
 *
 * @author      Akshai M <akshai.m@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    board_common_nucleo_init();

    if (IS_USED(MODULE_SX126X_STM32WL)) {
        /* Initialize the GPIO control for RF 3-port switch (SP3T) */
        gpio_init(FE_CTRL1, GPIO_OUT);
        gpio_init(FE_CTRL2, GPIO_OUT);
        gpio_init(FE_CTRL3, GPIO_OUT);
    }
}

/**
 * @brief Callback to set RF switch mode
 *
 * This function sets the GPIO's wired to the SP3T RF Switch. Nucleo-WL55JC
 * supports three modes of operation.
 */
#if IS_USED(MODULE_SX126X_STM32WL)
void nucleo_wl55jc_sx126x_set_rf_mode(sx126x_t *dev, sx126x_rf_mode_t rf_mode)
{
    (void)dev;
    switch (rf_mode) {
    case SX126X_RF_MODE_RX:
        gpio_set(FE_CTRL1);
        gpio_clear(FE_CTRL2);
        gpio_set(FE_CTRL3);
        break;

    case SX126X_RF_MODE_TX_LPA:
        gpio_set(FE_CTRL1);
        gpio_set(FE_CTRL2);
        gpio_set(FE_CTRL3);
        break;

    case SX126X_RF_MODE_TX_HPA:
        gpio_clear(FE_CTRL1);
        gpio_set(FE_CTRL2);
        gpio_set(FE_CTRL3);
        break;

    default:
        break;
    }
}
#endif
