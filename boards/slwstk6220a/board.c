/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_wstk6220
 * @{
 *
 * @file
 * @brief       Board specific implementations WSTK6220 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* enable access to the evaluation board controller chip. Without this, the
     * board controller does not forward the UART output to the USB port */
    gpio_init(BC_PIN, GPIO_OUT);
    gpio_set(BC_PIN);
    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    /* initialize the CPU */
    cpu_init();
}
