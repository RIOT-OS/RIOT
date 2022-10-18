/*
 * Copyright (C) 2015-2020 Freie Universität Berlin
 *               2022      Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sltb009a
 * @{
 *
 * @file
 * @brief       Board specific implementations SLTB009A board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "board.h"
#include "board_common.h"

void board_init(void)
{
#ifndef RIOTBOOT
    /* perform common board initialization */
    board_common_init();
#endif
    /* enable the virtual COM port */
    gpio_init(VCOM_EN_PIN, GPIO_OUT);
    gpio_set(VCOM_EN_PIN);
}
