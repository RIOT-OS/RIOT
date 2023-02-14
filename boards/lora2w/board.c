/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_lora-e5-dev
 * @{
 *
 * @file
 * @brief       Board specific implementations for the LoRa-E5 Development Board - STM32WLE5JC board
 *
 * @author      Francisco Molina <francois-xavier.molina@inria/fr>
 *
 * @}
 */

#include <assert.h>
#include "kernel_defines.h"
#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph/dac.h"

#if IS_USED(MODULE_SX126X_STM32WL)
#include "sx126x.h"
#endif

void board_init(void)
{
    if (IS_USED(MODULE_SX126X_STM32WL)) {
        /* Initialize the GPIO control for RF 3-port switch (SP3T) */
        gpio_init(FE_CTRL1, GPIO_OUT);
        gpio_init(FE_CTRL2, GPIO_OUT);
        dac_init(DAC_LINE(0));
    }
}

#if IS_USED(MODULE_SX126X_STM32WL)
/**
 * @brief Callback to set RF switch mode
 *
 * This function sets the GPIO's wired to the SP3T RF Switch. LoRa-E5-dev
 * supports two modes of operation.
 */
void lora_e5_dev_sx126x_set_rf_mode(sx126x_t *dev, sx126x_rf_mode_t rf_mode)
{
    (void) dev;
    switch (rf_mode) {
    case SX126X_RF_MODE_RX:
        gpio_set(FE_CTRL1);
        gpio_clear(FE_CTRL2);
         dac_set(DAC_LINE(0), 0);
        break;
    case SX126X_RF_MODE_TX_HPA:
        gpio_clear(FE_CTRL1);
        gpio_set(FE_CTRL2);
        dac_set(DAC_LINE(0), LORA2W_DAC);
        break;
    default:
    case SX126X_RF_MODE_TX_LPA:
        gpio_clear(FE_CTRL1);
        gpio_set(FE_CTRL2);
        dac_set(DAC_LINE(0), LORA2W_DAC);
        break;
    }
}
#endif
