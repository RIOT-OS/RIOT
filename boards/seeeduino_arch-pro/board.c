/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_seeduino_arch-pro
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Seeeduino Arch Pro board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"

#include "periph/gpio.h"

extern void SystemInit(void);

void board_init(void)
{
    /* initialize core clocks via CMSIS function */
    SystemInit();
}
