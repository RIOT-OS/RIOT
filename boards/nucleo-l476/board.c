/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-l476
 * @{
 *
 * @file
 * @brief       Board specific implementations for the nucleo-l476 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LED only if explicitly enabled (pin is already used
     * for SPI_DEV(0)) */
#ifdef AUTO_INIT_LED0
    gpio_init(LED0_PIN, GPIO_OUT);
#endif
}
