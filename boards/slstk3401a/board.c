/*
 * Copyright (C) 2015-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_slstk3401a
 * @{
 *
 * @file
 * @brief       Board specific implementations SLSTK3401A board
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
    /* initialize the CPU */
    cpu_init();

    /* perform common board initialization */
    board_common_init();

#ifdef MODULE_SI7021
    /* initialize the Si7021 sensor */
    gpio_init(SI7021_EN_PIN, GPIO_OUT);
    gpio_set(SI7021_EN_PIN);
#endif
}
