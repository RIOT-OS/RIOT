/*
 * Copyright (C) 2015-2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stk3600
 * @{
 *
 * @file
 * @brief       Board specific implementations STK3600 board
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
    /* initialize the CPU */
    cpu_init();

#ifndef RIOTBOOT
    /* perform common board initialization */
    board_common_init();
#endif
}
