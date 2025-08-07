/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_spark-core
 * @{
 *
 * @file
 * @brief       Board specific implementations for the spark-core board
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{

    /* disable systick interrupt, set by the spark bootloader */
    SysTick->CTRL = 0;
    /* signal to spark bootloader: do not enable IWDG! set Stop Mode Flag! */
    BKP->DR9 = 0xAFFF;
}
