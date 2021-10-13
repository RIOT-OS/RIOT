/*
 * Copyright (C)  2016 Freie Universität Berlin
 *                2016-2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_arduino-mkr
 * @{
 * @file
 * @brief       Board common implementations for the Arduino MKR boards
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
    /* initialize the CPU */
    cpu_init();
    /* initialize the on-board Amber "L" LED */
    gpio_init(LED0_PIN, GPIO_OUT);
}
