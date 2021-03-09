/*
 * Copyright (C)  2016 Freie Universität Berlin
 *                2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_arduino_zero
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Arduino Zero board and
 *              clones
 *
 * @author      Hauke Pertersen  <hauke.pertersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the on-board Amber "L" LED on pin PA17 */
    gpio_init(LED0_PIN, GPIO_OUT);
}
