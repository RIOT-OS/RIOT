/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf9160dk
 * @{
 *
 * @file
 * @brief       Board initialization for the nRF9160DK
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{

    gpio_init(BTN0_PIN, BTN0_MODE);
    gpio_init(BTN1_PIN, BTN1_MODE);
    gpio_init(BTN2_PIN, BTN2_MODE);
    gpio_init(BTN3_PIN, BTN3_MODE);
}
