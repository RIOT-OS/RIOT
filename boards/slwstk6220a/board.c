/*
 * SPDX-FileCopyrightText: 2015-2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_slwstk6220a
 * @{
 *
 * @file
 * @brief       Board specific implementations SLWSTK6220A board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "board_common.h"
#include "periph/gpio.h"

void board_init(void)
{
#ifndef RIOTBOOT
    /* perform common board initialization */
    board_common_init();

#ifdef MODULE_SI7021
    /* initialize the Si7021 sensor */
    gpio_init(SI7021_EN_PIN, GPIO_OUT);
    gpio_set(SI7021_EN_PIN);
#endif
#endif
}
