/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_saml1x
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Microchip
 *              SAML10 and SAML11 Xplained Pro board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void led_init(void);

void board_init(void)
{
    /* initialize the boards LEDs */
    led_init();

}

/**
 * @brief Initialize the boards on-board LED
 */
void led_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_OFF;
}
