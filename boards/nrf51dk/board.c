/*
 * Copyright (C) 2016 Stephane D'Alu
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nrf51dk
 * @{
 *
 * @file
 * @brief       Board specific implementations for the NRF51-DK board
 *
 * @author      Stephane D'Alu <sdalu@sdalu.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

void board_init(void)
{
    /* initialize the boards LEDs, set pins as output and turn LEDs off */
    NRF_GPIO->DIRSET = (LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
    NRF_GPIO->OUTSET = (LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);

    /* initialize the CPU */
    cpu_init();
}
