/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo32-l011
 * @{
 *
 * @file
 * @brief       Board specific implementations for the nucleo32-l011 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"


void board_init(void)
{
    /* initialize the boards LED */
    gpio_init(LED0_PIN, GPIO_OUT);

    /* initialize the CPU */
    cpu_init();
}
