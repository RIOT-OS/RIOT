/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f072
 * @{
 *
 * @file
 * @brief       Board specific implementations for the nucleo-f072 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      José Alamos <jialamos@uc.cl>
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
