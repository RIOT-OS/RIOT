/*
 * Copyright (C) 2017 Travis Griggs <travisgriggs@gmail.com>
 * Copyright (C) 2017 Dan Evans <photonthunder@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_seeeduino_xiao
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Seeeduino Xiao board
 *
 * @author      Travis Griggs <travisgriggs@gmail.com>
 * @author      Dan Evans <photonthunder@gmail.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Christopher Conroy <battloid.kouji@gmail.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the on-board LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_OFF;

    /* initialize the CPU */
    cpu_init();
}
