/*
 * Copyright (C) 2016 Stephane D'Alu
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_ble400
 * @{
 *
 * @file
 * @brief       Board specific implementations for the BLE400 board
 *
 * @author      Stephane D'Alu <sdalu@sdalu.com>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "cpu.h"

void board_init(void)
{
    /* initialize the boards LEDs, set pins as output and turn LEDs off */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_init(LED4_PIN, GPIO_OUT);

    /* initialize the CPU */
    cpu_init();
}
