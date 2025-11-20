/*
 * SPDX-FileCopyrightText: 2015-2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_e180-zg120b-tb
 * @{
 *
 * @file
 * @brief       Board specific implementations E180-ZG120B-TB board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"

void board_init(void)
{
#ifndef RIOTBOOT
    /* initialize the push buttons */
    gpio_init(PB0_PIN, GPIO_IN);
    gpio_init(PB1_PIN, GPIO_IN);
    gpio_init(PB2_PIN, GPIO_IN);
#endif
}
