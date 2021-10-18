/*
 * Copyright (C)    2016 Nicholas Jackson
 *                  2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         boards_cc2650_launchpad
 * @{
 *
 * @file
 * @brief           Board specific implementations for TI CC2650 LaunchPad
 *
 * @author          Nicholas Jackson <nicholas.jackson@griffithuni.edu.au>
 * @author          Sebastian Meiling <s@mlng.net>
 */

#include "cpu.h"
#include "board.h"

/**
 * @brief           Initialise the board.
 */

void board_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
}
