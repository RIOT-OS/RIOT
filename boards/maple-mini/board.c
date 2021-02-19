/*
 * Copyright (C) 2016 Frits Kuipers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_maple-mini
 * @{
 *
 * @file
 * @brief       Board specific implementations for the maple-mini board
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the boards LED */
    gpio_init(LED0_PIN, GPIO_OUT);
}
