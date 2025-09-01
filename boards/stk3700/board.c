/*
 * SPDX-FileCopyrightText: 2015-2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_stk3700
 * @{
 *
 * @file
 * @brief       Board specific implementations STK3700 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
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
}
