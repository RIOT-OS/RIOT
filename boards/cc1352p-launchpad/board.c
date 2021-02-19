/*
 * Copyright (C)    2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         boards_cc1352p_launchpad
 * @{
 *
 * @file
 * @brief           Board specific implementations for TI CC1352P LaunchPad
 *
 * @author          Luis A. Ruiz <luisan00@hotmail.com>
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
