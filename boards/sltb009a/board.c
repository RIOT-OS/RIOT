/*
 * SPDX-FileCopyrightText: 2015-2020 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
