/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Zolertia SL
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_remote-pa
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Re-Mote board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

/**
 * @brief Initialize the 2.4GHz Radio Frequency SW-controlled Switch
 *
 * The Re-Mote features an on-board RF switch to programmatically select to
 * enable either the internal ceramic antenna, or an external antenna over an
 * uFL connector.  As default we prefer to use the internal one.
 *
 */
static void rf_switch_init(void)
{
    /* Set RF 2.4GHz as default */
    gpio_init(RF_SWITCH_GPIO, GPIO_OUT);
    RF_SWITCH_2_4_GHZ;
}

void board_init(void)
{
    /* initialize the 2.4GHz RF switch */
    rf_switch_init();
}
