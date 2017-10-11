/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f303
 * @{
 *
 * @file
 * @brief       Board specific implementations for the nucleo-f303 board
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

#ifdef AUTO_INIT_LED0
    /* The LED pin is also used for SPI, so we enable it
       only if explicitly wanted by the user */
    gpio_init(LED0_PIN, GPIO_OUT);
#endif
}
