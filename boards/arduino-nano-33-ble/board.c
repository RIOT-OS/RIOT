/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-nano-33-ble
 * @{
 *
 * @file
 * @brief       Board initialization for the Arduino Nano 33 BLE
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT); /* Orange LED */
    gpio_init(LED1_PIN, GPIO_OUT); /* Red LED */
    gpio_set(LED1_PIN);
    gpio_init(LED2_PIN, GPIO_OUT); /* Green LED */
    gpio_set(LED2_PIN);
    gpio_init(LED3_PIN, GPIO_OUT); /* Blue LED */
    gpio_set(LED3_PIN);
    gpio_init(LED4_PIN, GPIO_OUT); /* PWR LED */
    gpio_clear(LED4_PIN);
}
