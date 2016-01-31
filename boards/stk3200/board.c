/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stk3200
 * @{
 *
 * @file
 * @brief       Board specific implementations STK3200 board
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
    /* initialize the CPU */
    cpu_init();

    /* enable access to the evaluation board controller chip. Without this, the
     * board controller does not forward the UART output to the USB port */
#if BC_ENABLED
    gpio_init(BC_PIN, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(BC_PIN);
#endif

    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init(LED1_PIN, GPIO_DIR_OUT, GPIO_NOPULL);
}