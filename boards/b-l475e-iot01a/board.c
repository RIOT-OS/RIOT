/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_b-l475e-iot01a
 * @{
 *
 * @file
 * @brief       Board specific implementations for the ST B-L475E-IOT01A board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
#ifdef AUTO_INIT_LED0
    /* The LED pin is also used for SPI, so we enable it
       only if explicitly wanted by the user */
    gpio_init(LED0_PIN, GPIO_OUT);
#endif
}
