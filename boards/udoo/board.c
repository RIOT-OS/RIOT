/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_udoo
 * @{
 *
 * @file
 * @brief       Board specific implementations for the UDOO board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the on-board Amber "L" LED @ pin PB27 */
    gpio_init(LED0_PIN, GPIO_OUT);
    /* initialize the CPU */
    cpu_init();
}
