/*
 * Copyright (C) 2014 Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_airfy-beacon
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Airfy Beacon board
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

void board_init(void)
{
    /* setup led(s) for debugging */
    NRF_GPIO->PIN_CNF[LED_RED_PIN] = GPIO_PIN_CNF_DIR_Output;

    /* initialize the CPU */
    cpu_init();
}
